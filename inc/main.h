#ifndef __MAIN_H
#define __MAIN_H
#include <hardware/pio_instructions.h>
#include <pico/stdlib.h>


/* Program Version*/
#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUILD_VERSION 0

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/


/* Exported function prototypes ----------------------------------------------*/

void     Watchdog        ( void );



/* Exported defines ----------------------------------------------------------*/

// Comms type

#define DYNAMENT_PROTOCOL   1
#define COMMS_PROTOCOL      DYNAMENT_PROTOCOL

// Checksum type
#define CSUM_STANDARD   0
#define CSUM_CRC        1         
#define CSUM_TYPE       CSUM_STANDARD

// System setup
#define NOP                     pio_encode_nop ( )
#define WATCHDOG_MILLISECONDS   8000    // Maximum 8 300 ms


// GPIO
//#define LED_PICO_PIN        25
#define UART_SEN_RX_PIN     16  // UART0_TX
#define UART_SEN_TX_PIN     17  // UART0_RX

//for ESP
// #define ESP_E_RX_PIN       5
// #define ESP_E_TX_PIN       4
// #define ESP_UART_ID      uart1
// #define ESP_BAUD_RATE      9600

//for MSP430
#define MSP430_RX_PIN       5
#define MSP430_TX_PIN       4
#define MSP430_UART_ID      uart1
#define MSP430_BAUD_RATE      9600

// UART
#define DATA_BITS           8
#define PARITY              UART_PARITY_NONE
#define STOP_BITS           1
#define UART_SEN            uart0
#define UART_BAUD_RATE      9600
#define UART_BUFFER_LENGTH  500
#define UART_TIMEOUT        1000

#endif /* _HALGPIO_H*/

/* End of file -----------------------*/

