#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"

const char *service_name = "Ajeet";
const char *pop = "ajeet48";

static uint8_t WIFI_LED   = 2;
static uint8_t RESET_PIN  = 0;
static uint8_t TOUCH_PIN  = 15; // Connect your touch sensor to GPIO 4
static uint8_t BUZZER_PIN = 21;

boolean BUZZER_STATE        = false;
unsigned long buzzer_timer  = 0;

char device1[] = "SecuritySwitch";
static Switch SecuritySwitch(device1, NULL);
bool SECURITY_STATE = false;

uint32_t chipId = 0;

#define touch T3;

void sysProvEvent(arduino_event_t *sys_event) {
    switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
            #if CONFIG_IDF_TARGET_ESP32
                Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
                printQR(service_name, pop, "ble");
            #else
                Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
                printQR(service_name, pop, "softap");
            #endif        
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.printf("\nConnected to Wi-Fi!\n");
            digitalWrite(WIFI_LED, HIGH);
            break;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();
    
    if(strcmp(device_name, device1) == 0) {
        Serial.printf("SecuritySwitch = %s\n", val.val.b? "true" : "false");
        
        if(strcmp(param_name, "Power") == 0) {
            Serial.printf("Received value = %s for %s - %s\n", 
                            val.val.b? "true" : "false", device_name, param_name);
            SECURITY_STATE = val.val.b;
            param->updateAndReport(val);
            
            if(SECURITY_STATE == true){
                esp_rmaker_raise_alert("Security is ON");
            }
            else{
                esp_rmaker_raise_alert("Security is OFF");
            }
        }
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(RESET_PIN, INPUT);
    pinMode(WIFI_LED, OUTPUT);
    digitalWrite(WIFI_LED, LOW);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(TOUCH_PIN, INPUT); // Set touch pin as input
    Node my_node;    
    my_node = RMaker.initNode("free");
    SecuritySwitch.addCb(write_callback);
    my_node.addDevice(SecuritySwitch);
    RMaker.enableOTA(OTA_USING_PARAMS);
    RMaker.enableTZService();
    RMaker.enableSchedule();
    for(int i=0; i<17; i=i+8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    Serial.printf("\nChip ID:  %d Service Name: %s\n", chipId, service_name);
    Serial.printf("\nStarting ESP-RainMaker\n");
    RMaker.start();
    WiFi.onEvent(sysProvEvent);
    
    #if CONFIG_IDF_TARGET_ESP32
        WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
    #else
        WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
    #endif
    
    SecuritySwitch.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
}

void loop() {
  int valu = touchRead(T4);
  int valuu = touchRead(T5);
  if(valu < 40){
    digitalWrite(33 , HIGH);
  }
  if(valuu< 40){
    digitalWrite(33 , LOW);
  }
    if(digitalRead(RESET_PIN) == LOW) {
        Serial.printf("Reset Button Pressed!\n");
        delay(100);
        int startTime = millis();
        while(digitalRead(RESET_PIN) == LOW) delay(50);
        int endTime = millis();
        
        if ((endTime - startTime) > 10000) {
            Serial.printf("Reset to factory.\n");
            RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
            Serial.printf("Reset Wi-Fi.\n");
            RMakerWiFiReset(2);
        }
    }
    
    delay(100);
  
    if (WiFi.status() != WL_CONNECTED){
        digitalWrite(WIFI_LED, LOW);
    } else {
        digitalWrite(WIFI_LED, HIGH);
    }
    
    detectTouch();
    controlBuzzer();
}

void detectTouch() {
    if(SECURITY_STATE == true) {
        if(digitalRead(TOUCH_PIN) == HIGH) {
            Serial.println("Touch detected!");
            esp_rmaker_raise_alert("Security Alert!\nTouch is detected.");
            digitalWrite(BUZZER_PIN, HIGH);
            BUZZER_STATE = true;
            buzzer_timer = millis();
        }
    }
}

void controlBuzzer(){
    if (BUZZER_STATE == true) {
        if (millis() - buzzer_timer > 5000) {
            digitalWrite(BUZZER_PIN, LOW);
            BUZZER_STATE = false;
            buzzer_timer = 0;
        }
    }
}
