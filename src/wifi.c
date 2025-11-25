#include "inc/wifi.h"
#include <stdio.h>

int wifi_connect(void) {
    if (cyw43_arch_init()) {
        printf("CYW43 init failed\n");
        return -1;
    }

    sleep_ms(500);  // Give chip time to stabilize
    cyw43_arch_enable_sta_mode();
    printf("Connecting to %s...\n", WIFI_SSID);

    int r = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS,
                                               CYW43_AUTH_WPA2_AES_PSK, 15000);
    if (r) {
        printf("Connect failed, code %d\n", r);
        return -1;
    }

    printf("Connected!\n");
    return 0;
}

