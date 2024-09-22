#include "WiFiProvHelper.h"
#include <WiFiProv.h>
#include <WiFi.h>

#include "ui.h"
#include "TimeHelper.h"
#include "BinanceWebSocket.h"  // Include the WebSocket header

extern "C" {
    #include "esp_wifi.h"
}

// Declare global variables if needed
extern float btcRate;
extern float highRate;
extern float lowRate;

extern const char *pop;
extern const char *service_name;

void SysProvEvent(arduino_event_t *sys_event) {
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("[DEBUG] Wi-Fi connected successfully.");
            updateConnectionStatus("Successfully Connected to Wi-Fi!", "Provisioning Complete", "Success", "softap");

            // Start background tasks (e.g., NTP sync) after a short delay
            delay(5000);
            initiateNTPTimeSync();  // Start NTP sync

            // Initialize Binance WebSocket for real-time Bitcoin data
            initBinanceWebSocket();

            // Load the main screen (ui_crypto)
            lv_scr_load(ui_crypto);
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[DEBUG] Disconnected from Wi-Fi.");
            break;

        case ARDUINO_EVENT_PROV_START:
            Serial.println("[DEBUG] Provisioning started. Please provide Wi-Fi credentials.");
            lv_scr_load(ui_wifiProv);  // Load ui_wifiProv.c for Wi-Fi provisioning
            updateConnectionStatus("Use ESP SoftAP Prov App to provision", service_name, pop, "softap");
            break;

        case ARDUINO_EVENT_PROV_END:
            Serial.println("[DEBUG] Provisioning process completed.");
            break;

        default:
            Serial.println("[DEBUG] Unhandled provisioning event.");
            break;
    }
}

bool isProvisioned() {
    wifi_config_t conf;
    esp_err_t ret = esp_wifi_get_config(WIFI_IF_STA, &conf);

    // Add debug information
    Serial.printf("[DEBUG] esp_wifi_get_config return value: %d\n", ret);
    Serial.printf("[DEBUG] Stored SSID: %s\n", (const char *)conf.sta.ssid);

    if (ret == ESP_OK && strlen((const char *)conf.sta.ssid) > 0) {
        return true;
    }
    return false;
}

void setupProvisioning(const char *pop, const char *service_name, const char *service_key, bool reset_provisioned) {
    WiFi.onEvent(SysProvEvent);

    // Start Wi-Fi provisioning using SoftAP
    WiFiProv.beginProvision(
        WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE,
        WIFI_PROV_SECURITY_1, pop, service_name, service_key, nullptr, reset_provisioned
    );
}

void resetProvisioning() {
    Serial.println("Resetting Wi-Fi credentials...");
    esp_wifi_restore();  // Restore Wi-Fi to default factory settings
    ESP.restart();       // Restart device to begin provisioning again
}
