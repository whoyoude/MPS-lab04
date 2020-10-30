//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"
#include <stdint.h>
#include <stdlib.h>


ADC_HandleTypeDef hADC3;
DAC_HandleTypeDef hDAC1;
uint32_t voltage=0;
//uint32_t x=0;   //uint32_t with name of x, x_1, x_2 and putout are for C language implement
//uint32_t x_1 =0;
//uint32_t  x_2=0;
//uint32_t putout=0;
float x=0;//float with name of x, x_1, x_2, putout and putout_1 are for assembly language implement
float x_1 =0;
float x_2=0;
float putout=0;
float putout_1=0;
float a = 0.3125;// Coefficient for assembly language implement
float b = 0.240385;
float c = 0.296875;

void configureDAC();
void configureADC();
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
//		//Use c to implemet
//		x_2 = x_1;
//		x_1 = x;
//		x = readvoltage();
//		putout = 0.312500*x + 0.240385*x_1 + 0.312500*x_2 + 0.296875*putout;
//		HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, putout);

		//Use assembly language
		asm ("VMOV.F32 %0,%1":"+t"(x_2):"t"(x_1));//Shifting the input value by 1 time unit
		asm ("VMOV.F32 %0,%1":"+t"(x_1):"t"(x));
		asm ("VMOV.F32 %0,%1":"+t"(putout_1):"t"(putout));
		x = readvoltage();

        asm volatile("VMUL.F32  %0,%1,%2":"+t"(putout):"t"(x),"t"(a));
        asm volatile("VMLA.F32  %0,%1,%2":"+t"(putout):"t"(x_1),"t"(b));
        asm volatile("VMLA.F32  %0,%1,%2":"+t"(putout):"t"(x_2),"t"(a));
        asm volatile("VMLA.F32  %0,%1,%2":"+t"(putout):"t"(putout_1),"t"(c));

	    HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, putout);

	}
}



void configureDAC(){
	__HAL_RCC_DAC_CLK_ENABLE();
	
	//Configure DAC_HandleTypeDef
	hDAC1.Instance = DAC;
	HAL_DAC_Init(&hDAC1);
	
	//Configure DAC_ChannelConfTypeDef
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
	HAL_ADC_PollForConversion(&hADC3,100);//wait for ADC conversion complete
	return HAL_ADC_GetValue(&hADC3);
}



void configureADC()
{  //A3 =>PF10 (ADC3_IN8)
	__HAL_RCC_ADC3_CLK_ENABLE();
        
	//Configure ADC_HandleTypeDef
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

	//Configure ADC_ChannelConfTypeDef
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
