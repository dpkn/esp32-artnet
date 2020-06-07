/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#endif
#include <DNSServer.h>
#include <memory>

#if defined(ESP8266)
extern "C" {
  #include "user_interface.h"
}
#define ESP_getChipId()   (ESP.getChipId())
#else
#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#endif

const char HTTP_HEAD_HTML[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\"content=\"width=device-width,initial-scale=1,user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM = "<style> *{box-sizing:border-box;} .c{text-align:center;}div,input{margin:2px 0;padding:10px;font-size:1em;}input{width:100%;border-radius:5px;border:1px solid#000;}html,body{width:100%;height:100%;margin:0;padding:0;top:0; }body{background:#191a1e;color:#fff;text-align:center;font-family:verdana;display:flex;align-items:center;justify-content:center;} .center{max-width:400px}a{color:#fff;}button{border:0;border-radius:0.3rem;background-color:#4f5468;border:1px solid#262832;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}button:hover{background: #262832;} .q{float:right;width:64px;text-align:right;} .l{background:url(\"data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyMiAyMiI+PGRlZnM+PGNsaXBQYXRoPjxyZWN0IHk9IjgiIHg9IjgiIGhlaWdodD0iMzIiIHdpZHRoPSIzMiIgZmlsbD0ibm9uZSIgcng9IjE2Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3QgeT0iNTA3LjgiIHg9IjM5Mi41NyIgaGVpZ2h0PSIzMiIgd2lkdGg9IjMyIiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cmVjdCB3aWR0aD0iMzIiIGhlaWdodD0iMzIiIHg9IjM5Mi41NyIgeT0iNTA3LjgiIGZpbGw9Im5vbmUiIHJ4PSIxNiIvPjwvY2xpcFBhdGg+PGNsaXBQYXRoPjxyZWN0IHk9IjUwNy44IiB4PSIzOTIuNTciIGhlaWdodD0iMzIiIHdpZHRoPSIzMiIgZmlsbD0ibm9uZSIgcng9IjE2Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3QgeT0iNTA3LjgiIHg9IjM5Mi41NyIgaGVpZ2h0PSIzMiIgd2lkdGg9IjMyIiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cmVjdCB3aWR0aD0iMzIiIGhlaWdodD0iMzIiIHg9IjM5Mi41NyIgeT0iNTA3LjgiIGZpbGw9Im5vbmUiIHJ4PSIxNiIvPjwvY2xpcFBhdGg+PGNsaXBQYXRoPjxyZWN0IHdpZHRoPSIzMiIgaGVpZ2h0PSIzMiIgeD0iOCIgeT0iOCIgZmlsbD0ibm9uZSIgcng9IjE2Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3Qgd2lkdGg9IjMyIiBoZWlnaHQ9IjMyIiB4PSIzOTIuNTciIHk9IjUwNy44IiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cmVjdCB5PSI1MDcuOCIgeD0iMzkyLjU3IiBoZWlnaHQ9IjMyIiB3aWR0aD0iMzIiIGZpbGw9Im5vbmUiIHJ4PSIxNiIvPjwvY2xpcFBhdGg+PGNsaXBQYXRoPjxyZWN0IHk9IjUwNy44IiB4PSIzOTIuNTciIGhlaWdodD0iMzIiIHdpZHRoPSIzMiIgZmlsbD0ibm9uZSIgcng9IjE2Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3QgeT0iOCIgeD0iOCIgaGVpZ2h0PSIzMiIgd2lkdGg9IjMyIiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cGF0aCBkPSJtMCA3MDYuNDdoMTQ5MC45M3YtNzA2LjQ3aC0xNDkwLjkzdjcwNi40NyIvPjwvY2xpcFBhdGg+PGNsaXBQYXRoPjxwYXRoIGQ9Im0yMi4yIDY4Ni4xMmgxNDQ3Ljczdi02NjcuMTloLTE0NDcuNzN2NjY3LjE5Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3QgeT0iNTA3LjgiIHg9IjM5Mi41NyIgaGVpZ2h0PSIzMiIgd2lkdGg9IjMyIiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cmVjdCB5PSI1MDcuOCIgeD0iMzkyLjU3IiBoZWlnaHQ9IjMyIiB3aWR0aD0iMzIiIGZpbGw9Im5vbmUiIHJ4PSIxNiIvPjwvY2xpcFBhdGg+PGNsaXBQYXRoPjxyZWN0IHk9IjUwNy44IiB4PSIzOTIuNTciIGhlaWdodD0iMzIiIHdpZHRoPSIzMiIgZmlsbD0ibm9uZSIgcng9IjE2Ii8+PC9jbGlwUGF0aD48Y2xpcFBhdGg+PHJlY3QgeT0iNTA3LjgiIHg9IjM5Mi41NyIgaGVpZ2h0PSIzMiIgd2lkdGg9IjMyIiBmaWxsPSJub25lIiByeD0iMTYiLz48L2NsaXBQYXRoPjxjbGlwUGF0aD48cmVjdCB5PSI4IiB4PSI4IiBoZWlnaHQ9IjMyIiB3aWR0aD0iMzIiIGZpbGw9Im5vbmUiIHJ4PSIxNiIvPjwvY2xpcFBhdGg+PC9kZWZzPjxnIHRyYW5zZm9ybT0ibWF0cml4KC45OTM5IDAgMCAuOTkzOS4wNi0xMDIzLjk2KSIgZmlsbD0iI2ZmZiI+PHBhdGggZD0ibTEyLjE1NyAxMDQ2LjU1Yy4wMTcuMDkzLS4wNDUuMTY4LS4xMzkuMTY4aC0xLjk5N2MtLjA5NCAwLS4xNTctLjA3Ni0uMTM5LS4xNjhsLjMxOC0xLjY4NWMtLjI0MS0uMjE5LS4zOTItLjUzNi0uMzkyLS44ODcgMC0uNjYzLjUzNy0xLjIgMS4yLTEuMi42NjMgMCAxLjIuNTM3IDEuMiAxLjIgMCAuMzQyLS4xNDMuNjUtLjM3Mi44Njl6Ii8+PHBhdGggZD0ibTE0LjIxNiAxMDQwLjE1aC02LjQxNXYtMS45OGMwLTEuNzY5IDEuNDM5LTMuMjA3IDMuMjA3LTMuMjA3IDEuNzY5IDAgMy4yMDcgMS40MzkgMy4yMDcgMy4yMDd6bTEuNzExLjAxOHYtMS45OThjMC0yLjcxMi0yLjIwNi00LjkxOS00LjkxOS00LjkxOS0yLjcxMiAwLTQuOTE5IDIuMjA3LTQuOTE5IDQuOTE5djEuOTk4Yy0uMzg4LjA4MS0uNjguNDI2LS42OC44Mzd2Ny40ODhjMCAuNDcyLjM4NC44NTUuODU2Ljg1NWg5LjQ4NWMuNDcyIDAgLjg1Ni0uMzg0Ljg1Ni0uODU1di03LjQ4OGMwLS40MTEtLjI5Mi0uNzU2LS42OC0uODM3Ii8+PC9nPjwvc3ZnPg==\")no-repeat left center;background-size:1.2em;} </style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();} </script>";
const char HTTP_HEAD_HTML_END[] PROGMEM = "</head><body><div class=\"center\">";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\"method=\"get\"><button>Select WiFi network</button></form><br/><form action=\"/0wifi\"method=\"get\"><button>Manual Input</button></form><br/><!--><form action=\"/i\"method=\"get\"><button>Info</button></form><br/><form action=\"/r\"method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM = "<div><a href='#p'onclick='c(this)'>{v}</a>&nbsp;<span class='q{i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM = "<form method='get'action='wifisave'><input id='s'name='s'length=32 placeholder='SSID'><br/><input id='p'name='p'length=64 type='password'placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}'name='{n}'length='{l}'placeholder='{p}'value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM = "<br/><button type='submit'>Save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM = "<div><h2>Connecting...</h2><br/>Trying to connect to WiFi.Bloclock should light up soon.If nothing happens,try refreshing this page.</div>";
const char HTTP_HTML_END[] PROGMEM = "</div></body></html>";

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter {
  public:
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();
    String        getSSID();
    String        getPassword();
    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter
    void          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
#ifdef ESP8266
    std::unique_ptr<ESP8266WebServer> server;
#else
    std::unique_ptr<WebServer>        server;
#endif

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD_HTML = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    WiFiManagerParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
