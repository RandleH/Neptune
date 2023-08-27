




#include "rh_cmn.h"



#ifdef __cplusplus
extern "C"{
#endif


/** @defgroup Common 
 *  Project Common Group
 *  @{
 */

/** @defgroup ISR
 *  @ingroup Common
 *  Project Interrupt Service Routine Group
 *  @{
 */



#if 0
/**
 * @brief   Interrupt: Reset Handler
 *          See implementation @ref "cfg/startup_stm32f411xe.S"
 * @return  (none)
*/
void Reset_Handler( void){} 
#endif

void NMI_Handler( void){
    while(1){

    }
}

void HardFault_Handler( void){
    while(1){

    }
}

void MemManage_Handler( void){
    while(1){

    }
}

void BusFault_Handler( void){
    while(1){

    }
}

void UsageFault_Handler( void){
    while(1){

    }
}

void SVC_Handler( void){
    while(1){

    }
}

void DebugMon_Handler( void){
    while(1){

    }
}

void PendSV_Handler( void){
    while(1){

    }
}


/**
 * @brief   Interrupt: Systick Handler
 *          Period: 1KHz
 * @return  (none)
*/
void SysTick_Handler( void){
    ++gCommonData.clockTick;
}



/* External Interrupts */
void WWDG_IRQHandler( void){}
void PVD_IRQHandler( void){}
void TAMP_STAMP_IRQHandler( void){}
void RTC_WKUP_IRQHandler( void){}

void FLASH_IRQHandler( void){}       
void RCC_IRQHandler( void){}         
void EXTI0_IRQHandler( void){}       
void EXTI1_IRQHandler( void){}       
void EXTI2_IRQHandler( void){}       
void EXTI3_IRQHandler( void){}       
void EXTI4_IRQHandler( void){}       
void DMA1_Stream0_IRQHandler( void){}
void DMA1_Stream1_IRQHandler( void){}
void DMA1_Stream2_IRQHandler( void){}
void DMA1_Stream3_IRQHandler( void){}
void DMA1_Stream4_IRQHandler( void){}
void DMA1_Stream5_IRQHandler( void){}
void DMA1_Stream6_IRQHandler( void){}
void ADC_IRQHandler( void){}        

void EXTI9_5_IRQHandler(void){}           
void TIM1_BRK_TIM9_IRQHandler(void){}     
void TIM1_UP_TIM10_IRQHandler(void){}     
void TIM1_TRG_COM_TIM11_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}           
void TIM2_IRQHandler(void){}              
void TIM3_IRQHandler(void){}              
void TIM4_IRQHandler(void){}              
void I2C1_EV_IRQHandler(void){}           
void I2C1_ER_IRQHandler(void){}           
void I2C2_EV_IRQHandler(void){}           
void I2C2_ER_IRQHandler(void){}           
void SPI1_IRQHandler(void){}              
void SPI2_IRQHandler(void){}              
void USART1_IRQHandler(void){}            
void USART2_IRQHandler(void){}           


void EXTI15_10_IRQHandler( void){}
void RTC_Alarm_IRQHandler( void){}
void OTG_FS_WKUP_IRQHandler( void){}

void DMA1_Stream7_IRQHandler( void){}

void SDIO_IRQHandler( void){}
void TIM5_IRQHandler( void){}
void SPI3_IRQHandler( void){}

void DMA2_Stream0_IRQHandler( void){}
void DMA2_Stream1_IRQHandler( void){}
void DMA2_Stream2_IRQHandler( void){}
void DMA2_Stream3_IRQHandler( void){}
void DMA2_Stream4_IRQHandler( void){}

void OTG_FS_IRQHandler( void){}
void DMA2_Stream5_IRQHandler( void){}
void DMA2_Stream6_IRQHandler( void){}
void DMA2_Stream7_IRQHandler( void){}
void USART6_IRQHandler( void){}
void I2C3_EV_IRQHandler( void){}
void I2C3_ER_IRQHandler( void){}

void FPU_IRQHandler( void){}
  
void SPI4_IRQHandler( void){}
void SPI5_IRQHandler( void){}



/** @} */ // end of ISR
/** @} */ // end of Common




#ifdef __cplusplus
}
#endif