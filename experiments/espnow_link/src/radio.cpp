#include "radio.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <cstring>
#include <cstdio>
#include "local_config.h"

#if ESP_IDF_VERSION != ESP_IDF_VERSION_VAL(4,4,7)
#error "Review callback API and pin the approved SDK"
#endif
#if defined(LINK_TX) == defined(LINK_RX)
#error "Select exactly one role"
#endif
namespace radio {
#if defined(LINK_TX)
static const uint8_t* const self_mac = tx_mac;
static const uint8_t* const other_mac = rx_mac;
#else
static const uint8_t* const self_mac = rx_mac;
static const uint8_t* const other_mac = tx_mac;
#endif
static QueueHandle_t queue = nullptr;
static uint32_t dropped = 0;
static portMUX_TYPE drop_lock = portMUX_INITIALIZER_UNLOCKED;
static void post(const Event& e) {
    if (xQueueSend(queue, &e, 0) != pdTRUE) {
        portENTER_CRITICAL(&drop_lock); ++dropped; portEXIT_CRITICAL(&drop_lock);
    }
}
static void on_receive(const uint8_t* mac, const uint8_t* data, int n) {
    Event e{}; e.ms = millis(); e.length = n;
    if (mac) memcpy(e.peer, mac, 6);
    if (data && n == int(link::wire_size)) memcpy(e.bytes, data, link::wire_size);
    else if (!data) e.length = -1;
    post(e);
}
static void on_send(const uint8_t* mac, esp_now_send_status_t status) {
    Event e{}; e.send = true; e.ms = millis(); e.success = status == ESP_NOW_SEND_SUCCESS;
    if (mac) memcpy(e.peer, mac, 6);
    post(e);
}
static bool check(esp_err_t err, const char* step) {
    if (err == ESP_OK) return true;
    Serial.printf("INIT_ERROR step=%s code=%d\n", step, int(err)); return false;
}
bool channel_ok() {
    uint8_t actual = 0; wifi_second_chan_t secondary;
    return esp_wifi_get_channel(&actual, &secondary) == ESP_OK && actual == channel;
}
bool init() {
    queue = xQueueCreate(32, sizeof(Event));
    if (!queue) return false;
    WiFi.persistent(false);
    if (!WiFi.mode(WIFI_STA)) return false;
    WiFi.setAutoReconnect(false);
    const esp_err_t disconnected = esp_wifi_disconnect();
    if (disconnected != ESP_OK && disconnected != ESP_ERR_WIFI_NOT_CONNECT) {
        check(disconnected, "disconnect"); return false;
    }
    if (!check(esp_wifi_set_ps(WIFI_PS_NONE), "power_save") ||
        !check(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE), "channel") || !channel_ok()) return false;
    uint8_t actual[6]{};
    if (!check(esp_wifi_get_mac(WIFI_IF_STA, actual), "sta_mac") || memcmp(actual, self_mac, 6)) {
        Serial.println("LOCAL_MAC_MISMATCH"); return false;
    }
    if (!check(esp_now_init(), "esp_now") || !check(esp_now_set_pmk(pmk), "pmk")) return false;
    esp_now_peer_info_t p{};
    memcpy(p.peer_addr, other_mac, 6); memcpy(p.lmk, lmk, 16);
    p.channel = channel; p.ifidx = WIFI_IF_STA; p.encrypt = true;
    if (!check(esp_now_add_peer(&p), "encrypted_peer") ||
        !check(esp_now_register_recv_cb(on_receive), "recv_cb") ||
        !check(esp_now_register_send_cb(on_send), "send_cb")) return false;
    Serial.printf("RADIO_READY actual_channel=%u encrypted=1 peer_configured=1 sdk=%s build=%s\n",
                  unsigned(channel), ESP.getSdkVersion(), LINK_BUILD_SHA);
    return true;
}
bool pop(Event& e) { return xQueueReceive(queue, &e, 0) == pdTRUE; }
uint32_t drops() { portENTER_CRITICAL(&drop_lock); uint32_t n=dropped; portEXIT_CRITICAL(&drop_lock); return n; }
const uint8_t* peer() { return other_mac; }
bool send(const link::Wire& bytes) {
    return channel_ok() && esp_now_send(other_mac, bytes, link::wire_size) == ESP_OK;
}
bool command(char (&line)[48]) {
    static size_t used = 0; static bool overflow = false;
    for (unsigned i=0; i<48 && Serial.available(); ++i) {
        int c = Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            bool ok = !overflow && used;
            line[used] = 0; used = 0; overflow = false; return ok;
        }
        if (used < sizeof(line)-1) line[used++] = char(c); else overflow = true;
    }
    return false;
}
bool parse_start(const char* line, uint32_t& run, char& mode) {
    // Exact "start XXXXXXXX m" with hex run ID and one mode.
    if (strlen(line) != 16 || strncmp(line,"start ",6) || line[14]!=' ') return false;
    uint32_t value=0;
    for (int i=6;i<14;++i) {
        char c=line[i]; unsigned v;
        if(c>='0'&&c<='9') v=c-'0';
        else if(c>='a'&&c<='f') v=c-'a'+10;
        else if(c>='A'&&c<='F') v=c-'A'+10;
        else return false;
        value=(value<<4)|v;
    }
    mode=line[15]; if (!value || (mode!='s'&&mode!='b'&&mode!='p'&&mode!='n')) return false;
    run=value; return true;
}
}
