#ifndef NETWORKBASE_H
#define NETWORKBASE_H

#include <atomic>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <future>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/sockets.h>

#include "espdelay.h"
#include "private.h"

/*                              Macros                              */

#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1

#ifndef MAX_RETRY_COUNT
#define MAX_RETRY_COUNT             8
#endif

/*                           Declarations                           */

struct WIFBClient
{
    uint8_t mac[6];
    uint8_t ip[4];
    std::atomic_int sock{0};
    std::atomic_bool
        networkConnected{false},
        socketConnected{false};
};

std::string mac_addr_string(uint8_t addr[6]);
std::string ip_addr_string(uint8_t addr[4]);
std::string ip_addr_string(int addr);
std::string ip_addr_string(esp_ip4_addr_t addr);
bool match_mac_addr(const uint8_t addr1[6], const uint8_t addr2[6]);

#endif
