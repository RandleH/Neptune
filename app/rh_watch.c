



#include "rh_watch.h"
#include "rh_cmn.h"


/**
 * @brief   LED Blink Task
 * @param    param cast to `AppLed_t *` aka. `struct AppLed *`
*/
// void task_blink( void* param){
//     static u8 cnt = 10;
//     bool flag = true;
//     while(1){
//         vTaskDelay(400);
        
//         HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
//         g_AppTrace.printf( "LED: Turn on\r\n");
        
//         vTaskDelay(400);
        
//         HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
//         g_AppTrace.printf( "LED: Turn off\r\n");
    
//         if( cnt==0){
//             if(flag==true){
//                 rh_cmn_clk__mco_disable();
//                 flag = false;
//             }
//         }else{
//             --cnt;
//         }
//     }
// }







static void entrance_function( void* param){

    (void)param;

    /* Peripheral Initialization */
    rh_cmn_gpio__init();    
    rh_cmn_usart__init( kCmnConst__USART_BAUDRATE);
    rh_cmn_spi__init( kCmnSpiFreq_48MHz);
    rh_cmn_clk__mco_disable();

    /* BSP Initialization */
    rh_bsp_screen__init();

    /* Application Sanity Test */
    /**
     * @note    If failed, infinite loop
     * @note    Run when `RH_APP_CFG__ENABLE_CI` enabled
     * @note    All application will be reset after the test
    */
#if defined (RH_APP_CFG__ENABLE_CI) && (((RH_APP_CFG__ENABLE_CI))==(true))
    // extern int rh_app_trace__ci_sanity( void);
    
    // if(0!=rh_app_trace__ci_sanity()){
    //     while(1);
    // }
#endif

    /* System Application Initialization */
    watch.app.taskmgr->launch();
    watch.app.logger->launch();
    watch.app.logger->cache_mode(false);

    /* Print General Information */
    CmnChipUID_t uid;
    rh_cmn_chip__uid( &uid);
    watch.app.logger->printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);

    watch.app.logger->printf( "CPU Info: System Core Frequency: %ld Hz\r\n", HAL_RCC_GetSysClockFreq());
    watch.app.logger->printf( "CPU Info: AHB Clock Frequency: %ld Hz\r\n", HAL_RCC_GetHCLKFreq());
    watch.app.logger->printf( "CPU Info: APB1 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK1Freq());
    watch.app.logger->printf( "CPU Info: APB2 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK2Freq());

    /* User Application - Use Task Manager to launch user application */
    watch.app.taskmgr->create( watch.app.gui->launch_list, watch.app.gui->launch_list_len);

    vTaskDelete(NULL);
}






WatchTopStructure_t watch = {
    .entrance = entrance_function,
    .app = {
        .logger   = &g_AppTrace,
        .taskmgr  = &g_AppTaskMgr,
        .gui      = &g_AppGui
    },
};