#ifndef __ININDKIT_H
#define __ININDKIT_H

#include <HardwareSerial.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <OTA.h>

#include "services\WSerial_c.h"
#include "services\display_c.h"
#include "services\UDP_c.h"
#include "services\wifimanager_c.h"

#include "util/asyncDelay.h"
#include "util/din.h"

/********** POTENTIOMETERS GPIO define *****/
#define def_pin_POT1 36 // GPIO36
#define def_pin_POT2 39 // GPIO39
/********************* ADC ****************/
#define def_pin_ADC1 34 // GPIO34
/***************** Read 4@20 mA ***********/
#define def_pin_R4a20_1 33 // GPIO33
#define def_pin_R4a20_2 32 // GPIO32
/******************** Digitais **************/
#define def_pin_D1 13 // GPIO13 - Não funciona como entrada analogica somente digital
#define def_pin_D2 5  // GPIO5  - Não funciona como entrada analogica somente digital
#define def_pin_D3 14 // GPIO14 - Não funciona como entrada analogica somente digital
#define def_pin_D4 27 // GPIO27 - Não funciona como entrada analogica somente digital
/********************* DAC ****************/
#define def_pin_DAC1 25 // GPIO25
/***************** Write 4@20 mA **********/
#define def_pin_W4a20_1 26 // GPIO26
/********************* RELÊ ***************/
#define def_pin_RELE 23 // GPIO23
/***************** OLED Display ************/
#define def_pin_SDA 21 // GPIO21
#define def_pin_SCL 22 // GPIO22
/********************* PWM ****************/
#define def_pin_PWM 18 // GPIO18
/************* BUTTONS GPIO define *********/
#define def_pin_RTN1 15  // GPIO15
#define def_pin_RTN2 35  // GPIO35
#define def_pin_PUSH1 16 // GPIO16
#define def_pin_PUSH2 17 // GPIO17

// Use ESP, InIndKit, WiFi, ArduinoOTA, InIndKit.Display e InIndKit.Telnet
class IIKit_c
{
private:
    char DDNSName[15] = "inindkit";
    WifiManager_c wm;
    void errorMsg(String error, bool restart = true);    

public:
    DIn_c rtn_1;
    DIn_c rtn_2;
    DIn_c push_1;
    DIn_c push_2;
    Display_c disp;
    WSerial_c WSerial;    
    // HartUdp_c ds8500Serial(4000);    
    void setup();
    void loop(void);    
};

inline void IIKit_c::setup()
{
    this->WSerial.println("Booting");
    /*********** Inicializando Display ********/
    if (disp.start(def_pin_SDA, def_pin_SCL))
    {
        disp.setText(1, "Inicializando...");
        this->WSerial.println("Display running");
    }
    else
        errorMsg("Display error.", false);
    delay(50);
    /*************** READ EEPROM *************/
    EEPROM.begin(1);
    char idKit[2] = "0";
    /*************** Write EEPROM ************/
    // EEPROM.write(0,(uint8_t) idKit[0]);
    // EEPROM.commit();
    /********** Initializes with kit id ******/
    idKit[0] = (char)EEPROM.read(0); // id do kit utilizado
    strcat(DDNSName, idKit);
    /************** Starting WIFI ************/
    WiFi.mode(WIFI_STA);
    /********* Starting WIFI Manager *********/
    wm.start(&WSerial);
    wm.setApName(DDNSName);
    disp.setFuncMode(true);
    disp.setText(1, "Mode: Acces Point", true);
    disp.setText(2, "SSID: AutoConnectAP", true);
    disp.setText(3, "PSWD: ", true);
    if (wm.autoConnect("AutoConnectAP"))
    {
        WSerial.print("\nWifi running - IP:");
        WSerial.println(WiFi.localIP());
        disp.setFuncMode(false);
        disp.setText(1, WiFi.localIP().toString().c_str());
        disp.setText(2, DDNSName);
        disp.setText(3, "UFU Mode");
        delay(50);
    }
    else
        errorMsg("Wifi  error.\nAP MODE...", false);

    /************** Starting OTA *************/
    OTA::setup(DDNSName);// OTA tem que ser depois do wifi e wifiManager
    /*** Starting Telnet Mode in WSerial ****/
    WSerial.start(4000);
    /********** POTENTIOMETERS GPIO define *****/
    pinMode(def_pin_POT1, ANALOG);
    pinMode(def_pin_POT2, ANALOG);
    /************* BUTTONS GPIO define *********/
    pinMode(def_pin_RTN1, INPUT_PULLDOWN);
    pinMode(def_pin_RTN2, INPUT_PULLDOWN);
    pinMode(def_pin_PUSH1, INPUT_PULLDOWN);
    pinMode(def_pin_PUSH2, INPUT_PULLDOWN);
    /*************** IOs GPIO define **********/
    pinMode(def_pin_D1, OUTPUT);
    pinMode(def_pin_D2, OUTPUT);
    pinMode(def_pin_D3, OUTPUT);
    pinMode(def_pin_D4, OUTPUT);
    /***************** Read 4@20 mA ***********/
    pinMode(def_pin_R4a20_1, ANALOG);
    pinMode(def_pin_R4a20_2, ANALOG);
    /********************* PWM ****************/
    pinMode(def_pin_PWM, OUTPUT);
    /********************* DAC ****************/
    pinMode(def_pin_DAC1, ANALOG);
    /********************* ADC ****************/
    pinMode(def_pin_ADC1, ANALOG);
    /********************* RELÊ ***************/
    pinMode(def_pin_RELE, OUTPUT);
    /***************** Write 4@20 mA **********/
    pinMode(def_pin_W4a20_1, OUTPUT);
    /************* Set Btn Pin ****************/
    rtn_1.setPin(def_pin_RTN1);
    rtn_2.setPin(def_pin_RTN2);
    push_1.setPin(def_pin_PUSH1);
    push_2.setPin(def_pin_PUSH2); 
    /************ Web Portal ****************/
    push_1.setTimePressedDIn(3);
    push_1.onPressedWithTime([this]()
                             {
        if(wm.changeWebPortal())
        {
            ((Display_c *) this)->setFuncMode(true);
            ((Display_c *) this)->setText(2, "Web Portal ON", true);
            ((Display_c *) this)->setText(3, "", true);        
            //digitalWrite(def_pin_OUT1, LOW);            
        } else {
            ((Display_c *) this)->setFuncMode(false);
            ((Display_c *) this)->setText(2, DDNSName);            
            //digitalWrite(def_pin_OUT1, HIGH); 
        } });
    /************ HART MODEM ****************/
    // ds8500Serial.setup(def_pin_Hart_RXD, def_pin_Hart_TXD, def_pin_Hart_CTS, def_pin_Hart_RTS);
    /************ OTHER CONFIG **************/
    digitalWrite(def_pin_D1, LOW);
    digitalWrite(def_pin_D2, LOW);
    digitalWrite(def_pin_D3, LOW);
    digitalWrite(def_pin_D4, LOW);
    digitalWrite(def_pin_RELE, LOW);
    analogWrite(def_pin_PWM, 0);
    analogWrite(def_pin_DAC1, 0);
    analogWrite(def_pin_W4a20_1, 0);
}

void IIKit_c::loop(void)
{
    OTA::handle();
    WSerial.loop();
    disp.update();
    if (wm.getPortalRunning())
        wm.process();
    rtn_1.update();
    rtn_2.update();
    push_1.update();
    push_2.update();
    // ds8500Serial.loop();
}

void IIKit_c::errorMsg(String error, bool restart)
{
    WSerial.println(error);
    if (restart)
    {
        WSerial.println("Rebooting now...");
        delay(2000);
        ESP.restart();
        delay(2000);
    }
}
IIKit_c IIKit;
#endif