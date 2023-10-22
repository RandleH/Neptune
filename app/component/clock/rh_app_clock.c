


#include "rh_app.h"
#include "rh_watch.h"


#define self   (&g_AppClock)

#define COLOR_BATTERY_LOOP_INDICATOR                    lv_color_hex(0x24900D)
#define COLOR_BATTERY_LOOP_MAIN                         lv_color_hex(0x404040)
#define COLOR_BATTERY_ICON_INDICATOR                    lv_color_hex(0xAAAAAA)
#define COLOR_BATTERY_ICON_MAIN                         lv_color_hex(0xAAAAAA)
#define COLOR_DATE_FONT                                 lv_color_hex(0xAAAAAA)
#define COLOR_DATE_MAIN                                 lv_color_hex(0x3C3C3C)

#define WIDTH_BATTERY_LOOP                              (7)




static inline void util__theme_setup__usc( lv_obj_t *scr, lv_obj_t *list[kNumOfAppClockActiveWidget]){
    extern lv_img_dsc_t   ui_img_916048531;     /* Clock Panel Image */
    extern lv_img_dsc_t   ui_img_392295026;     /* USC Icon Image */
    extern lv_img_dsc_t   ui_img_1793134324;    /* Needle Miniute Image */
    extern lv_img_dsc_t   ui_img_249914142;     /* Needle Hour Image */

    if( NULL==scr){
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Screen>");
        return;
    }

    /* Initialize Screen Object */
    lv_obj_clear_flag( scr, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM );
    lv_obj_set_style_bg_color( scr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa( scr, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    /* Add Clock Panel */
    lv_obj_t *ui_ClockPanel = lv_img_create( scr);
    if( NULL!=ui_ClockPanel){
        lv_img_set_src( ui_ClockPanel, &ui_img_916048531);
        lv_obj_set_width( ui_ClockPanel, LV_SIZE_CONTENT);  /// 240
        lv_obj_set_height( ui_ClockPanel, LV_SIZE_CONTENT);   /// 240
        lv_obj_set_align( ui_ClockPanel, LV_ALIGN_CENTER );
        lv_obj_add_flag( ui_ClockPanel, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
        lv_obj_clear_flag( ui_ClockPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
        lv_obj_set_style_img_recolor(ui_ClockPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(ui_ClockPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Clock Panel>");
    }

    /* Add USC icon */
    
    lv_obj_t *ui_IconUsc = lv_img_create( scr);
    if( NULL!=ui_IconUsc){
        lv_img_set_src(ui_IconUsc, &ui_img_392295026);
        lv_obj_set_width( ui_IconUsc, LV_SIZE_CONTENT);  /// 33
        lv_obj_set_height( ui_IconUsc, LV_SIZE_CONTENT);   /// 48
        lv_obj_set_align( ui_IconUsc, LV_ALIGN_CENTER );
        lv_obj_add_flag( ui_IconUsc, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
        lv_obj_clear_flag( ui_IconUsc, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<USC Icon>");
    }
    

    /* Add Date Display Widget */
    lv_obj_t* ui_WidgetDate = lv_obj_create( scr);
    if( NULL!=ui_WidgetDate ){
        lv_obj_set_width( ui_WidgetDate, 48);
        lv_obj_set_height( ui_WidgetDate, 48);
        lv_obj_set_x( ui_WidgetDate, -60 );
        lv_obj_set_y( ui_WidgetDate, 0 );
        lv_obj_set_align( ui_WidgetDate, LV_ALIGN_CENTER );
        lv_obj_clear_flag( ui_WidgetDate, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
        lv_obj_set_style_radius(ui_WidgetDate, 24, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_WidgetDate, COLOR_DATE_MAIN, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(ui_WidgetDate, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(ui_WidgetDate, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Date Widget - Background>");
    }    

    list[kAppClockActiveWidget__week] = lv_label_create(ui_WidgetDate);
    if( NULL!=list[kAppClockActiveWidget__week] ){
        lv_obj_set_width( list[kAppClockActiveWidget__week], LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height( list[kAppClockActiveWidget__week], LV_SIZE_CONTENT);   /// 1
        lv_obj_set_x( list[kAppClockActiveWidget__week], 0 );
        lv_obj_set_y( list[kAppClockActiveWidget__week], -8 );
        lv_obj_set_align( list[kAppClockActiveWidget__week], LV_ALIGN_CENTER );
        lv_obj_set_style_text_color(list[kAppClockActiveWidget__week], COLOR_DATE_FONT, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_text_opa(list[kAppClockActiveWidget__week], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(list[kAppClockActiveWidget__week], &lv_font_montserrat_18, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(list[kAppClockActiveWidget__week], lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_shadow_opa(list[kAppClockActiveWidget__week], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Date Widget - Week>");
    }
    
    list[kAppClockActiveWidget__date] = lv_label_create(ui_WidgetDate);
    if( NULL!=list[kAppClockActiveWidget__date]){
        lv_obj_set_width( list[kAppClockActiveWidget__date], LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height( list[kAppClockActiveWidget__date], LV_SIZE_CONTENT);   /// 1
        lv_obj_set_x( list[kAppClockActiveWidget__date], 0 );
        lv_obj_set_y( list[kAppClockActiveWidget__date], 8 );
        lv_obj_set_align( list[kAppClockActiveWidget__date], LV_ALIGN_CENTER );
        lv_obj_set_style_text_color(list[kAppClockActiveWidget__date], lv_color_hex(0xB9B9B9), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_text_opa(list[kAppClockActiveWidget__date], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(list[kAppClockActiveWidget__date], &lv_font_montserrat_10, LV_PART_MAIN| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Date Widget - Date>");
    }
    
    list[kAppClockActiveWidget__battery] = lv_arc_create( scr);
    if( NULL!=list[kAppClockActiveWidget__battery]){
        lv_obj_set_width( list[kAppClockActiveWidget__battery], 48);
        lv_obj_set_height( list[kAppClockActiveWidget__battery], 48);
        lv_obj_set_x( list[kAppClockActiveWidget__battery], 60 );
        lv_obj_set_y( list[kAppClockActiveWidget__battery], 0 );
        lv_obj_set_align( list[kAppClockActiveWidget__battery], LV_ALIGN_CENTER );
        lv_arc_set_range(list[kAppClockActiveWidget__battery], 0,255);
        lv_arc_set_bg_angles(list[kAppClockActiveWidget__battery],270,629);
    
        lv_obj_set_style_arc_color(list[kAppClockActiveWidget__battery], COLOR_BATTERY_LOOP_MAIN, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_arc_opa(list[kAppClockActiveWidget__battery], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_arc_width(list[kAppClockActiveWidget__battery], WIDTH_BATTERY_LOOP, LV_PART_MAIN| LV_STATE_DEFAULT);

        lv_obj_set_style_arc_color(list[kAppClockActiveWidget__battery], COLOR_BATTERY_LOOP_INDICATOR, LV_PART_INDICATOR | LV_STATE_DEFAULT );
        lv_obj_set_style_arc_opa(list[kAppClockActiveWidget__battery], 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
        lv_obj_set_style_arc_width(list[kAppClockActiveWidget__battery], WIDTH_BATTERY_LOOP, LV_PART_INDICATOR| LV_STATE_DEFAULT);
        lv_obj_set_style_arc_rounded(list[kAppClockActiveWidget__battery], true, LV_PART_INDICATOR| LV_STATE_DEFAULT);

        lv_obj_set_style_bg_color(list[kAppClockActiveWidget__battery], lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(list[kAppClockActiveWidget__battery], 0, LV_PART_KNOB| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Battery Widget - Background>");
    }

    lv_obj_t* ui_BatteryBar = lv_bar_create( list[kAppClockActiveWidget__battery]);
    if( NULL!=ui_BatteryBar){
        lv_bar_set_range(ui_BatteryBar, 0,255);
        lv_bar_set_value(ui_BatteryBar,128,LV_ANIM_OFF);
        lv_obj_set_width( ui_BatteryBar, 18);
        lv_obj_set_height( ui_BatteryBar, 10);
        lv_obj_set_align( ui_BatteryBar, LV_ALIGN_CENTER );
        lv_obj_set_style_radius(ui_BatteryBar, 2, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BatteryBar, COLOR_BATTERY_ICON_INDICATOR, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(ui_BatteryBar, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_outline_color(ui_BatteryBar, COLOR_BATTERY_ICON_INDICATOR, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_outline_opa(ui_BatteryBar, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_outline_width(ui_BatteryBar, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_outline_pad(ui_BatteryBar, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

        lv_obj_set_style_radius(ui_BatteryBar, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BatteryBar, COLOR_BATTERY_ICON_INDICATOR, LV_PART_INDICATOR | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(ui_BatteryBar, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Battery Widget - Bar>");
    }

    lv_obj_t* ui_BatteryKnob = lv_obj_create( list[kAppClockActiveWidget__battery]);
    if( NULL!=ui_BatteryKnob){
        lv_obj_set_width( ui_BatteryKnob, 2);
        lv_obj_set_height( ui_BatteryKnob, 4);
        lv_obj_set_x( ui_BatteryKnob, 9 );
        lv_obj_set_y( ui_BatteryKnob, 0 );
        lv_obj_set_align( ui_BatteryKnob, LV_ALIGN_CENTER );
        lv_obj_clear_flag( ui_BatteryKnob, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
        lv_obj_set_style_radius(ui_BatteryKnob, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BatteryKnob, COLOR_BATTERY_ICON_INDICATOR, LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(ui_BatteryKnob, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

        lv_obj_set_style_radius(ui_BatteryKnob, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_BatteryKnob, COLOR_BATTERY_ICON_INDICATOR, LV_PART_SCROLLBAR | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(ui_BatteryKnob, 255, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(ui_BatteryKnob, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Battery Widget - BarKnob>");
    }

    list[kAppClockActiveWidget__sunmoon] = lv_img_create(scr);
    if( NULL!=list[kAppClockActiveWidget__sunmoon]){
        lv_obj_set_width( list[kAppClockActiveWidget__sunmoon], LV_SIZE_CONTENT);  /// 32
        lv_obj_set_height( list[kAppClockActiveWidget__sunmoon], LV_SIZE_CONTENT);   /// 32
        lv_obj_set_x( list[kAppClockActiveWidget__sunmoon], 0 );
        lv_obj_set_y( list[kAppClockActiveWidget__sunmoon], -60 );
        lv_obj_set_align( list[kAppClockActiveWidget__sunmoon], LV_ALIGN_CENTER );
        lv_obj_add_flag( list[kAppClockActiveWidget__sunmoon], LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
        lv_obj_clear_flag( list[kAppClockActiveWidget__sunmoon], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Day Widget - Sun/Moon>");
    }
    

    list[kAppClockActiveWidget__needle_min] = lv_img_create(scr);
    if( NULL!=list[kAppClockActiveWidget__needle_min]){
        lv_img_set_src(list[kAppClockActiveWidget__needle_min], &ui_img_1793134324);
        lv_obj_set_width( list[kAppClockActiveWidget__needle_min], LV_SIZE_CONTENT);  /// 73
        lv_obj_set_height( list[kAppClockActiveWidget__needle_min], LV_SIZE_CONTENT);   /// 8
        lv_obj_set_x( list[kAppClockActiveWidget__needle_min], 136 );
        lv_obj_set_y( list[kAppClockActiveWidget__needle_min], 116 );
        lv_obj_add_flag( list[kAppClockActiveWidget__needle_min], LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
        lv_obj_clear_flag( list[kAppClockActiveWidget__needle_min], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
        lv_img_set_pivot(list[kAppClockActiveWidget__needle_min],-16,4);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Needle - Minute>");
    }
    
    
    list[kAppClockActiveWidget__needle_hr] = lv_img_create(scr);
    if( NULL!=list[kAppClockActiveWidget__needle_hr]){
        lv_img_set_src(list[kAppClockActiveWidget__needle_hr], &ui_img_249914142);
        lv_obj_set_width( list[kAppClockActiveWidget__needle_hr], LV_SIZE_CONTENT);  /// 51
        lv_obj_set_height( list[kAppClockActiveWidget__needle_hr], LV_SIZE_CONTENT);   /// 8
        lv_obj_set_x( list[kAppClockActiveWidget__needle_hr], 136 );
        lv_obj_set_y( list[kAppClockActiveWidget__needle_hr], 116 );
        lv_obj_add_flag( list[kAppClockActiveWidget__needle_hr], LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
        lv_obj_clear_flag( list[kAppClockActiveWidget__needle_hr], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
        lv_img_set_pivot(list[kAppClockActiveWidget__needle_hr],-16,4);
    }else{
        watch.sys.logger->printf( "App Clock: NULL pointer created for %s\n", "<Needle - Hour>");
    }
}

static inline void util__theme_run__usc( lv_obj_t *scr, lv_obj_t* list[kNumOfAppClockActiveWidget]){
    extern lv_img_dsc_t   ui_img_1172288946;    /* Sun Icon Image */
    extern lv_img_dsc_t   ui_img_1997035523;    /* Moon Icon Image */

    watch.sys.gui->request();
    
    switch( watch.hw.rtc->date.Month){
        case 1: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Jan"); break;
        case 2: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Feb");break;
        case 3: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Mar");break;
        case 4: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Apr");break;
        case 5: lv_label_set_text_static( list[kAppClockActiveWidget__week], "May");break;
        case 6: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Jun");break;
        case 7: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Jul");break;
        case 8: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Aug");break;
        case 9: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Sep");break;
        case 10: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Oct");break;
        case 11: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Nov");break;
        case 12: lv_label_set_text_static( list[kAppClockActiveWidget__week], "Dec");break;
        default:
            watch.sys.logger->printf( "App Clock: Received a invalid month value which is %d\n", watch.hw.rtc->date.Month);
            break;
    }
    lv_label_set_text_fmt( list[kAppClockActiveWidget__date], "%02d/%02d", watch.hw.rtc->date.Month, watch.hw.rtc->date.Date);
    
    lv_arc_set_value( list[kAppClockActiveWidget__battery], 128);    // Range: 0~255

    if( watch.hw.rtc->time.Hours>=6 && watch.hw.rtc->time.Hours<=18 ){
        lv_img_set_src(list[kAppClockActiveWidget__sunmoon], &ui_img_1172288946); // Sun picture
    }else{
        lv_img_set_src(list[kAppClockActiveWidget__sunmoon], &ui_img_1997035523); // Moon picture
    }
    
    const i16 offset = 3600/4;
    lv_img_set_angle(list[kAppClockActiveWidget__needle_hr], \
        rh_cmn_math__abs_angle_hour( watch.hw.rtc->time.Hours, watch.hw.rtc->time.Minutes, watch.hw.rtc->time.Seconds ) - offset );

    lv_img_set_angle(list[kAppClockActiveWidget__needle_min], \
        rh_cmn_math__abs_angle_minute( watch.hw.rtc->time.Minutes, watch.hw.rtc->time.Seconds ) - offset );
    
    watch.sys.gui->yeild();
}

typedef void(*ThemeSetupFunction_t)( lv_obj_t *scr, lv_obj_t *list[kNumOfAppClockActiveWidget]);
typedef void(*ThemeRunFunction_t)( lv_obj_t *scr, lv_obj_t *list[kNumOfAppClockActiveWidget]);

const struct AppThemeList{
    ThemeSetupFunction_t  setup;
    ThemeRunFunction_t    run;
}clock_theme_list[kNumOfAppClockTheme] = {
    [kAppClockTheme__USC] = {
        .setup = util__theme_setup__usc,
        .run   = util__theme_run__usc
    }
};




void task_func__model( void* param){

    rh_cmn_rtc__update();

    while(1){
        ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(1000));
        rh_cmn_rtc__update();
        /* Update data */
        xTaskNotifyGive( self->isDisplayable->visual.handle);
    }
}


void task_func__visual( void* param){

    /* Static Zone*/
    watch.sys.gui->request();

    if( self->isDisplayable->screen==NULL ){  
        self->cache.active_widget_list = (lv_obj_t**)pvPortMalloc( kNumOfAppClockActiveWidget*sizeof(lv_obj_t*));

        memset( self->cache.active_widget_list, 0,  kNumOfAppClockActiveWidget*sizeof(lv_obj_t*));

        self->isDisplayable->screen = lv_obj_create(NULL);
        clock_theme_list[self->cache.theme].setup( self->isDisplayable->screen, self->cache.active_widget_list);
    }else{
        rh_cmn__assert( self->cache.active_widget_list!=NULL, "Dangling pointer" );
    }

    lv_scr_load( self->isDisplayable->screen);
    
    watch.sys.gui->yeild();

    /* Dynamic Zone */
    while(1){
        
        // clock_theme_list[self->cache.theme].run( self->isDisplayable->screen, self->cache.active_widget_list);
        util__theme_run__usc( self->isDisplayable->screen, self->cache.active_widget_list);

        ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
    }

}

void task_func__exit( void* param){
    vTaskSuspend( self->isDisplayable->ctrl.handle );
    vTaskSuspend( self->isDisplayable->model.handle );
    vTaskSuspend( self->isDisplayable->visual.handle );

    lv_obj_del( self->isDisplayable->screen);
    self->isDisplayable->screen = NULL;

    vPortFree( self->cache.active_widget_list);
}


void task_func__ctrl( void* param){
    while(1){
        // if(...){
        //     xTaskNotifyGive( self->isDisplayable->model.handle);
        // }

        vTaskDelay(1000);
    }
}






AppDisplyable_t disp_struct = {
    .model  = { .main_func=task_func__model  , .handle=NULL, .exit_func=task_func__exit},
    .visual = { .main_func=task_func__visual , .handle=NULL},
    .ctrl   = { .main_func=task_func__ctrl   , .handle=NULL},
    .frame_rate_ticks = 10,
    .screen = NULL
};


AppClock_t g_AppClock = {
    .isDisplayable = &disp_struct,
    .cache = {0}
};