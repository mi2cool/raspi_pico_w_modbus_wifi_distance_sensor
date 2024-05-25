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
#define MULTICAST_IP "239.255.255.250"
#define MULTICAST_PORT 1900

// global variables
int watchdog_counter = 0;
char* get_response_message(char *request_message)
{
    char *response_message;
    if ("*IDN?" == request_message)
    {
        response_message = "Raspberry Pi Pico";
    }
    else if ("WATCHDOG:COUNTER?" == request_message)
    {
        char s_watchdog_counter[7];
        snprintf(s_watchdog_counter, sizeof(s_watchdog_counter), "%i", watchdog_counter);
        strcpy(response_message, "WATCHDOG:COUNTER:");
        strcpy(response_message, s_watchdog_counter);
    }
    return response_message;
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

     memcpy(p->payload, request, p->len);

     if(strncmp("IDN*?", request, strlen("IDN*?"))){
        strcpy(response, "IDN:");
        strcat(response, "Raspberry Pi Pico");
     }

    // snprintf(s_watchdog_counter, sizeof(s_watchdog_counter), "%i", watchdog_counter);
    // strcpy(msg, "these ");
    // strcat(msg, "strings ");
    // strcat(msg, "are ");
    // strcat(msg, "concatenated.");
    // strcat(msg, s_watchdog_counter);
    // memcpy(p->payload, msg, strlen(msg));
    // p->len = strlen(msg);
    // printf("Send: %s\n", msg);

    // p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM);
    // memcpy(p_tx->payload, msg, strlen(msg));
    // p_tx->len = strlen(msg);

    // err = udp_sendto(upcb, p_tx, addr, port);
    // if (err != ERR_OK)
    // {
    //     printf("Error: udp_sendto() failed: %d\n", err);
    // }
    // else
    // {
    //     printf("Message sent.\n");
    // }

    // char *response = get_response_message(p->payload);
    
    
    
    // send_udp_response(response); //, p_tx, upcb, addr, port);
    

    printf("response: %s\n",response);
    p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(response), PBUF_RAM);
    memcpy(p_tx->payload, response, strlen(response));

    err = udp_sendto(upcb, p_tx, addr, port);

    // Must free receive pbuf before return
    pbuf_free(p);
}

char* create_response(const char* request) {
    // Define the prefix
    const char* response = "";
    
    // Calculate the lengths
    size_t input_len = strlen(response);
    size_t total_len = input_len + 1; // +1 for the null terminator

    if("*IDN" == request){
        response = "*IDN:Raspberry Pi Pico";
    }


    // Allocate memory for the new string
    char* result = (char*)malloc(total_len * sizeof(char));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Copy the prefix to the result
    strcpy(result, response);

    // Concatenate the input string to the result
    // strcat(result, input);

    return result;
}


void send_udp_response(char *resp_msg) //,        // response message
                    //    struct pbuf *p_tx,     // Tx buffer object
                    //    struct udp_pcb *pcb,   // Receiving Protocol Control Block
                    //    const ip_addr_t *addr, // Address of requester
                    //    u16_t port)            // Port for response
{
    err_t err;
    // snprintf(s_int_buffer, sizeof(s_int_buffer), "%i", watchdog_counter);
    // strcat(resp_msg, s_int_buffer);
    // strcpy(resp_msg, "*IDN:Raspberry Pi Pico:");
    // strcpy(resp_msg, id);
    // snprintf(s_int_buffer, sizeof(s_int_buffer), "%i", watchdog_counter);
    // free(s_int_buffer);

    // pcb = udp_new();
    // if (!pcb)
    // {
    //     printf("Error: udp_new() failed.\n");
    //     return;
    // }

    // if ("*IDN?")
    // {
    //     char id[255];
    //     int len = strlen(id);

    //     pico_get_unique_board_id_string(id, strlen(id));
    //     strcpy(resp_msg, "*IDN:Raspberry Pi Pico:");
    //     strcpy(resp_msg, id);
    //     snprintf(s_int_buffer, sizeof(s_int_buffer), "%i", watchdog_counter);
    //     strcat(resp_msg, s_int_buffer);
    // }

    // p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(resp_msg), PBUF_RAM);
    // memcpy(p_tx->payload, resp_msg, strlen(resp_msg));
    // // p_tx->len = strlen(msg);

    // err = udp_sendto(pcb, p_tx, addr, port);

    // // Free the pbuf
    // pbuf_free(p);

    // // Remove the PCB
    // udp_remove(pcb);
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

    // set_callback_func_on_udp_message_received_event();

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
