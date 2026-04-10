/********************************************************************\
 * gnc-ontogenesis-bridge.cpp -- Bridge between Meta-Cognition and *
 *                               Ontogenesis Kernel                 *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
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
 ********************************************************************/

/** @file gnc-ontogenesis-bridge.cpp
 *  @brief Implementation of the Meta-Cognition <-> Ontogenesis Kernel bridge.
 *
 *  This module wires Phase 5 recursive meta-cognition (the higher-level
 *  control loop that decides *what* to improve) to the ontogenesis kernel
 *  (the lower-level self-modification engine that generates/mutates ggml
 *  computational kernels at runtime).
 *
 *  Data flow:
 *
 *      Meta-Cognition  ─── directive ──>  Ontogenesis Kernel
 *                       <── result ────
 *
 *  When the ontogenesis kernel (OZC-272) is not yet registered the
 *  bridge operates in **stub mode**: directives are accepted and
 *  synthetic results are returned so the meta-cognitive improvement
 *  cycle can still run end-to-end.
 */

#include "gnc-ontogenesis-bridge.h"
#include <glib.h>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <cstring>

/* =================================================================
 * Internal: ontogenesis kernel stub
 *
 * Until the real ggnumlcash.cpp ontogenesis kernel lands (OZC-272),
 * we provide a lightweight stub that simulates kernel behaviour so
 * the full meta-cognition -> ontogenesis -> feedback loop can be
 * exercised and tested.
 * ================================================================= */

struct _GncOntogenesisKernel {
    guint             kernel_id;
    GncOntogenesisStatus status;
    gboolean          is_stub;           /* TRUE when using built-in stub */
    guint64           mutations_performed;
    guint64           kernels_generated;
    gdouble           cumulative_improvement;
};

/* =================================================================
 * Module state
 * ================================================================= */

static gboolean              bridge_initialized = FALSE;
static GncOntogenesisKernel *registered_kernel  = NULL;
static std::mutex            bridge_mutex;

/* Directive queue: meta-cognition -> ontogenesis */
static std::queue<GncOntogenesisDirective> directive_queue;
static std::mutex                          directive_mutex;

/* Result queue: ontogenesis -> meta-cognition */
static std::queue<GncOntogenesisResult*>   result_queue;
static std::mutex                          result_mutex;

/* Combined loop state */
static std::atomic<bool>  combined_loop_active{false};
static std::thread       *combined_loop_thread = nullptr;
static std::mutex         loop_mutex;

/* Forward declarations */
static GncOntogenesisResult* stub_process_directive(
    const GncOntogenesisDirective *directive);
static GncKernelMutationType map_process_to_mutation(
    GncMetaCognitiveProcessType process);
static void combined_loop_worker(
    GncMetaCognitiveSession *session,
    guint max_iterations,
    gdouble improvement_threshold);

/* =================================================================
 * Bridge Lifecycle
 * ================================================================= */

gboolean gnc_ontogenesis_bridge_init(void)
{
    std::lock_guard<std::mutex> lock(bridge_mutex);

    if (bridge_initialized) {
        g_warning("Ontogenesis bridge already initialized");
        return TRUE;
    }

    g_message("Initializing ontogenesis bridge (Meta-Cognition <-> Kernel)...");

    /* Create a default stub kernel so the bridge is always usable */
    if (!registered_kernel) {
        registered_kernel = g_new0(GncOntogenesisKernel, 1);
        registered_kernel->kernel_id   = 1;
        registered_kernel->status      = GNC_ONTOGENESIS_STATUS_READY;
        registered_kernel->is_stub     = TRUE;
        registered_kernel->mutations_performed  = 0;
        registered_kernel->kernels_generated    = 0;
        registered_kernel->cumulative_improvement = 0.0;
        g_message("  Stub ontogenesis kernel registered (will be replaced "
                  "when ggnumlcash.cpp lands via OZC-272)");
    }

    bridge_initialized = TRUE;

    g_message("Ontogenesis bridge initialized successfully");
    g_message("  Directive queue : meta-cognition -> ontogenesis");
    g_message("  Result queue    : ontogenesis -> meta-cognition");

    return TRUE;
}

void gnc_ontogenesis_bridge_shutdown(void)
{
    /* Stop combined loop first */
    gnc_ontogenesis_bridge_stop_combined_loop();

    std::lock_guard<std::mutex> lock(bridge_mutex);

    if (!bridge_initialized) return;

    g_message("Shutting down ontogenesis bridge...");

    /* Drain queues */
    {
        std::lock_guard<std::mutex> dl(directive_mutex);
        while (!directive_queue.empty()) directive_queue.pop();
    }
    {
        std::lock_guard<std::mutex> rl(result_mutex);
        while (!result_queue.empty()) {
            gnc_ontogenesis_result_free(result_queue.front());
            result_queue.pop();
        }
    }

    /* Free kernel handle */
    if (registered_kernel) {
        g_free(registered_kernel);
        registered_kernel = NULL;
    }

    bridge_initialized = FALSE;
    g_message("Ontogenesis bridge shutdown complete");
}

gboolean gnc_ontogenesis_bridge_is_ready(void)
{
    std::lock_guard<std::mutex> lock(bridge_mutex);
    return bridge_initialized && registered_kernel &&
           registered_kernel->status == GNC_ONTOGENESIS_STATUS_READY;
}

/* =================================================================
 * Kernel Registration
 * ================================================================= */

gboolean gnc_ontogenesis_bridge_register_kernel(GncOntogenesisKernel *kernel)
{
    if (!kernel) return FALSE;

    std::lock_guard<std::mutex> lock(bridge_mutex);

    /* Replace existing kernel (free old stub if present) */
    if (registered_kernel) {
        g_message("Replacing existing ontogenesis kernel (id=%u, stub=%s)",
                  registered_kernel->kernel_id,
                  registered_kernel->is_stub ? "yes" : "no");
        g_free(registered_kernel);
    }

    registered_kernel = kernel;
    g_message("Registered ontogenesis kernel id=%u (stub=%s)",
              kernel->kernel_id, kernel->is_stub ? "yes" : "no");

    return TRUE;
}

GncOntogenesisStatus gnc_ontogenesis_bridge_get_kernel_status(void)
{
    std::lock_guard<std::mutex> lock(bridge_mutex);
    if (!registered_kernel) return GNC_ONTOGENESIS_STATUS_UNINITIALIZED;
    return registered_kernel->status;
}

/* =================================================================
 * Meta-Cognition -> Ontogenesis  (top-down directives)
 * ================================================================= */

gboolean gnc_ontogenesis_bridge_submit_directive(
    const GncOntogenesisDirective *directive)
{
    if (!directive || !bridge_initialized) return FALSE;

    std::lock_guard<std::mutex> lock(directive_mutex);

    /* Deep-copy the directive (config pointers are not owned by us) */
    GncOntogenesisDirective copy = *directive;
    copy.suggested_config = NULL;  /* Don't keep dangling pointers */
    copy.evo_params       = NULL;

    directive_queue.push(copy);

    g_debug("Queued ontogenesis directive: mutation_type=%d, priority=%.2f",
            directive->mutation_type, directive->priority);

    /* In stub mode, immediately process the directive */
    {
        std::lock_guard<std::mutex> bl(bridge_mutex);
        if (registered_kernel && registered_kernel->is_stub) {
            GncOntogenesisResult *result = stub_process_directive(directive);
            if (result) {
                std::lock_guard<std::mutex> rl(result_mutex);
                result_queue.push(result);
            }
            /* Remove the directive we just processed */
            directive_queue.pop();
        }
    }

    return TRUE;
}

gint gnc_ontogenesis_bridge_submit_from_analysis(
    const GncSelfAnalysisResult *analysis)
{
    if (!analysis || !bridge_initialized) return -1;

    gint submitted = 0;

    /* If the analysis says evolution is needed, submit a directive
       targeting the appropriate kernel family. */
    if (analysis->requires_evolution) {
        GncOntogenesisDirective directive;
        memset(&directive, 0, sizeof(directive));

        directive.mutation_type    = map_process_to_mutation(analysis->process_type);
        directive.source_process   = analysis->process_type;
        directive.priority         = (analysis->improvement_score < -0.3) ? 1.0 : 0.6;
        directive.target_improvement = 0.1;  /* 10 % improvement target */
        directive.suggested_config = NULL;
        directive.evo_params       = NULL;

        if (gnc_ontogenesis_bridge_submit_directive(&directive)) {
            submitted++;
        }
    }

    /* Also submit per-suggestion directives for fine-grained mutations */
    if (analysis->optimization_suggestions) {
        for (gint i = 0; i < analysis->n_suggestions && i < 3; i++) {
            GncOntogenesisDirective directive;
            memset(&directive, 0, sizeof(directive));

            directive.mutation_type    = GNC_KERNEL_MUTATION_GENERAL;
            directive.source_process   = analysis->process_type;
            directive.priority         = 0.4;
            directive.target_improvement = 0.05;

            if (gnc_ontogenesis_bridge_submit_directive(&directive)) {
                submitted++;
            }
        }
    }

    g_debug("Submitted %d directives from self-analysis (process=%d, "
            "improvement=%.3f, evolution=%s)",
            submitted, analysis->process_type, analysis->improvement_score,
            analysis->requires_evolution ? "yes" : "no");

    return submitted;
}

guint gnc_ontogenesis_bridge_pending_directives(void)
{
    std::lock_guard<std::mutex> lock(directive_mutex);
    return static_cast<guint>(directive_queue.size());
}

/* =================================================================
 * Ontogenesis -> Meta-Cognition  (bottom-up feedback)
 * ================================================================= */

GncOntogenesisResult* gnc_ontogenesis_bridge_poll_result(void)
{
    std::lock_guard<std::mutex> lock(result_mutex);
    if (result_queue.empty()) return NULL;

    GncOntogenesisResult *result = result_queue.front();
    result_queue.pop();
    return result;
}

gboolean gnc_ontogenesis_bridge_integrate_result(
    const GncOntogenesisResult *result)
{
    if (!result) return FALSE;

    /* Map the kernel mutation back to a meta-cognitive process type */
    GncMetaCognitiveProcessType process;
    switch (result->mutation_type) {
    case GNC_KERNEL_MUTATION_ATTENTION:  process = GNC_METACOG_PROCESS_ATTENTION;  break;
    case GNC_KERNEL_MUTATION_INFERENCE:  process = GNC_METACOG_PROCESS_VALIDATION; break;
    case GNC_KERNEL_MUTATION_CLUSTERING: process = GNC_METACOG_PROCESS_CLUSTERING; break;
    case GNC_KERNEL_MUTATION_TENSOR:     process = GNC_METACOG_PROCESS_TENSOR_OPS; break;
    case GNC_KERNEL_MUTATION_MESSAGING:  process = GNC_METACOG_PROCESS_MESSAGING;  break;
    default:                             process = GNC_METACOG_PROCESS_ALL;        break;
    }

    /* Read current metrics, apply deltas from ontogenesis result,
       then write them back so the next meta-cognitive cycle sees
       the improvement. */
    GncCognitiveMetrics metrics;
    if (!gnc_meta_cognitive_get_metrics(process, &metrics)) {
        /* No existing metrics; start from scratch */
        memset(&metrics, 0, sizeof(metrics));
        metrics.accuracy        = 0.75;
        metrics.efficiency      = 0.70;
        metrics.stability_index = 0.80;
        metrics.latency_ms      = 10.0;
        metrics.throughput      = 100.0;
    }

    if (result->success) {
        /* Apply the improvement, clamped to [0, 1] where applicable */
        metrics.accuracy = std::min(1.0, metrics.accuracy *
                                   (1.0 + result->achieved_improvement));
        metrics.efficiency = std::min(1.0, metrics.efficiency *
                                     (1.0 + result->achieved_improvement * 0.5));
        metrics.stability_index = std::max(0.0, std::min(1.0,
            metrics.stability_index + result->stability_delta));
        metrics.latency_ms = std::max(0.1,
            metrics.latency_ms + result->latency_delta_ms);
        metrics.innovation_score = std::min(1.0,
            metrics.innovation_score + 0.02 * result->kernels_accepted);
    }

    gboolean ok = gnc_meta_cognitive_update_metrics(process, &metrics);

    g_debug("Integrated ontogenesis result for process %d: success=%s, "
            "improvement=%.3f, new_accuracy=%.3f",
            process, result->success ? "yes" : "no",
            result->achieved_improvement, metrics.accuracy);

    return ok;
}

void gnc_ontogenesis_result_free(GncOntogenesisResult *result)
{
    if (!result) return;
    g_free(result->diagnostic_message);
    g_free(result);
}

/* =================================================================
 * Combined Cycle: Meta-Cognition + Ontogenesis
 * ================================================================= */

gdouble gnc_ontogenesis_bridge_run_combined_step(
    GncMetaCognitiveSession *session)
{
    if (!session || !bridge_initialized) return -1.0;

    /* 1. Meta-cognitive self-analysis */
    GncSelfAnalysisResult *analysis =
        gnc_meta_cognitive_analyze_system(session);
    if (!analysis) return -1.0;

    gdouble improvement = analysis->improvement_score;

    /* 2. If evolution needed, submit directives to ontogenesis */
    if (analysis->requires_evolution) {
        gint n = gnc_ontogenesis_bridge_submit_from_analysis(analysis);
        g_debug("Combined step: submitted %d directives", n);
    }

    gnc_self_analysis_result_free(analysis);

    /* 3. Poll and integrate any completed ontogenesis results */
    GncOntogenesisResult *result;
    while ((result = gnc_ontogenesis_bridge_poll_result()) != NULL) {
        gnc_ontogenesis_bridge_integrate_result(result);
        if (result->success) {
            improvement += result->achieved_improvement;
        }
        gnc_ontogenesis_result_free(result);
    }

    return improvement;
}

static void combined_loop_worker(
    GncMetaCognitiveSession *session,
    guint max_iterations,
    gdouble improvement_threshold)
{
    g_message("Starting combined meta-cognition + ontogenesis loop "
              "(max_iter=%u, threshold=%.3f)",
              max_iterations, improvement_threshold);

    gdouble cumulative = 0.0;

    for (guint i = 0; i < max_iterations && combined_loop_active.load(); i++) {
        gdouble step_improvement =
            gnc_ontogenesis_bridge_run_combined_step(session);

        if (step_improvement < 0.0) {
            g_warning("Combined step failed at iteration %u", i);
            break;
        }

        cumulative += step_improvement;

        g_debug("Combined loop iteration %u: step=%.4f, cumulative=%.4f",
                i, step_improvement, cumulative);

        if (cumulative >= improvement_threshold) {
            g_message("Improvement threshold %.3f reached at iteration %u "
                      "(cumulative=%.4f)", improvement_threshold, i, cumulative);
            break;
        }

        /* Throttle to avoid spinning */
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    combined_loop_active.store(false);
    g_message("Combined loop finished (cumulative improvement=%.4f)", cumulative);
}

gboolean gnc_ontogenesis_bridge_start_combined_loop(
    GncMetaCognitiveSession *session,
    guint max_iterations,
    gdouble improvement_threshold)
{
    if (!session || !bridge_initialized) return FALSE;

    std::lock_guard<std::mutex> lock(loop_mutex);

    if (combined_loop_active.load()) {
        g_warning("Combined loop already running");
        return FALSE;
    }

    combined_loop_active.store(true);

    /* Clean up previous thread if any */
    if (combined_loop_thread) {
        if (combined_loop_thread->joinable())
            combined_loop_thread->join();
        delete combined_loop_thread;
    }

    combined_loop_thread = new std::thread(
        combined_loop_worker, session, max_iterations, improvement_threshold);

    g_message("Combined meta-cognition + ontogenesis loop started");
    return TRUE;
}

gboolean gnc_ontogenesis_bridge_stop_combined_loop(void)
{
    std::lock_guard<std::mutex> lock(loop_mutex);

    if (!combined_loop_active.load()) return FALSE;

    combined_loop_active.store(false);

    if (combined_loop_thread) {
        if (combined_loop_thread->joinable())
            combined_loop_thread->join();
        delete combined_loop_thread;
        combined_loop_thread = nullptr;
    }

    g_message("Combined loop stopped");
    return TRUE;
}

gboolean gnc_ontogenesis_bridge_is_combined_loop_active(void)
{
    return combined_loop_active.load() ? TRUE : FALSE;
}

/* =================================================================
 * Internal: Stub Kernel Processing
 *
 * Simulates what the real ontogenesis kernel (ggnumlcash.cpp) will
 * do once OZC-272 lands.  The stub applies a small random
 * improvement and always succeeds, which lets the full
 * meta-cognition feedback loop be tested today.
 * ================================================================= */

static GncOntogenesisResult* stub_process_directive(
    const GncOntogenesisDirective *directive)
{
    if (!directive || !registered_kernel) return NULL;

    registered_kernel->status = GNC_ONTOGENESIS_STATUS_MUTATING;
    registered_kernel->mutations_performed++;

    GncOntogenesisResult *result = g_new0(GncOntogenesisResult, 1);
    result->mutation_type = directive->mutation_type;
    result->success       = TRUE;

    /* Simulate a modest improvement proportional to priority */
    result->achieved_improvement = 0.01 + 0.04 * directive->priority;
    result->stability_delta      = 0.005;
    result->latency_delta_ms     = -0.2;   /* slightly faster */
    result->kernels_generated    = 3;
    result->kernels_accepted     = 2;

    registered_kernel->kernels_generated   += result->kernels_generated;
    registered_kernel->cumulative_improvement += result->achieved_improvement;

    result->diagnostic_message = g_strdup_printf(
        "[stub] Processed mutation_type=%d, priority=%.2f -> "
        "improvement=%.3f, generated=%u kernels, accepted=%u",
        directive->mutation_type, directive->priority,
        result->achieved_improvement,
        result->kernels_generated, result->kernels_accepted);

    registered_kernel->status = GNC_ONTOGENESIS_STATUS_READY;

    g_debug("%s", result->diagnostic_message);
    return result;
}

static GncKernelMutationType map_process_to_mutation(
    GncMetaCognitiveProcessType process)
{
    switch (process) {
    case GNC_METACOG_PROCESS_ATTENTION:  return GNC_KERNEL_MUTATION_ATTENTION;
    case GNC_METACOG_PROCESS_VALIDATION: return GNC_KERNEL_MUTATION_INFERENCE;
    case GNC_METACOG_PROCESS_CLUSTERING: return GNC_KERNEL_MUTATION_CLUSTERING;
    case GNC_METACOG_PROCESS_TENSOR_OPS: return GNC_KERNEL_MUTATION_TENSOR;
    case GNC_METACOG_PROCESS_MESSAGING:  return GNC_KERNEL_MUTATION_MESSAGING;
    default:                             return GNC_KERNEL_MUTATION_GENERAL;
    }
}
