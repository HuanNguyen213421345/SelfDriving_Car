#ifndef _UART_H_
#define _UART_H_

//#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

#define PERIPHERAL_BASE 	0xFE000000
#define PAGE_SIZE 			(4*1024)

#define GPIO_BASE_OFFSET 	0x200000
#define UART0_BASE_OFFSET 	0x201000

#define GPIO_BASE (PERIPHERAL_BASE + GPIO_BASE_OFFSET)
#define UART0_BASE (PERIPHERAL_BASE + UART0_BASE_OFFSET)

#define GPFSEL1			(0x04)
#define GPPUD 			(GPIO_BASE + 0x94)
#define GPPUDCLK0		(GPIO_BASE + 0x98)

#define UART_DR 		0x00    //data register
#define UART_FR 		0x18	//flag register
#define UART_IBRD 		0x24	//interger baudrate divisor
#define UART_FBRD 		0x28	//fractional baudrate divisor
#define UART_LCRH 		0x2C	//line controll register
#define UART_CR 		0x30	//controll register
#define UART_ICR 		0x44	//interupts clear register

#define UART_FR_TXFF     (1<<5)
#define UART_FR_RXFE     (1<<4)
#define UART_CR_UARTEN   (1<<0)
#define UART_CR_TXE		 (1<<8)
#define UART_CR_RXE		 (1<<9)
#define UART_LCRH_FEN    (1<<4)
#define UART_LCRH_WLEN_8 (3<<5)

#define UART_CLOCK 48000000UL

//static volatile uint32_t *uart_map;

//static volatile uint32_t* map_peripheral(off_t base);
void gpio_init_uart(void);
int uart_init(unsigned int baud);
void uart_putc(char c);
char uart_getc(void);

#endif


