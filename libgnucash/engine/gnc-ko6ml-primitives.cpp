/********************************************************************\
 * gnc-ko6ml-primitives.cpp -- ko6ml Cognitive Primitives         *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * This program implements the atomic vocabulary and bidirectional  *
 * translation mechanisms between ko6ml primitives and AtomSpace   *
 * hypergraph patterns for Phase 1 cognitive foundation.           *
 ********************************************************************/

#include "gnc-ko6ml-primitives.h"
#include "gnc-cognitive-accounting.h"
#include "gnc-cognitive-scheme.h"
#include "Account.h"
#include "Transaction.h"
#include "qof.h"
#include <glib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef HAVE_OPENCOG_ATOMSPACE
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
using namespace opencog;
#endif

/********************************************************************\
 * Static Variables and Initialization                             *
\********************************************************************/

static gboolean ko6ml_initialized = FALSE;
static GHashTable *ko6ml_primitive_registry = NULL;
static GHashTable *atomspace_ko6ml_mapping = NULL;

/** Prime numbers for factorization mapping */
static const guint PRIME_FACTORS[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
static const gsize N_PRIMES = sizeof(PRIME_FACTORS) / sizeof(PRIME_FACTORS[0]);

/********************************************************************\
 * Utility Functions                                               *
\********************************************************************/

const gchar* gnc_ko6ml_primitive_type_to_string(Ko6mlPrimitiveType type)
{
    switch (type) {
        case KO6ML_CONCEPT: return "concept";
        case KO6ML_RELATION: return "relation";
        case KO6ML_AGENT: return "agent";
        case KO6ML_STATE: return "state";
        case KO6ML_PROCESS: return "process";
        case KO6ML_CONTEXT: return "context";
        case KO6ML_MODALITY: return "modality";
        case KO6ML_TEMPORAL: return "temporal";
        case KO6ML_SPATIAL: return "spatial";
        case KO6ML_COMPOSITE: return "composite";
        default: return "unknown";
    }
}

Ko6mlPrimitiveType gnc_ko6ml_primitive_type_from_string(const gchar *type_str)
{
    if (!type_str) return (Ko6mlPrimitiveType)-1;
    
    if (g_strcmp0(type_str, "concept") == 0) return KO6ML_CONCEPT;
    if (g_strcmp0(type_str, "relation") == 0) return KO6ML_RELATION;
    if (g_strcmp0(type_str, "agent") == 0) return KO6ML_AGENT;
    if (g_strcmp0(type_str, "state") == 0) return KO6ML_STATE;
    if (g_strcmp0(type_str, "process") == 0) return KO6ML_PROCESS;
    if (g_strcmp0(type_str, "context") == 0) return KO6ML_CONTEXT;
    if (g_strcmp0(type_str, "modality") == 0) return KO6ML_MODALITY;
    if (g_strcmp0(type_str, "temporal") == 0) return KO6ML_TEMPORAL;
    if (g_strcmp0(type_str, "spatial") == 0) return KO6ML_SPATIAL;
    if (g_strcmp0(type_str, "composite") == 0) return KO6ML_COMPOSITE;
    
    return (Ko6mlPrimitiveType)-1;
}

/********************************************************************\
 * Core ko6ml Functions                                            *
\********************************************************************/

gboolean gnc_ko6ml_init(void)
{
    if (ko6ml_initialized) {
        return TRUE;
    }
    
    /* Initialize primitive registry */
    ko6ml_primitive_registry = g_hash_table_new_full(g_str_hash, g_str_equal, 
                                                     g_free, NULL);
    
    /* Initialize AtomSpace ↔ ko6ml mapping */
    atomspace_ko6ml_mapping = g_hash_table_new_full(g_direct_hash, g_direct_equal, 
                                                    NULL, NULL);
    
    /* Ensure cognitive accounting is initialized */
    if (!gnc_cognitive_accounting_init()) {
        g_warning("Failed to initialize cognitive accounting for ko6ml");
        return FALSE;
    }
    
    /* Initialize Scheme integration */
    if (!gnc_cognitive_scheme_init()) {
        g_warning("Failed to initialize Scheme integration for ko6ml");
        return FALSE;
    }
    
    ko6ml_initialized = TRUE;
    g_message("ko6ml primitive system initialized successfully");
    
    return TRUE;
}

void gnc_ko6ml_shutdown(void)
{
    if (!ko6ml_initialized) {
        return;
    }
    
    if (ko6ml_primitive_registry) {
        g_hash_table_destroy(ko6ml_primitive_registry);
        ko6ml_primitive_registry = NULL;
    }
    
    if (atomspace_ko6ml_mapping) {
        g_hash_table_destroy(atomspace_ko6ml_mapping);
        atomspace_ko6ml_mapping = NULL;
    }
    
    ko6ml_initialized = FALSE;
    g_message("ko6ml primitive system shut down");
}

Ko6mlPrimitive* gnc_ko6ml_primitive_new(Ko6mlPrimitiveType type, 
                                        const gchar *name, 
                                        const gchar *description)
{
    g_return_val_if_fail(name != NULL, NULL);
    g_return_val_if_fail(ko6ml_initialized, NULL);
    
    Ko6mlPrimitive *primitive = g_new0(Ko6mlPrimitive, 1);
    
    primitive->type = type;
    primitive->name = g_strdup(name);
    primitive->description = g_strdup(description ? description : "");
    primitive->properties = g_hash_table_new_full(g_str_hash, g_str_equal, 
                                                  g_free, g_free);
    primitive->atom_handle = 0;  /* Will be set during translation */
    primitive->salience = 0.5;   /* Default salience */
    primitive->autonomy_index = 0.5;  /* Default autonomy */
    
    /* Register primitive */
    g_hash_table_insert(ko6ml_primitive_registry, g_strdup(name), primitive);
    
    g_debug("Created ko6ml primitive: %s (type: %s)", name, 
            gnc_ko6ml_primitive_type_to_string(type));
    
    return primitive;
}

void gnc_ko6ml_primitive_free(Ko6mlPrimitive *primitive)
{
    if (!primitive) return;
    
    /* Remove from registry if present */
    if (primitive->name && ko6ml_primitive_registry) {
        g_hash_table_remove(ko6ml_primitive_registry, primitive->name);
    }
    
    g_free(primitive->name);
    g_free(primitive->description);
    
    if (primitive->properties) {
        g_hash_table_destroy(primitive->properties);
    }
    
    g_free(primitive);
}

void gnc_ko6ml_primitive_set_property(Ko6mlPrimitive *primitive, 
                                      const gchar *key, 
                                      const gchar *value)
{
    g_return_if_fail(primitive != NULL);
    g_return_if_fail(key != NULL);
    g_return_if_fail(value != NULL);
    
    g_hash_table_insert(primitive->properties, g_strdup(key), g_strdup(value));
}

const gchar* gnc_ko6ml_primitive_get_property(Ko6mlPrimitive *primitive, 
                                              const gchar *key)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(key != NULL, NULL);
    
    return (const gchar*)g_hash_table_lookup(primitive->properties, key);
}

/********************************************************************\
 * ko6ml ↔ AtomSpace Translation                                   *
\********************************************************************/

Ko6mlTranslationResult* gnc_ko6ml_to_atomspace(Ko6mlPrimitive *primitive)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(ko6ml_initialized, NULL);
    
    Ko6mlTranslationResult *result = g_new0(Ko6mlTranslationResult, 1);
    result->success = FALSE;
    result->error_message = NULL;
    
    /* Create AtomSpace representation based on primitive type */
    switch (primitive->type) {
        case KO6ML_CONCEPT: {
            /* Create ConceptNode */
            result->atom_handle = gnc_atomspace_create_concept_node(primitive->name);
            break;
        }
        case KO6ML_RELATION: {
            /* Create PredicateNode */
            result->atom_handle = gnc_atomspace_create_predicate_node(primitive->name);
            break;
        }
        case KO6ML_AGENT:
        case KO6ML_STATE: {
            /* Create ConceptNode with specific properties */
            result->atom_handle = gnc_atomspace_create_concept_node(primitive->name);
            
            /* Add type-specific properties */
            const gchar *type_str = gnc_ko6ml_primitive_type_to_string(primitive->type);
            GncAtomHandle type_node = gnc_atomspace_create_concept_node(type_str);
            gnc_atomspace_create_inheritance_link(result->atom_handle, type_node);
            break;
        }
        case KO6ML_PROCESS:
        case KO6ML_TEMPORAL: {
            /* Create procedural representation */
            result->atom_handle = gnc_atomspace_create_concept_node(primitive->name);
            
            /* Add temporal/process properties */
            GncAtomHandle process_node = gnc_atomspace_create_concept_node("Process");
            gnc_atomspace_create_inheritance_link(result->atom_handle, process_node);
            break;
        }
        default: {
            /* Generic ConceptNode for other types */
            result->atom_handle = gnc_atomspace_create_concept_node(primitive->name);
            break;
        }
    }
    
    if (result->atom_handle != 0) {
        /* Set truth values based on salience and autonomy */
        gnc_atomspace_set_truth_value(result->atom_handle, 
                                     primitive->salience, 
                                     primitive->autonomy_index);
        
        /* Create tensor shape */
        result->tensor_shape = gnc_ko6ml_create_tensor_shape(
            primitive->name, primitive, 
            (gsize)(primitive->type + 1),  /* Modality based on type */
            3,  /* Default depth */
            5   /* Default context */
        );
        result->tensor_shape.salience = primitive->salience;
        result->tensor_shape.autonomy_index = primitive->autonomy_index;
        
        /* Store mapping for reverse translation */
        g_hash_table_insert(atomspace_ko6ml_mapping, 
                           GUINT_TO_POINTER(result->atom_handle), primitive);
        
        primitive->atom_handle = result->atom_handle;
        result->success = TRUE;
        
        g_debug("Translated ko6ml primitive '%s' to AtomSpace handle %u", 
                primitive->name, result->atom_handle);
    } else {
        result->error_message = g_strdup("Failed to create AtomSpace representation");
        g_warning("Failed to translate ko6ml primitive '%s' to AtomSpace", 
                  primitive->name);
    }
    
    return result;
}

Ko6mlPrimitive* gnc_ko6ml_from_atomspace(GncAtomHandle atom_handle)
{
    g_return_val_if_fail(atom_handle != 0, NULL);
    g_return_val_if_fail(ko6ml_initialized, NULL);
    
    /* Try to find existing mapping first */
    Ko6mlPrimitive *existing = (Ko6mlPrimitive*)g_hash_table_lookup(
        atomspace_ko6ml_mapping, GUINT_TO_POINTER(atom_handle));
    
    if (existing) {
        /* Return copy of existing primitive */
        Ko6mlPrimitive *copy = gnc_ko6ml_primitive_new(existing->type, 
                                                       existing->name, 
                                                       existing->description);
        copy->salience = existing->salience;
        copy->autonomy_index = existing->autonomy_index;
        copy->atom_handle = existing->atom_handle;
        
        /* Copy properties */
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, existing->properties);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            gnc_ko6ml_primitive_set_property(copy, (const gchar*)key, (const gchar*)value);
        }
        
        return copy;
    }
    
    /* Create new primitive from AtomSpace data */
    /* This would require more sophisticated reverse engineering */
    /* For now, create a basic primitive */
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_CONCEPT, 
                                                        "reverse_engineered", 
                                                        "Primitive reverse-engineered from AtomSpace");
    primitive->atom_handle = atom_handle;
    
    return primitive;
}

gboolean gnc_ko6ml_round_trip_test(Ko6mlPrimitive *original)
{
    g_return_val_if_fail(original != NULL, FALSE);
    
    /* Translate to AtomSpace */
    Ko6mlTranslationResult *to_atomspace = gnc_ko6ml_to_atomspace(original);
    if (!to_atomspace || !to_atomspace->success) {
        g_debug("Round-trip test failed: ko6ml → AtomSpace translation failed");
        if (to_atomspace) gnc_ko6ml_translation_result_free(to_atomspace);
        return FALSE;
    }
    
    /* Translate back to ko6ml */
    Ko6mlPrimitive *back_to_ko6ml = gnc_ko6ml_from_atomspace(to_atomspace->atom_handle);
    if (!back_to_ko6ml) {
        g_debug("Round-trip test failed: AtomSpace → ko6ml translation failed");
        gnc_ko6ml_translation_result_free(to_atomspace);
        return FALSE;
    }
    
    /* Compare original and round-trip result */
    gboolean round_trip_success = TRUE;
    
    if (original->type != back_to_ko6ml->type) {
        g_debug("Round-trip test failed: type mismatch (%d vs %d)", 
                original->type, back_to_ko6ml->type);
        round_trip_success = FALSE;
    }
    
    if (g_strcmp0(original->name, back_to_ko6ml->name) != 0) {
        g_debug("Round-trip test failed: name mismatch ('%s' vs '%s')", 
                original->name, back_to_ko6ml->name);
        round_trip_success = FALSE;
    }
    
    /* Check salience and autonomy within tolerance */
    if (fabs(original->salience - back_to_ko6ml->salience) > 0.01) {
        g_debug("Round-trip test failed: salience mismatch (%.3f vs %.3f)", 
                original->salience, back_to_ko6ml->salience);
        round_trip_success = FALSE;
    }
    
    /* Cleanup */
    gnc_ko6ml_primitive_free(back_to_ko6ml);
    gnc_ko6ml_translation_result_free(to_atomspace);
    
    if (round_trip_success) {
        g_debug("Round-trip test passed for primitive '%s'", original->name);
    }
    
    return round_trip_success;
}

gdouble gnc_ko6ml_validate_translation_accuracy(Ko6mlPrimitive *primitive, 
                                                Ko6mlTranslationResult *result)
{
    g_return_val_if_fail(primitive != NULL, 0.0);
    g_return_val_if_fail(result != NULL, 0.0);
    
    if (!result->success) {
        return 0.0;
    }
    
    gdouble accuracy = 1.0;
    
    /* Validate tensor shape */
    if (!gnc_ko6ml_validate_tensor_shape(&result->tensor_shape)) {
        accuracy -= 0.2;
    }
    
    /* Validate salience preservation */
    if (fabs(primitive->salience - result->tensor_shape.salience) > 0.05) {
        accuracy -= 0.1;
    }
    
    /* Validate autonomy preservation */
    if (fabs(primitive->autonomy_index - result->tensor_shape.autonomy_index) > 0.05) {
        accuracy -= 0.1;
    }
    
    /* Validate AtomSpace handle */
    if (result->atom_handle == 0) {
        accuracy -= 0.3;
    }
    
    return MAX(accuracy, 0.0);
}

/********************************************************************\
 * Tensor Fragment Architecture                                    *
\********************************************************************/

Ko6mlTensorShape gnc_ko6ml_create_tensor_shape(const gchar *agent_id,
                                               gpointer state_data,
                                               gsize modality,
                                               gsize depth, 
                                               gsize context)
{
    Ko6mlTensorShape shape = {0};
    
    shape.modality = modality;
    shape.depth = depth;
    shape.context = context;
    shape.salience = 0.5;      /* Default salience */
    shape.autonomy_index = 0.5; /* Default autonomy */
    
    /* Adjust based on agent_id characteristics */
    if (agent_id) {
        gsize name_len = strlen(agent_id);
        shape.modality = MAX(1, name_len % 10);  /* Modality 1-10 */
        shape.depth = MAX(1, (name_len / 2) % 8); /* Depth 1-8 */
        shape.context = MAX(1, name_len % 12);   /* Context 1-12 */
    }
    
    return shape;
}

gboolean gnc_ko6ml_validate_tensor_shape(Ko6mlTensorShape *shape)
{
    g_return_val_if_fail(shape != NULL, FALSE);
    
    /* Validate dimension bounds */
    if (shape->modality == 0 || shape->modality > 20) {
        return FALSE;
    }
    
    if (shape->depth == 0 || shape->depth > 15) {
        return FALSE;
    }
    
    if (shape->context == 0 || shape->context > 25) {
        return FALSE;
    }
    
    /* Validate value ranges */
    if (shape->salience < 0.0 || shape->salience > 1.0) {
        return FALSE;
    }
    
    if (shape->autonomy_index < 0.0 || shape->autonomy_index > 1.0) {
        return FALSE;
    }
    
    return TRUE;
}

GncAtomHandle gnc_ko6ml_encode_hypergraph_node(Ko6mlPrimitive *primitive,
                                               Ko6mlTensorShape *shape)
{
    g_return_val_if_fail(primitive != NULL, 0);
    g_return_val_if_fail(shape != NULL, 0);
    g_return_val_if_fail(gnc_ko6ml_validate_tensor_shape(shape), 0);
    
    /* Create AtomSpace node if not already translated */
    if (primitive->atom_handle == 0) {
        Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(primitive);
        if (!result || !result->success) {
            if (result) gnc_ko6ml_translation_result_free(result);
            return 0;
        }
        gnc_ko6ml_translation_result_free(result);
    }
    
    /* Add tensor shape properties as EvaluationLinks */
    gchar *modality_str = g_strdup_printf("%zu", shape->modality);
    gchar *depth_str = g_strdup_printf("%zu", shape->depth);
    gchar *context_str = g_strdup_printf("%zu", shape->context);
    gchar *salience_str = g_strdup_printf("%.3f", shape->salience);
    gchar *autonomy_str = g_strdup_printf("%.3f", shape->autonomy_index);
    
    /* Create property predicates and evaluations */
    GncAtomHandle modality_pred = gnc_atomspace_create_predicate_node("modality");
    GncAtomHandle depth_pred = gnc_atomspace_create_predicate_node("depth");
    GncAtomHandle context_pred = gnc_atomspace_create_predicate_node("context");
    GncAtomHandle salience_pred = gnc_atomspace_create_predicate_node("salience");
    GncAtomHandle autonomy_pred = gnc_atomspace_create_predicate_node("autonomy_index");
    
    /* Create value nodes */
    GncAtomHandle modality_val = gnc_atomspace_create_concept_node(modality_str);
    GncAtomHandle depth_val = gnc_atomspace_create_concept_node(depth_str);
    GncAtomHandle context_val = gnc_atomspace_create_concept_node(context_str);
    GncAtomHandle salience_val = gnc_atomspace_create_concept_node(salience_str);
    GncAtomHandle autonomy_val = gnc_atomspace_create_concept_node(autonomy_str);
    
    /* Create evaluations linking properties to the node */
    gnc_atomspace_create_evaluation_link(modality_pred, primitive->atom_handle, modality_val);
    gnc_atomspace_create_evaluation_link(depth_pred, primitive->atom_handle, depth_val);
    gnc_atomspace_create_evaluation_link(context_pred, primitive->atom_handle, context_val);
    gnc_atomspace_create_evaluation_link(salience_pred, primitive->atom_handle, salience_val);
    gnc_atomspace_create_evaluation_link(autonomy_pred, primitive->atom_handle, autonomy_val);
    
    g_free(modality_str);
    g_free(depth_str);
    g_free(context_str);
    g_free(salience_str);
    g_free(autonomy_str);
    
    g_debug("Encoded hypergraph node for primitive '%s' with tensor shape [%zu,%zu,%zu,%.3f,%.3f]",
            primitive->name, shape->modality, shape->depth, shape->context, 
            shape->salience, shape->autonomy_index);
    
    return primitive->atom_handle;
}

GncAtomHandle gnc_ko6ml_encode_hypergraph_link(Ko6mlPrimitive *source_primitive,
                                               Ko6mlPrimitive *target_primitive,
                                               const gchar *relation_type,
                                               Ko6mlTensorShape *shape)
{
    g_return_val_if_fail(source_primitive != NULL, 0);
    g_return_val_if_fail(target_primitive != NULL, 0);
    g_return_val_if_fail(relation_type != NULL, 0);
    g_return_val_if_fail(shape != NULL, 0);
    
    /* Ensure both primitives have AtomSpace representations */
    gnc_ko6ml_encode_hypergraph_node(source_primitive, shape);
    gnc_ko6ml_encode_hypergraph_node(target_primitive, shape);
    
    /* Create relation predicate */
    GncAtomHandle relation_pred = gnc_atomspace_create_predicate_node(relation_type);
    
    /* Create evaluation link between source and target */
    GncAtomHandle link_handle = gnc_atomspace_create_evaluation_link(
        relation_pred, source_primitive->atom_handle, target_primitive->atom_handle);
    
    /* Set truth value based on tensor shape properties */
    gdouble link_strength = (shape->salience + shape->autonomy_index) / 2.0;
    gdouble link_confidence = MIN(shape->salience, shape->autonomy_index);
    gnc_atomspace_set_truth_value(link_handle, link_strength, link_confidence);
    
    g_debug("Created hypergraph link '%s' between '%s' and '%s'",
            relation_type, source_primitive->name, target_primitive->name);
    
    return link_handle;
}

/********************************************************************\
 * Prime Factorization Mapping                                     *
\********************************************************************/

guint* gnc_ko6ml_tensor_to_prime_factors(Ko6mlTensorShape *shape, gsize *n_factors)
{
    g_return_val_if_fail(shape != NULL, NULL);
    g_return_val_if_fail(n_factors != NULL, NULL);
    
    /* Map tensor dimensions to prime factors */
    guint *factors = g_new0(guint, 5);
    
    factors[0] = PRIME_FACTORS[MIN(shape->modality - 1, N_PRIMES - 1)];
    factors[1] = PRIME_FACTORS[MIN(shape->depth - 1, N_PRIMES - 1)];
    factors[2] = PRIME_FACTORS[MIN(shape->context - 1, N_PRIMES - 1)];
    
    /* Map salience and autonomy to primes based on quantized values */
    gsize salience_index = (gsize)(shape->salience * (N_PRIMES - 1));
    gsize autonomy_index = (gsize)(shape->autonomy_index * (N_PRIMES - 1));
    
    factors[3] = PRIME_FACTORS[MIN(salience_index, N_PRIMES - 1)];
    factors[4] = PRIME_FACTORS[MIN(autonomy_index, N_PRIMES - 1)];
    
    *n_factors = 5;
    return factors;
}

Ko6mlTensorShape gnc_ko6ml_prime_factors_to_tensor(guint *prime_factors, gsize n_factors)
{
    Ko6mlTensorShape shape = {0};
    
    g_return_val_if_fail(prime_factors != NULL, shape);
    g_return_val_if_fail(n_factors >= 5, shape);
    
    /* Reverse map prime factors to tensor dimensions */
    for (gsize i = 0; i < N_PRIMES && i < 3; i++) {
        if (prime_factors[0] == PRIME_FACTORS[i]) {
            shape.modality = i + 1;
            break;
        }
    }
    
    for (gsize i = 0; i < N_PRIMES && i < 3; i++) {
        if (prime_factors[1] == PRIME_FACTORS[i]) {
            shape.depth = i + 1;
            break;
        }
    }
    
    for (gsize i = 0; i < N_PRIMES && i < 3; i++) {
        if (prime_factors[2] == PRIME_FACTORS[i]) {
            shape.context = i + 1;
            break;
        }
    }
    
    /* Reverse map salience and autonomy */
    for (gsize i = 0; i < N_PRIMES; i++) {
        if (prime_factors[3] == PRIME_FACTORS[i]) {
            shape.salience = (gdouble)i / (N_PRIMES - 1);
            break;
        }
    }
    
    for (gsize i = 0; i < N_PRIMES; i++) {
        if (prime_factors[4] == PRIME_FACTORS[i]) {
            shape.autonomy_index = (gdouble)i / (N_PRIMES - 1);
            break;
        }
    }
    
    return shape;
}

/********************************************************************\
 * Performance and Optimization                                    *
\********************************************************************/

gdouble gnc_ko6ml_benchmark_tensor_operations(Ko6mlPrimitive *primitive, gint n_iterations)
{
    g_return_val_if_fail(primitive != NULL, -1.0);
    g_return_val_if_fail(n_iterations > 0, -1.0);
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (gint i = 0; i < n_iterations; i++) {
        /* Perform tensor operation */
        Ko6mlTensorShape shape = gnc_ko6ml_create_tensor_shape(
            primitive->name, primitive, 
            (i % 10) + 1, (i % 8) + 1, (i % 12) + 1);
        
        /* Encode hypergraph node */
        gnc_ko6ml_encode_hypergraph_node(primitive, &shape);
        
        /* Translate to AtomSpace and back */
        Ko6mlTranslationResult *result = gnc_ko6ml_to_atomspace(primitive);
        if (result) {
            gnc_ko6ml_from_atomspace(result->atom_handle);
            gnc_ko6ml_translation_result_free(result);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    gdouble elapsed_us = (end.tv_sec - start.tv_sec) * 1000000.0 + 
                        (end.tv_nsec - start.tv_nsec) / 1000.0;
    gdouble avg_time_us = elapsed_us / n_iterations;
    
    g_debug("Benchmark: %d tensor operations took %.2f μs average per operation", 
            n_iterations, avg_time_us);
    
    return avg_time_us;
}

Ko6mlTensorShape gnc_ko6ml_optimize_tensor_shape(Ko6mlTensorShape *shape)
{
    g_return_val_if_fail(shape != NULL, *shape);
    
    Ko6mlTensorShape optimized = *shape;
    
    /* Optimize for cache efficiency - prefer powers of 2 for dimensions */
    optimized.modality = 1 << (guint)ceil(log2(shape->modality));
    optimized.depth = 1 << (guint)ceil(log2(shape->depth));
    optimized.context = 1 << (guint)ceil(log2(shape->context));
    
    /* Ensure bounds */
    optimized.modality = MIN(optimized.modality, 16);
    optimized.depth = MIN(optimized.depth, 8);
    optimized.context = MIN(optimized.context, 16);
    
    g_debug("Optimized tensor shape: [%zu,%zu,%zu] → [%zu,%zu,%zu]",
            shape->modality, shape->depth, shape->context,
            optimized.modality, optimized.depth, optimized.context);
    
    return optimized;
}

/********************************************************************\
 * Utility Functions                                               *
\********************************************************************/

void gnc_ko6ml_translation_result_free(Ko6mlTranslationResult *result)
{
    if (!result) return;
    
    g_free(result->error_message);
    g_free(result);
}