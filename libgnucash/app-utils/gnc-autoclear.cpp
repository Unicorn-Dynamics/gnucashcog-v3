/********************************************************************
 * gnc-autoclear.cpp -- Knapsack algorithm functions                *
 *                                                                  *
 * Copyright 2020 Cristian Klein <cristian@kleinlabs.eu>            *
 * Copyright 2021 Christopher Lam to clear same-amount splits       *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652       *
 * Boston, MA  02110-1301,  USA       gnu@gnu.org                   *
 *******************************************************************/

#include <config.h>

#include <glib/gi18n.h>

#include "Account.h"
#include "Account.hpp"
#include "Split.h"
#include "Transaction.h"
#include "gnc-autoclear.h"

#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <stdexcept>
#include <cinttypes>
#include <charconv>
#include <cstring>

#define log_module "gnc.autoclear"

struct RuntimeMonitor
{
    uint64_t m_counter = 0;
    std::optional<double> m_seconds;
    std::chrono::steady_clock::time_point m_start;
    RuntimeMonitor (double seconds) : m_start(std::chrono::steady_clock::now())
    {
        if (seconds > 0) m_seconds = seconds;
    };
    double get_elapsed ()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - m_start).count();
    }
    bool should_abort ()
    {
        if (!m_seconds.has_value()) return false;
        if (++m_counter % 100000 != 0) return false;
        return get_elapsed() > *m_seconds;
    }
};

struct SplitInfo
{
    int64_t m_amount;
    Split* m_split;
};

using SplitInfoVec = std::vector<SplitInfo>;
using SplitVec = std::vector<Split*>;

struct Solution
{
    bool abort = false;
    SplitVec splits;
};

static const char*
path_to_str(const SplitInfoVec& path)
{
    if (path.empty()) return "<empty>";

    static char buff[1000];
    char* p = buff;
    char* end = buff + sizeof(buff);
    for (const auto& split_info : path)
    {
        if (p != buff)
        {
            if (p < end) *p++ = '|';
            else return "<overflow>";
        }
        auto res = std::to_chars (p, end, split_info.m_amount);
        if (res.ec == std::errc()) p = res.ptr;
        else return "<overflow>";
    }
    p = std::min (end - 1, p);
    *p = '\0';
    return buff;
}

static void
subset_sum (SplitInfoVec::const_iterator iter,
            SplitInfoVec::const_iterator end,
            SplitInfoVec& path, Solution& solution,
            int64_t target, RuntimeMonitor& monitor)
{
    DEBUG ("this=%" PRId64" target=%" PRId64 " path=%s",
           iter == end ? 0 : iter->m_amount, target, path_to_str (path));

    if (target == 0)
    {
        DEBUG ("SOLUTION FOUND: %s%s", path_to_str (path),
               solution.splits.empty() ? "" : " ABORT: AMBIGUOUS");
        if (!solution.splits.empty())
            solution.abort = true;
        else
        {
            solution.splits.resize (path.size());
            std::transform (path.begin(), path.end(), solution.splits.begin(),
                            [](SplitInfo& i){ return i.m_split; });
            return;
        }
    }

    if (solution.abort || iter == end)
        return;

    if (monitor.should_abort())
    {
        DEBUG ("ABORT: timeout");
        solution.abort = true;
        return;
    }

    auto next = std::next(iter);

    // 1st path: use current_num
    path.push_back (*iter);
    subset_sum (next, end, path, solution, target - iter->m_amount, monitor);
    path.pop_back ();

    // 2nd path: skip current_num
    subset_sum (next, end, path, solution, target, monitor);
}

GList *
gnc_account_get_autoclear_splits (Account *account, gnc_numeric toclear_value,
                                  time64 end_date, GError **error,
                                  double max_seconds)
{
    g_return_val_if_fail (account && error, nullptr);

    auto scu{xaccAccountGetCommoditySCU (account)};
    auto numeric_to_int64 = [scu](gnc_numeric num) -> int64_t
    {
        return gnc_numeric_num
            (gnc_numeric_convert
             (num, scu, GNC_HOW_DENOM_EXACT | GNC_HOW_RND_NEVER));
    };

    int64_t target{numeric_to_int64 (toclear_value)};
    SplitInfoVec splits;
    for (auto split : xaccAccountGetSplits (account))
    {
        if (xaccTransGetDate (xaccSplitGetParent (split)) > end_date)
            break;
        int64_t amt{numeric_to_int64 (xaccSplitGetAmount (split))};
        if (xaccSplitGetReconcile (split) != NREC)
            target -= amt;
        else if (amt == 0)
            DEBUG ("skipping zero-amount split %p", split);
        else
            splits.push_back ({amt, split});
    }

    static GQuark autoclear_quark = g_quark_from_static_string ("autoclear");
    if (target == 0)
    {
        g_set_error (error, autoclear_quark, 1, "%s",
                     "Account is already at Auto-Clear Balance.");
        return nullptr;
    }

    RuntimeMonitor monitor{max_seconds};
    Solution solution;
    SplitInfoVec path;
    path.reserve (splits.size());

    subset_sum (splits.begin(), splits.end(), path, solution, target, monitor);

    DEBUG ("finished subset_sum in %f seconds", monitor.get_elapsed());

    if (solution.splits.empty() || solution.abort)
    {
        g_set_error (error, autoclear_quark, 1, "%s",
                     "The selected amount cannot be cleared.");
        return nullptr;
    }

    return std::accumulate
        (solution.splits.begin(), solution.splits.end(),
         static_cast<GList*>(nullptr), g_list_prepend);
}
