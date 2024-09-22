// BinanceWebSocket.h

#ifndef BINANCEWEBSOCKET_H
#define BINANCEWEBSOCKET_H

#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// Externally defined function to update the UI
void updateBitcoinUI(float btcRate, float highRate, float lowRate);

// Function to initialize and connect the WebSocket client
void initBinanceWebSocket();

// Function to handle the WebSocket loop
void handleBinanceWebSocket();

#endif // BINANCEWEBSOCKET_H
