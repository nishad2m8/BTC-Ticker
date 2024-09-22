#include "WiFiProvScreen.h"
#include <WiFiProv.h>
#include <lvgl.h>

// Global LVGL objects for the Wi-Fi provisioning screen
lv_obj_t *ui_wifiProv;
lv_obj_t *ui_Container_QR;
lv_obj_t *ui_TextArea;

// Initialize the Wi-Fi provisioning screen UI
void ui_wifiProv_screen_init(void) {
    ui_wifiProv = lv_obj_create(NULL);

    ui_Container_QR = lv_obj_create(ui_wifiProv);
    lv_obj_set_size(ui_Container_QR, 150, 150);
    lv_obj_align(ui_Container_QR, LV_ALIGN_CENTER, 0, -50);

    ui_TextArea = lv_textarea_create(ui_wifiProv);
    lv_obj_set_size(ui_TextArea, 200, 100);
    lv_obj_align(ui_TextArea, LV_ALIGN_CENTER, 0, 100);
    lv_textarea_set_text(ui_TextArea, "Use ESP SoftAP Prov App to provision");
}

// Display the Wi-Fi Provisioning QR code in the QR container
void showQRCodeInContainer(const char *service_name, const char *pop) {
    char provQRCode[256];
    snprintf(provQRCode, sizeof(provQRCode), "{\"ver\":\"v1\",\"name\":\"%s\",\"pop\":\"%s\",\"transport\":\"softap\"}", service_name, pop);

    lv_obj_t *qrCode = lv_qrcode_create(ui_Container_QR, 180, lv_color_hex3(0x000), lv_color_hex3(0xFFF));
    if (qrCode != NULL) {
        lv_qrcode_update(qrCode, provQRCode, strlen(provQRCode));
        lv_obj_align(qrCode, LV_ALIGN_CENTER, 0, 0);
    } else {
        Serial.println("[ERROR] QR code creation failed!");
    }
}

// Update the connection status message in the text area
void updateConnectionStatus(const char *status, const char *service_name, const char *pop, const char *transport) {
    char message[256];
    snprintf(message, sizeof(message), "%s\nService: %s\nPOP: %s\nTransport: %s", status, service_name, pop, transport);
    lv_textarea_set_text(ui_TextArea, message);
}
