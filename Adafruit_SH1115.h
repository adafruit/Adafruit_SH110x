/*!
 * @file Adafruit_SH1115.h
 *
 * This is part of for Adafruit's SH1115 library for monochrome
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
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * BSD license, all text above, and the splash screen header file,
 * must be included in any redistribution.
 *
 */

#ifndef _Adafruit_SH1115_H_
#define _Adafruit_SH1115_H_

#include <Adafruit_SH110X.h>

// SH1115 specific commands (based on CH1115 controller)
#define SH1115_SETBREATHING 0x23    ///< Breathing display effect
#define SH1115_SETADDSCROLL 0x24    ///< Additional horizontal scroll setup
#define SH1115_SETSCROLLMODE 0x28   ///< Set scroll mode
#define SH1115_SETPUMPVOLTAGE 0x30  ///< Set pump voltage (0x30-0x33)
#define SH1115_SETIREF 0x82         ///< IREF resistor set
#define SH1115_SETSEGPADS 0xA2      ///< Set SEG pads hardware configuration
#define SH1115_SETDCDCCONTROL 0xAD  ///< DC-DC control mode set
#define SH1115_SETDCDC 0x8B         ///< DC-DC ON (use 0x8A for OFF)
#define SH1115_SETADAPTIVESAVE 0xD7 ///< Adaptive power save ON
#define SH1115_SETROWPERIOD 0xDC    ///< Row non-overlap/SEG Hiz period

/*!
    @brief  Class for interfacing with SH1115 OLED displays
*/
class Adafruit_SH1115 : public Adafruit_SH110X {
public:
  Adafruit_SH1115(uint16_t w, uint16_t h, TwoWire *twi = &Wire,
                  int8_t rst_pin = -1, uint32_t preclk = 400000,
                  uint32_t postclk = 100000);
  Adafruit_SH1115(uint16_t w, uint16_t h, int8_t mosi_pin, int8_t sclk_pin,
                  int8_t dc_pin, int8_t rst_pin, int8_t cs_pin);
  Adafruit_SH1115(uint16_t w, uint16_t h, SPIClass *spi, int8_t dc_pin,
                  int8_t rst_pin, int8_t cs_pin, uint32_t bitrate = 8000000UL);

  bool begin(uint8_t i2caddr = 0x3C, bool reset = true);

  // SH1115 specific features
  void setBreathing(bool enable, uint8_t maxBrightness = 0,
                    uint8_t interval = 1);
  void setPumpVoltage(uint8_t voltage);
  void setAdaptivePowerSave(bool enable);
};

#endif // _Adafruit_SH1115_H_