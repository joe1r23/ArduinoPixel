/*! \file arduino_ethernet.ino
 *  \brief A distributed system that implements a web server for controlling
 *  a NeoPixel LED strip.
 *  \details This is the server side code of system. The client side is an
 *  Android app that allows to control the LED strip. For the server side,
 *  the required equipment is an Arduino compatible board with Ethernet
 *  connectivity. For the client side, an Android phone or tablet is needed.
 *  \note Configuration:
 *   Server side:
 *     Specify the following: (look for indicated sections)
 *     > MAC address of the Ethernet controller
 *     > IP address (one of your choice that's not already taken
 *       in your network)
 *     > Port number that the server will operate on (you may leave this as is)
 *     > Number of the LEDs on the strip
 *     > Pin on the board that connects to the data pin of the strip
 *   Client side:
 *     > Install the ArduinoPixel app from Google Play Store.
 *     > Open the app, go to Settings, and update the host IP address and port
 *       number with the ones you specified below.
 *     > Enjoy!
 *  \note Specifications:
 *   > Server side:
 *     The server accepts the following HTTP requests:
 *     > GET @ / : Responds with a "Hello from Arduino Server" message
 *     > GET @ /strip/status : Responds with "ON" or "OFF" indicating the power
 *                             state of the strip
 *     > GET @ /strip/modes : Responds with a comma separated list of the
 *                            available modes
 *     > GET @ /strip/mode : Responds with the name of the active mode
 *     > GET @ /strip/color : Responds with a JSON representation of the base
 *                            color on the strip, i.e. {"r":x,"g":x,"b":x}
 *     > PUT @ /strip/status/on : Turns the strip on. No data required
 *     > PUT @ /strip/status/off : Turns the strip off. No data required
 *     > PUT @ /strip/mode : Updates the mode. The required data are the name
 *                           of the mode and, if applicable, a time period
 *                           in ms, e.g. SCANNER 100
 *     > PUT @ /strip/color : Updates the base color on the strip. The required
 *                            data is the color as a JSON object,
 *                            e.g. {"r":36,"g":113,"b":255}
 *   > Client side:
 *     You can use either a tool (such as curl) that is able to make HTTP
 *     requests or the ArduinoPixel Android app. The Android app:
 *     > allows to configure the host IP address and port number
 *     > allows to turn the strip on and off
 *     > allows for the continuous update of the base color on the strip
 *  \author Nick Lamprianidis <nlamprian@gmail.com>
 *  \version 2.0.0
 *  \date 2014
 *  \copyright The MIT License (MIT)
 *  \par
 *  Copyright (c) 2014 Nick Lamprianidis
 *  \par
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  \par
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *  \par
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include <SPI.h>
#include <Ethernet.h>

#include "arduino_pixel_server.h"
#include "led_strip/led_strip_neopixel.h"

using namespace arduino_pixel;

// ============================================================================
// == Please specify the following parameters =================================
// ================================================================== start ===
static byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0xAF, 0xF6};
static IPAddress ip(192, 168, 1, 10);
const int port = 80;
const int num_leds = 112;
const int strip_pin = 7;
// ================================================================== end =====
// ============================================================================

class ArduinoPixel : public ArduinoPixelServer {
 public:
  ArduinoPixel()
      : strip_neopixel_(num_leds, strip_pin, NEO_GRB + NEO_KHZ800),
        server_(port) {}

  virtual ~ArduinoPixel() {}

  using ArduinoPixelServer::init;

  void init() {
    strip_neopixel_.init();
    init(&strip_neopixel_);
    Ethernet.begin(mac, ip);
    server_.begin();
  }

  void check() {
    EthernetClient client = server_.available();
    if (client) processRequest(client);
    strip_neopixel_.colorize();
  }

 private:
  led_strip::LedStripNeoPixel strip_neopixel_;
  EthernetServer server_;
};

ArduinoPixel pixel;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial)
    ;
#endif

  pixel.init();

#ifdef DEBUG
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
  Serial.println();
#endif
}

void loop() { pixel.check(); }
