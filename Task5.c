#include "init.h"
#include <stdint.h>
#include <stdlib.h>


ADC_HandleTypeDef hADC3;
ADC_HandleTypeDef hADC1;
DAC_HandleTypeDef hDAC1;
TIM_HandleTypeDef htim6;
uint16_t v_adc3 = 0;
uint16_t v_adc1 = 0;
float x=0;
float x_1 =0;
float  x_2=0;
float x_3 = 0;
float x_4 = 0;
float y=0;
float y_1=0;
float y_2=0;
float y_3=0;
float y_4=0;
uint16_t flag=0;
uint16_t adc_state = 0;//0:no conversion  1:adc1 start conversion  2:adc1 end conversion adc2 start conversion  3:adc2 end conversion

void configureDAC();
void configureADC();
void Init_timer_HAL();

float rtof(uint16_t raw){
	return 3.3*raw/4095;
}

uint16_t ftor(float f){
	uint16_t x =f/3.3*4096;
	return x;

}

void update(){
	if(adc_state==3){
		adc_state=0;

				//execute
		x_4 = x_3;
		x_3 = x_2;
		x_2 = x_1;
		x_1 = x;
		x = (rtof(v_adc1)-1)*(rtof(v_adc3)-1);

		y_4 = y_3;
		y_3 = y_2;
		y_2 = y_1;
		y_1 = y;
		y = 0.001*x-0.002*x_2+0.001*x_4 +3.166*y_1-4.418*y_2+3.028*y_3-0.915*y_4;

		//y = 0.312500*x + 0.240385*x_1 + 0.312500*x_2 + 0.296875*y_1;
		uint16_t c=ftor(y+1);
		HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, c);
		//printf("x=%f, y=%f\n\r",x,y);
	}


}

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	configureDAC();
	HAL_DAC_Start(&hDAC1, DAC_CHANNEL_1);
	configureADC();
	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);

	Init_timer_HAL();


	while(1){

	}
}

void Init_timer_HAL(){
	HAL_Init();
	HAL_TIM_Base_MspInit(&htim6);

	//Set the TIM to be TIM6 and the corresponding prescaler and period
	htim6.Instance = TIM6;
	htim6.Init.Prescaler =9;
	htim6.Init.Period = 99;

	//Enable clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	//Set priority and enable IRQ
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//Initiate the timer
	HAL_TIM_Base_Init(&htim6);
	HAL_TIM_Base_Start_IT(&htim6);
}

void TIM6_DAC_IRQHandler(){
	TIM6->SR &= 0xFFFFFFFE;

	if(adc_state==0){
		adc_state=1;
		HAL_ADC_Start_IT(&hADC1);

	}else{
		asm("nop");
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

void ADC_IRQHandler(){
	if (adc_state==1){
	HAL_ADC_IRQHandler(&hADC1);
	}
	if(adc_state==2){
	HAL_ADC_IRQHandler(&hADC3);
	}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance == ADC1){
		//flag = 1;
		v_adc1 = HAL_ADC_GetValue(&hADC1);
		adc_state=2;
		HAL_ADC_Start_IT(&hADC3);
	}
	if(hadc->Instance == ADC3){
		//flag = 0;
		v_adc3 = HAL_ADC_GetValue(&hADC3);
		adc_state=3;
		update();
	}
}

void configureADC()
{  //A3 =>PF10 (ADC3_IN8)
	__HAL_RCC_ADC3_CLK_ENABLE();

	hADC3.Instance = ADC3;
	hADC3.Init.Resolution = ADC_RESOLUTION_12B;
	hADC3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	hADC3.Init.ScanConvMode = DISABLE;
	hADC3.Init.NbrOfConversion = 1;
	hADC3.Init.ContinuousConvMode = DISABLE;
	hADC3.Init.DiscontinuousConvMode = DISABLE;
	hADC3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hADC3.Init.DMAContinuousRequests = DISABLE;
	hADC3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hADC3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&hADC3);

	//ADC->CCR |= ADC_CCR_TSVREFE;
	ADC_ChannelConfTypeDef adcchan3;
	adcchan3.Channel = ADC_CHANNEL_8;
	adcchan3.Rank = ADC_REGULAR_RANK_1;
	adcchan3.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hADC3,&adcchan3);

	__HAL_RCC_ADC1_CLK_ENABLE();

	hADC1.Instance = ADC1;
	hADC1.Init.Resolution = ADC_RESOLUTION_12B;
	hADC1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	hADC1.Init.ScanConvMode = DISABLE;
	hADC1.Init.NbrOfConversion = 1;
	hADC1.Init.ContinuousConvMode = DISABLE;
	hADC1.Init.DiscontinuousConvMode = DISABLE;
	hADC1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hADC1.Init.DMAContinuousRequests = DISABLE;
	hADC1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hADC1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&hADC1);

	//ADC->CCR |= ADC_CCR_TSVREFE;
	ADC_ChannelConfTypeDef adcchan1;
	adcchan1.Channel = ADC_CHANNEL_12;
	adcchan1.Rank = ADC_REGULAR_RANK_1;
	adcchan1.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hADC1,&adcchan1);


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
    if(hadc->Instance == ADC1){
        	// Enable GPIO Clocks
           	__HAL_RCC_GPIOC_CLK_ENABLE();

        	// Initialize Pin
        	GPIO_InitStruct.Pin       = GPIO_PIN_2;
            GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
        	GPIO_InitStruct.Pull      = GPIO_PULLUP;
        	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
        	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    }

	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}
