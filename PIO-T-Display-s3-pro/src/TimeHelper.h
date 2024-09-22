#ifndef TIME_HELPER_H
#define TIME_HELPER_H

// Function declarations for time syncing and updating the UI
void initiateNTPTimeSync();  // Initiates non-blocking NTP sync
bool isTimeSynchronized();   // Returns true if NTP time is synced
void updateTimeAndDate();    // Updates the UI with current time and date

#endif // TIME_HELPER_H
