/********************************************************************\
 * gnc-cognitive-primitives.h -- Foundational Cognitive Primitives *
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

/**
 * @file gnc-cognitive-primitives.h
 * @brief Phase 1: Cognitive Primitives & Foundational Hypergraph Encoding
 * @author GnuCash Cognitive Engine
 * 
 * Foundational cognitive primitives that form the neural-symbolic building 
 * blocks for the entire distributed agentic cognitive grammar network.
 * These primitives enable the transformation of classical ledgers into
 * cognitive neural-symbolic tapestries.
 * 
 * Key Features:
 * - Fundamental cognitive data structures
 * - Hypergraph pattern encoding primitives
 * - Neural-symbolic bridge foundations
 * - Distributed agentic coordination primitives
 * - Financial grammar network elements
 * - Emergent behavior foundation patterns
 */

#ifndef GNC_COGNITIVE_PRIMITIVES_H
#define GNC_COGNITIVE_PRIMITIVES_H

#include <glib.h>
#include "qof.h"
#include "Account.h"
#include "Transaction.h"
#include "gnc-cognitive-accounting.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup CognitivePrimitives
 * @{
 */

/**
 * @name Fundamental Cognitive Data Types
 * Core primitive types for cognitive processing
 */

/** Primitive cognitive state representation */
typedef guint64 GncCognitivePrimitive;

/** Hypergraph node identifier for financial grammar */
typedef guint64 GncHypergraphNode;

/** Hypergraph edge connecting multiple nodes */
typedef struct {
    GncHypergraphNode *nodes;      /**< Array of connected nodes */
    guint node_count;              /**< Number of nodes in hyperedge */
    gdouble weight;                /**< Connection strength */
    gchar *relation_type;          /**< Type of relationship */
} GncHypergraphEdge;

/** Basic cognitive unit for financial reasoning */
typedef struct {
    GncCognitivePrimitive id;      /**< Unique primitive identifier */
    gchar *name;                   /**< Human-readable name */
    gchar *type;                   /**< Primitive type classification */
    gdouble activation_level;      /**< Current activation strength */
    gdouble confidence;            /**< Confidence in primitive state */
    GHashTable *properties;        /**< Additional properties */
    gint64 last_updated;           /**< Timestamp of last update */
} GncCognitivePrimitiveUnit;

/** Cognitive grammar pattern for financial semantics */
typedef struct {
    gchar *pattern_name;           /**< Pattern identifier */
    gchar *grammar_rule;           /**< Formal grammar rule */
    GncCognitivePrimitive *components; /**< Component primitives */
    guint component_count;         /**< Number of components */
    gdouble pattern_strength;      /**< Pattern recognition strength */
    gdouble semantic_coherence;    /**< Semantic consistency measure */
} GncCognitiveGrammarPattern;

/** Distributed agent primitive for coordination */
typedef struct {
    GncCognitivePrimitive agent_id; /**< Agent identifier */
    gchar *agent_type;             /**< Type of cognitive agent */
    gdouble coordination_weight;   /**< Weight in distributed decisions */
    GList *active_patterns;        /**< Currently active patterns */
    GList *neighbor_agents;        /**< Connected agents */
    gdouble autonomy_level;        /**< Level of independent action */
} GncDistributedAgentPrimitive;

/**
 * @name Hypergraph Encoding System
 * Foundational hypergraph structures for financial grammar
 */

/** Hypergraph container for financial network representation */
typedef struct {
    GHashTable *nodes;             /**< Node ID -> node data mapping */
    GList *edges;                  /**< List of hypergraph edges */
    gchar *graph_type;             /**< Type of hypergraph */
    gdouble coherence_measure;     /**< Overall graph coherence */
    guint max_nodes;               /**< Maximum node capacity */
    guint max_edges;               /**< Maximum edge capacity */
} GncFinancialHypergraph;

/** Financial grammar rule encoded as hypergraph pattern */
typedef struct {
    gchar *rule_name;              /**< Grammar rule name */
    GncFinancialHypergraph *pattern; /**< Hypergraph pattern encoding */
    gdouble rule_strength;         /**< Rule application strength */
    GList *preconditions;          /**< Required conditions */
    GList *postconditions;         /**< Result conditions */
    gchar *semantic_class;         /**< Semantic classification */
} GncFinancialGrammarRule;

/**
 * @name Neural-Symbolic Bridge Primitives
 * Foundational structures for connecting symbolic and neural processing
 */

/** Neural-symbolic embedding for primitive concepts */
typedef struct {
    GncCognitivePrimitive concept_id; /**< Associated primitive */
    gdouble *embedding_vector;     /**< Neural embedding vector */
    guint vector_dimension;        /**< Embedding dimensionality */
    gchar *symbolic_representation; /**< Symbolic form */
    gdouble embedding_confidence;  /**< Confidence in embedding */
} GncNeuralSymbolicEmbedding;

/** Bridge operation for neural-symbolic transformation */
typedef enum {
    GNC_NS_OP_SYMBOLIZE,          /**< Neural -> Symbolic */
    GNC_NS_OP_NEURALIZE,          /**< Symbolic -> Neural */
    GNC_NS_OP_BIDIRECTIONAL,     /**< Bidirectional transformation */
    GNC_NS_OP_FUSION             /**< Fusion of both representations */
} GncNeuralSymbolicOperation;

/** Transformation between neural and symbolic representations */
typedef struct {
    GncNeuralSymbolicOperation operation; /**< Type of transformation */
    GncNeuralSymbolicEmbedding *input;    /**< Input representation */
    GncNeuralSymbolicEmbedding *output;   /**< Output representation */
    gdouble transformation_accuracy;      /**< Accuracy of transformation */
    gint64 computation_cost;              /**< Computational cost measure */
} GncNeuralSymbolicBridge;

/**
 * @name Core Primitive Operations
 * Fundamental operations on cognitive primitives
 */

/** Initialize the cognitive primitives system */
gboolean gnc_cognitive_primitives_init(void);

/** Shutdown the cognitive primitives system */
void gnc_cognitive_primitives_shutdown(void);

/** Create a new cognitive primitive unit */
GncCognitivePrimitiveUnit* gnc_cognitive_primitive_create(
    const gchar *name,
    const gchar *type,
    gdouble initial_activation);

/** Free a cognitive primitive unit */
void gnc_cognitive_primitive_free(GncCognitivePrimitiveUnit *primitive);

/** Update primitive activation based on stimuli */
void gnc_cognitive_primitive_update_activation(
    GncCognitivePrimitiveUnit *primitive,
    gdouble stimulus_strength,
    gdouble decay_factor);

/** Get primitive property value */
const gchar* gnc_cognitive_primitive_get_property(
    GncCognitivePrimitiveUnit *primitive,
    const gchar *property_name);

/** Set primitive property value */
void gnc_cognitive_primitive_set_property(
    GncCognitivePrimitiveUnit *primitive,
    const gchar *property_name,
    const gchar *property_value);

/**
 * @name Hypergraph Operations
 * Core operations on financial hypergraph structures
 */

/** Create a new financial hypergraph */
GncFinancialHypergraph* gnc_financial_hypergraph_create(
    const gchar *graph_type,
    guint max_nodes,
    guint max_edges);

/** Free a financial hypergraph */
void gnc_financial_hypergraph_free(GncFinancialHypergraph *hypergraph);

/** Add a node to the hypergraph */
GncHypergraphNode gnc_financial_hypergraph_add_node(
    GncFinancialHypergraph *hypergraph,
    const gchar *node_type,
    gpointer node_data);

/** Create a hyperedge connecting multiple nodes */
GncHypergraphEdge* gnc_financial_hypergraph_create_edge(
    GncFinancialHypergraph *hypergraph,
    GncHypergraphNode *nodes,
    guint node_count,
    const gchar *relation_type,
    gdouble weight);

/** Encode an account as hypergraph pattern */
GncFinancialHypergraph* gnc_account_to_hypergraph_pattern(
    Account *account,
    gboolean include_children);

/** Encode a transaction as hypergraph pattern */
GncFinancialHypergraph* gnc_transaction_to_hypergraph_pattern(
    Transaction *transaction,
    gboolean include_splits);

/** Calculate hypergraph coherence measure */
gdouble gnc_financial_hypergraph_coherence(
    GncFinancialHypergraph *hypergraph);

/**
 * @name Grammar Pattern Operations
 * Operations on cognitive grammar patterns
 */

/** Create a new cognitive grammar pattern */
GncCognitiveGrammarPattern* gnc_cognitive_grammar_pattern_create(
    const gchar *pattern_name,
    const gchar *grammar_rule);

/** Free a cognitive grammar pattern */
void gnc_cognitive_grammar_pattern_free(GncCognitiveGrammarPattern *pattern);

/** Add component primitive to pattern */
void gnc_cognitive_grammar_pattern_add_component(
    GncCognitiveGrammarPattern *pattern,
    GncCognitivePrimitive component);

/** Evaluate pattern match against financial data */
gdouble gnc_cognitive_grammar_pattern_match(
    GncCognitiveGrammarPattern *pattern,
    gpointer financial_data,
    const gchar *data_type);

/** Generate financial grammar rules from account structure */
GList* gnc_generate_financial_grammar_rules(
    Account *root_account,
    guint max_depth);

/**
 * @name Distributed Agent Operations
 * Operations on distributed cognitive agents
 */

/** Create a new distributed agent primitive */
GncDistributedAgentPrimitive* gnc_distributed_agent_create(
    const gchar *agent_type,
    gdouble coordination_weight);

/** Free a distributed agent primitive */
void gnc_distributed_agent_free(GncDistributedAgentPrimitive *agent);

/** Connect agents for distributed coordination */
void gnc_distributed_agents_connect(
    GncDistributedAgentPrimitive *agent1,
    GncDistributedAgentPrimitive *agent2,
    gdouble connection_strength);

/** Coordinate distributed decision making */
gdouble gnc_distributed_agents_coordinate_decision(
    GList *agents,
    gpointer decision_context,
    const gchar *decision_type);

/** Propagate activation through agent network */
void gnc_distributed_agents_propagate_activation(
    GncDistributedAgentPrimitive *source_agent,
    gdouble activation_strength,
    guint max_hops);

/**
 * @name Neural-Symbolic Bridge Operations
 * Operations for neural-symbolic transformations
 */

/** Create neural-symbolic embedding for primitive */
GncNeuralSymbolicEmbedding* gnc_neural_symbolic_embedding_create(
    GncCognitivePrimitive concept_id,
    guint vector_dimension);

/** Free neural-symbolic embedding */
void gnc_neural_symbolic_embedding_free(GncNeuralSymbolicEmbedding *embedding);

/** Update embedding based on usage patterns */
void gnc_neural_symbolic_embedding_update(
    GncNeuralSymbolicEmbedding *embedding,
    gdouble *context_vector,
    gdouble learning_rate);

/** Create neural-symbolic bridge transformation */
GncNeuralSymbolicBridge* gnc_neural_symbolic_bridge_create(
    GncNeuralSymbolicOperation operation,
    GncNeuralSymbolicEmbedding *input,
    guint output_dimension);

/** Execute neural-symbolic transformation */
gboolean gnc_neural_symbolic_bridge_execute(
    GncNeuralSymbolicBridge *bridge);

/** Free neural-symbolic bridge */
void gnc_neural_symbolic_bridge_free(GncNeuralSymbolicBridge *bridge);

/**
 * @name Integration with Financial Data
 * Functions to integrate primitives with GnuCash financial structures
 */

/** Convert Account to cognitive primitives */
GList* gnc_account_to_cognitive_primitives(
    Account *account,
    gboolean recursive);

/** Convert Transaction to cognitive primitives */
GList* gnc_transaction_to_cognitive_primitives(
    Transaction *transaction);

/** Extract cognitive patterns from financial data */
GList* gnc_extract_cognitive_patterns_from_book(
    QofBook *book,
    guint pattern_complexity_threshold);

/** Validate cognitive primitive consistency */
gboolean gnc_cognitive_primitives_validate_consistency(
    GList *primitives,
    gdouble *consistency_score);

/**
 * @name Emergent Behavior Foundation
 * Foundational structures for emergent cognitive behaviors
 */

/** Emergence detection parameters */
typedef struct {
    gdouble complexity_threshold;    /**< Minimum complexity for emergence */
    gdouble novelty_threshold;       /**< Minimum novelty for emergence */
    gdouble coherence_threshold;     /**< Minimum coherence for valid emergence */
    guint observation_window;        /**< Time window for emergence detection */
    guint min_component_count;       /**< Minimum components for emergence */
} GncEmergenceDetectionParams;

/** Detected emergent pattern */
typedef struct {
    gchar *pattern_name;            /**< Name of emergent pattern */
    GList *component_primitives;    /**< Constituent primitives */
    gdouble emergence_strength;     /**< Strength of emergent behavior */
    gdouble pattern_complexity;     /**< Complexity measure */
    gdouble pattern_novelty;        /**< Novelty measure */
    gint64 first_observed;          /**< First observation timestamp */
    guint observation_count;        /**< Number of observations */
} GncEmergentPattern;

/** Detect emergent patterns in primitive interactions */
GList* gnc_detect_emergent_patterns(
    GList *primitives,
    GncEmergenceDetectionParams *params);

/** Evaluate emergent pattern stability */
gdouble gnc_emergent_pattern_stability(
    GncEmergentPattern *pattern,
    GList *current_primitives);

/** Free emergent pattern structure */
void gnc_emergent_pattern_free(GncEmergentPattern *pattern);

/**
 * @name System Integration
 * Integration points with other cognitive system components
 */

/** Initialize cognitive primitive integration with AtomSpace */
gboolean gnc_cognitive_primitives_integrate_atomspace(void);

/** Initialize cognitive primitive integration with tensor network */
gboolean gnc_cognitive_primitives_integrate_tensor_network(void);

/** Initialize cognitive primitive integration with ECAN */
gboolean gnc_cognitive_primitives_integrate_ecan(void);

/** Get system-wide primitive statistics */
typedef struct {
    guint total_primitives;         /**< Total number of primitives */
    guint active_primitives;        /**< Currently active primitives */
    gdouble average_activation;     /**< Average activation level */
    gdouble system_coherence;       /**< Overall system coherence */
    guint hypergraph_nodes;         /**< Total hypergraph nodes */
    guint hypergraph_edges;         /**< Total hypergraph edges */
    guint grammar_patterns;         /**< Number of grammar patterns */
    guint distributed_agents;       /**< Number of distributed agents */
} GncCognitivePrimitiveStats;

/** Get comprehensive primitive system statistics */
GncCognitivePrimitiveStats* gnc_cognitive_primitives_get_stats(void);

/** Free primitive statistics structure */
void gnc_cognitive_primitive_stats_free(GncCognitivePrimitiveStats *stats);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_COGNITIVE_PRIMITIVES_H */