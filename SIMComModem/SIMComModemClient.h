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

#ifndef _SIMCOM_CLIENT__H_
#define _SIMCOM_CLIENT__H_

#include <Client.h>
#include "SIMComModem.h"

class SIMComModemClient : public Client {
public:
  SIMComModemClient(SIMComModem *modem);

  virtual int connect(IPAddress address, uint16_t port);
  virtual int connect(const char *address, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t*, size_t);
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();
  virtual int read(uint8_t*, size_t);
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool() {
    return _connected;
  }

private:
  void endTransmit();

  SIMComModem *_modem;
  bool _connected;
  bool _transmitMode;
};

#endif // _SIMCOM_CLIENT__H_
