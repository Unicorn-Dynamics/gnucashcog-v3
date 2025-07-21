/********************************************************************\
 * gnc-cognitive-api.h -- Phase 4 Distributed Cognitive Mesh API  *
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
/** @addtogroup CognitiveAPI
    Phase 4: Distributed Cognitive Mesh API & Embodiment Layer
    REST/WebSocket APIs and bindings for Unity3D, ROS, and web agents
    @{ */

/** @file gnc-cognitive-api.h
    @brief Distributed Cognitive Mesh API for embodied cognition
    @author Copyright (C) 2024 GnuCash Cognitive Engine
*/

#ifndef GNC_COGNITIVE_API_H
#define GNC_COGNITIVE_API_H

#include "gnc-cognitive-accounting.h"
#include "gnc-tensor-network.h"
#include "qof.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @name API Server Management */
/** @{ */

/** HTTP status codes for API responses */
typedef enum {
    GNC_API_STATUS_OK = 200,
    GNC_API_STATUS_CREATED = 201,
    GNC_API_STATUS_ACCEPTED = 202,
    GNC_API_STATUS_BAD_REQUEST = 400,
    GNC_API_STATUS_UNAUTHORIZED = 401,
    GNC_API_STATUS_FORBIDDEN = 403,
    GNC_API_STATUS_NOT_FOUND = 404,
    GNC_API_STATUS_CONFLICT = 409,
    GNC_API_STATUS_INTERNAL_ERROR = 500,
    GNC_API_STATUS_NOT_IMPLEMENTED = 501,
    GNC_API_STATUS_SERVICE_UNAVAILABLE = 503
} GncApiStatus;

/** API request structure */
typedef struct {
    gchar *method;              /**< HTTP method (GET, POST, PUT, DELETE) */
    gchar *path;                /**< Request path */
    gchar *query_string;        /**< Query string parameters */
    GHashTable *headers;        /**< Request headers */
    gchar *body;                /**< Request body */
    gsize body_length;          /**< Body length */
    gpointer user_data;         /**< User-specific data */
} GncApiRequest;

/** API response structure */
typedef struct {
    GncApiStatus status_code;   /**< HTTP status code */
    GHashTable *headers;        /**< Response headers */
    gchar *body;                /**< Response body */
    gsize body_length;          /**< Body length */
    gchar *content_type;        /**< Content type */
} GncApiResponse;

/** API endpoint handler function */
typedef GncApiResponse* (*GncApiHandler)(const GncApiRequest *request);

/** WebSocket connection context */
typedef struct {
    gchar *connection_id;       /**< Unique connection identifier */
    gchar *agent_type;          /**< Type of connected agent */
    gboolean is_active;         /**< Connection active state */
    gint64 connect_time;        /**< Connection timestamp */
    GHashTable *subscriptions;  /**< Event subscriptions */
    gpointer user_data;         /**< Agent-specific data */
} GncWebSocketConnection;

/** WebSocket event structure */
typedef struct {
    gchar *event_type;          /**< Event type name */
    gchar *data;                /**< JSON event data */
    gint64 timestamp;           /**< Event timestamp */
    gdouble priority;           /**< Event priority */
} GncWebSocketEvent;

/** Initialize the Cognitive API server
 * @param port Port number for the HTTP server
 * @param websocket_port Port number for WebSocket server
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_cognitive_api_init(gint port, gint websocket_port);

/** Shutdown the Cognitive API server */
void gnc_cognitive_api_shutdown(void);

/** Register API endpoint handler
 * @param method HTTP method (GET, POST, PUT, DELETE)
 * @param path URL path pattern
 * @param handler Handler function
 * @return TRUE on successful registration, FALSE otherwise
 */
gboolean gnc_cognitive_api_register_handler(const gchar *method, 
                                           const gchar *path, 
                                           GncApiHandler handler);

/** Start the API server
 * @return TRUE if server started successfully, FALSE otherwise
 */
gboolean gnc_cognitive_api_start_server(void);

/** Stop the API server */
void gnc_cognitive_api_stop_server(void);

/** @} */

/** @name REST API Endpoints */
/** @{ */

/** Get current cognitive state
 * GET /api/v1/cognitive/state
 * @param request API request
 * @return JSON response with cognitive state
 */
GncApiResponse* gnc_api_get_cognitive_state(const GncApiRequest *request);

/** Process cognitive task
 * POST /api/v1/cognitive/process
 * @param request API request with task data
 * @return JSON response with processing result
 */
GncApiResponse* gnc_api_process_cognitive_task(const GncApiRequest *request);

/** Get attention allocation
 * GET /api/v1/attention/allocation
 * @param request API request
 * @return JSON response with attention distribution
 */
GncApiResponse* gnc_api_get_attention_allocation(const GncApiRequest *request);

/** Register new agent
 * POST /api/v1/agents/register
 * @param request API request with agent data
 * @return JSON response with agent registration
 */
GncApiResponse* gnc_api_register_agent(const GncApiRequest *request);

/** Unregister agent
 * DELETE /api/v1/agents/{id}
 * @param request API request with agent ID
 * @return JSON response confirming unregistration
 */
GncApiResponse* gnc_api_unregister_agent(const GncApiRequest *request);

/** Get tensor network status
 * GET /api/v1/network/status
 * @param request API request
 * @return JSON response with network status
 */
GncApiResponse* gnc_api_get_network_status(const GncApiRequest *request);

/** Submit transaction for processing
 * POST /api/v1/transactions/submit
 * @param request API request with transaction data
 * @return JSON response with processing result
 */
GncApiResponse* gnc_api_submit_transaction(const GncApiRequest *request);

/** Get account information
 * GET /api/v1/accounts/{id}
 * @param request API request with account ID
 * @return JSON response with account data
 */
GncApiResponse* gnc_api_get_account(const GncApiRequest *request);

/** @} */

/** @name WebSocket Communication */
/** @{ */

/** Initialize WebSocket server
 * @param port WebSocket server port
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_websocket_init(gint port);

/** Shutdown WebSocket server */
void gnc_websocket_shutdown(void);

/** Register WebSocket connection
 * @param connection_id Unique connection identifier
 * @param agent_type Type of connecting agent
 * @return WebSocket connection context
 */
GncWebSocketConnection* gnc_websocket_register_connection(const gchar *connection_id,
                                                         const gchar *agent_type);

/** Unregister WebSocket connection
 * @param connection_id Connection identifier
 * @return TRUE if connection was found and removed, FALSE otherwise
 */
gboolean gnc_websocket_unregister_connection(const gchar *connection_id);

/** Subscribe to WebSocket events
 * @param connection_id Connection identifier
 * @param event_type Event type to subscribe to
 * @return TRUE on successful subscription, FALSE otherwise
 */
gboolean gnc_websocket_subscribe(const gchar *connection_id, const gchar *event_type);

/** Unsubscribe from WebSocket events
 * @param connection_id Connection identifier
 * @param event_type Event type to unsubscribe from
 * @return TRUE on successful unsubscription, FALSE otherwise
 */
gboolean gnc_websocket_unsubscribe(const gchar *connection_id, const gchar *event_type);

/** Broadcast WebSocket event to all subscribers
 * @param event_type Event type
 * @param data JSON event data
 * @return Number of connections that received the event
 */
gint gnc_websocket_broadcast_event(const gchar *event_type, const gchar *data);

/** Send WebSocket event to specific connection
 * @param connection_id Target connection identifier
 * @param event_type Event type
 * @param data JSON event data
 * @return TRUE if event was sent, FALSE otherwise
 */
gboolean gnc_websocket_send_event(const gchar *connection_id,
                                  const gchar *event_type, 
                                  const gchar *data);

/** @} */

/** @name Unity3D Integration */
/** @{ */

/** Unity3D cognitive component structure */
typedef struct {
    gchar *component_id;        /**< Component identifier */
    gchar *component_type;      /**< Type of cognitive component */
    gdouble position[3];        /**< 3D position (x, y, z) */
    gdouble rotation[4];        /**< Quaternion rotation (x, y, z, w) */
    GncAttentionParams attention; /**< Attention parameters */
    GHashTable *properties;     /**< Component properties */
} GncUnityComponent;

/** Initialize Unity3D cognitive integration
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_unity_init(void);

/** Shutdown Unity3D integration */
void gnc_unity_shutdown(void);

/** Create Unity3D cognitive component
 * @param component_type Type of component to create
 * @param position 3D position array [x, y, z]
 * @param rotation Quaternion rotation array [x, y, z, w]
 * @return Unity component structure
 */
GncUnityComponent* gnc_unity_create_component(const gchar *component_type,
                                            gdouble position[3],
                                            gdouble rotation[4]);

/** Update Unity3D component attention visualization
 * @param component Unity component to update
 * @param attention_params New attention parameters
 * @return TRUE on successful update, FALSE otherwise
 */
gboolean gnc_unity_update_attention_visualization(GncUnityComponent *component,
                                                 const GncAttentionParams *attention_params);

/** Get Unity3D cognitive state representation
 * @return JSON string with cognitive state for Unity (caller must free)
 */
gchar* gnc_unity_get_cognitive_state_json(void);

/** Process Unity3D input commands
 * @param command JSON command from Unity
 * @return JSON response for Unity (caller must free)
 */
gchar* gnc_unity_process_command(const gchar *command);

/** @} */

/** @name ROS Integration */
/** @{ */

/** ROS cognitive node structure */
typedef struct {
    gchar *node_name;           /**< ROS node name */
    gchar *node_namespace;      /**< ROS node namespace */
    gboolean is_active;         /**< Node active state */
    GHashTable *topics;         /**< Published/subscribed topics */
    GHashTable *services;       /**< Provided services */
    GncAttentionParams attention; /**< Attention parameters */
} GncRosNode;

/** ROS message structure */
typedef struct {
    gchar *topic;               /**< Topic name */
    gchar *message_type;        /**< ROS message type */
    gchar *data;                /**< JSON message data */
    gint64 timestamp;           /**< Message timestamp */
    gdouble priority;           /**< Message priority */
} GncRosMessage;

/** Initialize ROS cognitive integration
 * @param node_name ROS node name
 * @param node_namespace ROS node namespace
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_ros_init(const gchar *node_name, const gchar *node_namespace);

/** Shutdown ROS integration */
void gnc_ros_shutdown(void);

/** Create ROS cognitive service
 * @param service_name Service name
 * @param service_type ROS service type
 * @return TRUE on successful service creation, FALSE otherwise
 */
gboolean gnc_ros_create_service(const gchar *service_name, const gchar *service_type);

/** Publish ROS cognitive message
 * @param topic Topic to publish to
 * @param message_type ROS message type
 * @param data JSON message data
 * @return TRUE on successful publish, FALSE otherwise
 */
gboolean gnc_ros_publish_message(const gchar *topic,
                                const gchar *message_type,
                                const gchar *data);

/** Subscribe to ROS topic
 * @param topic Topic to subscribe to
 * @param message_type Expected message type
 * @return TRUE on successful subscription, FALSE otherwise
 */
gboolean gnc_ros_subscribe_topic(const gchar *topic, const gchar *message_type);

/** Get ROS cognitive planning result
 * @param goal Planning goal specification
 * @return JSON planning result (caller must free)
 */
gchar* gnc_ros_get_planning_result(const gchar *goal);

/** Process ROS sensor data with cognitive analysis
 * @param sensor_data JSON sensor data
 * @return JSON cognitive analysis result (caller must free)
 */
gchar* gnc_ros_process_sensor_data(const gchar *sensor_data);

/** @} */

/** @name Web Agent Interface */
/** @{ */

/** Web agent context */
typedef struct {
    gchar *agent_id;            /**< Agent identifier */
    gchar *agent_type;          /**< Agent type */
    gchar *user_agent;          /**< Browser user agent */
    gchar *session_id;          /**< Session identifier */
    gboolean is_authenticated;  /**< Authentication state */
    GHashTable *capabilities;   /**< Agent capabilities */
    gint64 last_activity;       /**< Last activity timestamp */
} GncWebAgent;

/** Initialize web agent interface
 * @return TRUE on successful initialization, FALSE otherwise
 */
gboolean gnc_web_agent_init(void);

/** Shutdown web agent interface */
void gnc_web_agent_shutdown(void);

/** Register web agent
 * @param agent_type Agent type identifier
 * @param capabilities Agent capabilities
 * @param session_id Session identifier
 * @return Web agent context
 */
GncWebAgent* gnc_web_agent_register(const gchar *agent_type,
                                   GHashTable *capabilities,
                                   const gchar *session_id);

/** Unregister web agent
 * @param agent_id Agent identifier
 * @return TRUE if agent was found and removed, FALSE otherwise
 */
gboolean gnc_web_agent_unregister(const gchar *agent_id);

/** Process web agent command
 * @param agent_id Agent identifier
 * @param command JSON command
 * @return JSON response (caller must free)
 */
gchar* gnc_web_agent_process_command(const gchar *agent_id, const gchar *command);

/** Get web agent cognitive interface
 * @param agent_id Agent identifier
 * @return JSON interface description (caller must free)
 */
gchar* gnc_web_agent_get_interface(const gchar *agent_id);

/** @} */

/** @name Performance Monitoring */
/** @{ */

/** API performance metrics */
typedef struct {
    gdouble avg_response_time;  /**< Average response time in milliseconds */
    gint total_requests;        /**< Total number of requests */
    gint successful_requests;   /**< Number of successful requests */
    gint failed_requests;       /**< Number of failed requests */
    gdouble requests_per_second; /**< Current requests per second */
    gint active_connections;    /**< Active WebSocket connections */
    gdouble memory_usage_mb;    /**< Memory usage in megabytes */
    gdouble cpu_usage_percent;  /**< CPU usage percentage */
} GncApiMetrics;

/** Get API performance metrics
 * @return Current API performance metrics
 */
GncApiMetrics gnc_cognitive_api_get_metrics(void);

/** Reset API performance counters */
void gnc_cognitive_api_reset_metrics(void);

/** Enable/disable performance monitoring
 * @param enabled TRUE to enable monitoring, FALSE to disable
 */
void gnc_cognitive_api_set_monitoring(gboolean enabled);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GNC_COGNITIVE_API_H */

/** @} */
/** @} */