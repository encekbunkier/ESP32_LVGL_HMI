#ifndef _METERS_SETUP
#define _METERS_SETUP

#include <lvgl.h>
//#include <lv_demo.h>
#include "lvgl_ui.h"

#define LED_BUILTIN 13

// Locations
constexpr int METER_X = SCREENWIDTH/2;
constexpr int METER_SIZE = 200;

// Meter Ranges
constexpr int START_VALUE = 0;
constexpr int RPM_MAX = 3000;
constexpr int LOW_RPM_LIMIT = RPM_MAX* 0.1;
constexpr int WARNING_RPM_LIMIT = RPM_MAX * 0.7;
constexpr int HIGH_RPM_LIMIT = RPM_MAX * 0.80;

static lv_obj_t *meterLabel;
static lv_obj_t *meterLegendLabel;

static lv_obj_t *meter = {0};
static lv_meter_indicator_t * indicator = {0};
static lv_timer_t *timer_handle;
static uint32_t data = 1010;

//Function prototypes
void setRpmValue(int val);
void setupMeters(void);
static void set_Metervalue(void *indic, int32_t v);
void lv_setupLabels();

void lv_setupMeter(void);
void createMeterAnimation(lv_meter_indicator_t* indicator, uint32_t min, uint32_t max);

#endif
