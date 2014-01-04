Introduction
==================

SIMComModem is a library for the SIMCOM GSM/GPRS/GPS module and currently supports the following versions:
* SIM5218E

SIMComModem was initially designed for [Arduino](http://www.arduino.cc/) based devices using the [Arduino 3g GPRS/GSM/GPS Shield](http://www.cooking-hacks.com/documentation/tutorials/arduino-3g-gprs-gsm-gps).

**NOTE**: Unless stated otherwise, the following instructions are specific to [Arduino Uno](http://arduino.cc/en/Main/arduinoBoardUno) board. If you are using other boards, the exact steps may vary.

How to Install the library
==========================

1. Clone the latest source or download the latest zip.
2. Open the Arduino IDE, click `Sketch->Import Library...->Add Library...`, then navigate to `m2x-arduino-simcom/SIMComModem`
3. Now you can find SIMComModem examples under `File->Examples->SIMComModem`
3. Enjoy coding!

Using the SIMComModem library
=========================

You will need to include the libaries main header to gives access to the SIMComModem object using the following code:

```
#include "SIMComModem.h"
```

Then simply start the modem and point it to the correct APN which is typically done in the microcontroller startup phase using the following code:

```
void setup() {
  ...
  if (!SIMComModem.begin()) {
    Serial.println("Failed to start SIMCom Modem");
    while(true);
  }
  SIMComModem.setAPN("YOUR.APN.HERE");
  ...
}
```

LICENSE
=======

This library is released under the MIT license. See [`LICENSE`](LICENSE) for the terms.