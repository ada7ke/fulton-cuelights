// WebOTA.cpp

#ifdef ESP32

#ifdef ELEGANTOTA_USE_ASYNC_WEBSERVER

#include "WebOTA.h"
#include <ElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

AsyncWebServer web_server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Cue Lights Controller</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    /* Reset default margins and paddings */
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: Arial, sans-serif;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
      background-color: #f4f4f4;
      padding: 20px;
    }

    h1 {
      margin-bottom: 20px;
      text-align: center;
      font-size: 1.5em;
      color: #333;
    }

    .ota-link {
      position: absolute;
      top: 10px;
      right: 10px;
      font-size: 1em;
      text-decoration: none;
      color: #4CAF50;
      border: 1px solid #4CAF50;
      padding: 5px 10px;
      border-radius: 5px;
      transition: background-color 0.3s, color 0.3s;
    }

    .ota-link:hover {
      background-color: #4CAF50;
      color: #fff;
    }
  </style>
</head>
<body>
  <a href="/update/" class="ota-link">OTA Update</a>
  <h1>Cue Lights Controller</h1>
</body>
</html>
)rawliteral";

// Handle torque control routes
class CaptivePortalHandler : public AsyncWebHandler
{
public:
    CaptivePortalHandler() {}
    virtual ~CaptivePortalHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        return request->url() == "/";
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        if (request->method() == HTTP_GET && request->url() == "/")
        {
            request->send(200, "text/html", index_html);
        }
        else
        {
            request->send(200, "text/html", index_html);
        }
    }
};

// Prevent NVRAM from wearing out by only saving when the value changes
int last_torque_percent = -1;

void setupWebServer()
{
    web_server.addHandler(new CaptivePortalHandler()).setFilter(ON_AP_FILTER);

    web_server.onNotFound([&](AsyncWebServerRequest *request)
                          { request->send(200, "text/html", index_html); });

    web_server.begin();
}

unsigned long ota_progress_millis = 0;

void onOTAStart()
{
    // Log when OTA has started
    printf("OTA update started!\n");
    // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final)
{
    // Log every 1 second
    if (millis() - ota_progress_millis > 1000)
    {
        ota_progress_millis = millis();
        printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    }
}

void onOTAEnd(bool success)
{
    // Log when OTA has finished
    if (success)
    {
        printf("OTA update finished successfully!\n");
    }
    else
    {
        printf("There was an error during OTA update!\n");
    }
    // <Add your own code here>
}

void setupElegantOTA()
{
    ElegantOTA.begin(&web_server); // Start ElegantOTA
    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    setupWebServer();
    printf("HTTP server started.\n");
}

#endif

#endif