#ifndef WIFI_PROV_HELPER_H
#define WIFI_PROV_HELPER_H

#include <WiFi.h>

// Function declarations for provisioning and event handling
void SysProvEvent(arduino_event_t *sys_event);
void setupProvisioning(const char *pop, const char *service_name, const char *service_key, bool reset_provisioned);
void updateConnectionStatus(const char *status, const char *service_name, const char *pop, const char *transport);
bool isProvisioned();  // Checks if Wi-Fi credentials are already provisioned
void resetProvisioning();  // Resets Wi-Fi provisioning by erasing stored credentials

void initiateNTPTimeSync();
void fetchBitcoinPrice(float &btcRate, float &highRate, float &lowRate, void (*updateUI)(float, float, float));
void updateBitcoinUI(float btcRate, float highRate, float lowRate);

#endif // WIFI_PROV_HELPER_H
