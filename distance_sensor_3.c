#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"

#include "cyw43_configport.h"
#include <string.h>

#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/netif.h>
#include <lwip/dns.h>
#include <lwip/inet.h> // Add this header for AF_INET
#include "myutils/utils.h"

// #define WIFI_SSID "your_SSID"
// #define WIFI_PASSWORD "your_PASSWORD"

// global variables
int watchdog_counter = 0;

int create_response(char* request, char* response)
{
    char buffer[20];
    if (strncmp("*IDN?", request, strlen("*IDN?"))==0)
    {
        strcpy(response, "IDN:");
        strcat(response, "Raspberry Pi Pico");
    }else if (strncmp("WATCHDOG_COUNTER?", request, strlen("WATCHDOG_COUNTER?"))==0)
    {
        strcpy(response, "WATCHDOG_COUNTER:");        
        snprintf(buffer, sizeof(buffer), "%i", watchdog_counter);
        strcat(response, buffer);        
    }
    
    return strlen(response);
}

void udp_receive_callback(void *arg,             // User argument - udp_recv `arg` parameter
                          struct udp_pcb *upcb,  // Receiving Protocol Control Block
                          struct pbuf *p,        // Pointer to Datagram
                          const ip_addr_t *addr, // Address of sender
                          u16_t port)            // Sender port
{

    struct pbuf *p_tx;
    err_t err;
    char request[4096];
    char response[4096];

    memset(request, 0, strlen(request));
    memset(response, 0, strlen(response));

    memcpy(request, p->payload, p->len);
    printf("request: %s\n", request);

    // create the response message
    int len_resp = create_response(request, response);

    printf("response: %s\n", response);
    p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(response), PBUF_RAM);

    // copy reponse data to tx buffer
    memcpy(p_tx->payload, response, strlen(response));

    // send response
    err = udp_sendto(upcb, p_tx, addr, port);

    // Must free receive pbuf before return
    pbuf_free(p);
}

int main()
{
    stdio_init_all();

    int x = subtract(10, 2);
    int y = x - 1;
    int z = 2;

    printf("X: %i\n", x);
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    sleep_ms(4000);

    printf("Connecting to Wi-Fi...\n");
    while (0 != cyw43_arch_wifi_connect_timeout_ms("KaMi", "Kacki134!", CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        // return 1;
    }

    printf("Connected.\n");
    // Read the ip address in a human readable way
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

    struct udp_pcb *pcb = udp_new();
    if (!pcb)
    {
        printf("Failed to create PCB\n");
        return -3;
    }
    udp_bind(pcb, IP_ADDR_ANY, 5800);
    udp_recv(pcb, udp_receive_callback, NULL);

    while (true)
    {
        // printf("Hello, world!\n");
        sleep_ms(1000);
        printf("wifi_join_state: %u\n", cyw43_state.wifi_join_state);
        printf("watchdog_counter: %u\n", watchdog_counter);

        watchdog_counter += 1;
    }
}
