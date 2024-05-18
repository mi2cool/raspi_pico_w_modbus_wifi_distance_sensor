#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "cyw43_configport.h"
#include <string.h>

#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/netif.h>
#include <lwip/dns.h>
#include <lwip/inet.h>  // Add this header for AF_INET

// #define WIFI_SSID "your_SSID"
// #define WIFI_PASSWORD "your_PASSWORD"
#define MULTICAST_IP "239.255.255.250"
#define MULTICAST_PORT 1900
// #include "utils/utils.h"

void send_multicast_message() {
    struct udp_pcb *pcb = udp_new();
    if (!pcb) {
        printf("Failed to create PCB\n");
        return;
    }

    ip_addr_t multicast_addr;
    ipaddr_aton(MULTICAST_IP, &multicast_addr);

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen("Hello, Multicast!"), PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        udp_remove(pcb);
        return;
    }

    memcpy(p->payload, "Hello, Multicast!", p->len);

    err_t err = udp_sendto(pcb, p, &multicast_addr, MULTICAST_PORT);
    if (err != ERR_OK) {
        printf("Failed to send message: %d\n", err);
    } else {
        printf("Message sent: Hello, Multicast!\n");
    }

    pbuf_free(p);
    udp_remove(pcb);
}

int main()
{
    stdio_init_all();

    // int x = subtract(10, 2); y

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("KaMi", "Kacki134!", CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        return 1;
    }
    else
    {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }



    while (true)
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
        printf("wifi_join_state: %u", cyw43_state.wifi_join_state);
        send_multicast_message();
    }
}
