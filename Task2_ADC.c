/*
 * Task2.c
 *
 *  Created on: Oct 26, 2020
 *      Author: You Hu
 */
#include "init.h"
#include <stdint.h>
#include <stdlib.h>

ADC_HandleTypeDef hADC3;
DAC_HandleTypeDef hDAC1;
uint16_t putout =0;

uint16_t readvoltage();

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureDAC();
	HAL_DAC_Start(&hDAC1, DAC_CHANNEL_1);
	configureADC();

	while(1){
		putout=readvoltage()*4096/4095;
		HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, putout);
	}
}


void configureDAC(){
	__HAL_RCC_DAC_CLK_ENABLE();

	hDAC1.Instance = DAC;
	HAL_DAC_Init(&hDAC1);

	DAC_ChannelConfTypeDef dacchan;
	dacchan.DAC_Trigger = DAC_TRIGGER_NONE;
	dacchan.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&hDAC1, &dacchan, DAC_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
    if(hdac->Instance == DAC){
    	// Enable GPIO Clocks
       	__HAL_RCC_GPIOA_CLK_ENABLE();

    	// Initialize Pin
    	GPIO_InitStruct.Pin       = GPIO_PIN_4;
        GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    	GPIO_InitStruct.Pull      = GPIO_NOPULL;
    	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
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

