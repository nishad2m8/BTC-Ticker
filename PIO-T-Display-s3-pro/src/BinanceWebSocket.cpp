// BinanceWebSocket.cpp

#include "BinanceWebSocket.h"
#include <WiFi.h>

// Global WebSocket client instance
WebSocketsClient webSocket;

float btcRate = 0.0;
float highRate = 0.0;
float lowRate = 0.0;

// Function to handle incoming WebSocket messages
void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT: {
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            // Handle different types of messages
            if (doc.containsKey("e") && doc["e"] == "24hrTicker") {
                btcRate = doc["c"].as<float>();  // Last price
                highRate = doc["h"].as<float>();  // High price
                lowRate = doc["l"].as<float>();  // Low price

                // Update the UI with the latest data
                updateBitcoinUI(btcRate, highRate, lowRate);
            }

            break;
        }
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected");
            break;
        case WStype_CONNECTED:
            Serial.println("WebSocket Connected");
            // Subscribe to the 24hr ticker stream for BTCUSDT
            webSocket.sendTXT("{\"method\": \"SUBSCRIBE\", \"params\": [\"btcusdt@ticker\"], \"id\": 1}");
            break;
        case WStype_ERROR:
            Serial.println("WebSocket Error");
            break;
        default:
            break;
    }
}

// Function to initialize and connect the WebSocket client
void initBinanceWebSocket() {
    // Initialize WebSocket connection to Binance
    webSocket.beginSSL("stream.binance.com", 9443, "/ws");
    webSocket.onEvent(onWebSocketEvent);
}

// Function to handle the WebSocket loop
void handleBinanceWebSocket() {
    webSocket.loop();
}
