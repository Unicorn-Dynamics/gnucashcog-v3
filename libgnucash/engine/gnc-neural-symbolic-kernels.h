/********************************************************************\
 * gnc-neural-symbolic-kernels.h -- Custom ggml kernels for        *
 * neural-symbolic synthesis                                        *
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
/** @addtogroup NeuralSymbolicKernels
    Custom ggml kernels for neural-symbolic synthesis enabling seamless
    integration between symbolic reasoning (AtomSpace) and neural computation.
    Implements Phase 3 requirements for custom tensor operations.
    @{ */

/** @file gnc-neural-symbolic-kernels.h
    @brief Custom ggml kernels for neural-symbolic synthesis
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_NEURAL_SYMBOLIC_KERNELS_H
#define GNC_NEURAL_SYMBOLIC_KERNELS_H

#include "gnc-cognitive-accounting.h"
#include "gnc-tensor-network.h"

#ifdef HAVE_GGML
#include <ggml.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @name Symbolic Tensor Operations */
/** @{ */

/** Symbolic tensor operation types for neural-symbolic synthesis */
typedef enum {
    GNC_SYMBOLIC_OP_LOGICAL_AND,        /**< Logical AND for truth value combination */
    GNC_SYMBOLIC_OP_LOGICAL_OR,         /**< Logical OR for truth value combination */
    GNC_SYMBOLIC_OP_LOGICAL_NOT,        /**< Logical NOT for truth value negation */
    GNC_SYMBOLIC_OP_TRUTH_REVISION,     /**< PLN truth value revision */
    GNC_SYMBOLIC_OP_ATTENTION_FLOW,     /**< ECAN attention flow dynamics */
    GNC_SYMBOLIC_OP_PATTERN_MATCH,      /**< Pattern matching for cognitive primitives */
    GNC_SYMBOLIC_OP_SYMBOLIC_UNIFY,     /**< Symbolic unification operation */
    GNC_SYMBOLIC_OP_INFERENCE_STEP      /**< Single inference step in reasoning chain */
} GncSymbolicOpType;

/** Neural-symbolic kernel context for custom operations */
typedef struct {
    GncSymbolicOpType op_type;          /**< Operation type */
    gsize input_dims[4];                /**< Input tensor dimensions */
    gsize output_dims[4];               /**< Output tensor dimensions */
    gdouble symbolic_params[8];         /**< Symbolic operation parameters */
    gboolean preserve_logic;            /**< Whether to preserve logical consistency */
    gboolean gradient_free;             /**< Whether operation is gradient-free */
#ifdef HAVE_GGML
    struct ggml_context *ggml_ctx;      /**< GGML context for computation */
    struct ggml_tensor *symbol_embeddings; /**< Symbolic concept embeddings */
#endif
} GncNeuralSymbolicKernel;

/** @} */

/** @name Kernel Initialization and Management */
/** @{ */

/** Initialize neural-symbolic kernel system */
gboolean gnc_neural_symbolic_kernels_init(void);

/** Shutdown neural-symbolic kernel system */
void gnc_neural_symbolic_kernels_shutdown(void);

/** Create a new neural-symbolic kernel */
GncNeuralSymbolicKernel* gnc_neural_symbolic_kernel_create(GncSymbolicOpType op_type);

/** Destroy a neural-symbolic kernel */
void gnc_neural_symbolic_kernel_destroy(GncNeuralSymbolicKernel *kernel);

/** Register custom kernel with ggml */
gboolean gnc_neural_symbolic_kernel_register(GncNeuralSymbolicKernel *kernel);

/** @} */

/** @name Custom Symbolic Tensor Operations */
/** @{ */

/** Apply logical AND operation to symbolic tensors */
gboolean gnc_symbolic_tensor_logical_and(GncTensorData *input_a, 
                                        GncTensorData *input_b,
                                        GncTensorData *output);

/** Apply logical OR operation to symbolic tensors */
gboolean gnc_symbolic_tensor_logical_or(GncTensorData *input_a,
                                       GncTensorData *input_b, 
                                       GncTensorData *output);

/** Apply PLN truth value revision to symbolic tensors */
gboolean gnc_symbolic_tensor_truth_revision(GncTensorData *prior_tensor,
                                           GncTensorData *evidence_tensor,
                                           GncTensorData *revised_tensor);

/** Apply ECAN attention flow dynamics to tensor network */
gboolean gnc_symbolic_tensor_attention_flow(GncTensorData *attention_state,
                                           GncTensorData *flow_matrix,
                                           GncTensorData *updated_attention);

/** Pattern matching operation for cognitive primitives */
gboolean gnc_symbolic_tensor_pattern_match(GncTensorData *pattern_tensor,
                                          GncTensorData *data_tensor,
                                          GncTensorData *match_scores);

/** Symbolic unification for reasoning chains */
gboolean gnc_symbolic_tensor_unify(GncTensorData *concept_a,
                                  GncTensorData *concept_b,
                                  GncTensorData *unification_result);

/** @} */

/** @name Neural-Symbolic Inference Engine */
/** @{ */

/** Neural-symbolic inference context */
typedef struct {
    GHashTable *symbol_table;           /**< Mapping from symbols to tensor indices */
    GncTensorData *inference_state;     /**< Current inference state tensor */
    GQueue *reasoning_chain;            /**< Chain of reasoning operations */
    gdouble logical_consistency;        /**< Logical consistency score */
    gboolean neural_mode;               /**< Neural computation mode active */
    gboolean symbolic_mode;             /**< Symbolic reasoning mode active */
    gint64 inference_timestamp;         /**< Last inference timestamp */
} GncNeuralSymbolicInference;

/** Create neural-symbolic inference engine */
GncNeuralSymbolicInference* gnc_neural_symbolic_inference_create(void);

/** Destroy neural-symbolic inference engine */
void gnc_neural_symbolic_inference_destroy(GncNeuralSymbolicInference *inference);

/** Execute single inference step */
gboolean gnc_neural_symbolic_inference_step(GncNeuralSymbolicInference *inference,
                                           GncTensorData *input,
                                           GncTensorData *output);

/** Execute complete inference chain */
gboolean gnc_neural_symbolic_inference_chain(GncNeuralSymbolicInference *inference,
                                            GList *input_tensors,
                                            GList **output_tensors);

/** Switch between neural and symbolic modes */
gboolean gnc_neural_symbolic_inference_switch_mode(GncNeuralSymbolicInference *inference,
                                                  gboolean neural_mode);

/** @} */

/** @name AtomSpace Integration Hooks */
/** @{ */

/** Convert AtomSpace atoms to neural tensor representation */
gboolean gnc_atomspace_to_neural_tensor(GncAtomHandle atom_handle,
                                       GncTensorData *tensor_output);

/** Convert neural tensor back to AtomSpace representation */
gboolean gnc_neural_tensor_to_atomspace(GncTensorData *tensor_input,
                                       GncAtomHandle *atom_handle);

/** Execute neural computation on AtomSpace subgraph */
gboolean gnc_atomspace_neural_compute(GList *atom_handles,
                                     GncNeuralSymbolicInference *inference,
                                     GList **result_atoms);

/** Validate logical consistency between neural and symbolic representations */
gboolean gnc_neural_symbolic_consistency_check(GncTensorData *neural_state,
                                              GList *symbolic_atoms,
                                              gdouble *consistency_score);

/** @} */

/** @name Cognitive Primitive Operations */
/** @{ */

/** Custom operation for concept formation */
gboolean gnc_cognitive_primitive_concept_formation(GncTensorData *feature_tensor,
                                                  GncTensorData *concept_tensor);

/** Custom operation for category learning */
gboolean gnc_cognitive_primitive_category_learning(GncTensorData *examples_tensor,
                                                  GncTensorData *category_tensor);

/** Custom operation for analogical reasoning */
gboolean gnc_cognitive_primitive_analogical_reasoning(GncTensorData *source_tensor,
                                                     GncTensorData *target_tensor,
                                                     GncTensorData *analogy_tensor);

/** Custom operation for causal reasoning */
gboolean gnc_cognitive_primitive_causal_reasoning(GncTensorData *events_tensor,
                                                 GncTensorData *causal_tensor);

/** @} */

/** @name Performance and Benchmarking */
/** @{ */

/** Kernel performance metrics */
typedef struct {
    gdouble computation_time_ms;        /**< Computation time in milliseconds */
    gsize memory_usage_bytes;           /**< Memory usage in bytes */
    gdouble logical_accuracy;           /**< Logical reasoning accuracy */
    gdouble neural_convergence;         /**< Neural computation convergence */
    gint64 operations_count;            /**< Number of operations performed */
    gdouble throughput_ops_per_sec;     /**< Operations per second throughput */
} GncKernelPerformanceMetrics;

/** Benchmark neural-symbolic kernel performance */
gboolean gnc_neural_symbolic_kernel_benchmark(GncNeuralSymbolicKernel *kernel,
                                             GncTensorData *test_data,
                                             GncKernelPerformanceMetrics *metrics);

/** Profile tensor operation memory usage */
gboolean gnc_tensor_operation_memory_profile(GncSymbolicOpType op_type,
                                            gsize input_size,
                                            gsize *peak_memory,
                                            gsize *steady_memory);

/** Validate tensor signature consistency */
gboolean gnc_tensor_signature_validate(GncTensorData *tensor,
                                      const gchar *expected_signature,
                                      gboolean *signature_valid);

/** @} */

/** @name Gradient-Free Symbolic Reasoning */
/** @{ */

/** Symbolic reasoning context without gradients */
typedef struct {
    GHashTable *rule_base;              /**< Logical rules for reasoning */
    GQueue *inference_queue;            /**< Queue of pending inferences */
    gdouble confidence_threshold;       /**< Minimum confidence for conclusions */
    gboolean monotonic_reasoning;       /**< Whether reasoning is monotonic */
} GncGradientFreeReasoning;

/** Create gradient-free symbolic reasoning engine */
GncGradientFreeReasoning* gnc_gradient_free_reasoning_create(void);

/** Destroy gradient-free symbolic reasoning engine */
void gnc_gradient_free_reasoning_destroy(GncGradientFreeReasoning *reasoning);

/** Apply symbolic reasoning rule without gradients */
gboolean gnc_gradient_free_apply_rule(GncGradientFreeReasoning *reasoning,
                                     const gchar *rule_name,
                                     GncTensorData *premises,
                                     GncTensorData *conclusion);

/** Forward chaining inference without gradients */
gboolean gnc_gradient_free_forward_chain(GncGradientFreeReasoning *reasoning,
                                        GncTensorData *facts,
                                        GncTensorData *derived_facts);

/** Backward chaining inference without gradients */
gboolean gnc_gradient_free_backward_chain(GncGradientFreeReasoning *reasoning,
                                         GncTensorData *goal,
                                         GncTensorData *required_facts);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_NEURAL_SYMBOLIC_KERNELS_H */

/** @} */
/** @} */