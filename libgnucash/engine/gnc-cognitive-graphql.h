/********************************************************************\
 * gnc-cognitive-graphql.h -- GraphQL Interface for Complex Queries*
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
/** @addtogroup CognitiveGraphQL
    GraphQL interface for complex cognitive queries and real-time subscriptions
    @{ */

/** @file gnc-cognitive-graphql.h
    @brief GraphQL interface for cognitive data queries
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_COGNITIVE_GRAPHQL_H
#define GNC_COGNITIVE_GRAPHQL_H

#include "gnc-cognitive-api.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name GraphQL Schema Definition */
/** @{ */

/** GraphQL query context */
typedef struct {
    gchar *query;               /**< GraphQL query string */
    GHashTable *variables;      /**< Query variables */
    gchar *operation_name;      /**< Operation name (for multi-op queries) */
    gpointer context;           /**< User context data */
} GncGraphQLQuery;

/** GraphQL subscription context */
typedef struct {
    gchar *subscription_id;     /**< Unique subscription identifier */
    gchar *query;               /**< Subscription query string */
    GHashTable *variables;      /**< Query variables */
    gchar *connection_id;       /**< WebSocket connection ID */
    gboolean is_active;         /**< Subscription active state */
    gint64 created_time;        /**< Subscription creation time */
} GncGraphQLSubscription;

/** GraphQL execution result */
typedef struct {
    gchar *data;                /**< JSON result data */
    GList *errors;              /**< List of error messages */
    GHashTable *extensions;     /**< Extensions data */
    gboolean has_errors;        /**< Whether execution had errors */
} GncGraphQLResult;

/** @} */

/** @name GraphQL Schema */

/** GraphQL Schema Definition (SDL format) */
extern const gchar* GNC_GRAPHQL_SCHEMA;

/** Initialize GraphQL interface
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_graphql_init(void);

/** Shutdown GraphQL interface */
void gnc_graphql_shutdown(void);

/** Execute GraphQL query
 * @param query GraphQL query context
 * @return GraphQL execution result (caller must free)
 */
GncGraphQLResult* gnc_graphql_execute_query(const GncGraphQLQuery *query);

/** Execute GraphQL mutation
 * @param mutation GraphQL mutation context
 * @return GraphQL execution result (caller must free)
 */
GncGraphQLResult* gnc_graphql_execute_mutation(const GncGraphQLQuery *mutation);

/** Create GraphQL subscription
 * @param subscription Subscription context
 * @return TRUE if subscription was created successfully, FALSE otherwise
 */
gboolean gnc_graphql_create_subscription(const GncGraphQLSubscription *subscription);

/** Cancel GraphQL subscription
 * @param subscription_id Subscription identifier
 * @return TRUE if subscription was found and cancelled, FALSE otherwise
 */
gboolean gnc_graphql_cancel_subscription(const gchar *subscription_id);

/** @} */

/** @name Query Resolvers */
/** @{ */

/** Resolve cognitive state query */
gchar* gnc_graphql_resolve_cognitive_state(GHashTable *args, gpointer context);

/** Resolve attention allocation query */
gchar* gnc_graphql_resolve_attention_allocation(GHashTable *args, gpointer context);

/** Resolve tensor network status query */
gchar* gnc_graphql_resolve_tensor_network(GHashTable *args, gpointer context);

/** Resolve account query */
gchar* gnc_graphql_resolve_account(GHashTable *args, gpointer context);

/** Resolve transaction query */
gchar* gnc_graphql_resolve_transaction(GHashTable *args, gpointer context);

/** Resolve cognitive patterns query */
gchar* gnc_graphql_resolve_patterns(GHashTable *args, gpointer context);

/** Resolve agent status query */
gchar* gnc_graphql_resolve_agents(GHashTable *args, gpointer context);

/** @} */

/** @name Mutation Resolvers */
/** @{ */

/** Process cognitive task mutation */
gchar* gnc_graphql_mutate_process_task(GHashTable *args, gpointer context);

/** Register agent mutation */
gchar* gnc_graphql_mutate_register_agent(GHashTable *args, gpointer context);

/** Update attention allocation mutation */
gchar* gnc_graphql_mutate_update_attention(GHashTable *args, gpointer context);

/** Submit transaction mutation */
gchar* gnc_graphql_mutate_submit_transaction(GHashTable *args, gpointer context);

/** @} */

/** @name Subscription Resolvers */
/** @{ */

/** Subscribe to cognitive state changes */
gboolean gnc_graphql_subscribe_cognitive_state(const GncGraphQLSubscription *subscription);

/** Subscribe to attention allocation changes */
gboolean gnc_graphql_subscribe_attention_changes(const GncGraphQLSubscription *subscription);

/** Subscribe to transaction events */
gboolean gnc_graphql_subscribe_transaction_events(const GncGraphQLSubscription *subscription);

/** Subscribe to pattern discovery events */
gboolean gnc_graphql_subscribe_pattern_events(const GncGraphQLSubscription *subscription);

/** @} */

/** @name Utility Functions */
/** @{ */

/** Create GraphQL query context
 * @param query_string GraphQL query string
 * @param variables Query variables (can be NULL)
 * @param operation_name Operation name (can be NULL)
 * @return GraphQL query context (caller must free)
 */
GncGraphQLQuery* gnc_graphql_create_query(const gchar *query_string,
                                        GHashTable *variables,
                                        const gchar *operation_name);

/** Free GraphQL query context
 * @param query Query context to free
 */
void gnc_graphql_free_query(GncGraphQLQuery *query);

/** Create GraphQL subscription context
 * @param query_string Subscription query string
 * @param variables Query variables (can be NULL)
 * @param connection_id WebSocket connection ID
 * @return GraphQL subscription context (caller must free)
 */
GncGraphQLSubscription* gnc_graphql_create_subscription_context(const gchar *query_string,
                                                               GHashTable *variables,
                                                               const gchar *connection_id);

/** Free GraphQL subscription context
 * @param subscription Subscription context to free
 */
void gnc_graphql_free_subscription(GncGraphQLSubscription *subscription);

/** Free GraphQL result
 * @param result Result to free
 */
void gnc_graphql_free_result(GncGraphQLResult *result);

/** Convert GraphQL result to JSON string
 * @param result GraphQL execution result
 * @return JSON string (caller must free)
 */
gchar* gnc_graphql_result_to_json(const GncGraphQLResult *result);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_COGNITIVE_GRAPHQL_H */

/** @} */
/** @} */