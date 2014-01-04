#include "SIMComModem.h"

#define DEFAULT_HARDWARE_RESET_PIN 2
char* __ok__="OK";

SIMComModemClass SIMComModem;


void modem_power_toggle(uint8_t power_pin) {
  pinMode(power_pin, OUTPUT);
  digitalWrite(power_pin, HIGH);
  delay(200);
  digitalWrite(power_pin, LOW);
}

SIMComModemClass::SIMComModemClass() : _hardware_power_pin(DEFAULT_HARDWARE_RESET_PIN),
                                       _sim_pin(NULL),
                                       _powered_on(false) {

}

bool SIMComModemClass::begin() {
  if (!_powered_on) {
    // Double check to see if our modem isn't on
    writeCommand("AT");
    _powered_on == !!checkResponse(NULL, NULL, 500);
  }
  if (!_powered_on)
    hwStart();

  SERIAL_PORT_HARDWARE.begin(115200);
  while(!SERIAL_PORT_HARDWARE);

  for (int i=0; i<30; i++) {
    writeCommand("AT");
    if (checkResponse(NULL, NULL, 2000)) {
      // writeCommand("ATE0"); // disable command echo
      // writeCommand("AT&V"); // read current configuration
      // writeCommand("AT+CFUN?");
      checkResponse(NULL, NULL, 2000);
      return true;
    }
  }
  return false;
}

void SIMComModemClass::setSIMPin(char *pin) {
  if (_sim_pin) {
    free(_sim_pin);
    _sim_pin = NULL;
  }

  if (pin) {
    uint8_t size = strlen(pin);
    // does the modem even allow a sim bigger than 4 characters?
    if (size > 10)
      size = 0;
    if (size == 0)
      return;

    _sim_pin = (char *)malloc(size);
    strcpy(_sim_pin, pin);
  }
}

bool SIMComModemClass::setAPN(char *apn) {
  writeCommand("AT+CGSOCKCONT=1,\"IP\",\"%s\"", apn);
  return !!checkResponse();
}

void SIMComModemClass::setHardwarePowerPin(uint8_t pin) {
  _hardware_power_pin = pin;
}

void SIMComModemClass::hwStart() {
  // No need to attempt a power toggle when we "know" we have been powered on
  if (_powered_on)
    return;

  modem_power_toggle(_hardware_power_pin);
  _powered_on = true;
}

void SIMComModemClass::hwShutdown() {
  if (!_powered_on)
    return;

  modem_power_toggle(_hardware_power_pin);
  _powered_on = false;
}

size_t SIMComModemClass::writeBytes(const uint8_t *bytes, size_t size) {
#ifdef DEBUG
  // could this be shortened/cleaned up?
  static bool gWriteIsNewline=true;
  if (gWriteIsNewline)
    DLog("\n$ ");
  int offset = 0;
  char tmp[83];
  do {
    int chunk = size - offset;
    memset(&tmp, 0, 83);
    strncpy(tmp, (char *)bytes+offset, chunk > 80 ? 80 : chunk);
    DLog(tmp);
    offset+=80;
  } while (offset < size);
  gWriteIsNewline = (bytes[size-1] == '\n');
#endif
  return SERIAL_PORT_HARDWARE.write(bytes, size);
}

void SIMComModemClass::writeCommand(const char *format, ... ) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsprintf(buffer,format, args);
  va_end(args);

  DLog("$ %s\n", buffer);
  // Clean the input buffer
  while(SERIAL_PORT_HARDWARE.available() > 0) SERIAL_PORT_HARDWARE.read();
  SERIAL_PORT_HARDWARE.println(buffer);
}

bool SIMComModemClass::readLine(char *buffer, uint8_t size, unsigned int timeout) {
  uint8_t i=0;
  memset(buffer, 0, size);
  unsigned long previous = millis();
  do {
    if (SERIAL_PORT_HARDWARE.available() != 0) {
      char ch = SERIAL_PORT_HARDWARE.read();
      if (ch == 10 || ch == 13) {
        // check for an extra line-break/carriage-return and munch it
        ch = SERIAL_PORT_HARDWARE.peek();
        if (ch == 10 || ch == 13)
          SERIAL_PORT_HARDWARE.read();
        // an empty line is not returned
        if (i == 0)
          continue;

        DLog("> %s\n", buffer);
        return true;
      }
      // filter out non-ascii characters
      if (ch >= 32 && ch <= 126)
        buffer[i++] = ch;
      if (i == size)
        break;
    }
  } while((millis() - previous) < timeout);
  DLog(" * timeout\n");
  return false;
}

int SIMComModemClass::checkResponse(char *string, char *string2, uint16_t timeout) {
  bool result;

  if((string==0) && (string2==0))
    string=__ok__;

  char line[255];
  unsigned long previous = millis();
  bool read;
  do {
    read = readLine(line, 255, timeout);
    if (read) {
      if (strncmp(line, string, strlen(string)) == 0)
        return 1;
      if (string2 != NULL && strncmp(line, string2, strlen(string2)) == 0)
        return 2;
    }
  } while((millis() - previous) < timeout);

  return 0;
}