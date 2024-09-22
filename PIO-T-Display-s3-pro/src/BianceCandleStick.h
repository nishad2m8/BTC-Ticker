#ifndef BIANCE_CANDLESTICK_H
#define BIANCE_CANDLESTICK_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to fetch and display the candlestick chart from Binance API
void biance_candle_stick_create(lv_obj_t *parent);

// Declare the fetch_candle_data function here
bool fetch_candle_data();

#ifdef __cplusplus
}
#endif

#endif // BIANCE_CANDLESTICK_H
