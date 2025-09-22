#include "uart.h"

static volatile uint32_t* uart_map;
static volatile uint32_t* gpio_map;

static volatile uint32_t* map_peripheral(off_t base)
{
	int fd = open("/dev/mem" ,O_RDWR | O_SYNC);
	if(fd < 0) {perror("open"); return NULL;}
	
	void *map = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base &~(PAGE_SIZE - 1));
	close(fd);
	
	if(map == MAP_FAILED) {perror("mmap"); return NULL;}
	
	return (volatile uint32_t*)((char*)map + (base &(PAGE_SIZE -1)));
}

void gpio_init_uart(void)
{
	gpio_map = map_peripheral(GPIO_BASE);
	if(!gpio_map){
		std::cerr << "GPIO Init Failed!!!";
		exit(1);
	}
	
	uint32_t val = gpio_map[GPFSEL1/4];
	
	val &= ~((7<<12) | (7<<15));
	val |= ((4<<12) | (4<<15));
	
	gpio_map[GPFSEL1/4] = val;
}	

int uart_init(unsigned int baud)
{
	gpio_init_uart();
	uart_map = map_peripheral(UART0_BASE);
	
	if(!uart_map) return -1;
	
	uart_map[UART_CR/4] = 0; //disable uart

	uart_map[UART_ICR/4] = 0x7FF; //clear interrupts
	
	double div = (double)UART_CLOCK / (16.0*baud); //set baud rate
	uart_map[UART_IBRD/4] = (uint32_t)div;
	uart_map[UART_FBRD/4] = (uint32_t)((div - (int)div)*64 + 0.5);
	uart_map[UART_LCRH/4] = UART_LCRH_FEN | UART_LCRH_WLEN_8; // fifo enable
	uart_map[UART_CR/4] = UART_CR_RXE | UART_CR_TXE | UART_CR_UARTEN; //tx rx enable
	
	return 0;
}

void uart_putc(char c)
{
	while(uart_map[UART_FR/4] & UART_FR_TXFF);
	uart_map[UART_DR/4] = c;
}

char uart_getc(void)
{
	while(uart_map[UART_FR/4] & UART_FR_RXFE);
	return (char)(uart_map[UART_DR/4] & 0xFF);
}
