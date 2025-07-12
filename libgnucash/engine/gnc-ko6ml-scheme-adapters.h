/********************************************************************\
 * gnc-ko6ml-scheme-adapters.h -- Scheme adapters for ko6ml       *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * Modular Scheme adapters for agentic grammar AtomSpace          *
 * integration with round-trip translation capabilities.           *
 ********************************************************************/

/** @addtogroup Engine
    @{ */
/** @addtogroup Ko6mlSchemeAdapters
    Modular Scheme adapters for agentic grammar AtomSpace integration
    @{ */

/** @file gnc-ko6ml-scheme-adapters.h
    @brief Scheme adapters for ko6ml agentic grammar AtomSpace integration
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_KO6ML_SCHEME_ADAPTERS_H
#define GNC_KO6ML_SCHEME_ADAPTERS_H

#include "gnc-ko6ml-primitives.h"
#include "gnc-cognitive-scheme.h"
#include "Account.h"
#include "Transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name Scheme Adapter Types */
/** @{ */

/** Scheme adapter function pointer for ko6ml primitive processing */
typedef gchar* (*Ko6mlSchemeAdapterFunc)(Ko6mlPrimitive *primitive, gpointer user_data);

/** Scheme adapter registry entry */
typedef struct {
    Ko6mlPrimitiveType type;      /**< Primitive type this adapter handles */
    gchar *name;                  /**< Adapter name */
    Ko6mlSchemeAdapterFunc func;  /**< Adapter function */
    gpointer user_data;           /**< User data passed to adapter */
    gdouble performance_score;    /**< Performance score [0.0, 1.0] */
} Ko6mlSchemeAdapter;

/** Translation validation result */
typedef struct {
    gboolean success;             /**< Translation success flag */
    gdouble accuracy;             /**< Translation accuracy [0.0, 1.0] */
    gdouble performance_ms;       /**< Performance in milliseconds */
    gchar *scheme_code;           /**< Generated Scheme code */
    gchar *error_message;         /**< Error message if failed */
} Ko6mlSchemeTranslationResult;

/** @} */

/** @name Core Adapter Functions */
/** @{ */

/** Initialize ko6ml Scheme adapter system
 * @return TRUE on success, FALSE on failure
 */
gboolean gnc_ko6ml_scheme_adapters_init(void);

/** Shutdown ko6ml Scheme adapter system */
void gnc_ko6ml_scheme_adapters_shutdown(void);

/** Register a new Scheme adapter
 * @param type Primitive type to handle
 * @param name Adapter name
 * @param func Adapter function
 * @param user_data User data for adapter
 * @return TRUE on success, FALSE on failure
 */
gboolean gnc_ko6ml_register_scheme_adapter(Ko6mlPrimitiveType type,
                                           const gchar *name,
                                           Ko6mlSchemeAdapterFunc func,
                                           gpointer user_data);

/** Unregister a Scheme adapter
 * @param type Primitive type
 * @param name Adapter name
 * @return TRUE on success, FALSE if not found
 */
gboolean gnc_ko6ml_unregister_scheme_adapter(Ko6mlPrimitiveType type, const gchar *name);

/** Get best adapter for primitive type
 * @param type Primitive type
 * @return Best adapter or NULL if none found
 */
Ko6mlSchemeAdapter* gnc_ko6ml_get_best_adapter(Ko6mlPrimitiveType type);

/** @} */

/** @name Translation Functions */
/** @{ */

/** Translate ko6ml primitive to Scheme representation
 * @param primitive ko6ml primitive to translate
 * @return Translation result (caller must free)
 */
Ko6mlSchemeTranslationResult* gnc_ko6ml_primitive_to_scheme(Ko6mlPrimitive *primitive);

/** Translate Scheme code back to ko6ml primitive
 * @param scheme_code Scheme code to translate
 * @return ko6ml primitive (caller must free) or NULL if failed
 */
Ko6mlPrimitive* gnc_ko6ml_scheme_to_primitive(const gchar *scheme_code);

/** Round-trip translation test: ko6ml → Scheme → ko6ml
 * @param original Original ko6ml primitive
 * @return Translation result with round-trip validation
 */
Ko6mlSchemeTranslationResult* gnc_ko6ml_scheme_round_trip_test(Ko6mlPrimitive *original);

/** @} */

/** @name Agentic Grammar Support */
/** @{ */

/** Generate agentic grammar pattern for primitive
 * @param primitive ko6ml primitive
 * @return Scheme code for agentic grammar pattern (caller must free)
 */
gchar* gnc_ko6ml_generate_agentic_grammar(Ko6mlPrimitive *primitive);

/** Create AtomSpace BindLink pattern for agentic behavior
 * @param agent_primitive Agent primitive
 * @param context_primitive Context primitive
 * @return Scheme code for BindLink pattern (caller must free)
 */
gchar* gnc_ko6ml_create_agentic_bindlink(Ko6mlPrimitive *agent_primitive,
                                         Ko6mlPrimitive *context_primitive);

/** Generate cognitive rule for primitive interaction
 * @param source_primitive Source primitive
 * @param target_primitive Target primitive
 * @param relation_type Relation type string
 * @return Scheme code for cognitive rule (caller must free)
 */
gchar* gnc_ko6ml_generate_cognitive_rule(Ko6mlPrimitive *source_primitive,
                                         Ko6mlPrimitive *target_primitive,
                                         const gchar *relation_type);

/** @} */

/** @name Performance and Validation */
/** @{ */

/** Benchmark adapter performance
 * @param adapter Adapter to benchmark
 * @param test_primitive Test primitive
 * @param n_iterations Number of iterations
 * @return Average time per translation in microseconds
 */
gdouble gnc_ko6ml_benchmark_adapter(Ko6mlSchemeAdapter *adapter,
                                    Ko6mlPrimitive *test_primitive,
                                    gint n_iterations);

/** Validate adapter translation accuracy
 * @param adapter Adapter to validate
 * @param test_primitive Test primitive
 * @return Accuracy score [0.0, 1.0]
 */
gdouble gnc_ko6ml_validate_adapter_accuracy(Ko6mlSchemeAdapter *adapter,
                                            Ko6mlPrimitive *test_primitive);

/** Update adapter performance score
 * @param adapter Adapter to update
 * @param new_score New performance score
 */
void gnc_ko6ml_update_adapter_performance(Ko6mlSchemeAdapter *adapter, gdouble new_score);

/** @} */

/** @name Built-in Adapters */
/** @{ */

/** Adapter for KO6ML_CONCEPT primitives */
gchar* gnc_ko6ml_concept_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_RELATION primitives */
gchar* gnc_ko6ml_relation_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_AGENT primitives */
gchar* gnc_ko6ml_agent_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_STATE primitives */
gchar* gnc_ko6ml_state_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_PROCESS primitives */
gchar* gnc_ko6ml_process_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_CONTEXT primitives */
gchar* gnc_ko6ml_context_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_MODALITY primitives */
gchar* gnc_ko6ml_modality_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_TEMPORAL primitives */
gchar* gnc_ko6ml_temporal_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_SPATIAL primitives */
gchar* gnc_ko6ml_spatial_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** Adapter for KO6ML_COMPOSITE primitives */
gchar* gnc_ko6ml_composite_adapter(Ko6mlPrimitive *primitive, gpointer user_data);

/** @} */

/** @name Utility Functions */
/** @{ */

/** Free translation result
 * @param result Translation result to free
 */
void gnc_ko6ml_scheme_translation_result_free(Ko6mlSchemeTranslationResult *result);

/** Free adapter structure
 * @param adapter Adapter to free
 */
void gnc_ko6ml_scheme_adapter_free(Ko6mlSchemeAdapter *adapter);

/** Get adapter registry statistics
 * @param n_adapters Output: number of registered adapters
 * @param avg_performance Output: average performance score
 */
void gnc_ko6ml_get_adapter_stats(gsize *n_adapters, gdouble *avg_performance);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_KO6ML_SCHEME_ADAPTERS_H */
/** @} */
/** @} */