/*!
 * @file Adafruit_SH1115.cpp
 *
 * This is part of Adafruit's SH1115 driver for monochrome OLED displays.
 * The SH1115 (also known as CH1115) is similar to SH1106 but has some
 * differences in initialization and features.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * BSD license, all text above, and the splash screen included below,
 * must be included in any redistribution.
 *
 */

#include "Adafruit_SH1115.h"
#include "splash.h"

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SH1115 displays.
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
    @param  preclk
            Speed (in Hz) for Wire transmissions in library calls.
    @param  postclk
            Speed (in Hz) for Wire transmissions following library calls.
*/
Adafruit_SH1115::Adafruit_SH1115(uint16_t w, uint16_t h, TwoWire *twi,
                                 int8_t rst_pin, uint32_t preclk,
                                 uint32_t postclk)
    : Adafruit_SH110X(w, h, twi, rst_pin, preclk, postclk) {
  _page_start_offset =
      0; // SH1115 is 128x64, no column offset needed (unlike SH1106)
}

/*!
    @brief  Constructor for SPI SH1115 displays, using software (bitbang)
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
*/
Adafruit_SH1115::Adafruit_SH1115(uint16_t w, uint16_t h, int8_t mosi_pin,
                                 int8_t sclk_pin, int8_t dc_pin, int8_t rst_pin,
                                 int8_t cs_pin)
    : Adafruit_SH110X(w, h, mosi_pin, sclk_pin, dc_pin, rst_pin, cs_pin) {
  _page_start_offset = 0;
}

/*!
    @brief  Constructor for SPI SH1115 displays, using native hardware SPI.
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
            SPI clock rate for transfers to this display. Default is
            8000000UL (8 MHz).
*/
Adafruit_SH1115::Adafruit_SH1115(uint16_t w, uint16_t h, SPIClass *spi,
                                 int8_t dc_pin, int8_t rst_pin, int8_t cs_pin,
                                 uint32_t bitrate)
    : Adafruit_SH110X(w, h, spi, dc_pin, rst_pin, cs_pin, bitrate) {
  _page_start_offset = 0;
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  i2caddr
            I2C address of corresponding display (or pass 0 to use default
            of 0x3C for 128x64 display, 0x3D for any other size).
    @param  reset
            If true, and if the reset pin passed to the constructor is
            valid, a hard reset will be performed before initializing the
            display. If using multiple displays on the same bus, and if they
            all share the same reset pin, you should only pass true on the
            first display being initialized, false on all others, else the
            already-initialized displays would be reset. Default if unspecified
            is true.
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool Adafruit_SH1115::begin(uint8_t i2caddr, bool reset) {

  if (!Adafruit_SH110X::_init(i2caddr, reset)) {
    return false;
  }

  // SH1115-specific initialization sequence
  // Based on CH1115 datasheet Power On and Initialization section

  static const uint8_t init_sequence[] = {
      SH110X_DISPLAYOFF, // 0xAE - Display OFF

      // Display configuration
      SH110X_SETDISPLAYCLOCKDIV, // 0xD5
      0x50,                      // Default divide ratio (POR value)

      SH110X_SETMULTIPLEX, // 0xA8
      0x3F,                // 64 MUX (for 128x64 display)

      SH110X_SETDISPLAYOFFSET, // 0xD3
      0x00,                    // No offset

      SH110X_SETSTARTLINE | 0x00, // 0x40 - Start line 0

      // Charge pump (DC-DC converter)
      SH1115_SETDCDCCONTROL, // 0xAD
      SH1115_SETDCDC,        // 0x8B - Enable DC-DC

      // Pump voltage
      SH1115_SETPUMPVOLTAGE | 0x02, // 0x32 - 8.0V (default)

      // Segment remap and COM configuration
      SH110X_SEGREMAP | 0x01, // 0xA1 - Column address 127 mapped to SEG0
      SH110X_COMSCANDEC,      // 0xC8 - Scan from COM[N-1] to COM0

      SH1115_SETSEGPADS | 0x00, // 0xA2 - SEG pads: Even on the left

      SH110X_SETCOMPINS, // 0xDA
      0x12,              // Alternative COM config, disable remap

      // Timing and driving
      SH110X_SETPRECHARGE, // 0xD9
      0x22,                // Discharge=2 DCLKs, Pre-charge=2 DCLKs (POR)

      SH110X_SETVCOMDETECT, // 0xDB
      0x35,                 // VCOM deselect level (POR value)

      SH1115_SETROWPERIOD, // 0xDC
      0x01,                // Row non-overlap: 3 DCLKs (default)

      // Display settings
      SH110X_SETCONTRAST, // 0x81
      0x80,               // Contrast = 128 (POR value)

      SH110X_NORMALDISPLAY,       // 0xA6 - Normal display (not inverted)
      SH110X_DISPLAYALLON_RESUME, // 0xA4 - Resume from RAM

      SH1115_SETADAPTIVESAVE, // 0xD7 - Enable adaptive power save

      // Clear display RAM
      SH110X_SETPAGEADDR,   // Set page address
      SH110X_SETLOWCOLUMN,  // Set lower column address
      SH110X_SETHIGHCOLUMN, // Set higher column address
  };

  // Send init commands
  uint8_t *ptr = (uint8_t *)init_sequence;

  for (uint8_t i = 0; i < sizeof(init_sequence); i++) {
    oled_command(ptr[i]);
  }

  // Clear the display RAM
  clearDisplay();
  display();

  delay(100); // Wait for display to stabilize

  // Turn on display
  oled_command(SH110X_DISPLAYON); // 0xAF

  return true;
}

// SH1115 SPECIFIC FEATURES ------------------------------------------------

/*!
    @brief  Enable or disable breathing display effect.
    @param  enable
            true to enable breathing effect, false to disable
    @param  maxBrightness
            Maximum brightness level (0-3):
            0 = Contrast+1 x 256 (default)
            1 = Contrast+1 x 128
            2 = Contrast+1 x 64
            3 = Contrast+1 x 32
    @param  interval
            Time interval step (0-7):
            0 = 1 frame, 1 = 2 frames (default), ..., 7 = 8 frames
    @return None (void).
*/
void Adafruit_SH1115::setBreathing(bool enable, uint8_t maxBrightness,
                                   uint8_t interval) {
  oled_command(SH1115_SETBREATHING);

  uint8_t config = 0;
  if (enable) {
    config |= 0x10; // Set ON/OFF bit
  }
  config |= ((maxBrightness & 0x03) << 3); // A4-A3
  config |= (interval & 0x07);             // A2-A0

  oled_command(config);
}

/*!
    @brief  Set the internal charge pump voltage.
    @param  voltage
            Voltage level (0-3):
            0 = 10.0V
            1 = 7.4V
            2 = 8.0V (default/POR)
            3 = 9.0V
    @return None (void).
    @note   Make sure VDD2 voltage supports the selected pump voltage.
*/
void Adafruit_SH1115::setPumpVoltage(uint8_t voltage) {
  oled_command(SH1115_SETPUMPVOLTAGE | (voltage & 0x03));
}

/*!
    @brief  Enable or disable adaptive power save mode.
    @param  enable
            true to enable adaptive power save (default), false to disable
    @return None (void).
    @note   Adaptive power save reduces power consumption when fewer
            pixels are lit.
*/
void Adafruit_SH1115::setAdaptivePowerSave(bool enable) {
  oled_command(enable ? SH1115_SETADAPTIVESAVE : 0xD6);
}