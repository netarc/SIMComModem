#ifndef _SIMCOM_MODEM__H_
#define _SIMCOM_MODEM__H_

// Core library for code-sense
#if defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#define ARDUINO_PLATFORM
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#define ARDUINO_PLATFORM
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#define ARDUINO_PLATFORM
#elif defined(ENERGIA) // LaunchPad MSP430, Stellaris and Tiva, Experimeter Board FR5739 specific
#include "Energia.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#define ARDUINO_PLATFORM
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#define ARDUINO_PLATFORM
#elif defined(ARDUINO) && (ARDUINO < 100) // Arduino 23 specific
#include "WProgram.h"
#define ARDUINO_PLATFORM
#else // error
#error Platform not defined
#endif


#ifdef DEBUG
  #ifdef ARDUINO_PLATFORM
    static void _printf(const char *format, ...) {
      char buf[80];
      va_list ap;
      va_start(ap, format);
      vsnprintf(buf, sizeof(buf), format, ap);
      for(char *p = &buf[0]; *p; p++) {
        if(*p == '\n')
          SERIAL_PORT_MONITOR.write('\r');
        SERIAL_PORT_MONITOR.write(*p);
      }
      va_end(ap);
    }
    #define __printf(...) _printf(__VA_ARGS__)
  #else
    #define __printf(...) printf(__VA_ARGS__)
  #endif

  #define DLog(format, ...) __printf((format), ##__VA_ARGS__)
#else
  #define DLog(format, ...)
#endif


class SIMComModemClass {
public:
  SIMComModemClass();

  // This will start the modem if it has not been already and will also initialize the modem
  bool begin();

  // Send a hardware start signal if the modem is not already known to be on.
  void hwStart();

  // Send a hardware shutdown signal if the modem is not already known to be off.
  void hwShutdown();

  // Set the pin used to unlock the SIM.
  // By default there is no pin set and can always be cleared by setting to NULL.
  void setSIMPin(char *pin);
  // Set the APN to be used for data access.
  // By default there is no APN set and the device default is used.
  bool setAPN(char *apn);

  // Set the pin used to signal the modem on/off
  // Generally this is never needed unless the shield is not directly plugged in and a different pin is used.
  void setHardwarePowerPin(uint8_t pin);

  void writeCommand(const char *format, ... );
  size_t writeBytes(const uint8_t *bytes, size_t size);
  bool readLine(char *buffer, uint8_t size, unsigned int timeout);
  int checkResponse(char *string=NULL, char *string2=NULL, uint16_t timeout=1000);

private:
  bool    _powered_on;
  char    *_sim_pin;
  uint8_t _hardware_power_pin;
};

extern SIMComModemClass SIMComModem;

#include <SIMComModemClient.h>

#endif // _SIMCOM_MODEM__H_
