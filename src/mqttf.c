#include "pico/stdlib.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include <stdio.h>
#include <string.h>

#define MQTT_SERVER "broker.hivemq.com"
//#define MQTT_PORT   1883
#define MQTT_TOPIC  "sensor/n2o"

// Timing
#define MQTT_RECONNECT_DELAY_MS   4000
#define MQTT_HEARTBEAT_MS        30000

// State
static mqtt_client_t *client = NULL;
static ip_addr_t server_ip;
static bool have_server_ip = false;

static bool reconnect_pending = false;
static absolute_time_t next_reconnect_due;
static absolute_time_t next_heartbeat_due;

static inline bool time_due(absolute_time_t t) {
    return absolute_time_diff_us(get_absolute_time(), t) <= 0;
}

// Forward decls
static void mqtt_start_connect(void);

// ---------------------- MQTT connection callback ----------------------
static void mqtt_connection_cb(mqtt_client_t *c, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connected and accepted!\n");
        reconnect_pending = false;
        next_heartbeat_due = make_timeout_time_ms(MQTT_HEARTBEAT_MS);
    } else {
        printf("MQTT disconnected or failed (status=%d). Scheduling reconnect...\n", status);
        reconnect_pending = true;
        next_reconnect_due = make_timeout_time_ms(MQTT_RECONNECT_DELAY_MS);
    }
}

// ---------------------- Resolve broker (once) ----------------------
static void resolve_broker_once(void)
{
    if (have_server_ip) return;

    printf("Resolving broker: %s...\n", MQTT_SERVER);
    err_t err = dns_gethostbyname(MQTT_SERVER, &server_ip, NULL, NULL);
    if (err == ERR_OK) {
        printf("Broker resolved: %s\n", ipaddr_ntoa(&server_ip));
        have_server_ip = true;
    } else if (err == ERR_INPROGRESS) {
        // lwIP will resolve asynchronously; try again next call
        printf("DNS request sent, waiting...\n");
    } else {
        printf("DNS resolve failed: %d\n", err);
        // Try again later
    }
}

// ---------------------- Begin a connection attempt ----------------------
static void mqtt_start_connect(void)
{
    if (!have_server_ip) return;

    if (client == NULL) {
        client = mqtt_client_new();
        if (!client) {
            printf("Failed to allocate MQTT client\n");
            reconnect_pending = true;
            next_reconnect_due = make_timeout_time_ms(MQTT_RECONNECT_DELAY_MS);
            return;
        }
    }

    struct mqtt_connect_client_info_t ci = {
        .client_id  = "PicoW_N2O",
        .keep_alive = 60,
    };

    printf("Connecting to MQTT broker...\n");
    err_t err = mqtt_client_connect(client, &server_ip, MQTT_PORT, mqtt_connection_cb, NULL, &ci);
    if (err == ERR_OK) {
        printf("MQTT connect request sent.\n");
    } else {
        printf("MQTT connect error: %d\n", err);
        reconnect_pending = true;
        next_reconnect_due = make_timeout_time_ms(MQTT_RECONNECT_DELAY_MS);
    }
}

// ---------------------- Public: init once after Wi-Fi ----------------------
void mqtt_init_connection(void)
{
    have_server_ip = false;
    reconnect_pending = false;
    client = NULL;

    // Resolve now (or soon) and kick a first connect
    resolve_broker_once();
    if (have_server_ip) mqtt_start_connect();

    // schedule first heartbeat window regardless
    next_heartbeat_due = make_timeout_time_ms(MQTT_HEARTBEAT_MS);
}

// ---------------------- Public: call every loop iteration ----------------------
void mqtt_service(void)
{
    //  Ensure broker IP known
    if (!have_server_ip) {
        resolve_broker_once();
        if (have_server_ip && (client == NULL || !mqtt_client_is_connected(client))) {
            // Start first connection after resolution
            mqtt_start_connect();
        }
    }

    //  Handle scheduled reconnect
    if (reconnect_pending && time_due(next_reconnect_due)) {
        reconnect_pending = false;

        if (client) {
            mqtt_client_free(client);
            client = NULL; // recreated in start_connect
        }
        mqtt_start_connect();
    }

    //  Heartbeat
    if (client && mqtt_client_is_connected(client) && time_due(next_heartbeat_due)) {
        next_heartbeat_due = make_timeout_time_ms(MQTT_HEARTBEAT_MS);
        const char *msg = "{\"status\":\"alive\"}";
        err_t err = mqtt_publish(client, MQTT_TOPIC, msg, strlen(msg), 0, 0, NULL, NULL);
        if (err == ERR_OK) {
            printf("Heartbeat sent\n");
        } else {
            printf("Heartbeat publish failed: %d\n", err);
        }
    }
}

// ---------------------- Public: publish one reading ----------------------
void mqtt_publish_reading(float reading)
{
    if (!client || !mqtt_client_is_connected(client)) {
        // Not connected; schedule a reconnect (service() will handle it)
        printf("MQTT not connected, scheduling reconnect...\n");
        reconnect_pending = true;
        next_reconnect_due = make_timeout_time_ms(MQTT_RECONNECT_DELAY_MS);
        return;
    }
    // ----------- Publish rate limiter (IMPORTANT!) ------------
    static absolute_time_t next_pub_allowed = 0;

    if (!time_reached(next_pub_allowed)) {
        printf("Publish skipped (rate limit)\n");
        return;
    }

    // allow next publish 500 ms later
    next_pub_allowed = make_timeout_time_ms(500);
    // -----

    char payload[64];
    snprintf(payload, sizeof(payload), "{\"gas1\": %.2f}", reading);

    err_t err = mqtt_publish(client, MQTT_TOPIC, payload, strlen(payload), 0, 0, NULL, NULL);
    if (err == ERR_OK) {
        printf("Published successfully: %s\n", payload);
    } else {
        printf("Publish failed: %d\n", err);
    }
    sleep_ms(100);   // give lwIP time to flush
}
