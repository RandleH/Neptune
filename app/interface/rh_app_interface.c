


#include "rh_app_interface.h"
#include "rh_cmn.h"
#include "FreeRTOS.h"


// ******************************************************************************************************
// *                                                                                                    *
// * pvPortRealloc                                                                 						*
// *                                                                                                    *
// * Funzione per riallocare dimensione memoria in heap region											*
// *                                                                                                    *
// * Parameter : (void *) Pointer to a memory block previously allocated with malloc, calloc or realloc *
// *			 (size_t) Number of byte to resize section                                              *
// *                                                                                                    *
// * Return    : (void *) Pointer to allocate region, NULL if error on allocation                       *
// *                                                                                                    *
// ******************************************************************************************************




uint32_t HAL_GetTick(void){
	return (uint32_t)xTaskGetTickCount();
}

void RH_WEAK lv_mem_init(void){
    /* Dummy function to pass compile */
}

void* RH_WEAK lv_malloc_core( size_t size){
    return pvPortMalloc( size);
}


void RH_WEAK lv_free_core( void * ptr){
    return vPortFree( ptr);
}


void* RH_WEAK lv_realloc_core( void* p, size_t new_size){
    return pvPortRealloc( p, new_size);
}

#if (LVGL_VERSION_MAJOR==9) && (LVGL_VERSION_MINOR==0) && (LVGL_VERSION_PATCH==0)
lv_result_t RH_WEAK lv_mem_test_core( void){
    return 0;
}
#endif

void lv_mem_monitor_core(lv_mem_monitor_t * mon_p){
	(void)mon_p;
}


void *pvPortRealloc( void *pv, size_t xWantedSize ){
    static size_t xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * 8 ) - 1 );

    typedef struct A_BLOCK_LINK{
        struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
        size_t xBlockSize;						/*<< The size of the free block. */
    } BlockLink_t;
    
    static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

	// Local variable	
	size_t move_size;
	size_t block_size;
	BlockLink_t *pxLink;
	void *pvReturn = NULL;
	uint8_t *puc = ( uint8_t * ) pv;

	// Se NULL, exit
	if (xWantedSize > 0)
	{
		// Controllo se buffer valido
		if (pv != NULL)
		{
			// The memory being freed will have an BlockLink_t structure immediately before it.
			puc -= xHeapStructSize;

			// This casting is to keep the compiler from issuing warnings.
			pxLink = ( void * ) puc;

			// Check allocate block
			if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0)
			{
				// The block is being returned to the heap - it is no longer allocated.
				block_size = (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;

				// Alloco nuovo spazio di memoria
				pvReturn = pvPortCalloc(1, xWantedSize);

				// Check creation
				if (pvReturn != NULL)
				{
					// Sposta soltanto fino al limite
					if (block_size < xWantedSize)
					{
						// Il nuovo posto disponibile è inferiore
						move_size = block_size;
					}
					else
					{
						// Il nuovo posto disponibile è maggiore
						move_size = xWantedSize;
					}

					// Copio dati nel nuovo spazio di memoria
					memcpy(pvReturn, pv, move_size);

					// Libero vecchio blocco di memoria
					vPortFree(pv);
				}
			}
			else
			{
				// Puntatore nullo, alloca memoria come fosse nuova
				pvReturn = pvPortCalloc(1, xWantedSize);
			}
		}
		else
		{
			// Puntatore nullo, alloca memoria come fosse nuova
			pvReturn = pvPortCalloc(1, xWantedSize);
		}
	}
	else
	{
		// Exit without memory block
		pvReturn = NULL;
	}

	// Exit with memory block
	return pvReturn;
}




/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize ){
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize ){
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}



void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName ){

}


void vApplicationMallocFailedHook( void ){
    rh_cmn__assert( false, "FreeRTOS malloc failed!");
}






