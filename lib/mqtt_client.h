#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"

#ifndef MQTT_TOPIC_LEN
#define MQTT_TOPIC_LEN 100
#endif

#ifndef MQTT_OUTPUT_RINGBUF_SIZE
#define MQTT_OUTPUT_RINGBUF_SIZE 256
#endif

typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
} MQTT_CLIENT_DATA_T;

void pub_request_cb(void *arg, err_t err);
const char *full_topic(MQTT_CLIENT_DATA_T *state, const char *name);
void publish_temperature(MQTT_CLIENT_DATA_T *state);
void sub_request_cb(void *arg, err_t err);
void unsub_request_cb(void *arg, err_t err);
void sub_unsub_topics(MQTT_CLIENT_DATA_T* state, bool sub);
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
void temperature_worker_fn(async_context_t *context, async_at_time_worker_t *worker);
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void start_client(MQTT_CLIENT_DATA_T *state);
void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);

#endif // MQTT_CLIENT_H
