



#include "rh_watch.h"
#include "rh_cmn.h"


/**
 * @brief   LED Blink Task
 * @param    param cast to `AppLed_t *` aka. `struct AppLed *`
*/
void task_blink( void* param){
    static u8 cnt = 10;
    bool flag = true;
    while(1){
        vTaskDelay(400);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        g_AppTrace.printf( "LED: Turn on\r\n");
        
        vTaskDelay(400);
        
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_13);
        g_AppTrace.printf( "LED: Turn off\r\n");
    
        if( cnt==0){
            if(flag==true){
                rh_cmn_clk__mco_disable();
                flag = false;
            }
        }else{
            --cnt;
        }
    }
}

/**
 * @brief   System Report
 * @param   param       Call back `printf()`. Must NOT be NULL.
*/
void task_report( void* param){
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 5000;
    xLastWakeTime = xTaskGetTickCount();

    HeapStats_t report_heap;
    vPortGetHeapStats( &report_heap);
    while(1){
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        watch.app.logger->printf( "============================================================\n");
        watch.app.logger->printf( "System Report\n");
        watch.app.logger->printf( "============================================================\n");
        watch.app.logger->printf( "Heap Memory Usage ------------------------------------------\n");
        watch.app.logger->printf( " - Number of Free Memory Blocks:              %ld\tbytes\n", report_heap.xNumberOfFreeBlocks);
        watch.app.logger->printf( " - Minimum Remaining Free Bytes Since Boot:   %ld\tbytes\n", report_heap.xMinimumEverFreeBytesRemaining);
        watch.app.logger->printf( " - Maximum Allocable Bytes:                   %ld\tbytes\n", report_heap.xSizeOfLargestFreeBlockInBytes);
        watch.app.logger->printf( " - Total Remaining Free Bytes:                %ld\tbytes\n", report_heap.xAvailableHeapSpaceInBytes);
        watch.app.logger->printf( " - Total Heap Size:                           %ld\tbytes\n", configTOTAL_HEAP_SIZE);
        watch.app.logger->printf( " - Number of calls to pvPortMalloc()          %ld\t\n", report_heap.xNumberOfSuccessfulAllocations);
        watch.app.logger->printf( " - Number of calls to pvPortFree()            %ld\t\n", report_heap.xNumberOfSuccessfulFrees);
        watch.app.logger->printf( "\n\n");

        watch.app.logger->printf( "Task Statistic ---------------------------------------------\n");
        watch.app.logger->printf( " - Total Number of Tasks                      %ld\t\n", uxTaskGetNumberOfTasks());
        
        watch.app.logger->printf( " -- Task Name:                                %s\n", pcTaskGetName( NULL));
        watch.app.logger->printf( " -- Task Stack Peak Usage:                    %ld/%ld\n", sizeof(StackType_t)*(128-uxTaskGetStackHighWaterMark2( NULL)), 128*sizeof(StackType_t));
        watch.app.logger->printf( "\n\n""\033[0m");

        // ulTaskGetIdleRunTimePercent();
        // ulTaskGetRunTimePercent(  )
        
    }   
}





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

    /* Print General Information */
    CmnChipUID_t uid;
    rh_cmn_chip__uid( &uid);
    watch.app.logger->printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);

    watch.app.logger->printf( "CPU Info: System Core Frequency: %ld Hz\r\n", HAL_RCC_GetSysClockFreq());
    watch.app.logger->printf( "CPU Info: AHB Clock Frequency: %ld Hz\r\n", HAL_RCC_GetHCLKFreq());
    watch.app.logger->printf( "CPU Info: APB1 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK1Freq());
    watch.app.logger->printf( "CPU Info: APB2 Clock Frequency: %ld Hz\r\n", HAL_RCC_GetPCLK2Freq());

    /* User Application */
    AppTaskUnit_t list[] = {
        {
            .pcName       = "LED",
            .pvParameters = NULL,
            .pvTaskCode   = task_blink,
            .usStackDepth = 512,
            .uxPriority   = 30
        },
        {
            .pcName       = "Report",
            .pvParameters = NULL,
            .pvTaskCode   = task_report,
            .usStackDepth = 256,
            .uxPriority   = 30
        }
    };

    watch.app.taskmgr->create( list, sizeof(list)/sizeof(*list));
    
    /* Use Task Manager to launch user application */
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