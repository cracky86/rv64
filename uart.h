#ifndef __UART_H__
#define __UART_H__

#define UART_BASE 0x10000000
#define UART_TX (*(volatile unsigned char *)(UART_BASE))
#define UART_RX (*(volatile unsigned char *)(UART_BASE))
#define UART_STATUS (*(volatile unsigned char *)(UART_BASE + 5))

// Sends byte to uart console, self explanatory
void putchar(char c) {
  while ((UART_STATUS & 0x20) == 0);
  UART_TX = c;
  return;
}

// Receives a byte from uart console, also self explanatory
void getchar(char* out) {
  while ((UART_STATUS & 0x01) == 0);
  *out = UART_RX;
}


#endif
