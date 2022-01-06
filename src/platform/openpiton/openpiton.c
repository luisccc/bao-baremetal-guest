#include <8250_uart.h>

#define OPENPITON_DEFAULT_UART_ADDR		    0xfff0c2c000
#define OPENPITON_DEFAULT_UART_FREQ		    60000000
#define OPENPITON_DEFAULT_UART_BAUDRATE		115200
#define OPENPITON_DEFAULT_UART_REG_SHIFT	0
#define OPENPITON_DEFAULT_UART_REG_WIDTH	1


void uart_init(){
    uart8250_init(OPENPITON_DEFAULT_UART_ADDR, OPENPITON_DEFAULT_UART_FREQ,
			     OPENPITON_DEFAULT_UART_BAUDRATE, 0, 1);
}

void uart_putc(char c)
{
    uart8250_putc(c);
}

char uart_getchar(void)
{
    return uart8250_getc();
}

void uart_enable_rxirq()
{
    uart8250_enable_rx_int();
}

void uart_clear_rxirq()
{
    uart8250_interrupt_handler(); 
}
