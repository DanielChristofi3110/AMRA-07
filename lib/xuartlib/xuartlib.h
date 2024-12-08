#ifndef XUARTLIB_H
#define XUARTLIB_H


// Transmit byte
void uart_transmit(uint8_t data);

// Receive byte
uint8_t uart_receive_char(void);

// Receive four bytes
uint32_t uart_receive_int(void);

// Transmit a character
void uart_charln(const uint8_t s);

// Transmit a string (char*)
void uart_println_str(const uint8_t* str);

// Transmit an integer
void uart_intln(const uint32_t num);

// Transmit an array of integers
void uart_println_int(const uint32_t* num);

#endif
