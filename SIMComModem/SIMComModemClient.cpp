#include "SIMComModem.h"
#include "SIMComModemClient.h"


SIMComModemClient::SIMComModemClient() : _transmitMode(false),
                                         _connected(false) {

}

int SIMComModemClient::connect(IPAddress ip, uint16_t port) {
  char address[18];
  sprintf(address, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  return connect(address, port);
}

int SIMComModemClient::connect(const char *address, uint16_t port) {
  _transmitMode = true;
  SIMComModem.writeCommand("AT+CHTTPACT=\"%s\",%d", address, port);
  int resp = SIMComModem.checkResponse("+CHTTPACT: REQUEST", "+CME ERROR", 15000);
  return resp == 1 ? 1 : 0;
}

void SIMComModemClient::endTransmit() {
  if (_transmitMode) {
    _transmitMode = false;

    SIMComModem.writeCommand("\x1A\x00");
    if (SIMComModem.checkResponse("OK", NULL, 15000)) {
      _connected = true;
      return;
      char response[100];
      do {
        bool result = SIMComModem.readLine(response, 100, 1000);
        if (result) {
          if (strstr(response, "+CHTTPACT") != NULL) {
            char *tkn = strtok(response, " ,");
            tkn = strtok(NULL, " ,");
            Serial.print("token: ");
            Serial.println(tkn);

            if (strcmp(tkn, "DATA") == 0) {
              char data[512];

              tkn = strtok(NULL, " ,");
              Serial.print("size token: ");
              Serial.println(tkn);
              uint32_t data_size = atoi(tkn);
              uint32_t bytes_read;
              uint32_t offset = 1;

              Serial1.setTimeout(15000);
              bytes_read = Serial1.readBytes(data, 1);
              if (data[0] == 10 || data[0] == 13)
                offset = 0;

              bytes_read = Serial1.readBytes(&data[offset], data_size);

              if (bytes_read == data_size) {
                Serial.println(data);
                Serial.println(">> download finished");
                return;
              } else {
                Serial.println(data);
                Serial.println(">> download timeout");
              }
            }
          }
        }
      } while(true);

    }
  }
}

size_t SIMComModemClient::write(uint8_t ch) {
  return SIMComModem.writeBytes(&ch, 1);
}

size_t SIMComModemClient::write(const uint8_t *bytes, size_t size) {
  return SIMComModem.writeBytes(bytes, size);
}

int SIMComModemClient::available() {
  endTransmit();
  return Serial1.available();
}

int SIMComModemClient::read() {
  endTransmit();
  return Serial1.read();
}

int SIMComModemClient::peek() {
  endTransmit();
  return Serial1.peek();
}

void SIMComModemClient::flush() {
  return Serial1.flush();
}

int SIMComModemClient::read(uint8_t *buffer, size_t size) {
  // endTransmit();
  // return Serial1.read(buffer, size);
  return 0;
}

void SIMComModemClient::stop() {
  _connected = false;
  if (_transmitMode) {
    _transmitMode = false;
    SIMComModem.writeCommand("\x1A\x00");
  }
}

uint8_t SIMComModemClient::connected() {
  return _connected;
}