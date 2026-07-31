#include "display.h"

//Display::Display() {
//
//}

Display::Display(SPIClass* spi, int cs, int dc, int rst)
  : _cs(cs), _dc(dc), _rst(rst), _spi(spi) {
    #ifdef DISPLAY_EPAPER
      _epd = new EPaperGFX(EPD_DRIVER_CLASS(_cs, _dc, _rst, EPD_BUSY));
      tft  = _epd;
    #else
      tft = new Adafruit_ST7735(_spi, _cs, _dc, _rst);
    #endif
}

void Display::begin() {
  #ifdef DISPLAY_EPAPER
    // Shares the SPI bus with the SD card, so hand GxEPD2 the same SPIClass
    // and let it wrap its transfers in its own transactions.
    _epd->init(0, true, 10, false, *_spi, SPISettings(TFT_SPI_SPEED, MSBFIRST, SPI_MODE0));

    _epd->setFullWindow();
    _epd->setTextWrap(false);
    _epd->setRotation(DISPLAY_ROTATION);

    this->clearScreen();

    this->drawMonochromeImage(logo2, 160, 80);

    this->flush(true);
  #else
    pinMode(TFT_BL, OUTPUT);

    this->ctrlBacklight(false);

    //tft.init();
    #ifndef JCMK_HOST_BOARD
      ((Adafruit_ST7735*)tft)->initR(INITR_MINI160x80_PLUGIN);
    #else
      ((Adafruit_ST7735*)tft)->initR(INITR_MINI160x80);
    #endif

    ((Adafruit_ST7735*)tft)->setSPISpeed(TFT_SPI_SPEED);

    this->clearScreen();

    tft->setTextWrap(false);

    tft->setRotation(DISPLAY_ROTATION);

    this->drawMonochromeImage(logo2, 160, 80);

    this->ctrlBacklight(true);
  #endif
}

// Refreshes the e-Paper panel from the framebuffer. Rate limited, because a
// refresh takes between half a second and four seconds and blocks while it
// runs. Does nothing when nothing has been drawn since the last refresh.
void Display::flush(bool force) {
  #ifdef DISPLAY_EPAPER
    if (!_epd->dirty) return;

    uint32_t now = millis();

    if ((!force) && (now - _last_flush < EPD_MIN_REFRESH_TIME)) return;

    // Partial refreshes are fast but leave ghosting behind, so fall back to a
    // full refresh periodically to clean the panel up.
    bool full = (_partial_count >= EPD_FULL_REFRESH_EVERY);

    _epd->display(!full);

    _partial_count = full ? 0 : _partial_count + 1;
    _epd->dirty    = false;
    _last_flush    = millis();
  #endif
}

void Display::drawCenteredText(String text, bool centerVertically) {
  tft->setRotation(DISPLAY_ROTATION);  // Landscape
  tft->setTextSize(1);  // 6x8 per char
  tft->setTextColor(COLOR_FG, COLOR_BG);
  tft->setTextWrap(false);

  uint8_t charWidth = 6;
  uint8_t charHeight = 8;

  uint16_t textWidth = text.length() * charWidth;
  uint16_t textHeight = charHeight;

  uint16_t x = (TFT_WIDTH - textWidth) / 2;
  uint16_t y = centerVertically ? (TFT_HEIGHT - textHeight) / 2 : tft->getCursorY();

  tft->setCursor(x, y);
  tft->print(text);

  // These are status messages, usually followed by a delay() or a blocking
  // operation, so they have to reach the panel now rather than whenever the
  // main loop next comes around.
  this->flush(true);
}

// https://javl.github.io/image2cpp/
void Display::drawMonochromeImage(const uint8_t* imageData, int width, int height) {
  // Center on panels larger than the image (the logo is drawn for 160x80)
  int x0 = (TFT_WIDTH  - width)  / 2;
  int y0 = (TFT_HEIGHT - height) / 2;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;

  tft->startWrite();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int byteIndex = (y * (width / 8)) + (x / 8);
      uint8_t byteVal = pgm_read_byte(&imageData[byteIndex]);

      // MSB first (bit 7 is leftmost pixel)
      bool pixelOn = (byteVal >> (7 - (x % 8))) & 0x01;
      uint16_t color = pixelOn ? COLOR_FG : COLOR_BG;

      tft->writePixel(x0 + x, y0 + y, color);
    }
  }

  tft->endWrite();
}

void Display::ctrlBacklight(bool on) {
  #ifdef DISPLAY_EPAPER
    // No backlight to control, and the panel holds its image with the power
    // off. Blanking it is the closest equivalent of turning the light off.
    if (!on) {
      this->clearScreen();
      this->flush(true);
    }
  #else
    if (on)
      digitalWrite(TFT_BL, ON);
    else
      digitalWrite(TFT_BL, OFF);
  #endif
}

void Display::clearScreen() {
  tft->fillScreen(COLOR_BG);
}

void Display::main(uint32_t currentTime) {
  // Catches anything drawn by code that does not flush for itself, and keeps
  // the refresh rate limit in one place.
  this->flush();
}
