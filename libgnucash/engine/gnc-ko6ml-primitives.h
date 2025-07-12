/********************************************************************\
 * gnc-ko6ml-primitives.h -- ko6ml Cognitive Primitives           *
 * Copyright (C) 2024 GnuCash Cognitive Engine                     *
 *                                                                  *
 * This program implements the atomic vocabulary and bidirectional  *
 * translation mechanisms between ko6ml primitives and AtomSpace   *
 * hypergraph patterns for Phase 1 cognitive foundation.           *
 ********************************************************************/

/** @addtogroup Engine
    @{ */
/** @addtogroup Ko6mlPrimitives
    ko6ml cognitive primitives and AtomSpace hypergraph encoding
    for foundational neural-symbolic representation.
    @{ */

/** @file gnc-ko6ml-primitives.h
    @brief ko6ml cognitive primitives and bidirectional AtomSpace translation
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_KO6ML_PRIMITIVES_H
#define GNC_KO6ML_PRIMITIVES_H

#include "gnc-cognitive-accounting.h"
#include "Account.h"
#include "Transaction.h"
#include "gnc-engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name ko6ml Primitive Types */
/** @{ */

/** ko6ml cognitive primitive types */
typedef enum {
    KO6ML_CONCEPT = 0,        /**< Basic conceptual primitives */
    KO6ML_RELATION,           /**< Relational primitives */
    KO6ML_AGENT,              /**< Agentic primitives */
    KO6ML_STATE,              /**< State primitives */
    KO6ML_PROCESS,            /**< Process primitives */
    KO6ML_CONTEXT,            /**< Contextual primitives */
    KO6ML_MODALITY,           /**< Modal primitives */
    KO6ML_TEMPORAL,           /**< Temporal primitives */
    KO6ML_SPATIAL,            /**< Spatial primitives */
    KO6ML_COMPOSITE          /**< Composite primitives */
} Ko6mlPrimitiveType;

/** ko6ml primitive structure */
typedef struct {
    Ko6mlPrimitiveType type;  /**< Type of primitive */
    gchar *name;              /**< Name identifier */
    gchar *description;       /**< Human-readable description */
    GHashTable *properties;   /**< Property-value pairs */
    GncAtomHandle atom_handle; /**< Corresponding AtomSpace handle */
    gdouble salience;         /**< Salience weight [0.0, 1.0] */
    gdouble autonomy_index;   /**< Autonomy level [0.0, 1.0] */
} Ko6mlPrimitive;

/** Tensor fragment dimensions: [modality, depth, context, salience, autonomy_index] */
typedef struct {
    gsize modality;           /**< Modality dimension */
    gsize depth;              /**< Cognitive depth dimension */
    gsize context;            /**< Context dimension */
    gdouble salience;         /**< Salience value [0.0, 1.0] */
    gdouble autonomy_index;   /**< Autonomy index [0.0, 1.0] */
} Ko6mlTensorShape;

/** ko6ml to AtomSpace translation result */
typedef struct {
    GncAtomHandle atom_handle;  /**< Resulting AtomSpace handle */
    Ko6mlTensorShape tensor_shape; /**< Associated tensor shape */
    gboolean success;           /**< Translation success flag */
    gchar *error_message;       /**< Error message if failed */
} Ko6mlTranslationResult;

/** @} */

/** @name Core ko6ml Functions */
/** @{ */

/** Initialize ko6ml primitive system
 * @return TRUE on success, FALSE on failure
 */
gboolean gnc_ko6ml_init(void);

/** Shutdown ko6ml primitive system */
void gnc_ko6ml_shutdown(void);

/** Create a new ko6ml primitive
 * @param type Primitive type
 * @param name Primitive name
 * @param description Description of primitive
 * @return Newly created primitive (caller must free)
 */
Ko6mlPrimitive* gnc_ko6ml_primitive_new(Ko6mlPrimitiveType type, 
                                        const gchar *name, 
                                        const gchar *description);

/** Free a ko6ml primitive
 * @param primitive Primitive to free
 */
void gnc_ko6ml_primitive_free(Ko6mlPrimitive *primitive);

/** Set property on ko6ml primitive
 * @param primitive Target primitive
 * @param key Property key
 * @param value Property value
 */
void gnc_ko6ml_primitive_set_property(Ko6mlPrimitive *primitive, 
                                      const gchar *key, 
                                      const gchar *value);

/** Get property from ko6ml primitive
 * @param primitive Source primitive
 * @param key Property key
 * @return Property value (do not free)
 */
const gchar* gnc_ko6ml_primitive_get_property(Ko6mlPrimitive *primitive, 
                                              const gchar *key);

/** @} */

/** @name ko6ml ↔ AtomSpace Translation */
/** @{ */

/** Translate ko6ml primitive to AtomSpace hypergraph pattern
 * @param primitive ko6ml primitive to translate
 * @return Translation result (caller must free)
 */
Ko6mlTranslationResult* gnc_ko6ml_to_atomspace(Ko6mlPrimitive *primitive);

/** Translate AtomSpace handle back to ko6ml primitive
 * @param atom_handle AtomSpace handle to translate
 * @return ko6ml primitive (caller must free) or NULL if failed
 */
Ko6mlPrimitive* gnc_ko6ml_from_atomspace(GncAtomHandle atom_handle);

/** Round-trip translation test: ko6ml → AtomSpace → ko6ml
 * @param original Original ko6ml primitive
 * @return TRUE if round-trip preserves data integrity
 */
gboolean gnc_ko6ml_round_trip_test(Ko6mlPrimitive *original);

/** Validate translation accuracy between ko6ml and AtomSpace
 * @param primitive Original ko6ml primitive
 * @param result Translation result
 * @return Accuracy score [0.0, 1.0]
 */
gdouble gnc_ko6ml_validate_translation_accuracy(Ko6mlPrimitive *primitive, 
                                                Ko6mlTranslationResult *result);

/** @} */

/** @name Tensor Fragment Architecture */
/** @{ */

/** Create tensor shape from agent/state encoding
 * @param agent_id Agent identifier
 * @param state_data State data structure
 * @param modality Modality dimension
 * @param depth Cognitive depth
 * @param context Context dimension
 * @return Tensor shape structure
 */
Ko6mlTensorShape gnc_ko6ml_create_tensor_shape(const gchar *agent_id,
                                               gpointer state_data,
                                               gsize modality,
                                               gsize depth, 
                                               gsize context);

/** Validate tensor shape dimensions
 * @param shape Tensor shape to validate
 * @return TRUE if valid, FALSE otherwise
 */
gboolean gnc_ko6ml_validate_tensor_shape(Ko6mlTensorShape *shape);

/** Encode hypergraph node with tensor shape
 * @param primitive ko6ml primitive to encode
 * @param shape Tensor shape for encoding
 * @return AtomSpace handle with tensor encoding
 */
GncAtomHandle gnc_ko6ml_encode_hypergraph_node(Ko6mlPrimitive *primitive,
                                               Ko6mlTensorShape *shape);

/** Encode hypergraph link with tensor properties
 * @param source_primitive Source primitive
 * @param target_primitive Target primitive
 * @param relation_type Relation type
 * @param shape Tensor shape for link
 * @return AtomSpace handle for encoded link
 */
GncAtomHandle gnc_ko6ml_encode_hypergraph_link(Ko6mlPrimitive *source_primitive,
                                               Ko6mlPrimitive *target_primitive,
                                               const gchar *relation_type,
                                               Ko6mlTensorShape *shape);

/** @} */

/** @name Prime Factorization Mapping */
/** @{ */

/** Map tensor dimensions to prime factorization
 * @param shape Tensor shape to map
 * @return Array of prime factors (caller must free)
 */
guint* gnc_ko6ml_tensor_to_prime_factors(Ko6mlTensorShape *shape, gsize *n_factors);

/** Reconstruct tensor shape from prime factorization
 * @param prime_factors Array of prime factors
 * @param n_factors Number of factors
 * @return Reconstructed tensor shape
 */
Ko6mlTensorShape gnc_ko6ml_prime_factors_to_tensor(guint *prime_factors, gsize n_factors);

/** @} */

/** @name Performance and Optimization */
/** @{ */

/** Benchmark tensor operation performance
 * @param primitive ko6ml primitive for benchmarking
 * @param n_iterations Number of iterations to run
 * @return Average time per operation in microseconds
 */
gdouble gnc_ko6ml_benchmark_tensor_operations(Ko6mlPrimitive *primitive, gint n_iterations);

/** Optimize tensor shape for performance
 * @param shape Original tensor shape
 * @return Optimized tensor shape
 */
Ko6mlTensorShape gnc_ko6ml_optimize_tensor_shape(Ko6mlTensorShape *shape);

/** @} */

/** @name Utility Functions */
/** @{ */

/** Get string representation of primitive type
 * @param type Primitive type
 * @return String representation (do not free)
 */
const gchar* gnc_ko6ml_primitive_type_to_string(Ko6mlPrimitiveType type);

/** Parse primitive type from string
 * @param type_str String representation
 * @return Primitive type or -1 if invalid
 */
Ko6mlPrimitiveType gnc_ko6ml_primitive_type_from_string(const gchar *type_str);

/** Free translation result
 * @param result Translation result to free
 */
void gnc_ko6ml_translation_result_free(Ko6mlTranslationResult *result);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_KO6ML_PRIMITIVES_H */
/** @} */
/** @} */