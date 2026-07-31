#ifndef configs_h
#define configs_h

// ============================================================
// Board selection
// ============================================================
// Uncomment the line below (or build with -DBOARD_WAVESHARE_C5_KIT) to use the
// pin map for the Waveshare ESP32-C5-WIFI6-KIT. The default map targets the
// JCMK C5 Wardriver host board with an ESP32-C5-DevKitC-1.
//
// This selects GPIO assignments only. JCMK_HOST_BOARD below is a separate
// switch covering ST7735 module quirks and is independent of the board.
//#define BOARD_WAVESHARE_C5_KIT

#define JCMK_HOST_BOARD

// ============================================================
// Pin map
// ============================================================
#ifndef BOARD_WAVESHARE_C5_KIT

//// ---- JCMK C5 Wardriver host board (ESP32-C5-DevKitC-1) ----
/*
1 BTN
2 SPI
4 BAT I2C
5 BAT I2C
6 SPI
7 SPI
8 BTN
9 BTN
10 SD CS
13 GPS UART
14 GPS UART
23 TFT
24 TFT
25 EPD BUSY (e-Paper builds only)
27 TFT BL / EPD RST
28 ACT LED
*/

// SPI bus, shared by the display and the SD card
#define SPI_SCK   6
#define SPI_MISO  2
#define SPI_MOSI  7
#define SD_CS     10

// Display control lines
#define PIN_DISP_CS   23
#define PIN_DISP_DC   24
#define PIN_DISP_RST  -1   // TFT reset runs to the DevKit RST line
#define PIN_DISP_BL   27
#define PIN_EPD_RST   27   // e-Paper has no backlight, so RST reuses that pin
#define PIN_EPD_BUSY  25

// User buttons — these need external pull-down resistors
#define U_BTN 9
#define D_BTN 8
#define C_BTN 1

// Battery fuel gauge
#define I2C_SCL 4
#define I2C_SDA 5

// GPS UART
#define TX_TO_GPS 13
#define RX_TO_GPS 14

// Activity LED
#define LED_PIN 28

#else

//// ---- Waveshare ESP32-C5-WIFI6-KIT ----
/*
0  ACT LED
1  BTN
2  SPI
3  SPI
4  BAT I2C
5  BAT I2C
7  SPI
8  BTN
9  BTN
10 SD CS
13 GPS UART
14 GPS UART
23 TFT
24 TFT
25 EPD BUSY (e-Paper builds only)
27 TFT BL / EPD RST
*/
// Two pins move relative to the stock map, both because of onboard circuitry
// on this board (verified against the ESP32-C5-WIFI6-KIT-NXRX schematic).
// Everything else is unchanged, so the same wiring carries over.
//
//   SCK 6 -> 3   R39 (0R, populated) ties GPIO6 to the BAT_ADC node, which
//                carries C14+C15 = 200nF to ground plus a 200K/100K divider.
//                No clock survives that. GPIO3 is free on P1-4; its MTDI strap
//                only selects the SDIO sampling edge, which this firmware
//                never uses, and a clock output is high-Z at reset anyway.
//                If you would rather remove R39 — the firmware reads the
//                battery over I2C, so the onboard divider is redundant — put
//                SCK back on 6 to get the native IOMUX routing.
//   LED 28 -> 0  GPIO28 is the BOOT net on this board: the BOOT button to
//                ground, the CH343 auto-download transistor and a 10K pull-up.
//                An LED to ground holds that strapping pin below VIH at reset,
//                so the chip would come up in serial download mode instead of
//                running the firmware. GPIO0 is clean — its 32.768kHz crystal
//                network (Y1, R12) is not populated.
//
// Pins to leave alone on this board: GPIO6 (see above), GPIO11/GPIO12 (UART
// console through the CH343), GPIO15 (module pin 19 is NC) and GPIO28.

// SPI bus, shared by the display and the SD card
#define SPI_SCK   3
#define SPI_MISO  2
#define SPI_MOSI  7
#define SD_CS     10

// Display control lines
#define PIN_DISP_CS   23
#define PIN_DISP_DC   24
#define PIN_DISP_RST  -1   // TFT reset runs to the board RST pin
#define PIN_DISP_BL   27
#define PIN_EPD_RST   27   // 10K pull-up (R8) keeps the boot strap high
#define PIN_EPD_BUSY  25   // P1-13, nothing else on the net

// User buttons — these need external pull-down resistors, so they are kept off
// strapping pins
#define U_BTN 9
#define D_BTN 8
#define C_BTN 1

// Battery fuel gauge
#define I2C_SCL 4
#define I2C_SDA 5

// GPS UART — also this board's native USB D-/D+, so flash and monitor through
// the UART Type-C port rather than the USB one
#define TX_TO_GPS 13
#define RX_TO_GPS 14

// Activity LED
#define LED_PIN 0

#endif

//// Firmware info stuff
#define FIRMWARE_VERSION "v2.3.0"
#define DEVICE_NAME      "JCMK C5 Wardriver"

//// Role stuff
#define SOLO
// #define CORE
// #define NODE

#if !defined(SOLO) && !defined(CORE) && !defined(NODE)
  #error "Define exactly one role: SOLO, CORE, or NODE"
#endif
#if defined(SOLO) && defined(CORE) && defined(NODE)
  #error "Define exactly one role: SOLO, CORE, or NODE"
#elif defined(SOLO) && defined(CORE)
  #error "Define exactly one role: SOLO, CORE, or NODE"
#elif defined(CORE) && defined(NODE)
  #error "Define exactly one role: SOLO, CORE, or NODE"
#elif defined(SOLO) && defined(NODE)
  #error "Define exactly one role: SOLO, CORE, or NODE"
#endif

#define SOLO_MODE 1
#define NODE_MODE 2
#define CORE_MODE 3

#define ENOW_KEY_MAX_LEN 32
#define ENOW_TEXT_MAX    200

//// BLE stuff
#define BLE_SCAN_DURATION   1 * 500 // 0.5 second


//// Display stuff
#define ON  HIGH
#define OFF LOW

// Display control pins come from the board pin map above
#define TFT_CS   PIN_DISP_CS
#define TFT_DC   PIN_DISP_DC
#define TFT_MOSI SPI_MOSI
#define TFT_SCLK SPI_SCK
#define TOUCH_CS -1

// Uncomment the line below (or build with -DDISPLAY_EPAPER) to build for the
// LAFVIN 2.13" SPI e-Paper panel instead of the stock 0.96" ST7735 TFT.
// Only one display driver is compiled in at a time.
//#define DISPLAY_EPAPER

#ifndef DISPLAY_EPAPER

//// ---- ST7735 0.96" 160x80 TFT (default) ----
#define TFT_HEIGHT 80
#define TFT_WIDTH  160

#define TFT_SPI_SPEED 27000000

#define TFT_RST  PIN_DISP_RST
#define TFT_BL   PIN_DISP_BL

#define DISPLAY_ROTATION 3

#else

//// ---- LAFVIN 2.13" SPI e-Paper, 250x122, SSD1680 ----
// The panel shares SCK/MOSI with the SD card and needs two more control lines
// than the TFT did: BUSY (new) and a real RST (the TFT left RST unconnected).
// The backlight pin is free on an e-Paper build, so RST reuses that pin.
//
// Both control pins land on ESP32-C5 strapping pins, which is unavoidable —
// GPIO25/26/27/28 are what is left over once the display, SD, GPS, I2C and
// buttons are assigned. On the Waveshare ESP32-C5-WIFI6-KIT, whose schematic
// (ESP32-C5-WIFI6-KIT-NXRX) is the one available to check against, both are
// still safe:
//   GPIO25 is module pin 26 straight through to header P1-13 with nothing else
//     on the net — no pull, no load. Its strap only selects the SDIO sampling
//     clock edge, and this firmware reaches the SD card over SPI and never
//     uses SDIO, so whatever the panel drives BUSY to at reset is irrelevant.
//   GPIO27 is module pin 18 with R8, a 10K pull-up to 3V3, so the boot-mode
//     strap reads high at reset regardless of what is hanging off the pin. It
//     also runs through R3 (0R) to the DI pin of the onboard WS2812B-0807,
//     which is a high-impedance CMOS input and does not load the line. The
//     reset pulse GxEPD2 issues is not a valid WS2812 frame, so the LED stays
//     dark. Do not add a pull-down here: GPIO27 and GPIO28 both low at reset
//     is an invalid strapping combination.
#define TFT_HEIGHT 122
#define TFT_WIDTH  250

// SSD1680 is specified up to 20MHz; 4MHz is the speed GxEPD2 uses by default
// and is comfortable over ribbon/jumper wiring.
#define TFT_SPI_SPEED 4000000

#define TFT_RST  PIN_EPD_RST
#define TFT_BL   -1  // e-Paper has no backlight
#define EPD_BUSY PIN_EPD_BUSY

// GxEPD2 driver class for the panel. LAFVIN ships this HAT as a Waveshare
// 2.13" V4 (122x250, SSD1680); their epd2in13_V4 driver and GxEPD2_213_B74
// agree command for command:
//   0x01 driver output control  F9 00 00
//   0x3C border waveform        05
//   0x21 display update control 00 80
//   0x18 temperature sensor     80
//   0x22 full refresh           F7
//   BUSY asserted high
// The only difference is the partial refresh argument — LAFVIN sends FF and
// GxEPD2 sends FC, which is the same mode 2 update but leaves the panel
// powered for the next partial instead of shutting it down each time.
//
// If a differently sourced panel comes up blank or garbled, try:
//   GxEPD2_213_BN   DEPG0213BN  122x250, SSD1680
//   GxEPD2_213_B73  GDEH0213B73 122x250, SSD1675B  (older V2 panels)
//   GxEPD2_213_B72  GDEH0213B72 122x250, SSD1675A  (older V2 panels)
#define EPD_DRIVER_CLASS GxEPD2_213_B74

// 1 = landscape with the ribbon cable on the left, 3 = rotated 180 degrees
#define DISPLAY_ROTATION 1

// Refresh policy. A full refresh takes ~3.6s and flashes the panel; a fast
// partial refresh takes ~0.5s but leaves faint ghosting behind, so a full
// refresh is forced every EPD_FULL_REFRESH_EVERY partials to clean up.
#define EPD_MIN_REFRESH_TIME    1500  // ms between panel refreshes
#define EPD_FULL_REFRESH_EVERY  20    // partial refreshes before a full one

#endif


//// UI Stuff
#ifndef DISPLAY_EPAPER
  #define UI_UPDATE_TIME 5 * 1000  // stats redraw interval
  #define TIMER_UPDATE   1 * 1000  // countdown redraw interval
  #define MENU_MAX_VISIBLE 7       // menu rows that fit on screen
  #define UPLOAD_PROGRESS_STEP 1   // redraw upload % every N percent
#else
  // e-Paper redraws are slow and wear the panel, so everything ticks slower
  // and the taller screen fits more menu rows.
  #define UI_UPDATE_TIME 10 * 1000
  #define TIMER_UPDATE   5 * 1000
  #define MENU_MAX_VISIBLE 13
  #define UPLOAD_PROGRESS_STEP 10
#endif

// Button pins come from the board pin map above
#define C_PULL false
#define U_PULL false
#define D_PULL false

#define WEB_PAGE_TIMEOUT 60 * 1000 // 60 seconds
#define STATION_CONNECT_TIMEOUT 5 * 1000 // 5 seconds
#define WIFI_CONFIG "/settings.json"
#define LOG_FILE_NAME "wardrive"
#define SETTING_SANITY "t_ssid"

#define SMALL_CHAR_HEIGHT 8

//// Stats screen (Screen 1) row layout — row positions and text sizes differ
//// between the 160x80 TFT and the 250x122 e-Paper panel.
#ifndef DISPLAY_EPAPER
  #define STAT_HDR_Y      0
  #define STAT_STATUS_Y   9
  #define STAT_DIV1_Y     19
  #define STAT_LABEL_Y    21
  #define STAT_COUNT_Y    30
  #define STAT_COUNT_SIZE 2
  #define STAT_DIV2_Y     47
  #define STAT_TOTAL_Y    50
  #define STAT_TOTAL_SIZE 2
  #define STAT_FOOTER_Y   71
#else
  #define STAT_HDR_Y      0
  #define STAT_STATUS_Y   12
  #define STAT_DIV1_Y     24
  #define STAT_LABEL_Y    28
  #define STAT_COUNT_Y    40
  #define STAT_COUNT_SIZE 3
  #define STAT_DIV2_Y     70
  #define STAT_TOTAL_Y    76
  #define STAT_TOTAL_SIZE 3
  #define STAT_FOOTER_Y   108
#endif


//// Buffer stuff
#define BUF_SIZE 2 * 1024
#define SNAP_LEN 2324


//// Battery stuff — I2C pins come from the board pin map above
#define HAS_BATTERY


//// GPS stuff — UART pins come from the board pin map above
#define GPS_SERIAL_INDEX 1


//// SD stuff — SPI pins and SD_CS come from the board pin map above
#define UPDATE_KEY "UpdateFile"

//// Debug log
#define DEBUG_LOG_FILE "/debug.log"


//// Switch stuff



//// Device stuff
#define HAS_PSRAM
#define HAS_GPS
#define HAS_SD


////WiFi stuff
#define mac_history_len 200
#define CHANNEL_TIMER 80
#define LOG_ROLL_ENTRIES  10000  // start a new log file after this many entries


// ============================================================
// Chunk 1: Extended feature constants
// ============================================================

//// Geofence stuff
#define MAX_GEOFENCES         5

//// SSID Exclusion stuff
#define MAX_SSID_EXCLUSIONS   10

//// Dock mode stuff
#define DOCK_CONNECT_ATTEMPTS  3
#define DOCK_CONNECT_TIMEOUT   10 * 1000   // 10 seconds per attempt
#define DOCK_SCAN_INTERVAL     30 * 1000   // passive scan every 30s while docked
#define DOCK_DEPART_SCANS      2           // consecutive misses before resuming wardrive
#define DOCK_FAIL_DISPLAY_MS   20 * 1000   // show K1T failure message for 20s
#define STANDBY_SCAN_INTERVAL  30 * 1000   // scan for K1T every 30s while in standby (no GPS)

//// Settings JSON buffer — bumped from 2048 to handle 30 settings entries
#define SETTINGS_JSON_SIZE     4096

// ============================================================
// Chunk 6: Dock mode state constants
// ============================================================
#define DOCK_STATE_NONE       0  // wardriving normally
#define DOCK_STATE_CONNECTING 1  // attempting WiFi connect to trigger SSID
#define DOCK_STATE_UPLOADING  2  // uploading all pending log files
#define DOCK_STATE_MONITORING 3  // watching for trigger SSID departure
#define DOCK_STATE_FAILED     4  // connect failed, showing message before resume

#endif
