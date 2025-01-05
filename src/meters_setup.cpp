#include "meters_setup.h"
//#include "lvgl_ui.h"

void setupMeters()
{
    lv_setupLabels();
    lv_setupMeter();
}

static void set_Metervalue(void *indic, int32_t v)
{
    lv_meter_set_indicator_value(meter,(lv_meter_indicator_t*)indic, v);
    char text[6];
    itoa((int16_t)v, text, 10);
    lv_label_set_text( meterLabel, text );
}

void lv_setupLabels()
{
    static lv_style_t style_label;
    lv_style_init(&style_label);
    // Increase font size
    //lv_style_set_text_font(&style_label, &lv_font_montserrat_36);

    // RPM
    meterLegendLabel = lv_label_create( lv_scr_act() );    
    lv_obj_add_style(meterLegendLabel, &style_label, LV_PART_MAIN);    
    lv_label_set_text( meterLegendLabel, "RPM" );
    lv_obj_set_pos( meterLegendLabel, METER_X-5, TOP_LINE_Y); 

    meterLabel = lv_label_create( lv_scr_act() );    
    lv_obj_add_style(meterLabel, &style_label, LV_PART_MAIN);    
    lv_label_set_text( meterLabel, "000" );
    lv_obj_set_pos( meterLabel, METER_X-5, BOTTOM_LINE_Y); 
}



/**
 * RPM meter
 */
void lv_setupMeter(void)
{
    meter = lv_meter_create(lv_scr_act());
    lv_obj_center(meter);
    lv_obj_set_size(meter, METER_SIZE, METER_SIZE);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);
    // Set angular range of display
    lv_meter_set_scale_range(meter, scale, START_VALUE, RPM_MAX, 270, 135);    

    /*Add green arc for normal*/
    indicator = lv_meter_add_arc(meter, scale, 3, okColor, 0);
    lv_meter_set_indicator_start_value(meter, indicator, START_VALUE);
    lv_meter_set_indicator_end_value(meter, indicator, WARNING_RPM_LIMIT);

    /*Add yellow arc for warning*/
    indicator = lv_meter_add_arc(meter, scale, 3, warningColor, 0);
    lv_meter_set_indicator_start_value(meter, indicator, WARNING_RPM_LIMIT);
    lv_meter_set_indicator_end_value(meter, indicator, HIGH_RPM_LIMIT);

    /*Add a red arc to the end*/
    indicator = lv_meter_add_arc(meter, scale, 3, alarmColor, 0);
    lv_meter_set_indicator_start_value(meter, indicator, HIGH_RPM_LIMIT);
    lv_meter_set_indicator_end_value(meter, indicator, RPM_MAX);

    /*Add a needle line indicator*/
    indicator = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    // Put in grid
    lv_obj_set_grid_cell(meter, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_START, 0, 2);
    createMeterAnimation(indicator, START_VALUE, RPM_MAX);
}

// Startup animation for meter
void createMeterAnimation(lv_meter_indicator_t* indicator, uint32_t min, uint32_t max)
{
    //Create an animation for startup
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_Metervalue);
    lv_anim_set_var(&a, indicator);
    lv_anim_set_values(&a, min, max);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_start(&a);         
}


