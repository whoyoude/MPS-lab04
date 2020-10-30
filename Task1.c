/*
 * Task1.c
 *
 *  Created on: Oct 26, 2020
 *      Author: You Hu
 */

#include "init.h"
#include <stdint.h>
#include <stdlib.h>


ADC_HandleTypeDef hADC3;


float high =0;
float low =3.3;
float avg =0;
int iter=0;

void configureADC();
uint16_t readvoltage();
void Init_GPIO_interrup_HAL();

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureADC();
	Init_GPIO_interrup_HAL();

	while(1);
}



uint16_t readvoltage(){
	HAL_ADC_Start(&hADC3);
	HAL_ADC_PollForConversion(&hADC3,100);
	//if(res == HAL_OK){
		//printf("Hi\r\n");
		return HAL_ADC_GetValue(&hADC3);
}



void configureADC()
{  //A3 =>PF10 (ADC3_IN8)
	__HAL_RCC_ADC3_CLK_ENABLE();

	hADC3.Instance = ADC3;
	hADC3.Init.Resolution = ADC_RESOLUTION_12B;
	hADC3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hADC3.Init.ScanConvMode = DISABLE;
	hADC3.Init.NbrOfConversion = 1;
	hADC3.Init.ContinuousConvMode = ENABLE;
	hADC3.Init.DiscontinuousConvMode = DISABLE;
	hADC3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hADC3.Init.DMAContinuousRequests = DISABLE;
	hADC3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hADC3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&hADC3);

	//ADC->CCR |= ADC_CCR_TSVREFE;
	ADC_ChannelConfTypeDef adcchan;
	adcchan.Channel = ADC_CHANNEL_8;
	adcchan.Rank = ADC_REGULAR_RANK_1;
	adcchan.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	HAL_ADC_ConfigChannel(&hADC3,&adcchan);

}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
    if(hadc->Instance == ADC3){
    	// Enable GPIO Clocks
       	__HAL_RCC_GPIOF_CLK_ENABLE();

    	// Initialize Pin
    	GPIO_InitStruct.Pin       = GPIO_PIN_10;
        GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    	GPIO_InitStruct.Pull      = GPIO_PULLUP;
    	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    }
}


//Pa0
void Init_GPIO_interrup_HAL(){
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_Init();

	//GPIO port J clock enable
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	//Set pins to enable interrupt
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Set priority and enable the IRQ
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_SET){//Ensure the interrupt condition is met
		uint16_t raw = readvoltage();
		float voltage=3.3*raw/4096;
		float total=avg*iter+voltage;
		iter+=1;
		avg=total/iter;
		high=high<voltage?voltage:high;
		low=low>voltage?voltage:low;
		printf("\033[0;0H\033[2J");
		printf("raw: %x\r\ndecimal: %10.6f\r\nhigh: %10.6f\r\nlow: %10.6f\r\navg:%10.6f\r\n",raw,voltage,high,low,avg);
	}
}


