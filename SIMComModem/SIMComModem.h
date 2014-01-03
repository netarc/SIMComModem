/*
  Copyright (c) 2014 Josh Hollenbeck

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef _SIMCOM_MODEM__H_
#define _SIMCOM_MODEM__H_

#include <Arduino.h>
#define DEBUG

class SIMComModem {
public:
  SIMComModem();

  // Set the pin used to unlock the SIM.
  // By default there is no pin set and can always be cleared by setting to NULL.
  void setSIMPin(char *pin);
  // Set the APN to be used for data access.
  // By default there is no APN set and the device default is used.
  bool setAPN(char *apn);

  // Set the pin used to reset the modem
  // Generally this is never needed unless the shield is not directly plugged in and a different pin is used.
  void setHardwareResetPin(uint8_t pin);

  // Start the modem
  // If the modem is already on, this has no effect. Otherwise this will power on the modem.
  // @returns true if successful
  bool start();

  // Restart the modem
  // This will perform a shutdown if running followed by a start
  // @returns true if successful
  bool restart();

  // Shutdown the modem
  // This will essentially power off the device
  // @returns true if successful
  bool shutdown();

  void writeCommand(const char *format, ... );
  size_t writeBytes(const uint8_t *bytes, size_t size);
  bool readLine(char *buffer, uint8_t size, unsigned int timeout);
  int checkResponse(char *string=NULL, char *string2=NULL, uint16_t timeout=1000);

private:
  char    *_sim_pin;
  uint8_t _reset_pin;
};

#include <SIMComModemClient.h>

#endif // _SIMCOM_MODEM__H_
