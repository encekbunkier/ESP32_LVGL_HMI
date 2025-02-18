#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "lvgl_ui.h"
#include "meters_setup.h"
#include "audioplayer.h"

#undef TFT_BL
#define TFT_BL 2

extern lv_img_dsc_t drawing;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

// ST7262 IPS LCD 800x480
Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
    bus,
    800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

#define LV_USE_LOG 0
//#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
//#endif

#include "touch.h"

lv_obj_t *countLabel;
unsigned long count = 0;
lv_obj_t *led[3];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (touch_has_signal())
    {
        if (touch_touched())
        {
            data->state = LV_INDEV_STATE_PR;

            /*Set the coordinates*/
            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
        }
        else if (touch_released())
        {
            data->state = LV_INDEV_STATE_REL;
        }
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setupUI()
{
    String LVGL_Arduino = "HMI 1.0";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    
    lv_init();
    delay(500);
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    gfx->begin();
    gfx->fillScreen(BLACK);
    touch_init();

    pinMode(TFT_BL, OUTPUT);
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[SCREEN_BUF_SIZE];
    static lv_color_t buf2[SCREEN_BUF_SIZE];
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, SCREEN_BUF_SIZE);

    lv_disp_draw_buf_init( &draw_buf, buf1, buf2, SCREEN_BUF_SIZE);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = SCREENWIDTH;
    disp_drv.ver_res = SCREENHEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    Serial.println( "Setup done" );
}

void showMainScreen()
{
    const int NUM_COLS = 3;
    const int NUM_ROWS = 6;
    const int colWidth = SCREENWIDTH / NUM_COLS - 20;
    const int rowHeight = SCREENHEIGHT / NUM_ROWS - 20;
    static lv_coord_t col_dsc[NUM_COLS + 1] = {colWidth, colWidth, colWidth, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[NUM_ROWS + 1] = {rowHeight, rowHeight, rowHeight, rowHeight, rowHeight,
                                               rowHeight, LV_GRID_TEMPLATE_LAST};
    gfx->fillScreen(BLACK);
    /*Create a grid*/
    lv_obj_set_style_grid_column_dsc_array(lv_scr_act(), col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(lv_scr_act(), row_dsc, 0);
    lv_obj_set_size(lv_scr_act(), SCREENWIDTH, SCREENHEIGHT);
    lv_obj_center(lv_scr_act());
    lv_obj_set_layout(lv_scr_act(), LV_LAYOUT_GRID);

    // Create text label
    countLabel = lv_label_create(lv_scr_act());
    lv_label_set_text(countLabel, "00000");
    lv_obj_set_style_text_align(countLabel, LV_TEXT_ALIGN_CENTER, 0);
    static lv_style_t style_label;
    lv_style_init(&style_label);
    // Increase font size
    lv_style_set_text_font(&style_label, &lv_font_montserrat_48);
    lv_obj_add_style(countLabel, &style_label, LV_PART_MAIN);

    lv_obj_align(countLabel, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_grid_cell(countLabel, LV_GRID_ALIGN_STRETCH, 2, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    setupMeters();
    buttonSetup();
    ledSetup();

#ifdef KEYBOARD
    lv_example_keyboard_1();
#endif
}

void buttonSetup()
{
    const int BTN_ROW_START = 5;
    const int NUM_BUTTONS = 6;
    static const char *buttons[NUM_BUTTONS] = {"AUX1", "AUX2", "AUX3", "AUX4", "AUX5", "SHUTDOWN"};

    lv_obj_t *button;
    lv_obj_t *buttonLabel;
    int i = 0;
    for (int col = 0; col < 3; col++)
    {
        button = lv_btn_create(lv_scr_act());
        lv_obj_set_height(button, LV_SIZE_CONTENT*2);
        lv_obj_set_grid_cell(button, LV_GRID_ALIGN_STRETCH, col, 1,
                             LV_GRID_ALIGN_STRETCH, BTN_ROW_START, 1);

        buttonLabel = lv_label_create(button);
        lv_label_set_text(buttonLabel, buttons[i++]);
        lv_obj_center(buttonLabel);
        // Assign handlers
        switch(col)
        {
        case 0:
            lv_obj_add_event_cb(button, event_handlerBtn1, LV_EVENT_CLICKED, &col);
            break;
        case 1:
            lv_obj_add_event_cb(button, event_handlerBtn2, LV_EVENT_CLICKED, &col);
            break;
        case 2:
            lv_obj_add_event_cb(button, event_handlerBtn3, LV_EVENT_CLICKED, &col);
            break;
        }
    }
}

void ledSetup()
{
    const int LED_ROW_START = 4;
    const int NUM_LEDS = 3;

    for (int col = 0; col < 3; col++)
    {
        led[col] = lv_led_create(lv_scr_act());
        lv_led_set_brightness(led[col], LV_LED_BRIGHT_MAX);
        lv_led_set_color(led[col], lv_palette_main(LV_PALETTE_RED));
        (col == 1) ? lv_led_on(led[col]) : lv_led_off(led[col]);

        lv_obj_set_grid_cell(led[col], LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, LED_ROW_START, 1);
    }
}

// Handler for status screen button
void event_handlerBtn1(lv_event_t *e)
{
    count = 0;
}

void event_handlerBtn2(lv_event_t *e)
{
    lv_led_off(led[0]);
    play();
}

void event_handlerBtn3(lv_event_t *e)
{
    lv_led_on(led[0]);
}

// Update UI
void uiLoop()
{
    static unsigned long lastTick = millis();
    if (millis() - lastTick > 500)
    {
        lastTick = millis();
        count++;
        char text[8];
        sprintf(text, "%05d", count);
        lv_label_set_text(countLabel, text);
    }

    lv_timer_handler(); /* let the GUI do its work */
    lv_tick_inc(5); 
    delay(5);
}

void updateTimer(lv_timer_t * timer)
{
  /*Use the user_data*/
  //uint32_t *user_data = static_cast<uint32_t*>(timer->user_data);
}

#ifdef KEYBOARD
void lv_example_keyboard_1(void)
{
    //Create a keyboard to use it with an of the text areas
    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);

    //Create a text area. The keyboard will write here
    lv_obj_t *ta1;
    ta1 = lv_textarea_create(lv_scr_act());

    static lv_style_t style_label;
    lv_style_init(&style_label);
    lv_style_set_text_font(&style_label, &lv_font_montserrat_48);
    lv_obj_add_style(ta1, &style_label, LV_PART_MAIN);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta1, "00000.0");
    lv_obj_set_size(ta1, SCREENWIDTH/2, 80);
    lv_obj_set_grid_cell(kb, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    lv_keyboard_set_textarea(kb, ta1);

    // Put in grid
    lv_obj_set_grid_cell(kb, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 2);
}

void ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED)
    {
        lv_label_set_text(countLabel, "Enter");
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        //lv_label_set_text(countLabel, lv_label_get_text(ta));
        lv_label_set_text(countLabel,"Exit");

        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        char buf[10];
        itoa((int)code, buf, 10);
        lv_label_set_text(countLabel, buf);
    }
}
#endif

#ifdef SPINBOX
static lv_obj_t *spinbox;

static void lv_spinbox_increment_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_increment(spinbox);
    }
}

static void lv_spinbox_decrement_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_decrement(spinbox);
    }
}

void lv_example_spinbox_1(void)
{
    spinbox = lv_spinbox_create(lv_scr_act());
    lv_spinbox_set_range(spinbox, -1000, 25000);
    lv_spinbox_set_digit_format(spinbox, 5, 4);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 100);
    lv_obj_center(spinbox);
    //lv_obj_set_grid_cell(spinbox, LV_GRID_ALIGN_START, 1, 2, LV_GRID_ALIGN_START, 1, 1);
    lv_coord_t h = lv_obj_get_height(spinbox);

    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 2*h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL, NULL);

    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 2*h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, NULL);
}
#endif