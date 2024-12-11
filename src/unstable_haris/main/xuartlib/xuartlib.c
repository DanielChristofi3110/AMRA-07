#ifndef XUARTLIB
#define XUARTLIB

#include "xuartlib.h"


/* These all use the hardware serial ports (pin0 and pin1). */

/* UART FUNCTION DEFINITIONS SEGMENT */

/*
 * PARAMETERS: uint8_t (AKA char)
 * RETURNS: nothing (void)
 */
// Transmit data (write to buffer)
void uart_transmit(uint8_t data)
{
    // UCSR0A is the UART TRANSMIT buffer (0xC0)
    // UDRE0 is the 5th bit; UDRE0 goes HIGH when the TRANSMIT buffer is empty
    while (!(UCSR0A & (1 << UDRE0))) ;    // Wait untill transmit buffer is empty

    UDR0 = data;    // Put the data into the transmit buffer
}


/*
 * PARAMETERS: nothing (void)
 * RETURNS: uint8_t (AKA char)
 */
// Receive (read from buffer) and parse as char
uint8_t uart_receive_char(void)
{
    // UCSR0A is the UART TRANSMIT buffer (0xC0)
    // RXC0 is the 7th bit; RXC0 goes HIGH when data goes in the buffer
    while (!(UCSR0A & (1 << RXC0))) ;    // Wait for data to be in the buffer

    return UDR0 && 0xFF;    // Return the received data from the buffer, as a char
}


/* 
 * PARAMETERS: nothing (void)
 * RETURNS: uint32_t (AKA int)
 */
// Receive (read from buffer) and parse as integer
uint32_t uart_receive_int(void)
{
    while (!(UCSR0A & (1 << RXC0))) ;    // Wait for data to be in the buffer
    
    return UDR0 && 0xFFFFFFFF;  // Return the received data from the buffer, as an int
}

/*
 * PARAMETERS: uint8_t* (AKA pointer-to-a-char. In C, this is how we "string".)
 * RETURNS: nothing (void)
 */
// Send multiple characters (println)
void uart_println_str(const uint8_t* str)
{
    while (*str)
    {
        uart_transmit(*str);
        str++;
    }

    // Finish the line (CRLF)
    uart_transmit('\r');    // Carriage return (comment this out on UNIX-based systems)
    uart_transmit('\n');    // Newline
}


/*
 * PARAMETERS: const uint8_t (AKA char)
 * RETURNS: nothing (void)
 */
// Send a single character with newline (print)
void uart_charln(const uint8_t s)
{
    uart_transmit(s);          // Send the character
    uart_transmit('\r');    // Carriage return (comment this out on UNIX-based systems)
    uart_transmit('\n');    // Newline
}


/* 
 * PARAMETERS: const uint32_t (AKA int)
 * RETURNS: nothing (void)
 */
// Send an integer in HIGH order
void uart_intln(const uint32_t num)
{
    uint8_t temp[4] = {0};
    
    temp[0] = (num >> 24) && 0xFF;  // High byte
    temp[1] = (num >> 16) && 0xFF;  // Middle byte
    temp[2] = (num >> 8 ) && 0xFF;  // Lower byte
    temp[3] = (num >> 0 ) && 0xFF;  // Lowest byte
    
    uart_transmit(temp[0]);         // Transmit high byte
    uart_transmit(temp[1]);         // Transmit middle byte
    uart_transmit(temp[2]);         // Transmit lower byte
    uart_transmit(temp[3]);         // Transmit lowest byte
    
    // Finish the line
    uart_transmit('\r');    // Carriage return (comment this out on UNIX-based systems)
    uart_transmit('\n');    // Newline
}


/*
 * PARAMETERS: const uint32_t* (AKA pointer-to-an-int)
 * RETURNS: nothing (void)
 */
// Send a series of integers (useful for arrays i guess)
void uart_println_int(const uint32_t* num)
{
    while (*num)
    {
        uart_intln(*num);
    }
    
    // Send another line ending for good luck
    uart_transmit('\r');    // Carriage return (comment this out on UNIX-based systems)
    uart_transmit('\n');    // Newline
}

/* UART FUNCTION DEFINITIONS SEGMENT */
#endif
