#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include<string.h>    //strlen
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver.h"




#define SSID "SSID"
#define PASSPHARSE "PASSWORD"
#define TCPServerIP "192.168.102.188"

#define MESSAGE "HelloTCPServer"


void wifi_connect();
esp_err_t event_handler(void *ctx, system_event_t *event);
void initialise_wifi(void);
void tcp_client(void);
void send_data(struct sensor *sensor_data);
void send_data_buffer(uint8_t *data, size_t data_len);
void close_socket(void);


#endif /* WIFI_H */
