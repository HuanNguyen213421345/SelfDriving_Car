#include "port_min.h"

extern int uart_fd;

void min_tx_start(uint8_t port)
{
	
}
void min_tx_finished(uint8_t port)
{
	
}

uint16_t min_tx_space(uint8_t port)
{
	return 512;
}

void min_tx_byte(uint8_t port, uint8_t byte)
{
//	serWriteByte(uart_fd, byte);
	uart_putc(byte);
}

uint32_t min_time_ms(void)
{
	return myTick()/ 1000;
}

void min_application_handler(uint8_t min_id,
		uint8_t const *min_payload, uint8_t len_payload, uint8_t port)
{

}
