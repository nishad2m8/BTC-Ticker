#include "TimeHelper.h"
#include "Config.h"  // Include the configuration header
#include <Arduino.h>
#include <time.h>
#include <lvgl.h>
#include "ui.h"

// Variable to track if NTP synchronization has been initiated
static bool ntpSyncInProgress = false;

// Syncs time with the NTP server in a non-blocking way
void initiateNTPTimeSync() {
    if (!ntpSyncInProgress) {
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
        ntpSyncInProgress = true;
        Serial.println("Started NTP time sync...");

        time_t now = time(nullptr);
        while (now < 8 * 3600 * 2) {
            delay(500);
            now = time(nullptr);
        }

        // Print the raw UTC time to ensure synchronization
        Serial.print("Raw UTC Time: ");
        Serial.println(asctime(gmtime(&now)));

        // Apply time zone offset if necessary
        setenv("TZ", TIME_ZONE, 1);
        tzset();
        struct tm *timeInfo = localtime(&now);
        Serial.print("Local Time: ");
        Serial.println(asctime(timeInfo));
        
    }
}
