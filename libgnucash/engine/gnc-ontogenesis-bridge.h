/********************************************************************\
 * gnc-ontogenesis-bridge.h -- Bridge between Meta-Cognition and   *
 *                             Ontogenesis Kernel                   *
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

/** @addtogroup Engine
    @{ */
/** @addtogroup OntogenesisBridge
    Bridge connecting Phase 5 recursive meta-cognition (the higher-level
    control loop) with the ontogenesis kernel (the lower-level
    self-modification mechanism from ggnumlcash.cpp).

    The meta-cognitive layer observes system performance and decides
    *what* should be optimized.  The ontogenesis kernel performs the
    actual runtime code generation / kernel mutation.  This bridge
    translates meta-cognitive optimization directives into concrete
    ontogenesis kernel operations and feeds kernel-generated metrics
    back into the meta-cognitive feedback loop.
    @{ */

/** @file gnc-ontogenesis-bridge.h
    @brief Meta-Cognition <-> Ontogenesis Kernel integration bridge
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_ONTOGENESIS_BRIDGE_H
#define GNC_ONTOGENESIS_BRIDGE_H

#include "gnc-meta-cognitive.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================
 * Ontogenesis Kernel Abstraction
 *
 * The ontogenesis kernel (ggnumlcash.cpp / OZC-272) is a
 * self-generating ggml-based engine that creates new computational
 * kernels at runtime.  The types below abstract its interface so
 * the meta-cognitive layer can drive it without hard-coupling to
 * a specific kernel revision.
 * ================================================================ */

/** Opaque handle to an ontogenesis kernel instance */
typedef struct _GncOntogenesisKernel GncOntogenesisKernel;

/** Status of an ontogenesis kernel */
typedef enum {
    GNC_ONTOGENESIS_STATUS_UNINITIALIZED,  /**< Kernel not yet loaded */
    GNC_ONTOGENESIS_STATUS_READY,          /**< Kernel ready for mutations */
    GNC_ONTOGENESIS_STATUS_MUTATING,       /**< Kernel currently generating new code */
    GNC_ONTOGENESIS_STATUS_VALIDATING,     /**< Validating a generated kernel */
    GNC_ONTOGENESIS_STATUS_ERROR           /**< Kernel in error state */
} GncOntogenesisStatus;

/** Type of kernel mutation the meta-cognitive layer can request */
typedef enum {
    GNC_KERNEL_MUTATION_ATTENTION,         /**< Optimize ECAN attention kernels */
    GNC_KERNEL_MUTATION_INFERENCE,         /**< Optimize PLN inference kernels */
    GNC_KERNEL_MUTATION_CLUSTERING,        /**< Optimize pattern-clustering kernels */
    GNC_KERNEL_MUTATION_TENSOR,            /**< Optimize tensor-network kernels */
    GNC_KERNEL_MUTATION_MESSAGING,         /**< Optimize inter-node messaging kernels */
    GNC_KERNEL_MUTATION_GENERAL            /**< General-purpose kernel evolution */
} GncKernelMutationType;

/** Directive issued by the meta-cognitive layer to the ontogenesis kernel */
typedef struct {
    GncKernelMutationType mutation_type;   /**< Which kernel family to mutate */
    GncMetaCognitiveProcessType source_process; /**< Meta-cog process that issued the directive */
    gdouble priority;                      /**< Urgency (0.0 = low, 1.0 = critical) */
    gdouble target_improvement;            /**< Desired improvement factor */
    const GncCognitiveArchConfig *suggested_config; /**< Suggested parameter space (may be NULL) */
    const GncEvolutionaryParams  *evo_params;       /**< MOSES parameters to use (may be NULL) */
} GncOntogenesisDirective;

/** Result reported by the ontogenesis kernel back to meta-cognition */
typedef struct {
    GncKernelMutationType mutation_type;   /**< Which kernel family was mutated */
    gboolean success;                      /**< Whether the mutation succeeded */
    gdouble achieved_improvement;          /**< Measured improvement after mutation */
    gdouble stability_delta;               /**< Change in stability index */
    gdouble latency_delta_ms;              /**< Change in processing latency */
    guint kernels_generated;               /**< Number of new kernels generated */
    guint kernels_accepted;                /**< Number of kernels that passed validation */
    gchar *diagnostic_message;             /**< Human-readable diagnostic (caller frees) */
} GncOntogenesisResult;

/* ================================================================
 * Bridge Lifecycle
 * ================================================================ */

/** Initialize the ontogenesis bridge.
 *  Must be called after gnc_meta_cognitive_init().
 *  @return TRUE on success */
gboolean gnc_ontogenesis_bridge_init(void);

/** Shut down the ontogenesis bridge.
 *  Releases all kernel handles and pending directives. */
void gnc_ontogenesis_bridge_shutdown(void);

/** Check whether the bridge is operational.
 *  @return TRUE if both meta-cognition and ontogenesis sides are ready */
gboolean gnc_ontogenesis_bridge_is_ready(void);

/* ================================================================
 * Kernel Registration
 * ================================================================ */

/** Register an ontogenesis kernel instance with the bridge.
 *  The bridge takes ownership of the kernel handle.
 *  @param kernel  Kernel handle (from ggnumlcash.cpp or stub)
 *  @return TRUE if registration succeeded */
gboolean gnc_ontogenesis_bridge_register_kernel(GncOntogenesisKernel *kernel);

/** Query the current status of the registered kernel.
 *  @return Kernel status, or GNC_ONTOGENESIS_STATUS_UNINITIALIZED if none */
GncOntogenesisStatus gnc_ontogenesis_bridge_get_kernel_status(void);

/** Create a new stub ontogenesis kernel for testing or as a placeholder.
 *  The returned handle can be passed to gnc_ontogenesis_bridge_register_kernel().
 *  @param kernel_id  Unique kernel identifier
 *  @return New kernel handle (caller owns; bridge takes ownership after register) */
GncOntogenesisKernel* gnc_ontogenesis_kernel_new_stub(guint kernel_id);

/* ================================================================
 * Meta-Cognition -> Ontogenesis  (top-down directives)
 * ================================================================ */

/** Submit an optimization directive from meta-cognition to the kernel.
 *  The directive is queued and processed asynchronously.
 *  @param directive  Optimization directive (copied internally)
 *  @return TRUE if the directive was accepted */
gboolean gnc_ontogenesis_bridge_submit_directive(
    const GncOntogenesisDirective *directive);

/** Convenience: translate a GncSelfAnalysisResult into directives and
 *  submit them automatically.  Called by the improvement cycle when
 *  evolution is needed.
 *  @param analysis  Self-analysis result from meta-cognition
 *  @return Number of directives submitted, or -1 on error */
gint gnc_ontogenesis_bridge_submit_from_analysis(
    const GncSelfAnalysisResult *analysis);

/** Get the number of pending (unprocessed) directives.
 *  @return Pending directive count */
guint gnc_ontogenesis_bridge_pending_directives(void);

/* ================================================================
 * Ontogenesis -> Meta-Cognition  (bottom-up feedback)
 * ================================================================ */

/** Poll for the next completed ontogenesis result.
 *  @return Result structure (caller frees with gnc_ontogenesis_result_free()),
 *          or NULL if no results are pending */
GncOntogenesisResult* gnc_ontogenesis_bridge_poll_result(void);

/** Feed an ontogenesis result back into the meta-cognitive metrics.
 *  Updates the global metrics so the next self-analysis cycle sees
 *  the effect of the kernel mutation.
 *  @param result  Ontogenesis result to integrate
 *  @return TRUE if metrics were updated successfully */
gboolean gnc_ontogenesis_bridge_integrate_result(
    const GncOntogenesisResult *result);

/** Free an ontogenesis result structure.
 *  @param result  Result to free (may be NULL) */
void gnc_ontogenesis_result_free(GncOntogenesisResult *result);

/* ================================================================
 * Combined Cycle: Meta-Cognition + Ontogenesis
 * ================================================================ */

/** Run a single combined meta-cognition + ontogenesis step.
 *
 *  1. Perform meta-cognitive self-analysis
 *  2. If evolution is needed, submit directives to ontogenesis kernel
 *  3. Poll for completed ontogenesis results
 *  4. Integrate results back into meta-cognitive metrics
 *
 *  @param session   Meta-cognitive session handle
 *  @return Aggregate improvement score, or -1.0 on error */
gdouble gnc_ontogenesis_bridge_run_combined_step(
    GncMetaCognitiveSession *session);

/** Start a background loop that repeatedly runs combined steps.
 *  @param session             Meta-cognitive session
 *  @param max_iterations      Maximum combined iterations
 *  @param improvement_threshold  Stop when aggregate improvement exceeds this
 *  @return TRUE if the loop was started */
gboolean gnc_ontogenesis_bridge_start_combined_loop(
    GncMetaCognitiveSession *session,
    guint max_iterations,
    gdouble improvement_threshold);

/** Stop the running combined loop.
 *  @return TRUE if stopped, FALSE if no loop was running */
gboolean gnc_ontogenesis_bridge_stop_combined_loop(void);

/** Stop the combined loop only if it is running for the given session.
 *  Safe to call when destroying a session without affecting loops
 *  owned by other sessions.
 *  @param session  Session being destroyed
 *  @return TRUE if stopped, FALSE if loop was not using this session */
gboolean gnc_ontogenesis_bridge_stop_combined_loop_for_session(
    GncMetaCognitiveSession *session);

/** Check if the combined loop is running.
 *  @return TRUE if the combined loop is active */
gboolean gnc_ontogenesis_bridge_is_combined_loop_active(void);

#ifdef __cplusplus
}
#endif

#endif /* GNC_ONTOGENESIS_BRIDGE_H */

/** @} */
/** @} */
