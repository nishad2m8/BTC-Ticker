#ifndef WIFI_PROV_SCREEN_H
#define WIFI_PROV_SCREEN_H

#include <lvgl.h>

void ui_wifiProv_screen_init(void);
void showQRCodeInContainer(const char *service_name, const char *pop);
void updateConnectionStatus(const char *status, const char *service_name, const char *pop, const char *transport);

#endif // WIFI_PROV_SCREEN_H
