/*
 * Task3.c
 *
 *  Created on: Oct 26, 2020
 *      Author: You Hu
 */


#include "init.h"
#include <stdint.h>
#include <stdlib.h>

void load_add();
void multiply();
// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();

    //Task 3. set1
		printf("\033[0;0H\033[2JSet1\n\r");
		load_add_1();
		multiply_1();
		t3_3_1();
		t3_4_1();
		HAL_Delay(1000);


		printf("\r\nSet2\n\r");
		load_add_2();
		multiply_2();
		t3_3_2();
		t3_4_2();
		HAL_Delay(10);

}


void load_add_1(){
	asm("LDR r0, =1");
	asm("LDR r1, =2");
	asm("ADD r2, r0, r1");
	uint32_t var;
	asm("STR r2,%0" : "=m" (var));
	printf("load add: %u\r\n",var);
}

void multiply_1(){
	uint32_t var1=10;
	uint32_t var2=15;
	uint32_t result=15;
	asm("MUL %0,%1,%2" :"=r" (result): "r" (var1), "r" (var2));
	printf("multiply: %u\r\n",result);

}

void t3_3_1(){
	uint32_t x=9;
	uint32_t result;
	asm("LDR r2, =2");
	asm("MUL r0,%0,r2" :: "r" (x));
	asm("LDR r3, =3");
	asm("UDIV r1, r0, r3");
	asm("ADD %0,r1, #5":"=r" (result));
	printf("t3_3: %u\r\n",result);
}

void t3_4_1(){
	uint32_t x=9;
	uint32_t result;
	asm("LDR r2, =2");
	asm("LDR r1, =15");
	asm("MLA r0,%0,r2,r1" :: "r" (x));
	asm("LDR r3, =3");
	asm("UDIV %0, r0, r3": "=r" (result));
	printf("t3_4: %u\r\n",result);
}

void load_add_2(){
	asm("LDR r0, =1");
	asm("LDR r1, =2");
	asm("ADD r2, r0, r1");
	uint32_t var;
	asm("STR r2,%0" : "=m" (var));
	printf("load add: %u\r\n",var);
}

void multiply_2(){
	float var1=10;
	float var2=15;
	float result=15;
	asm volatile("VMUL.F32 %0,%1,%2" :"=t" (result): "t" (var1), "t" (var2));
	printf("multiply: %f\r\n",result);

}

void t3_3_2(){
	float x=2.1;
	float result;
	asm("VMOV s2, #2.0");
	asm("VMUL.F32 s0,%0,s2" :: "t" (x));
	asm("VMOV s3, #3.0");
	asm("VDIV.F32 s1, s0, s3");
	asm("VMOV s5, #5.0");
	asm("VADD.F32 %0,s1, s5":"=t" (result));
	printf("t3_3: %f\r\n",result);
}

void t3_4_2(){
	float x=2.1;
	float result;
	asm volatile("VMOV s0, #2.0");
	asm volatile("VMOV s1, #3.0");
	asm volatile("VDIV.F32 s2, s0, s1");
	asm volatile("VMOV s3, #5.0");
	asm volatile("VMLA.F32 s3,%0,s2" :: "w" (x));
	asm volatile("VSTR.F32 s3,%0" : "=m" (result));
	printf("t3_4: %f\r\n",result);
}
