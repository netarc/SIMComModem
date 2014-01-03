#include "SIMComModem.h"

#define DEFAULT_HARDWARE_RESET_PIN 2
char* __ok__="OK";


SIMComModem::SIMComModem() : _reset_pin(DEFAULT_HARDWARE_RESET_PIN),
                             _sim_pin(NULL) {

}

void SIMComModem::setSIMPin(char *pin) {
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

bool SIMComModem::setAPN(char *apn) {
  writeCommand("AT+CGSOCKCONT=1,\"IP\",\"%s\"", apn);
  return !!checkResponse();
}

void SIMComModem::setHardwareResetPin(uint8_t pin) {
  _reset_pin = pin;
}

bool SIMComModem::start() {
  Serial1.begin(115200);
  while(!Serial1);

  // theGSM3ShieldV1ModemCore.setStatus(IDLE);
  pinMode(_reset_pin, OUTPUT);
  digitalWrite(_reset_pin, HIGH);
  delay(3000);
  digitalWrite(_reset_pin, LOW);
  //delay(1000);

  for (int i=0; i<30; i++) {
    writeCommand("AT");
    if (checkResponse(NULL, NULL, 2000))
      return true;
  }
  return false;
}

bool SIMComModem::restart() {
  // theGSM3ShieldV1ModemCore.setStatus(IDLE);
  pinMode(_reset_pin, OUTPUT);
  digitalWrite(_reset_pin, HIGH);
  delay(12000);
  digitalWrite(_reset_pin, LOW);
  delay(1000);
  return true;
}

bool SIMComModem::shutdown() {
  pinMode(_reset_pin, OUTPUT);
  digitalWrite(_reset_pin, HIGH);
  delay(1500);
  digitalWrite(_reset_pin, LOW);
}


static bool gWriteIsNewline=true;
size_t SIMComModem::writeBytes(const uint8_t *bytes, size_t size) {
#ifdef DEBUG
  if (gWriteIsNewline)
    Serial.print("$ ");
  Serial.write(bytes, size);
  gWriteIsNewline = (bytes[size-1] == '\n');
#endif
  return Serial1.write(bytes, size);
}

void SIMComModem::writeCommand(const char *format, ... ) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsprintf(buffer,format, args);
  va_end(args);

#ifdef DEBUG
  Serial.print("$ ");
  Serial.println(buffer);
#endif

  // Clean the input buffer
  while(Serial1.available() > 0) Serial1.read();
  Serial1.println(buffer);
}

bool SIMComModem::readLine(char *buffer, uint8_t size, unsigned int timeout) {
  uint8_t i=0;
  memset(buffer, 0, size);
  unsigned long previous = millis();
  do {
    if (Serial1.available() != 0) {
      char ch = Serial1.read();
      if (ch == 10 || ch == 13) {
        ch = Serial1.peek();
        if (ch == 10 || ch == 13)
          Serial1.read();
        if (i == 0)
          continue;
        Serial.print("> ");
        Serial.println(buffer);
        return true;
      }
      if (ch >= 32 && ch <= 126)
        buffer[i++] = ch;
      if (i == size)
        break;
    }
  } while((millis() - previous) < timeout);
  return false;
}

int SIMComModem::checkResponse(char *string, char *string2, uint16_t timeout) {
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