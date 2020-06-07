#ifndef _UART_H_
#define _UART_H_
#include "MCIMX6Y2.h"

/*UART1的寄存器的基地址*/
#define UART1_BASE          (0x2020000u)

#define UART1    ((UART_Type *)UART1_BASE)
  			 		  						  					  				 	   		  	  	 	  
void Uart_Init(void);		


void PutChar(int c);

 #endif
