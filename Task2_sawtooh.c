/*
 * Task2_swooth.c
 *
 *  Created on: Oct 26, 2020
 *      Author: You Hu
 */
#include "init.h"
#include <stdint.h>
#include <stdlib.h>

DAC_HandleTypeDef hDAC1;


void configureDAC();
uint16_t readvoltage();

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureDAC();
	HAL_DAC_Start(&hDAC1, DAC_CHANNEL_1);
	int putout=0;

	while(1){
		if(putout>4096){
			putout=0;
		}
		HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, putout);
		putout++;
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

