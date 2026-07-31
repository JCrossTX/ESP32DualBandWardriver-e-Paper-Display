#ifndef display_h
#define display_h

#include <FS.h>
#include <LinkedList.h>
#include <Adafruit_GFX.h>
#include <functional>

#include "configs.h"
#include "assets.h"

#include "BatteryInterface.h"

#ifdef DISPLAY_EPAPER
  #include <GxEPD2_BW.h>
#else
  #include <Adafruit_ST7735.h>
#endif

extern BatteryInterface battery;

#ifdef DISPLAY_EPAPER
  // The panel is monochrome, so every color the UI asks for collapses to either
  // ink or paper. The UI was written for a TFT that draws light text on a black
  // background, which on paper would mean flooding the screen with ink, so the
  // two base colors are swapped: what the UI calls BLACK is the paper
  // background and what it calls WHITE is the ink.
  #define ST77XX_BLACK   GxEPD_WHITE
  #define ST77XX_WHITE   GxEPD_BLACK
  #define ST77XX_RED     GxEPD_BLACK
  #define ST77XX_GREEN   GxEPD_BLACK
  #define ST77XX_BLUE    GxEPD_BLACK
  #define ST77XX_YELLOW  GxEPD_BLACK
  #define ST77XX_CYAN    GxEPD_BLACK
  #define ST77XX_MAGENTA GxEPD_BLACK
  #define ST77XX_ORANGE  GxEPD_BLACK
#endif

#ifdef DISPLAY_EPAPER
  // The host board workaround below is an RGB565 shade, which GxEPD2 would
  // treat as "not black" and render as blank paper.
  #define CYAN GxEPD_BLACK
#elif defined(JCMK_HOST_BOARD)
  #define CYAN 0xFFE0
#else
  #define CYAN ST77XX_CYAN
#endif

// Semantic colors for the stats screen. The TFT uses 16-bit RGB565 shades that
// have no meaning on a monochrome panel, so those collapse to plain ink.
#define COLOR_BG ST77XX_BLACK
#define COLOR_FG ST77XX_WHITE
#ifdef DISPLAY_EPAPER
  #define COLOR_DIM     GxEPD_BLACK
  #define COLOR_DIVIDER GxEPD_BLACK
  #define COLOR_BLE     GxEPD_BLACK
#else
  #define COLOR_DIM     0x7BEF  // gray
  #define COLOR_DIVIDER 0x4208  // dark gray
  #define COLOR_BLE     0xF81F  // magenta
#endif

#ifdef DISPLAY_EPAPER
// GxEPD2 draws into a RAM framebuffer and nothing reaches the panel until
// display() is called. Wrapping the two primitives that write to that buffer
// lets Display know when something actually changed, so the panel can be
// refreshed on a schedule instead of on every draw call. Every other
// Adafruit_GFX primitive funnels down into drawPixel().
class EPaperGFX : public GxEPD2_BW<EPD_DRIVER_CLASS, EPD_DRIVER_CLASS::HEIGHT> {
  public:
    typedef GxEPD2_BW<EPD_DRIVER_CLASS, EPD_DRIVER_CLASS::HEIGHT> Base;

    EPaperGFX(EPD_DRIVER_CLASS epd) : Base(epd) {}

    bool dirty = false;

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
      this->dirty = true;
      Base::drawPixel(x, y, color);
    }

    void fillScreen(uint16_t color) {
      this->dirty = true;
      Base::fillScreen(color);
    }
};
#endif

class Display {
  public:
    int _cs, _dc, _rst;
    Display(SPIClass* spi, int cs, int dc, int rst);

    // Points at whichever driver was compiled in. Everything the UI needs is
    // Adafruit_GFX, so screen code does not care which panel is attached.
    Adafruit_GFX* tft;

    void begin();
    void main(uint32_t currentTime);
    void clearScreen();
    void ctrlBacklight(bool on = true);
    void drawCenteredText(String text, bool centerVertically = false);

    // Push what has been drawn to the panel. No-op on the TFT, which draws
    // straight to the screen. force skips the refresh rate limit.
    void flush(bool force = false);

  private:
    SPIClass* _spi;

    #ifdef DISPLAY_EPAPER
      EPaperGFX* _epd;
      uint32_t _last_flush     = 0;
      uint16_t _partial_count  = EPD_FULL_REFRESH_EVERY; // first refresh is full
    #endif

    void drawMonochromeImage(const uint8_t* imageData, int width, int height);

};

#endif
