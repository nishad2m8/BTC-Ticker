#include "BianceCandleStick.h"
#include <lvgl.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <float.h>
#include <stdio.h>
#include <time.h>

#define CHART_WIDTH 480       // Increased chart width for better visibility
#define CHART_HEIGHT 180      // Increased chart height for better display
#define CANDLE_WIDTH 20       // Wider candle for better readability
#define CANDLE_PADDING 6      // Increased padding between candles
#define MAX_CANDLES 24        // Number of candles displayed
static long timestamps[MAX_CANDLES];
#define BINANCE_API_URL "https://api.binance.com/api/v3/klines"

// Structure to hold candle data
typedef struct {
    float open;
    float close;
    float high;
    float low;
} candle_t;

static candle_t candles[MAX_CANDLES];
static lv_obj_t *chart_container;

// Fetch candle data from Binance API
bool fetch_candle_data() {
    HTTPClient http;
    String url = String(BINANCE_API_URL) + "?symbol=BTCUSDT&interval=12h&limit=" + String(MAX_CANDLES);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("HTTP GET failed, code: %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    Serial.println("API Response: ");
    Serial.println(payload);  // Print the raw JSON response for debugging

    DynamicJsonDocument doc(8192); // Allocate memory for the dynamic JSON document

    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        http.end();
        return false;
    }

    for (int i = 0; i < MAX_CANDLES; i++) {
        JsonArray candle = doc[i].as<JsonArray>();
        timestamps[i] = candle[0].as<long>();  // Store the timestamp
        candles[i].open = candle[1].as<float>();
        candles[i].close = candle[4].as<float>();
        candles[i].high = candle[2].as<float>();
        candles[i].low = candle[3].as<float>();

        // Serial.printf("Candle %d - Open: %.2f, Close: %.2f, High: %.2f, Low: %.2f\n", i, candles[i].open, candles[i].close, candles[i].high, candles[i].low);
    }

    http.end();
    return true;
}

// Determine min and max price levels
static void get_price_levels(float *min_price, float *max_price) {
    *min_price = FLT_MAX;
    *max_price = FLT_MIN;

    for (int i = 0; i < MAX_CANDLES; i++) {
        if (candles[i].low < *min_price) {
            *min_price = candles[i].low;
        }
        if (candles[i].high > *max_price) {
            *max_price = candles[i].high;
        }
    }
}

// Draw a single candlestick with enhancements
static void draw_candlestick(lv_obj_t *parent, int index, float open, float close, float high, float low) {
    int x = index * (CANDLE_WIDTH + CANDLE_PADDING);
    int chart_height = CHART_HEIGHT;

    float min_price, max_price;
    get_price_levels(&min_price, &max_price);

    // Normalize prices to chart height
    int y_top = chart_height * (1.0f - (high - min_price) / (max_price - min_price));
    int y_bottom = chart_height * (1.0f - (low - min_price) / (max_price - min_price));
    int y_open = chart_height * (1.0f - (open - min_price) / (max_price - min_price));
    int y_close = chart_height * (1.0f - (close - min_price) / (max_price - min_price));

    // Ensure values are within bounds
    y_top = constrain(y_top, 0, chart_height);
    y_bottom = constrain(y_bottom, 0, chart_height);
    y_open = constrain(y_open, 0, chart_height);
    y_close = constrain(y_close, 0, chart_height);

    // Candle color: green for bullish, red for bearish
    lv_color_t candle_color = (close >= open) ? lv_palette_main(LV_PALETTE_GREEN) : lv_palette_main(LV_PALETTE_RED);

    // Create the wick as a rectangle (same color as the body)
    lv_obj_t *wick = lv_obj_create(parent);
    lv_obj_set_size(wick, CANDLE_WIDTH / 4, y_bottom - y_top);  // Thicker wick for better visibility
    lv_obj_set_style_bg_color(wick, candle_color, 0);
    lv_obj_align(wick, LV_ALIGN_TOP_LEFT, x + (CANDLE_WIDTH / 2) - (CANDLE_WIDTH / 8), y_top); // Center the wick
    lv_obj_set_style_radius(wick, 0, 0);
    lv_obj_set_scrollbar_mode(wick, LV_SCROLLBAR_MODE_OFF);

    // Create the body of the candlestick
    lv_obj_t *body = lv_obj_create(parent);
    int body_y = (y_open < y_close) ? y_open : y_close;
    lv_obj_set_size(body, CANDLE_WIDTH - 1, abs(y_close - y_open));
    lv_obj_set_style_bg_color(body, candle_color, 0);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, x, body_y);
    lv_obj_set_style_radius(body, 0, 0);
    lv_obj_set_scrollbar_mode(body, LV_SCROLLBAR_MODE_OFF);
    
}


// Draw X and Y axis labels with improvements
static void draw_axes_labels(lv_obj_t *parent, int labels_to_show) {
    float min_price, max_price;
    get_price_levels(&min_price, &max_price);

    float interval = (max_price - min_price) / 2;
    float price_levels[3] = {min_price, min_price + interval, max_price};

    int candle_step = (CANDLE_WIDTH + CANDLE_PADDING);
    int max_labels = CHART_WIDTH / (candle_step * 2);
    int step = (MAX_CANDLES - 1) / (labels_to_show - 1);

    // Draw X-axis labels (Date HH:MM format)
    for (int i = 0; i < MAX_CANDLES; i += step) {
        lv_obj_t *x_label = lv_label_create(parent);

        time_t raw_time = (time_t)(timestamps[i] / 1000);  // Convert milliseconds to seconds
        struct tm *time_info = localtime(&raw_time);

        // Format timestamp into "Day HH:MM"
        char buf[12];
        strftime(buf, sizeof(buf), "%d %H:%M", time_info); 

        lv_label_set_text(x_label, buf);
        lv_obj_set_style_text_color(x_label, lv_color_white(), 0);
        lv_obj_align(x_label, LV_ALIGN_BOTTOM_LEFT, i * candle_step + 2, 20);
    }

    // Draw Y-axis labels for price levels
    for (int i = 0; i < 3; i++) {
        lv_obj_t *y_label = lv_label_create(parent);
        char buf[8];
        snprintf(buf, sizeof(buf), "%.0f", price_levels[i]);
        lv_label_set_text(y_label, buf);

        int y_pos = CHART_HEIGHT * (1.0f - (price_levels[i] - min_price) / (max_price - min_price));
        lv_obj_align(y_label, LV_ALIGN_TOP_LEFT, -15, y_pos);
    }
}

// Add grid lines for visual clarity
static void draw_grid_lines(lv_obj_t *parent) {
    // Draw horizontal grid lines
    for (int i = 0; i < 3; i++) {
        lv_obj_t *h_line = lv_obj_create(parent);
        lv_obj_set_size(h_line, CHART_WIDTH, 1);
        lv_obj_set_style_bg_color(h_line, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_obj_align(h_line, LV_ALIGN_TOP_LEFT, 0, CHART_HEIGHT * i / 2);

        // Move the line to the background
        lv_obj_move_background(h_line);
    }

    // Draw vertical grid lines
    for (int i = 0; i < MAX_CANDLES; i += 4) {
        lv_obj_t *v_line = lv_obj_create(parent);
        lv_obj_set_size(v_line, 1, CHART_HEIGHT);
        lv_obj_set_style_bg_color(v_line, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_obj_align(v_line, LV_ALIGN_TOP_LEFT, i * (CANDLE_WIDTH + CANDLE_PADDING), 0);

        // Move the line to the background
        lv_obj_move_background(v_line);
    }
}


// Main function to create the candlestick chart
void biance_candle_stick_create(lv_obj_t *parent) {
    if (!fetch_candle_data()) {
        Serial.println("Failed to fetch candle data from Binance.");
        return;
    }

    chart_container = lv_obj_create(parent);
    lv_obj_set_size(chart_container, CHART_WIDTH, CHART_HEIGHT);
    lv_obj_align(chart_container, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_opa(chart_container, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(chart_container, LV_OBJ_FLAG_SCROLLABLE);

    // Draw all candlesticks
    for (int i = 0; i < MAX_CANDLES; i++) {
        draw_candlestick(chart_container, i, candles[i].open, candles[i].close, candles[i].high, candles[i].low);
    }

    // Draw the axes labels and grid lines
    draw_axes_labels(chart_container, 6);
    draw_grid_lines(chart_container);
}
