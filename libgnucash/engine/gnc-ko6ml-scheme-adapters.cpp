/********************************************************************\
 * gnc-ko6ml-scheme-adapters.cpp -- Scheme adapters for ko6ml     *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * Modular Scheme adapters for agentic grammar AtomSpace          *
 * integration with round-trip translation capabilities.           *
 ********************************************************************/

#include "gnc-ko6ml-scheme-adapters.h"
#include "gnc-ko6ml-primitives.h"
#include "gnc-cognitive-scheme.h"
#include "Account.h"
#include "Transaction.h"
#include <glib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/********************************************************************\
 * Static Variables and Data Structures                           *
\********************************************************************/

static gboolean adapters_initialized = FALSE;
static GHashTable *adapter_registry = NULL;  /* Ko6mlPrimitiveType -> GList of adapters */

/********************************************************************\
 * Scheme Template Generation Utilities                           *
\********************************************************************/

static gchar* generate_scheme_tensor_encoding(Ko6mlTensorShape *shape)
{
    return g_strdup_printf(
        "(list 'tensor-shape\n"
        "      '(modality . %zu)\n"
        "      '(depth . %zu)\n"
        "      '(context . %zu)\n"
        "      '(salience . %.3f)\n"
        "      '(autonomy-index . %.3f))",
        shape->modality, shape->depth, shape->context,
        shape->salience, shape->autonomy_index
    );
}

static gchar* generate_scheme_properties(GHashTable *properties)
{
    if (!properties || g_hash_table_size(properties) == 0) {
        return g_strdup("'()");
    }
    
    GString *props_str = g_string_new("(list");
    GHashTableIter iter;
    gpointer key, value;
    
    g_hash_table_iter_init(&iter, properties);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        g_string_append_printf(props_str, "\n      '(%s . \"%s\")",
                              (gchar*)key, (gchar*)value);
    }
    
    g_string_append(props_str, ")");
    return g_string_free(props_str, FALSE);
}

/********************************************************************\
 * Core Adapter Functions                                         *
\********************************************************************/

gboolean gnc_ko6ml_scheme_adapters_init(void)
{
    if (adapters_initialized) {
        return TRUE;
    }
    
    /* Initialize adapter registry */
    adapter_registry = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                             NULL, (GDestroyNotify)g_list_free);
    
    /* Ensure ko6ml system is initialized */
    if (!gnc_ko6ml_init()) {
        g_warning("Failed to initialize ko6ml system for Scheme adapters");
        return FALSE;
    }
    
    /* Register built-in adapters */
    gnc_ko6ml_register_scheme_adapter(KO6ML_CONCEPT, "concept_adapter", 
                                      gnc_ko6ml_concept_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_RELATION, "relation_adapter", 
                                      gnc_ko6ml_relation_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_AGENT, "agent_adapter", 
                                      gnc_ko6ml_agent_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_STATE, "state_adapter", 
                                      gnc_ko6ml_state_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_PROCESS, "process_adapter", 
                                      gnc_ko6ml_process_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_CONTEXT, "context_adapter", 
                                      gnc_ko6ml_context_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_MODALITY, "modality_adapter", 
                                      gnc_ko6ml_modality_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_TEMPORAL, "temporal_adapter", 
                                      gnc_ko6ml_temporal_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_SPATIAL, "spatial_adapter", 
                                      gnc_ko6ml_spatial_adapter, NULL);
    gnc_ko6ml_register_scheme_adapter(KO6ML_COMPOSITE, "composite_adapter", 
                                      gnc_ko6ml_composite_adapter, NULL);
    
    adapters_initialized = TRUE;
    g_message("ko6ml Scheme adapters initialized successfully");
    
    return TRUE;
}

void gnc_ko6ml_scheme_adapters_shutdown(void)
{
    if (!adapters_initialized) {
        return;
    }
    
    if (adapter_registry) {
        /* Free adapter lists and their contents */
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, adapter_registry);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            GList *adapter_list = (GList*)value;
            g_list_free_full(adapter_list, (GDestroyNotify)gnc_ko6ml_scheme_adapter_free);
        }
        g_hash_table_destroy(adapter_registry);
        adapter_registry = NULL;
    }
    
    adapters_initialized = FALSE;
    g_message("ko6ml Scheme adapters shut down");
}

gboolean gnc_ko6ml_register_scheme_adapter(Ko6mlPrimitiveType type,
                                           const gchar *name,
                                           Ko6mlSchemeAdapterFunc func,
                                           gpointer user_data)
{
    g_return_val_if_fail(adapters_initialized, FALSE);
    g_return_val_if_fail(name != NULL, FALSE);
    g_return_val_if_fail(func != NULL, FALSE);
    
    Ko6mlSchemeAdapter *adapter = g_new0(Ko6mlSchemeAdapter, 1);
    adapter->type = type;
    adapter->name = g_strdup(name);
    adapter->func = func;
    adapter->user_data = user_data;
    adapter->performance_score = 0.5; /* Default performance score */
    
    /* Add to registry */
    GList *adapter_list = (GList*)g_hash_table_lookup(adapter_registry, GINT_TO_POINTER(type));
    adapter_list = g_list_append(adapter_list, adapter);
    g_hash_table_insert(adapter_registry, GINT_TO_POINTER(type), adapter_list);
    
    g_debug("Registered Scheme adapter '%s' for type %s", name, 
            gnc_ko6ml_primitive_type_to_string(type));
    
    return TRUE;
}

gboolean gnc_ko6ml_unregister_scheme_adapter(Ko6mlPrimitiveType type, const gchar *name)
{
    g_return_val_if_fail(adapters_initialized, FALSE);
    g_return_val_if_fail(name != NULL, FALSE);
    
    GList *adapter_list = (GList*)g_hash_table_lookup(adapter_registry, GINT_TO_POINTER(type));
    if (!adapter_list) {
        return FALSE;
    }
    
    /* Find and remove adapter */
    for (GList *l = adapter_list; l; l = l->next) {
        Ko6mlSchemeAdapter *adapter = (Ko6mlSchemeAdapter*)l->data;
        if (g_strcmp0(adapter->name, name) == 0) {
            adapter_list = g_list_remove(adapter_list, adapter);
            gnc_ko6ml_scheme_adapter_free(adapter);
            g_hash_table_insert(adapter_registry, GINT_TO_POINTER(type), adapter_list);
            g_debug("Unregistered Scheme adapter '%s'", name);
            return TRUE;
        }
    }
    
    return FALSE;
}

Ko6mlSchemeAdapter* gnc_ko6ml_get_best_adapter(Ko6mlPrimitiveType type)
{
    g_return_val_if_fail(adapters_initialized, NULL);
    
    GList *adapter_list = (GList*)g_hash_table_lookup(adapter_registry, GINT_TO_POINTER(type));
    if (!adapter_list) {
        return NULL;
    }
    
    /* Find adapter with highest performance score */
    Ko6mlSchemeAdapter *best_adapter = NULL;
    gdouble best_score = -1.0;
    
    for (GList *l = adapter_list; l; l = l->next) {
        Ko6mlSchemeAdapter *adapter = (Ko6mlSchemeAdapter*)l->data;
        if (adapter->performance_score > best_score) {
            best_score = adapter->performance_score;
            best_adapter = adapter;
        }
    }
    
    return best_adapter;
}

/********************************************************************\
 * Translation Functions                                           *
\********************************************************************/

Ko6mlSchemeTranslationResult* gnc_ko6ml_primitive_to_scheme(Ko6mlPrimitive *primitive)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(adapters_initialized, NULL);
    
    Ko6mlSchemeTranslationResult *result = g_new0(Ko6mlSchemeTranslationResult, 1);
    result->success = FALSE;
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /* Get best adapter for primitive type */
    Ko6mlSchemeAdapter *adapter = gnc_ko6ml_get_best_adapter(primitive->type);
    if (!adapter) {
        result->error_message = g_strdup_printf("No adapter found for type %s",
                                               gnc_ko6ml_primitive_type_to_string(primitive->type));
        clock_gettime(CLOCK_MONOTONIC, &end);
        result->performance_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                                (end.tv_nsec - start.tv_nsec) / 1000000.0;
        return result;
    }
    
    /* Use adapter to generate Scheme code */
    result->scheme_code = adapter->func(primitive, adapter->user_data);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    result->performance_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                            (end.tv_nsec - start.tv_nsec) / 1000000.0;
    
    if (result->scheme_code) {
        result->success = TRUE;
        result->accuracy = 1.0; /* Assume perfect accuracy for now */
        g_debug("Translated primitive '%s' to Scheme in %.2f ms", 
                primitive->name, result->performance_ms);
    } else {
        result->error_message = g_strdup("Adapter failed to generate Scheme code");
    }
    
    return result;
}

Ko6mlPrimitive* gnc_ko6ml_scheme_to_primitive(const gchar *scheme_code)
{
    g_return_val_if_fail(scheme_code != NULL, NULL);
    g_return_val_if_fail(adapters_initialized, NULL);
    
    /* This is a simplified reverse translation */
    /* In a full implementation, this would parse the Scheme code */
    /* and reconstruct the ko6ml primitive */
    
    /* For now, create a basic primitive based on pattern matching */
    if (strstr(scheme_code, "ConceptNode")) {
        Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_CONCEPT, 
                                                           "reverse_concept", 
                                                           "Concept from Scheme");
        return primitive;
    } else if (strstr(scheme_code, "PredicateNode")) {
        Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_RELATION, 
                                                           "reverse_relation", 
                                                           "Relation from Scheme");
        return primitive;
    }
    
    /* Default to concept type */
    Ko6mlPrimitive *primitive = gnc_ko6ml_primitive_new(KO6ML_CONCEPT, 
                                                       "reverse_unknown", 
                                                       "Unknown from Scheme");
    return primitive;
}

Ko6mlSchemeTranslationResult* gnc_ko6ml_scheme_round_trip_test(Ko6mlPrimitive *original)
{
    g_return_val_if_fail(original != NULL, NULL);
    
    /* Translate to Scheme */
    Ko6mlSchemeTranslationResult *to_scheme = gnc_ko6ml_primitive_to_scheme(original);
    if (!to_scheme || !to_scheme->success) {
        if (to_scheme) {
            to_scheme->accuracy = 0.0;
        }
        return to_scheme;
    }
    
    /* Translate back from Scheme */
    Ko6mlPrimitive *back_to_ko6ml = gnc_ko6ml_scheme_to_primitive(to_scheme->scheme_code);
    if (!back_to_ko6ml) {
        to_scheme->success = FALSE;
        to_scheme->accuracy = 0.0;
        g_free(to_scheme->error_message);
        to_scheme->error_message = g_strdup("Failed to translate back from Scheme");
        return to_scheme;
    }
    
    /* Calculate accuracy based on preservation of key attributes */
    gdouble accuracy = 1.0;
    
    if (original->type != back_to_ko6ml->type) {
        accuracy -= 0.3;
    }
    
    if (g_strcmp0(original->name, back_to_ko6ml->name) != 0) {
        accuracy -= 0.2;
    }
    
    if (fabs(original->salience - back_to_ko6ml->salience) > 0.1) {
        accuracy -= 0.2;
    }
    
    if (fabs(original->autonomy_index - back_to_ko6ml->autonomy_index) > 0.1) {
        accuracy -= 0.2;
    }
    
    /* Check properties */
    if (g_hash_table_size(original->properties) != g_hash_table_size(back_to_ko6ml->properties)) {
        accuracy -= 0.1;
    }
    
    to_scheme->accuracy = MAX(accuracy, 0.0);
    
    gnc_ko6ml_primitive_free(back_to_ko6ml);
    
    g_debug("Round-trip test for primitive '%s': accuracy %.2f", 
            original->name, to_scheme->accuracy);
    
    return to_scheme;
}

/********************************************************************\
 * Agentic Grammar Support                                        *
\********************************************************************/

gchar* gnc_ko6ml_generate_agentic_grammar(Ko6mlPrimitive *primitive)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    
    gchar *properties = generate_scheme_properties(primitive->properties);
    
    gchar *agentic_pattern = g_strdup_printf(
        ";; Agentic Grammar Pattern for %s\n"
        "(define agentic-pattern-%s\n"
        "  (lambda (context)\n"
        "    (let ((agent (ConceptNode \"%s\"))\n"
        "          (props %s)\n"
        "          (salience %.3f)\n"
        "          (autonomy %.3f))\n"
        "      (cog-set-tv! agent (cog-new-stv salience autonomy))\n"
        "      (for-each\n"
        "        (lambda (prop)\n"
        "          (EvaluationLink\n"
        "            (PredicateNode (car prop))\n"
        "            (ListLink agent (ConceptNode (cdr prop)))))\n"
        "        props)\n"
        "      agent)))",
        primitive->name, primitive->name, primitive->name,
        properties, primitive->salience, primitive->autonomy_index
    );
    
    g_free(properties);
    return agentic_pattern;
}

gchar* gnc_ko6ml_create_agentic_bindlink(Ko6mlPrimitive *agent_primitive,
                                         Ko6mlPrimitive *context_primitive)
{
    g_return_val_if_fail(agent_primitive != NULL, NULL);
    g_return_val_if_fail(context_primitive != NULL, NULL);
    
    return g_strdup_printf(
        ";; Agentic BindLink Pattern\n"
        "(BindLink\n"
        "  (VariableList\n"
        "    (VariableNode \"$action\")\n"
        "    (VariableNode \"$result\"))\n"
        "  (AndLink\n"
        "    (EvaluationLink\n"
        "      (PredicateNode \"hasContext\")\n"
        "      (ListLink\n"
        "        (ConceptNode \"%s\")\n"
        "        (ConceptNode \"%s\")))\n"
        "    (EvaluationLink\n"
        "      (PredicateNode \"canPerform\")\n"
        "      (ListLink\n"
        "        (ConceptNode \"%s\")\n"
        "        (VariableNode \"$action\")))\n"
        "    (EvaluationLink\n"
        "      (PredicateNode \"produces\")\n"
        "      (ListLink\n"
        "        (VariableNode \"$action\")\n"
        "        (VariableNode \"$result\"))))\n"
        "  (EvaluationLink\n"
        "    (PredicateNode \"agenticExecution\")\n"
        "    (ListLink\n"
        "      (ConceptNode \"%s\")\n"
        "      (VariableNode \"$action\")\n"
        "      (VariableNode \"$result\"))))",
        agent_primitive->name, context_primitive->name,
        agent_primitive->name, agent_primitive->name
    );
}

gchar* gnc_ko6ml_generate_cognitive_rule(Ko6mlPrimitive *source_primitive,
                                         Ko6mlPrimitive *target_primitive,
                                         const gchar *relation_type)
{
    g_return_val_if_fail(source_primitive != NULL, NULL);
    g_return_val_if_fail(target_primitive != NULL, NULL);
    g_return_val_if_fail(relation_type != NULL, NULL);
    
    return g_strdup_printf(
        ";; Cognitive Rule: %s %s %s\n"
        "(ImplicationLink\n"
        "  (AndLink\n"
        "    (ConceptNode \"%s\")\n"
        "    (ConceptNode \"%s\")\n"
        "    (EvaluationLink\n"
        "      (PredicateNode \"contextuallyRelevant\")\n"
        "      (ListLink\n"
        "        (ConceptNode \"%s\")\n"
        "        (ConceptNode \"%s\"))))\n"
        "  (EvaluationLink\n"
        "    (PredicateNode \"%s\")\n"
        "    (ListLink\n"
        "      (ConceptNode \"%s\")\n"
        "      (ConceptNode \"%s\"))))",
        source_primitive->name, relation_type, target_primitive->name,
        source_primitive->name, target_primitive->name,
        source_primitive->name, target_primitive->name,
        relation_type, source_primitive->name, target_primitive->name
    );
}

/********************************************************************\
 * Performance and Validation                                     *
\********************************************************************/

gdouble gnc_ko6ml_benchmark_adapter(Ko6mlSchemeAdapter *adapter,
                                    Ko6mlPrimitive *test_primitive,
                                    gint n_iterations)
{
    g_return_val_if_fail(adapter != NULL, -1.0);
    g_return_val_if_fail(test_primitive != NULL, -1.0);
    g_return_val_if_fail(n_iterations > 0, -1.0);
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (gint i = 0; i < n_iterations; i++) {
        gchar *scheme_code = adapter->func(test_primitive, adapter->user_data);
        g_free(scheme_code);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    gdouble elapsed_us = (end.tv_sec - start.tv_sec) * 1000000.0 + 
                        (end.tv_nsec - start.tv_nsec) / 1000.0;
    gdouble avg_time_us = elapsed_us / n_iterations;
    
    g_debug("Adapter '%s' benchmark: %.2f μs per translation", 
            adapter->name, avg_time_us);
    
    return avg_time_us;
}

gdouble gnc_ko6ml_validate_adapter_accuracy(Ko6mlSchemeAdapter *adapter,
                                            Ko6mlPrimitive *test_primitive)
{
    g_return_val_if_fail(adapter != NULL, 0.0);
    g_return_val_if_fail(test_primitive != NULL, 0.0);
    
    /* Generate Scheme code */
    gchar *scheme_code = adapter->func(test_primitive, adapter->user_data);
    if (!scheme_code) {
        return 0.0;
    }
    
    gdouble accuracy = 1.0;
    
    /* Check if primitive name appears in Scheme code */
    if (!strstr(scheme_code, test_primitive->name)) {
        accuracy -= 0.4;
    }
    
    /* Check if appropriate Scheme constructs are present */
    const gchar *expected_constructs[] = {
        "ConceptNode", "PredicateNode", "EvaluationLink", "ListLink"
    };
    
    for (gsize i = 0; i < G_N_ELEMENTS(expected_constructs); i++) {
        if (!strstr(scheme_code, expected_constructs[i])) {
            accuracy -= 0.1;
        }
    }
    
    /* Check for salience and autonomy values */
    gchar *salience_str = g_strdup_printf("%.3f", test_primitive->salience);
    gchar *autonomy_str = g_strdup_printf("%.3f", test_primitive->autonomy_index);
    
    if (!strstr(scheme_code, salience_str)) {
        accuracy -= 0.1;
    }
    
    if (!strstr(scheme_code, autonomy_str)) {
        accuracy -= 0.1;
    }
    
    g_free(salience_str);
    g_free(autonomy_str);
    g_free(scheme_code);
    
    return MAX(accuracy, 0.0);
}

void gnc_ko6ml_update_adapter_performance(Ko6mlSchemeAdapter *adapter, gdouble new_score)
{
    g_return_if_fail(adapter != NULL);
    g_return_if_fail(new_score >= 0.0 && new_score <= 1.0);
    
    /* Use exponential moving average */
    adapter->performance_score = 0.7 * adapter->performance_score + 0.3 * new_score;
    
    g_debug("Updated adapter '%s' performance score to %.3f", 
            adapter->name, adapter->performance_score);
}

/********************************************************************\
 * Built-in Adapters                                              *
\********************************************************************/

gchar* gnc_ko6ml_concept_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_CONCEPT, NULL);
    
    gchar *properties = generate_scheme_properties(primitive->properties);
    
    gchar *scheme_code = g_strdup_printf(
        ";; ko6ml Concept: %s\n"
        "(let ((concept (ConceptNode \"%s\"))\n"
        "      (properties %s))\n"
        "  (cog-set-tv! concept (cog-new-stv %.3f %.3f))\n"
        "  (for-each\n"
        "    (lambda (prop)\n"
        "      (EvaluationLink\n"
        "        (PredicateNode (car prop))\n"
        "        (ListLink concept (ConceptNode (cdr prop)))))\n"
        "    properties)\n"
        "  concept)",
        primitive->name, primitive->name, properties,
        primitive->salience, primitive->autonomy_index
    );
    
    g_free(properties);
    return scheme_code;
}

gchar* gnc_ko6ml_relation_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_RELATION, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Relation: %s\n"
        "(let ((relation (PredicateNode \"%s\")))\n"
        "  (cog-set-tv! relation (cog-new-stv %.3f %.3f))\n"
        "  relation)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_agent_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_AGENT, NULL);
    
    gchar *properties = generate_scheme_properties(primitive->properties);
    
    gchar *scheme_code = g_strdup_printf(
        ";; ko6ml Agent: %s\n"
        "(let ((agent (ConceptNode \"%s\"))\n"
        "      (agent-type (ConceptNode \"Agent\"))\n"
        "      (properties %s))\n"
        "  (cog-set-tv! agent (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink agent agent-type)\n"
        "  (EvaluationLink\n"
        "    (PredicateNode \"hasAutonomy\")\n"
        "    (ListLink agent (NumberNode %.3f)))\n"
        "  (for-each\n"
        "    (lambda (prop)\n"
        "      (EvaluationLink\n"
        "        (PredicateNode (car prop))\n"
        "        (ListLink agent (ConceptNode (cdr prop)))))\n"
        "    properties)\n"
        "  agent)",
        primitive->name, primitive->name, properties,
        primitive->salience, primitive->autonomy_index, primitive->autonomy_index
    );
    
    g_free(properties);
    return scheme_code;
}

gchar* gnc_ko6ml_state_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_STATE, NULL);
    
    return g_strdup_printf(
        ";; ko6ml State: %s\n"
        "(let ((state (ConceptNode \"%s\"))\n"
        "      (state-type (ConceptNode \"State\")))\n"
        "  (cog-set-tv! state (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink state state-type)\n"
        "  (EvaluationLink\n"
        "    (PredicateNode \"hasTemporalState\")\n"
        "    (ListLink state (ConceptNode \"current\")))\n"
        "  state)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_process_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_PROCESS, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Process: %s\n"
        "(let ((process (ConceptNode \"%s\"))\n"
        "      (process-type (ConceptNode \"Process\")))\n"
        "  (cog-set-tv! process (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink process process-type)\n"
        "  (EvaluationLink\n"
        "    (PredicateNode \"hasProcessState\")\n"
        "    (ListLink process (ConceptNode \"active\")))\n"
        "  process)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_context_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_CONTEXT, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Context: %s\n"
        "(let ((context (ConceptNode \"%s\"))\n"
        "      (context-type (ConceptNode \"Context\")))\n"
        "  (cog-set-tv! context (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink context context-type)\n"
        "  context)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_modality_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_MODALITY, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Modality: %s\n"
        "(let ((modality (ConceptNode \"%s\"))\n"
        "      (modality-type (ConceptNode \"Modality\")))\n"
        "  (cog-set-tv! modality (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink modality modality-type)\n"
        "  modality)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_temporal_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_TEMPORAL, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Temporal: %s\n"
        "(let ((temporal (ConceptNode \"%s\"))\n"
        "      (temporal-type (ConceptNode \"Temporal\")))\n"
        "  (cog-set-tv! temporal (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink temporal temporal-type)\n"
        "  temporal)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_spatial_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_SPATIAL, NULL);
    
    return g_strdup_printf(
        ";; ko6ml Spatial: %s\n"
        "(let ((spatial (ConceptNode \"%s\"))\n"
        "      (spatial-type (ConceptNode \"Spatial\")))\n"
        "  (cog-set-tv! spatial (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink spatial spatial-type)\n"
        "  spatial)",
        primitive->name, primitive->name,
        primitive->salience, primitive->autonomy_index
    );
}

gchar* gnc_ko6ml_composite_adapter(Ko6mlPrimitive *primitive, gpointer user_data)
{
    g_return_val_if_fail(primitive != NULL, NULL);
    g_return_val_if_fail(primitive->type == KO6ML_COMPOSITE, NULL);
    
    gchar *properties = generate_scheme_properties(primitive->properties);
    
    gchar *scheme_code = g_strdup_printf(
        ";; ko6ml Composite: %s\n"
        "(let ((composite (ConceptNode \"%s\"))\n"
        "      (composite-type (ConceptNode \"Composite\"))\n"
        "      (properties %s))\n"
        "  (cog-set-tv! composite (cog-new-stv %.3f %.3f))\n"
        "  (InheritanceLink composite composite-type)\n"
        "  (for-each\n"
        "    (lambda (prop)\n"
        "      (EvaluationLink\n"
        "        (PredicateNode (car prop))\n"
        "        (ListLink composite (ConceptNode (cdr prop)))))\n"
        "    properties)\n"
        "  composite)",
        primitive->name, primitive->name, properties,
        primitive->salience, primitive->autonomy_index
    );
    
    g_free(properties);
    return scheme_code;
}

/********************************************************************\
 * Utility Functions                                              *
\********************************************************************/

void gnc_ko6ml_scheme_translation_result_free(Ko6mlSchemeTranslationResult *result)
{
    if (!result) return;
    
    g_free(result->scheme_code);
    g_free(result->error_message);
    g_free(result);
}

void gnc_ko6ml_scheme_adapter_free(Ko6mlSchemeAdapter *adapter)
{
    if (!adapter) return;
    
    g_free(adapter->name);
    g_free(adapter);
}

void gnc_ko6ml_get_adapter_stats(gsize *n_adapters, gdouble *avg_performance)
{
    g_return_if_fail(adapters_initialized);
    
    gsize total_adapters = 0;
    gdouble total_performance = 0.0;
    
    if (adapter_registry) {
        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, adapter_registry);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            GList *adapter_list = (GList*)value;
            for (GList *l = adapter_list; l; l = l->next) {
                Ko6mlSchemeAdapter *adapter = (Ko6mlSchemeAdapter*)l->data;
                total_adapters++;
                total_performance += adapter->performance_score;
            }
        }
    }
    
    if (n_adapters) {
        *n_adapters = total_adapters;
    }
    
    if (avg_performance) {
        *avg_performance = total_adapters > 0 ? total_performance / total_adapters : 0.0;
    }
}