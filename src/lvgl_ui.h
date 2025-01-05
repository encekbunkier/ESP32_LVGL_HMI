#ifndef _LVGL_UI
#define _LVGL_UI

#include <lvgl.h>

/*Screen resolution*/
constexpr uint16_t SCREENWIDTH  = 800;
constexpr uint16_t SCREENHEIGHT = 480;
constexpr uint16_t SCREEN_BUF_SIZE = SCREENWIDTH * SCREENHEIGHT / 10;

// Locations
constexpr int TOP_LINE_Y = 1;
constexpr int BOTTOM_LINE_Y = SCREENHEIGHT - 20;

static lv_disp_draw_buf_t draw_buf;

static lv_color_t buf1[SCREEN_BUF_SIZE];
static lv_color_t buf2[SCREEN_BUF_SIZE];
static lv_color_t okColor;
static lv_color_t warningColor;
static lv_color_t alarmColor;

//Function prototypes
void setupUI(void);
void buttonSetup();
void ledSetup();
void uiLoop();
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p );
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );
void updateTimer(lv_timer_t * timer);
void showMainScreen();
void event_handlerBtn1(lv_event_t *e);
void event_handlerBtn2(lv_event_t *e);
void event_handlerBtn3(lv_event_t *e);
void ta_event_cb(lv_event_t *e);
void lv_example_keyboard_1(void);
void lv_example_spinbox_1();

#endif