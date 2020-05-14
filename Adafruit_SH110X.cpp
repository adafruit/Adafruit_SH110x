/*!
 * @file Adafruit_SSD1306.cpp
 *
 * @mainpage Arduino library for monochrome OLEDs based on SSD1306 drivers.
 *
 * @section intro_sec Introduction
 *
 * This is documentation for Adafruit's SSD1306 library for monochrome
 * OLED displays: http://www.adafruit.com/category/63_98
 *
 * These displays use I2C or SPI to communicate. I2C requires 2 pins
 * (SCL+SDA) and optionally a RESET pin. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit-GFX-Library">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * @section license License
 *
 * BSD license, all text above, and the splash screen included below,
 * must be included in any redistribution.
 *
 */

#include "Adafruit_SH110X.h"
#include "splash.h"


// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SH110X displays.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SH110X library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SH110X datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SH110X's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SH110X library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @return Adafruit_SH110X object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
Adafruit_SH110X::Adafruit_SH110X(uint16_t w, uint16_t h, TwoWire *twi,
				 int8_t rst_pin, uint32_t clkDuring, uint32_t clkAfter) :
  Adafruit_MonoOLED(w, h, twi, rst_pin, clkDuring, clkAfter)
{
}

/*!
    @brief  Constructor for SPI SH110X displays, using software (bitbang)
            SPI.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  mosi_pin
            MOSI (master out, slave in) pin (using Arduino pin numbering).
            This transfers serial data from microcontroller to display.
    @param  sclk_pin
            SCLK (serial clock) pin (using Arduino pin numbering).
            This clocks each bit from MOSI.
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @return Adafruit_SH110X object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
Adafruit_SH110X::Adafruit_SH110X(uint16_t w, uint16_t h, int8_t mosi_pin, 
				 int8_t sclk_pin, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin) : Adafruit_MonoOLED(w, h, mosi_pin, sclk_pin, dc_pin, rst_pin, cs_pin) {
}

/*!
    @brief  Constructor for SPI SH110X displays, using native hardware SPI.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  spi
            Pointer to an existing SPIClass instance (e.g. &SPI, the
            microcontroller's primary SPI bus).
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @param  bitrate
            SPI clock rate for transfers to this display. Default if
            unspecified is 8000000UL (8 MHz).
    @return Adafruit_SH110X object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
Adafruit_SH110X::Adafruit_SH110X(uint16_t w, uint16_t h, SPIClass *spi,
    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, uint32_t bitrate) :
  Adafruit_MonoOLED(w, h, spi, dc_pin, rst_pin, cs_pin, bitrate) {
}

/*!
    @brief  Destructor for Adafruit_SH110X object.
*/
Adafruit_SH110X::~Adafruit_SH110X(void) {
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  addr
            I2C address of corresponding SH110X display (or pass 0 to use
            default of 0x3C for 128x32 display, 0x3D for all others).
            SPI displays (hardware or software) do not use addresses, but
            this argument is still required (pass 0 or any value really,
            it will simply be ignored). Default if unspecified is 0.
    @param  reset
            If true, and if the reset pin passed to the constructor is
            valid, a hard reset will be performed before initializing the
            display. If using multiple SH110X displays on the same bus, and
            if they all share the same reset pin, you should only pass true
            on the first display being initialized, false on all others,
            else the already-initialized displays would be reset. Default if
            unspecified is true.
    @param  periphBegin
            If true, and if a hardware peripheral is being used (I2C or SPI,
            but not software SPI), call that peripheral's begin() function,
            else (false) it has already been done in one's sketch code.
            Cases where false might be used include multiple displays or
            other devices sharing a common bus, or situations on some
            platforms where a nonstandard begin() function is available
            (e.g. a TwoWire interface on non-default pins, as can be done
            on the ESP8266 and perhaps others).
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool Adafruit_SH110X::begin(uint8_t vcs, uint8_t addr, boolean reset) {

  Adafruit_MonoOLED::_init(addr, reset);

  if(HEIGHT > 32) {
    drawBitmap((WIDTH - splash1_width) / 2, (HEIGHT - splash1_height) / 2,
      splash1_data, splash1_width, splash1_height, 1);
  } else {
    drawBitmap((WIDTH - splash2_width) / 2, (HEIGHT - splash2_height) / 2,
      splash2_data, splash2_width, splash2_height, 1);
  }

  // Init sequence, make sure its under 32 bytes, or split into multiples!
  static const uint8_t init[] = {
    SH110X_DISPLAYOFF,                   // 0xAE
    SH110X_SETDISPSTARTLINE, 0x0,        // 0xDC 0x00
    SH110X_SETCONTRAST, 0x2F,            // 0x81, 0x2F
    SH110X_MEMORYMODE,                   // 0x20
    SH110X_SEGREMAP,                     // 0xA0
    SH110X_COMSCANINC,                   // 0xC0
    SH110X_SETMULTIPLEX, 0x7F,           // 0xa8, 0x7f,
    SH110X_SETDISPLAYOFFSET, 0x60,       // 0xd3, 0x60,
    SH110X_SETDISPLAYCLOCKDIV, 0x51,     // 0xd5, 0x51,
    SH110X_SETPRECHARGE, 0x22,           // 0xd9, 0x22,
    SH110X_SETVCOMDETECT, 0x35,          // 0xdb, 0x35,
    SH110X_SETPAGEADDR,                  // 0xb0
    SH110X_SETCOMPINS, 0x12,             // 0xda, 0x12,
    SH110X_DISPLAYALLON_RESUME,          // 0xa4
    SH110X_NORMALDISPLAY,                // 0xa6
    SH110X_DISPLAYON,                    // 0xaf
  };

  if (! oled_commandList(init, sizeof(init))) {
    return false;
  }

  return true; // Success
}


// REFRESH DISPLAY ---------------------------------------------------------

/*!
    @brief  Push data currently in RAM to SH110X display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void Adafruit_SH110X::display(void) {
#if defined(ESP8266)
  // ESP8266 needs a periodic yield() call to avoid watchdog reset.
  // With the limited size of SH110X displays, and the fast bitrate
  // being used (1 MHz or more), I think one yield() immediately before
  // a screen write and one immediately after should cover it.  But if
  // not, if this becomes a problem, yields() might be added in the
  // 32-byte transfer condition below.
  yield();
#endif
  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr   = buffer;
  uint8_t dc_byte = 0x40;
  uint8_t pages = ((HEIGHT + 7) / 8);

  uint8_t bytes_per_page = WIDTH;

  uint16_t maxbuff = i2c_dev->maxBufferSize() - 1;

  if (i2c_dev) { // I2C
    // how many buffers-worth are we sending?
    uint8_t buffs = (bytes_per_page / maxbuff);
    if (count % maxbuff) {
      buffs++;
    }
    Serial.print("Have to send "); Serial.print(buffs); 
    Serial.println(" buffs per page");

    for (uint8_t p=0; p<pages; p++) {
      uint8_t bytes_remaining = bytes_per_page;
      
      Serial.print(F("Writing page addr "));
      Serial.println(p);

      uint8_t cmd[] = {0x00, SH110X_SETPAGEADDR + p, 0x10, 0x00};
      i2c_dev->write(cmd, 4);

      while (bytes_remaining) {
	uint8_t to_write = min(bytes_remaining, maxbuff);
	i2c_dev->write(ptr, to_write, true, &dc_byte, 1);
	ptr += to_write;
	bytes_remaining -= to_write;
#if defined(ESP8266)
	yield();
#endif
      }
    }
  } else { // SPI
    //SH110X_MODE_DATA
    //while(count--) SPIwrite(*ptr++);
  }

}

