
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "inc/main.h"
#include <string.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h>
#include <pico/binary_info.h>
#include "inc/comms.h"
#include "inc/DynamentComms.h"
#include <hardware/pio_instructions.h>
#include <hardware/gpio.h>
#include <time.h>
#include "inc/wifi.h"
#include "inc/mqttf.h"

/* FOR HOST FUNCTION */
float latest_gas_value = 0.0f;
bool latest_value_valid = false;

char msp430_rx_buffer[32];
int msp430_rx_index = 0;

void HandleHostCommands(void);
/* =================================================*/

/* Private typedef */
//#define GAS_READING_MEASURAND   30057
#define POLL_COUNT              8

/* Private variables ---------------------------------------------------------*/
struct repeating_timer timer_heartbeat;
int pollCounter = POLL_COUNT;

/* Function declarations */
void RequestGasReading();
void ReadingReceived(int status, float value);
void DualReadingReceived(int status, float reading1, float reading2);

void HandleHostCommands(void)
{
    while (uart_is_readable(MSP430_UART_ID)) {

        char c = uart_getc(MSP430_UART_ID);

        if (c == '\n') {
            msp430_rx_buffer[msp430_rx_index] = '\0';

            // Compare command
            if (strcmp(msp430_rx_buffer, "GET") == 0 ||
                strcmp(msp430_rx_buffer, "VALUE?") == 0) {

                char msg[64];

                if (latest_value_valid) 
                {
                    sprintf(msg, "VALUE:%0.2f\n", latest_gas_value);
                } else {
                    sprintf(msg, "VALUE:INVALID\n");
                }

                uart_puts(MSP430_UART_ID, msg);
            }

            msp430_rx_index = 0;  // Reset buffer
        }
        else {
            if (msp430_rx_index < sizeof(msp430_rx_buffer) - 1) {
                msp430_rx_buffer[msp430_rx_index++] = c;
            }
        }
    }
}

static bool timer_heartbeat_isr(struct repeating_timer *t)
{
    if (cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN)) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    } else {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }

    if (pollCounter > 0) {
        --pollCounter;
        if (pollCounter <= 0) {
            RequestGasReading();
            pollCounter = POLL_COUNT;
        }
    }
    return true;
}

int main() {
   
    stdio_init_all();
    sleep_ms(2000);
    printf("Starting Wi-Fi Test......\n");
    if(wifi_connect()==0){
         printf("Connected to Wi-Fi successfully!\n");  
     }
     
     else{
         printf("Failed to connect to Wi-Fi.\n");
     }
    sleep_ms(2000);
    mqtt_init_connection();

    watchdog_enable(WATCHDOG_MILLISECONDS, 1);

    uart_init(UART_SEN, UART_BAUD_RATE);
    gpio_set_function(UART_SEN_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_SEN_TX_PIN, GPIO_FUNC_UART);

    uart_init(MSP430_UART_ID, MSP430_BAUD_RATE);
    gpio_set_function(MSP430_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(MSP430_TX_PIN, GPIO_FUNC_UART);
   
    add_repeating_timer_ms(1000, timer_heartbeat_isr, NULL, &timer_heartbeat);

    initialise_comms();
    if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL)
        InitialiseDynamentComms();

    for (;;) 
    {
        Watchdog();
        HandleHostCommands(); 
        if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL)
            DynamentCommsHandler();
        else
           // ModbusCommsHandler();
        
        mqtt_service();          //  service MQTT here (non-blocking
        cyw43_arch_poll(); // keep network stack running
    }

    return 0;
}

void Watchdog(void)
{
    watchdog_update();
}

void RequestGasReading()
{
    //printf("RequestGasReading called\n");
    if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL) {
        //printf("Using Dynament_Protocol\n");
        RequestLiveData2(DualReadingReceived);
       
    } 
    else 
    {
        printf("Using another protocol\n");
        //ReadMeasurand(GAS_READING_MEASURAND, ReadingReceived);
    }
}

void ReadingReceived(int status, float value)
{
    if (status == READ_RESPONSE_VALUE_VALID) {
        // Handle valid reading
    } else {
        // Handle invalid/timed-out reading
    }
}

void DualReadingReceived(int status, float reading1, float reading2)
{
    if (status == READ_RESPONSE_VALUE_VALID) {
        printf("--------- Callback Executed: Status = %d, **** \033[93mGas1 = %f\033[0m, Gas2 = %f\n", status, reading1, reading2);
        //uart_write_blocking(ESP_UART_ID, (const uint8_t*)&reading1, sizeof(reading1));
        //printf("Sent Successfully to ESP32\n");
        //printf("Callback Executed: Gas1 = %f, Gas2 = %f\n", reading1, reading2);
        // Store for MSP430 host
        latest_gas_value = reading1;
        latest_value_valid = true;

        mqtt_publish_reading(reading1);
    printf("Gas1 = %f Gas2 = %f\n", reading1, reading2);

    } else {
        printf("Invalid reading\n");
        printf("Callback Executed: Status = %d (Invalid)\n", status);
    }
}

