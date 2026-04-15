/********************************************************************\
 * gnc-meta-cognitive.h -- Recursive Meta-Cognition Engine        *
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
/** @addtogroup MetaCognitive
    Recursive meta-cognition and evolutionary optimization system.
    Enables the system to observe, analyze, and recursively improve 
    itself using evolutionary algorithms and self-modification.
    @{ */

/** @file gnc-meta-cognitive.h
    @brief Recursive meta-cognitive analysis and evolutionary optimization
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_META_COGNITIVE_H
#define GNC_META_COGNITIVE_H

#include "gnc-cognitive-accounting.h"
#include "gnc-tensor-network.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name Meta-Cognitive Analysis Types */
/** @{ */

/** Handle for meta-cognitive analysis sessions */
typedef struct _GncMetaCognitiveSession GncMetaCognitiveSession;

/** Types of cognitive processes that can be analyzed */
typedef enum {
    GNC_METACOG_PROCESS_ATTENTION,      /**< ECAN attention allocation */
    GNC_METACOG_PROCESS_VALIDATION,     /**< PLN validation reasoning */
    GNC_METACOG_PROCESS_CLUSTERING,     /**< Pattern recognition */
    GNC_METACOG_PROCESS_PREDICTION,     /**< URE predictive reasoning */
    GNC_METACOG_PROCESS_EVOLUTION,      /**< MOSES strategy evolution */
    GNC_METACOG_PROCESS_TENSOR_OPS,     /**< Tensor network operations */
    GNC_METACOG_PROCESS_MESSAGING,      /**< Inter-node communication */
    GNC_METACOG_PROCESS_ALL             /**< All cognitive processes */
} GncMetaCognitiveProcessType;

/** Performance metrics for cognitive processes */
typedef struct {
    gdouble accuracy;                   /**< Task completion accuracy (0.0-1.0) */
    gdouble efficiency;                 /**< Resource efficiency (0.0-1.0) */
    gdouble latency_ms;                 /**< Average processing latency */
    gdouble throughput;                 /**< Operations per second */
    gdouble memory_usage_mb;            /**< Memory consumption */
    gdouble cpu_usage_percent;          /**< CPU utilization */
    gdouble stability_index;            /**< Stability measure (0.0-1.0) */
    gdouble learning_rate;              /**< Adaptation speed */
    gdouble innovation_score;           /**< Novel pattern generation */
    guint64 total_operations;           /**< Cumulative operation count */
    guint64 error_count;                /**< Number of errors/failures */
    time_t last_update;                 /**< Last metrics update time */
} GncCognitiveMetrics;

/** Self-analysis results */
typedef struct {
    GncMetaCognitiveProcessType process_type;  /**< Process being analyzed */
    GncCognitiveMetrics current_metrics;       /**< Current performance */
    GncCognitiveMetrics baseline_metrics;      /**< Baseline for comparison */
    gdouble improvement_score;                 /**< Overall improvement (-1.0 to 1.0) */
    gchar **optimization_suggestions;          /**< Array of improvement suggestions */
    gint n_suggestions;                        /**< Number of suggestions */
    gboolean requires_evolution;               /**< Whether evolution is needed */
    gdouble confidence_level;                  /**< Confidence in analysis (0.0-1.0) */
} GncSelfAnalysisResult;

/** Evolutionary optimization parameters */
typedef struct {
    guint population_size;              /**< Size of evolutionary population */
    gdouble mutation_rate;              /**< Probability of mutations */
    gdouble crossover_rate;             /**< Probability of crossover */
    guint max_generations;              /**< Maximum evolutionary generations */
    gdouble fitness_threshold;          /**< Target fitness level */
    gdouble diversity_pressure;         /**< Pressure to maintain diversity */
    gboolean elitism_enabled;           /**< Preserve best individuals */
    guint elite_count;                  /**< Number of elites to preserve */
} GncEvolutionaryParams;

/** Cognitive architecture configuration */
typedef struct {
    /* ECAN attention parameters */
    gdouble sti_funds;                  /**< Available STI funds */
    gdouble lti_funds;                  /**< Available LTI funds */
    gdouble attention_decay_rate;       /**< Attention decay rate */
    gdouble wage_rate;                  /**< Activity reward rate */
    
    /* PLN reasoning parameters */
    gdouble truth_threshold;            /**< Truth value threshold */
    gdouble confidence_threshold;       /**< Confidence threshold */
    guint max_inference_steps;          /**< Maximum PLN inference steps */
    
    /* Tensor network parameters */
    guint tensor_cache_size;            /**< Tensor data cache size */
    gdouble message_priority_threshold; /**< Message priority cutoff */
    guint max_concurrent_ops;           /**< Max parallel operations */
    
    /* Learning parameters */
    gdouble base_learning_rate;         /**< Base adaptation rate */
    gdouble learning_momentum;          /**< Learning momentum factor */
    gdouble regularization_strength;    /**< Overfitting prevention */
} GncCognitiveArchConfig;

/** @} */

/** @name Meta-Cognitive Engine Functions */
/** @{ */

/** Initialize the meta-cognitive analysis system
 * @return TRUE if initialization successful, FALSE otherwise */
gboolean gnc_meta_cognitive_init(void);

/** Shutdown the meta-cognitive analysis system */
void gnc_meta_cognitive_shutdown(void);

/** Create a new meta-cognitive analysis session
 * @return New session handle, or NULL on failure */
GncMetaCognitiveSession* gnc_meta_cognitive_session_new(void);

/** Destroy a meta-cognitive analysis session
 * @param session Session to destroy */
void gnc_meta_cognitive_session_destroy(GncMetaCognitiveSession *session);

/** @} */

/** @name Self-Analysis Functions */
/** @{ */

/** Perform self-analysis of a cognitive process
 * @param session Meta-cognitive session
 * @param process_type Type of cognitive process to analyze
 * @return Self-analysis results, must be freed with gnc_self_analysis_result_free() */
GncSelfAnalysisResult* gnc_meta_cognitive_analyze_process(
    GncMetaCognitiveSession *session,
    GncMetaCognitiveProcessType process_type);

/** Analyze overall system performance
 * @param session Meta-cognitive session
 * @return Comprehensive system analysis */
GncSelfAnalysisResult* gnc_meta_cognitive_analyze_system(
    GncMetaCognitiveSession *session);

/** Get current performance metrics for a process
 * @param process_type Process type to query
 * @param metrics Output metrics structure
 * @return TRUE if metrics available, FALSE otherwise */
gboolean gnc_meta_cognitive_get_metrics(
    GncMetaCognitiveProcessType process_type,
    GncCognitiveMetrics *metrics);

/** Update performance metrics for a process
 * @param process_type Process type to update
 * @param metrics New metrics data
 * @return TRUE if update successful, FALSE otherwise */
gboolean gnc_meta_cognitive_update_metrics(
    GncMetaCognitiveProcessType process_type,
    const GncCognitiveMetrics *metrics);

/** Atomically apply an ontogenesis improvement to global metrics.
 *  Performs the read-modify-write under a single lock acquisition,
 *  avoiding the TOCTOU race that would occur with separate
 *  get_metrics / update_metrics calls.
 *  @param process_type  Process type whose metrics to update
 *  @param achieved_improvement  Fractional improvement (e.g. 0.05 = 5 %)
 *  @param stability_delta  Additive delta for stability index
 *  @param latency_delta_ms  Additive delta for latency in milliseconds
 *  @param kernels_accepted  Number of accepted kernels (for innovation score)
 *  @return TRUE on success */
gboolean gnc_meta_cognitive_apply_improvement(
    GncMetaCognitiveProcessType process_type,
    gdouble achieved_improvement,
    gdouble stability_delta,
    gdouble latency_delta_ms,
    guint   kernels_accepted);

/** Free self-analysis result structure
 * @param result Result to free */
void gnc_self_analysis_result_free(GncSelfAnalysisResult *result);

/** @} */

/** @name Evolutionary Optimization Functions */
/** @{ */

/** Evolve cognitive architecture using genetic algorithms
 * @param session Meta-cognitive session
 * @param params Evolutionary parameters
 * @param target_process Process to optimize
 * @return New optimized configuration, or NULL on failure */
GncCognitiveArchConfig* gnc_meta_cognitive_evolve_architecture(
    GncMetaCognitiveSession *session,
    const GncEvolutionaryParams *params,
    GncMetaCognitiveProcessType target_process);

/** Apply evolved cognitive architecture configuration
 * @param config Configuration to apply
 * @return TRUE if applied successfully, FALSE otherwise */
gboolean gnc_meta_cognitive_apply_config(const GncCognitiveArchConfig *config);

/** Rollback to previous stable configuration
 * @return TRUE if rollback successful, FALSE otherwise */
gboolean gnc_meta_cognitive_rollback_config(void);

/** Save current configuration as stable baseline
 * @return TRUE if saved successfully, FALSE otherwise */
gboolean gnc_meta_cognitive_save_stable_config(void);

/** @} */

/** @name Recursive Improvement Functions */
/** @{ */

/** Start recursive self-improvement cycle
 * @param session Meta-cognitive session
 * @param max_iterations Maximum improvement iterations
 * @param improvement_threshold Minimum improvement required to continue
 * @return TRUE if cycle started successfully, FALSE otherwise */
gboolean gnc_meta_cognitive_start_improvement_cycle(
    GncMetaCognitiveSession *session,
    guint max_iterations,
    gdouble improvement_threshold);

/** Stop any running self-improvement cycle
 * @param session Meta-cognitive session
 * @return TRUE if stopped successfully, FALSE otherwise */
gboolean gnc_meta_cognitive_stop_improvement_cycle(
    GncMetaCognitiveSession *session);

/** Check if self-improvement cycle is running
 * @param session Meta-cognitive session
 * @return TRUE if cycle is active, FALSE otherwise */
gboolean gnc_meta_cognitive_is_improving(GncMetaCognitiveSession *session);

/** Get current improvement cycle status
 * @param session Meta-cognitive session
 * @param iteration Current iteration number (output)
 * @param improvement Current improvement score (output)
 * @return TRUE if status available, FALSE otherwise */
gboolean gnc_meta_cognitive_get_improvement_status(
    GncMetaCognitiveSession *session,
    guint *iteration,
    gdouble *improvement);

/** @} */

/** @name Safety and Monitoring Functions */
/** @{ */

/** Enable/disable safety bounds for self-modification
 * @param enabled Whether safety bounds are enabled
 * @param min_performance Minimum acceptable performance
 * @param max_deviation Maximum allowed deviation from baseline */
void gnc_meta_cognitive_set_safety_bounds(
    gboolean enabled,
    gdouble min_performance,
    gdouble max_deviation);

/** Check if current system state is stable
 * @return TRUE if system is stable, FALSE if unstable */
gboolean gnc_meta_cognitive_is_stable(void);

/** Detect performance regression
 * @param process_type Process to check
 * @param regression_threshold Regression detection threshold
 * @return TRUE if regression detected, FALSE otherwise */
gboolean gnc_meta_cognitive_detect_regression(
    GncMetaCognitiveProcessType process_type,
    gdouble regression_threshold);

/** Enable human override mode (disable autonomous evolution)
 * @param enabled Whether human override is active */
void gnc_meta_cognitive_set_human_override(gboolean enabled);

/** @} */

/** @name Introspection and Pattern Analysis */
/** @{ */

/** Analyze cognitive behavior patterns
 * @param session Meta-cognitive session
 * @param lookback_hours Hours of history to analyze
 * @return Pattern analysis results */
gchar** gnc_meta_cognitive_analyze_patterns(
    GncMetaCognitiveSession *session,
    guint lookback_hours);

/** Detect emergence of new cognitive behaviors
 * @param session Meta-cognitive session
 * @param novelty_threshold Minimum novelty score for detection
 * @return Array of detected emergent behaviors */
gchar** gnc_meta_cognitive_detect_emergence(
    GncMetaCognitiveSession *session,
    gdouble novelty_threshold);

/** Generate cognitive fitness landscape visualization data
 * @param session Meta-cognitive session
 * @param process_type Process to analyze
 * @return JSON string with fitness landscape data */
gchar* gnc_meta_cognitive_generate_fitness_landscape(
    GncMetaCognitiveSession *session,
    GncMetaCognitiveProcessType process_type);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_META_COGNITIVE_H */

/** @} */
/** @} */