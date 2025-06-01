#ifndef CONECT_TOPICOS_H
#define CONECT_TOPICOS_H

#include "lib/mqtt_client.h"
#include "lib/matrixws.h"

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
#if MQTT_UNIQUE_TOPIC
    const char *basic_topic = state->topic + strlen(state->mqtt_client_info.client_id) + 1;
#else
    const char *basic_topic = state->topic;
#endif
    strncpy(state->data, (const char *)data, len);
    state->len = len;
    state->data[len] = '\0';

    INFO_printf("[DEBUG] Recebido comando no tópico %s: %s\n", basic_topic, state->data);
    DEBUG_printf("Topic: %s, Message: %s\n", state->topic, state->data);
    // Organize topic handling using a switch for better structure
    enum topic_cmd {
        TOPIC_LED,
        TOPIC_PRINT,
        TOPIC_MATRIZ,
        TOPIC_PING,
        TOPIC_EXIT,
        TOPIC_UNKNOWN
    } topic = TOPIC_UNKNOWN;

    if (strcmp(basic_topic, "/led") == 0) topic = TOPIC_LED;
    else if (strcmp(basic_topic, "/print") == 0) topic = TOPIC_PRINT;
    else if (strcmp(basic_topic, "/matriz") == 0) topic = TOPIC_MATRIZ;
    else if (strcmp(basic_topic, "/ping") == 0) topic = TOPIC_PING;
    else if (strcmp(basic_topic, "/exit") == 0) topic = TOPIC_EXIT;

    switch (topic) {
        case TOPIC_LED: {
            INFO_printf("[DEBUG] Comando recebido: %s\n", state->data);
            // Subcomandos de LED
            enum led_cmd {
                LED_CMD_NONE,
                LED_CMD_RED_ON,
                LED_CMD_RED_OFF,
                LED_CMD_BLUE_ON,
                LED_CMD_BLUE_OFF,
                LED_CMD_GREEN_ON,
                LED_CMD_GREEN_OFF,
                LED_CMD_SIMPLE_ON,
                LED_CMD_SIMPLE_OFF,
                LED_CMD_OFF
            } cmd = LED_CMD_NONE;

                if (strcmp((const char *)state->data, "led_red_on") == 0) cmd = LED_CMD_RED_ON;
                else if (strcmp((const char *)state->data, "led_red_off") == 0) cmd = LED_CMD_RED_OFF;
                else if (strcmp((const char *)state->data, "led_blue_on") == 0) cmd = LED_CMD_BLUE_ON;
                else if (strcmp((const char *)state->data, "led_blue_off") == 0) cmd = LED_CMD_BLUE_OFF;
                else if (strcmp((const char *)state->data, "led_green_on") == 0) cmd = LED_CMD_GREEN_ON;
                else if (strcmp((const char *)state->data, "led_green_off") == 0) cmd = LED_CMD_GREEN_OFF;
                else if (lwip_stricmp((const char *)state->data, "On") == 0 || strcmp((const char *)state->data, "1") == 0) cmd = LED_CMD_SIMPLE_ON;
                else if (lwip_stricmp((const char *)state->data, "Off") == 0 || strcmp((const char *)state->data, "0") == 0) cmd = LED_CMD_SIMPLE_OFF;
                else if (strcmp((const char *)state->data, "leds_off") == 0) cmd = LED_CMD_OFF;

            switch (cmd) {
                case LED_CMD_RED_ON:
                    control_red_led(true);
                    break;
                case LED_CMD_RED_OFF:
                    control_red_led(false);
                    break;
                case LED_CMD_BLUE_ON:
                    control_blue_led(true);
                    break;
                case LED_CMD_BLUE_OFF:
                    control_blue_led(false);
                    break;
                case LED_CMD_GREEN_ON:
                    control_green_led(true);
                    break;
                case LED_CMD_GREEN_OFF:
                    control_green_led(false);
                    break;
                case LED_CMD_SIMPLE_ON:
                    control_led(state, true);
                    break;
                case LED_CMD_SIMPLE_OFF:
                    control_led(state, false);
                    break;
                case LED_CMD_OFF:
                    control_red_led(false);
                    control_blue_led(false);
                    control_green_led(false);
                    break;
                default:
                    INFO_printf("[DEBUG] Comando de LED desconhecido: %s\n", state->data);
                    break;
            }
            break;
        }
        case TOPIC_PRINT: // Escreve uma mensagem no monitor serial
            INFO_printf("%.*s\n", len, data);
            break;
        case TOPIC_MATRIZ: {
            // Desliga todos os LEDs da matriz se o comando for "desliga"
            if (strcmp((const char *)state->data, "desliga") == 0) {
            INFO_printf("[MATRIZ] Desligando todos os LEDs da matriz\n");
            for (int i = 0; i < 25; i++) {
                cores_matriz((uint)i, 0, 0, 0);
            }
            bf(); // Atualiza a matriz
            } else {
            // Liga um LED individual da matriz 5x5 usando a função cores_matriz
            int led_num = 0;
            if (sscanf((const char *)state->data, "led%d", &led_num) == 1 && led_num >= 1 && led_num <= 25) {
                INFO_printf("[MATRIZ] Ligando LED %d da matriz\n", led_num);
                // Exemplo: cor vermelha (R=BRILHO, G=0, B=0)
                cores_matriz((uint)(led_num-1), BRILHO, 0, 0);
                bf(); // Atualiza a matriz
            } else {
                INFO_printf("[MATRIZ] Comando inválido para matriz: %s\n", state->data);
            }
            }
            break;
        }
        case TOPIC_PING: {
            char buf[11];
            snprintf(buf, sizeof(buf), "%u", to_ms_since_boot(get_absolute_time()) / 1000);
            mqtt_publish(state->mqtt_client_inst, full_topic(state, "/uptime"), buf, strlen(buf), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
            break;
        }
        case TOPIC_EXIT:
            state->stop_client = true; // stop the client when ALL subscriptions are stopped
            sub_unsub_topics(state, false); // unsubscribe
            break;
        default:
            // Unknown topic, do nothing or log
            break;
    }
}

void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
    if (status == MQTT_CONNECT_ACCEPTED) {
        state->connect_done = true;
        sub_unsub_topics(state, true); // subscribe;

        // indicate online
        if (state->mqtt_client_info.will_topic) {
            mqtt_publish(state->mqtt_client_inst, state->mqtt_client_info.will_topic, "1", 1, MQTT_WILL_QOS, true, pub_request_cb, state);
        }

        // Publish temperature every 10 sec if it's changed
        temperature_worker.user_data = state;
        async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &temperature_worker, 0);
    } else if (status == MQTT_CONNECT_DISCONNECTED) {
        if (!state->connect_done) {
            panic("Failed to connect to mqtt server");
        }
    }
    else {
        panic("Unexpected status");
    }
}

void start_client(MQTT_CLIENT_DATA_T *state) {
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    const int port = MQTT_TLS_PORT;
    INFO_printf("Using TLS\n");
#else
    const int port = MQTT_PORT;
    INFO_printf("Warning: Not using TLS\n");
#endif

    state->mqtt_client_inst = mqtt_client_new();
    if (!state->mqtt_client_inst) {
        panic("MQTT client instance creation error");
    }
    INFO_printf("IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
    INFO_printf("Connecting to mqtt server at %s\n", ipaddr_ntoa(&state->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->mqtt_client_inst, &state->mqtt_server_address, port, mqtt_connection_cb, state, &state->mqtt_client_info) != ERR_OK) {
        panic("MQTT broker connection error");
    }
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    // This is important for MBEDTLS_SSL_SERVER_NAME_INDICATION
    mbedtls_ssl_set_hostname(altcp_tls_context(state->mqtt_client_inst->conn), MQTT_SERVER);
#endif
    mqtt_set_inpub_callback(state->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, state);
    cyw43_arch_lwip_end();
}

void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T*)arg;
    if (ipaddr) {
        state->mqtt_server_address = *ipaddr;
        start_client(state);
    } else {
        panic("dns request failed");
    }
}

#endif