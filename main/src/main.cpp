/** WIFB
 * ESP32-based wireless audio transceiver
 * Copyright 2022 K Hughes Production LLC
 */

#include <iostream>
#include <cstring>

#include "debugmacros.h"
#include "private.h"

#include "ringbuffer.h"
#include "espdelay.h"
#include "esp32button.h"
#include "espi2s.h"
#include "wifbnetwork.h"
// #include "ltcstaticwavetables.h"

// #include "oscillator.h"


/*                              Macros                              */

/* I2S master clock */
#define I2S_MCK                     (GPIO_NUM_0)

/* I2S word select */
#define I2S_WS                      (GPIO_NUM_26)

/* I2S bit clock */
#define I2S_BCK                     (GPIO_NUM_14)

/* I2S data */
#define I2S_DI                      (GPIO_NUM_35)

/* I2S output data */
#define I2S_DO                      (GPIO_NUM_25)

/* Shutdown signal output */
#define I2S_SHUTDOWN                (GPIO_NUM_21)

/* Momentary switch */
#define BUTTON_PIN                  (GPIO_NUM_35)

/* Audio sample rate */
#ifndef SAMPLE_RATE
#define SAMPLE_RATE                 48000
#endif

/* Bit depth for audio I/O */
#ifndef BITS_PER_SAMPLE
#define BITS_PER_SAMPLE             16
#endif

/* Mono or stereo */
#ifndef NUM_CHANNELS
#define NUM_CHANNELS                1
#endif

/* Sample width in bytes */
#define SAMPLE_WIDTH                ((BITS_PER_SAMPLE) / 8)

/* Audio data type depends on bit depth */
#if ((BITS_PER_SAMPLE) == 8)
#define AUDIO_DATATYPE              uint8_t
#elif ((BITS_PER_SAMPLE) == 16)
#define AUDIO_DATATYPE              int16_t
#elif ((BITS_PER_SAMPLE) == 24)
#define AUDIO_DATATYPE              int_fast32_t
#elif ((BITS_PER_SAMPLE) == 32)
#define AUDIO_DATATYPE              int_fast32_t
#endif

/* Length in samples of each buffer in ring */
#ifndef RING_BUFFER_LENGTH
#define RING_BUFFER_LENGTH          128
#endif

/* Number of buffers in ring buffer */
#ifndef RING_LENGTH
#define RING_LENGTH                 2
#endif

/* Size in bytes of each buffer in ring */
#define RING_BUFFER_SIZE            ((RING_BUFFER_LENGTH) * (SAMPLE_WIDTH))

/* Size in bytes of each transmission via socket */
#ifndef TRANSMIT_CHUNKSIZE
#if ((RING_BUFFER_SIZE) >= 1024)
#define TRANSMIT_CHUNKSIZE          ((RING_BUFFER_SIZE) / 16)
#elif ((RING_BUFFER_SIZE) >= 512)
#define TRANSMIT_CHUNKSIZE          ((RING_BUFFER_SIZE) / 8)
#elif ((RING_BUFFER_SIZE) >= 256)
#define TRANSMIT_CHUNKSIZE          ((RING_BUFFER_SIZE) / 4)
#else
#define TRANSMIT_CHUNKSIZE          (RING_BUFFER_SIZE)
#endif
#endif

/* Whether this unit defaults to transmit mode */
#ifndef DEFUALT_MODE_TRANSMIT
#define DEFUALT_MODE_TRANSMIT        0
#endif

/* Transmitter ipv4 address */
#ifndef TRANSMITTER_IPV4_ADDR
#define TRANSMITTER_IPV4_ADDR       "192.168.4.1"
#endif

/*                             Variables                            */

/* Transmit or receive */
static bool txMode(DEFUALT_MODE_TRANSMIT);

/* Audio I/O */
static Buffer::AtomicMultiReadRingBuffer<AUDIO_DATATYPE> ringBuffer(
        RING_BUFFER_LENGTH,
        RING_LENGTH
    );
static I2S::Bus i2s;

/* Hardware button */
static Esp32Button::DualActionButton button(BUTTON_PIN);

static WIFBDevice self;
static std::vector<std::shared_ptr<WIFBDevice>> connectedClients;
static int retryNum = 0;
static EventGroupHandle_t staEventGroup;

/*                           Declarations                           */

/* Audio */

void i2s_to_ring_buffer(void);
void i2s_to_buffer_loop(void);
void ring_buffer_to_i2s(void);
void buffer_to_i2s_loop(void);
// void osc_to_ring_buffer(void);
// void osc_to_buffer_loop(void);

// Osc::OscillatorBase<float> osc;

/* Networking */

std::shared_ptr<WIFBDevice> get_client_from_mac(const uint8_t addr[6]);

/* Transmitter */

void ap_event_handler(
        void* arg,
        esp_event_base_t eventBase,
        int32_t eventId,
        void* data
    );
int config_ap(void);
void purge_disconnected_clients(void);
void socket_server_tcp(void);
void socket_server_udp(void);
void client_sock_handler(std::shared_ptr<WIFBDevice> client);

/* Receiver */

void sta_event_handler(
        void* arg,
        esp_event_base_t eventBase,
        int32_t eventId,
        void* data
    );
int config_sta(void);
void socket_client(void);

/* Main */

extern "C" void app_main(void);

/*                            Definitions                           */

/* Audio */

void i2s_to_ring_buffer(void)
{
    // DEBUG_OUT("Getting available samples...\n");

    /* Read from i2s input to ring buffer */
    const int unwritten(ringBuffer.unwritten());

    // DEBUG_OUT(+unwritten << " unwritten samples");
    // DEBUG_OUT(" available to write to ring buffer\n");

    if (!unwritten) return;

    try
    {
        DEBUG_OUT("Reading from i2s...\n");

        i2s.read(ringBuffer.get_write_buffer(), unwritten);

        DEBUG_OUT("Read from i2s\n");
    }
    catch (...)
    {
        DEBUG_ERR("Error reading from i2s\n");
        return;
    }

    DEBUG_OUT("Reporting " << unwritten);
    DEBUG_OUT(" written samples to ring buffer\n");

    ringBuffer.report_written_samples(unwritten);
}

void i2s_to_buffer_loop(void)
{
    DEBUG_OUT("Running i2s_to_buffer_loop...\n");
    int delayCounter(0);
    while (true)
    {
        i2s_to_ring_buffer();
        delay_ticks_count(&delayCounter, 125, 1);
    }
    DEBUG_ERR("i2s_to_buffer_loop exited unexpectedly\n");
}

void ring_buffer_to_i2s(void)
{
    /* Write from ring buffer to i2s output */
    if (!ringBuffer.buffers_buffered()) return;
    const int unread(ringBuffer.unread());
    
    #if _DEBUG
    if (!unread)
    {
        DEBUG_ERR("This point should not be reachable\n");
        return;
    }
    #endif
    
    i2s.write(ringBuffer.get_read_buffer(), unread);
    ringBuffer.report_read_samples(unread);
}

void buffer_to_i2s_loop(void)
{
    DEBUG_OUT("Running buffer_to_i2s_loop...\n");
    int delayCounter(0);
    while (true)
    {
        ring_buffer_to_i2s();
        delay_ticks_count(&delayCounter, 125, 1);
    }
    DEBUG_ERR("buffer_to_i2s_loop exited unexpectedly\n");
}

// void osc_to_ring_buffer(void)
// {
//     /* Read from i2s input to ring buffer */
//     const int unwritten(ringBuffer.unwritten());

//     if (!unwritten) return;

//     osc.get_int<AUDIO_DATATYPE>(
//             ringBuffer.get_write_sample(),
//             unwritten
//         );
//     ringBuffer.report_written_samples(unwritten);
// }

// void osc_to_buffer_loop(void)
// {
//     int delayCounter(0);
//     while (true)
//     {
//         osc_to_ring_buffer();
//         delay_ticks_count(&delayCounter, 125, 1);
//     }
// }

/* Networking */

std::shared_ptr<WIFBDevice> get_client_from_mac(const uint8_t addr[6])
{
    DEBUG_OUT("Retrieving client from mac addr...\n");

    for (std::shared_ptr<WIFBDevice> c: connectedClients)
    {
        if (c == nullptr) continue;
        else if (match_mac_addr(c->mac, addr)) return c;
    }

    DEBUG_OUT("Client not found in index\n");

    return nullptr;
}

/* Transmitter */

void ap_event_handler(
        void* arg,
        esp_event_base_t eventBase,
        int32_t eventId,
        void* data
    )
{
    if (eventId == WIFI_EVENT_AP_STACONNECTED)
    {
        #if _DEBUG
        wifi_event_ap_staconnected_t* event = reinterpret_cast<wifi_event_ap_staconnected_t*>(data);
        DEBUG_OUT("Station " << mac_addr_string(event->mac) << " connected\n");
        DEBUG_OUT("eventBase == " << eventBase << '\n');
        #endif
    }
    else if (eventId == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = reinterpret_cast<wifi_event_ap_stadisconnected_t*>(data);
        
        std::shared_ptr<WIFBDevice> client = get_client_from_mac(event->mac);
        if (client != nullptr)
        {
            client->socketConnected = false;
            client->networkConnected = false;
            client->sock = 0;

            DEBUG_OUT("Disconnected client:\n");
            DEBUG_OUT("\t  ip: " << ip_addr_string(client->ip) << '\n');
            DEBUG_OUT("\t mac: " << mac_addr_string(client->mac) << '\n');
            DEBUG_OUT("\tsock: " << client->sock << '\n');
        }
    }
}

int config_ap(void)
{
    esp_err_t ret = nvs_flash_init();
    if (
            (ret == ESP_ERR_NVS_NO_FREE_PAGES)
            || (ret == ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&initConfig);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &ap_event_handler,
            NULL,
            NULL
        );

    wifi_config_t config;
    strcpy((char*)config.ap.ssid, CONFIG_SSID);
    config.ap.ssid_len = strlen(CONFIG_SSID);
    config.ap.channel = CONFIG_CHANNEL;
    strcpy((char*)config.ap.password, CONFIG_PASSWD);
    config.ap.authmode = (
            strlen(CONFIG_PASSWD)
            ? WIFI_AUTH_WPA2_PSK
            : WIFI_AUTH_OPEN
        );
    config.ap.ssid_hidden = 0;
    config.ap.max_connection = CONFIG_MAX_STA_CONNECTIONS;
    config.ap.beacon_interval = 100;

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &config);
    esp_wifi_get_mac(WIFI_IF_AP, self.mac);
    esp_err_t rc = esp_wifi_start();
    return (rc != ESP_OK) ? rc : 0;
}

void purge_disconnected_clients()
{
    #if _DEBUG
    int lengthBeforePurge(static_cast<int>(connectedClients.size()));
    DEBUG_OUT("Purging ");
    #endif

    std::vector<std::vector<std::shared_ptr<WIFBDevice>>::iterator> purgeList;
    int length(0);
    for (
            auto it = std::begin(connectedClients);
            it != std::end(connectedClients);
            ++it
        )
    {
        std::shared_ptr<WIFBDevice> c = *it;
        if (c == nullptr) continue;
        else if (!c->socketConnected)
        {
            purgeList.push_back(it);
            ++length;
        }
    }

    DEBUG_OUT(length << " disconnected clients\n");
    
    for (int i(0); i < length; ++i)
    {
        connectedClients.erase(purgeList[i]);
    }

    #if _DEBUG
    int numPurged(
            lengthBeforePurge
            - static_cast<int>(connectedClients.size())
        );
    if (numPurged == length)
    {
        DEBUG_OUT("Sucessfully purged ");
        DEBUG_OUT(length << " disconnected clients\n");
    }
    else
    {
        DEBUG_ERR("Error: purged " << numPurged);
        DEBUG_ERR(" of " << length << " disconnected clients\n");
    }
    #endif
}

void socket_server_tcp(void)
{
    DEBUG_OUT("Starting tcp socket server\n");

    struct sockaddr_in serverAddress, clientAddress;
    
    DEBUG_OUT("Creating socket...\n");

    // Create a socket that we will listen upon.
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        DEBUG_ERR("socket: " << sock << errno << '\n');
        return;
    }

    DEBUG_OUT("Binding socket to port...\n");

    // Bind our server socket to a port.
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(CONFIG_PORT);
    int rc = bind(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (rc < 0)
    {
        DEBUG_OUT("bind: " << rc << errno << '\n');
        return;
    }

    DEBUG_OUT("Listening for connections...\n");

    // Flag the socket as listening for new connections.
    rc = listen(sock, 5);
    if (rc < 0)
    {
        DEBUG_OUT("listen: " << rc << errno << '\n');
        return;
    }

    int delayCounter(0);
    uint8_t incomingMacAddr[6];
    socklen_t clientAddressLength;
    int clientSock;
    std::shared_ptr<WIFBDevice> client;

    while (true)
    {
        DEBUG_OUT("Listening for new connections...\n");
        DEBUG_OUT("Accepting connection from client...\n");

        // Listen for a new client connection.
        clientAddressLength = sizeof(clientAddress);
        clientSock = accept(sock, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSock < 0)
        {
            DEBUG_OUT("accept: " << clientSock << " " << errno << '\n');
            return;
        }

        // Index client mac address
        recv(clientSock, incomingMacAddr, 6, 0);
        
        // Check if client is reconnecting or new
        client = get_client_from_mac(incomingMacAddr);
        if (client == nullptr)
        {
            // Create new client
            DEBUG_OUT("New client found:\n");

            client = std::make_shared<WIFBDevice>();
            connectedClients.push_back(client);
            
            // Purge inactive receivers from client list
            if (connectedClients.size() > CONFIG_MAX_STA_CONNECTIONS)
            {
                purge_disconnected_clients();
            }

            memcpy(client->mac, incomingMacAddr, 6);
            memcpy(
                    client->ip,
                    reinterpret_cast<uint8_t*>(&clientAddress.sin_addr.s_addr),
                    4
                );
            DEBUG_OUT("New client created\n");
        }
        else
        {
            DEBUG_OUT("Existing client found:\n");
        }

        // Update client status in index
        client->networkConnected = true;
        client->socketConnected = true;
        client->sock = clientSock;

        DEBUG_OUT("\t  ip: " << ip_addr_string(client->ip) << '\n');
        DEBUG_OUT("\t mac: " << mac_addr_string(client->mac) << '\n');
        DEBUG_OUT("\tsock: " << client->sock << '\n');

        // Launch handler for individual client
        client_sock_handler(client);
        // std::thread t(client_sock_handler, client);

        DEBUG_OUT("Client handler launched\n");
        
        DEBUG_OUT("Incrementing delay counter...\n");
        delay_ticks_count(&delayCounter, 125, 1);
        DEBUG_OUT("Delay counter incremented\n");
    }
    DEBUG_ERR("Exiting socket_server_tcp\n");
}

void socket_server_udp(void)
{
    DEBUG_OUT("Starting udp socket server\n");

    int rc;
    int delayCounter(0);
    uint8_t rx_buffer[128];
    const char* tx_buffer = "hello from socket_server_udp";
    struct sockaddr_in serverAddress;

    while (true)
    {
        DEBUG_OUT("Creating socket...\n");
        self.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (self.sock < 0)
        {
            DEBUG_ERR("Error creating socket\n");
            break;
        }
        else
        {
            DEBUG_OUT("Socket created\n");
        }

        int enable = 1;
        lwip_setsockopt(
                self.sock,
                IPPROTO_IP,
                IP_PKTINFO,
                &enable,
                sizeof(enable)
            );

        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(
                self.sock,
                SOL_SOCKET,
                SO_RCVTIMEO,
                &timeout,
                sizeof(timeout)
            );

        DEBUG_OUT("Binding socket to port...\n");
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(CONFIG_PORT);
        rc = bind(
                self.sock,
                reinterpret_cast<sockaddr*>(&serverAddress),
                sizeof(serverAddress)
            );
        if (rc < 0)
        {
            DEBUG_ERR("bind err; rc: " << rc << '\n');
            return;
        }

        struct sockaddr_storage source_addr;
        socklen_t socklen = sizeof(source_addr);

        struct iovec iov;
        struct msghdr msg;
        uint8_t cmsg_buf[CMSG_SPACE(sizeof(struct in_pktinfo))];

        iov.iov_base = rx_buffer;
        iov.iov_len = sizeof(rx_buffer);
        msg.msg_control = cmsg_buf;
        msg.msg_controllen = sizeof(cmsg_buf);
        msg.msg_flags = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = reinterpret_cast<sockaddr*>(&source_addr);
        msg.msg_namelen = socklen;

        while (true)
        {
            DEBUG_OUT("Waiting for data...\n");
            int length = recvmsg(self.sock, &msg, 0);

            if (length < 0)
            {
                DEBUG_ERR("Error receiving data\n");
                break;
            }

            rx_buffer[length] = 0;
            DEBUG_OUT("Data received of length " << length);
            DEBUG_OUT("\n\t" << rx_buffer << '\n');

            rc = sendto(
                    self.sock,
                    tx_buffer,
                    strlen(tx_buffer),
                    0,
                    reinterpret_cast<sockaddr*>(&source_addr),
                    sizeof(source_addr)
                );
            if (rc < 0)
            {
                DEBUG_ERR("Error occurred during sending\n");
                break;
            }
            DEBUG_OUT("Data sent\n");

            delay_ticks_count(&delayCounter, 125, 1);
        }

        if (self.sock != -1)
        {
            DEBUG_ERR("Shutting down socket and restarting...\n");
            shutdown(self.sock, 0);
            close(self.sock);
        }
    }

    DEBUG_ERR("Exiting socket_server_udp\n");
}

void client_sock_handler(std::shared_ptr<WIFBDevice> client)
{
    int delayCounter(0);
    int rc;

    int_fast8_t numReaders = ringBuffer.num_readers();

    DEBUG_OUT("Current num readers: " << +numReaders);
    DEBUG_OUT("\nSetting new num readers to: ");
    DEBUG_OUT(+((numReaders > 1) ? (numReaders + 1) : numReaders));
    DEBUG_OUT('\n');

    /* Set the number of connected clients.
    The first client is already accounted for
    in the ring buffer read counter. */
    ringBuffer.set_num_readers(
            (numReaders > 1)
            ? (numReaders + 1)
            : numReaders
        );

    DEBUG_OUT("Num readers set to " << +ringBuffer.num_readers() << '\n');

    while (client->socketConnected)
    {
        DEBUG_OUT(ringBuffer.buffered() << " samples buffered");
        DEBUG_OUT(" of total ring sample length of ");
        DEBUG_OUT(ringBuffer.size() << '\n');
        DEBUG_OUT("Transmit chunk size is " << (TRANSMIT_CHUNKSIZE) << '\n');

        if (ringBuffer.buffered() >= (TRANSMIT_CHUNKSIZE))
        {
            DEBUG_OUT("Sending data to client\n");

            rc = send(
                    client->sock,
                    ringBuffer.get_read_byte(),
                    (TRANSMIT_CHUNKSIZE),
                    0
                );
            if (rc < 0)
            {
                DEBUG_ERR("Error sending data\n");
            }
            
            DEBUG_OUT("Reporting " << (TRANSMIT_CHUNKSIZE) << " read bytes to buffer\n");
            
            ringBuffer.report_read_bytes(TRANSMIT_CHUNKSIZE);
        }

        DEBUG_OUT("Incrementing delay counter\n");

        delay_ticks_count(&delayCounter, 125, 1);

        DEBUG_OUT("Cycling...\n");
    }

    DEBUG_OUT("Decrementing num readers for disconnected client\n");

    /* Do not decrement the read counter below one */
    ringBuffer.set_num_readers(
            (numReaders > 1)
            ? (numReaders - 1)
            : numReaders
        );

    DEBUG_OUT("Closing client socket\n");

    close(client->sock);
}

/* Receiver */

void sta_event_handler(
        void* arg,
        esp_event_base_t eventBase,
        int32_t eventId,
        void* data
    )
{
    if ((eventBase == WIFI_EVENT) && (eventId == WIFI_EVENT_STA_START))
    {
        DEBUG_OUT("Wifi started; connecting to AP...\n");

        esp_wifi_connect();
    }
    else if ((eventBase == WIFI_EVENT) && (eventId == WIFI_EVENT_STA_DISCONNECTED))
    {
        DEBUG_ERR("Failed to connect to AP\n");
        
        self.networkConnected = false;
        self.socketConnected = false;

        if (retryNum++ < MAX_RETRY_COUNT)
        {
            esp_wifi_connect();
        
            DEBUG_ERR("Retrying connection to AP\n");
        }
        else
        {
            xEventGroupSetBits(staEventGroup, WIFI_FAIL_BIT);
        }
    }
    else if ((eventBase == IP_EVENT) && (eventId == IP_EVENT_STA_GOT_IP))
    {
        ip_event_got_ip_t* event = reinterpret_cast<ip_event_got_ip_t*>(data);

        DEBUG_OUT("Got IP: " << ip_addr_string(event->ip_info.ip) << '\n');
        
        self.networkConnected = true;
        self.socketConnected = false;
        
        memcpy(
                self.ip,
                reinterpret_cast<uint8_t*>(&event->ip_info.ip),
                4
            );

        retryNum = 0;
        xEventGroupSetBits(staEventGroup, WIFI_CONNECTED_BIT);
    }
    else
    {
        DEBUG_ERR("An unknown event occured:\n");
        DEBUG_ERR("\teventBase == " << eventBase << '\n');
    }
}

int config_sta(void)
{
    esp_err_t ret = nvs_flash_init();
    if (
            (ret == ESP_ERR_NVS_NO_FREE_PAGES)
            || (ret == ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    staEventGroup = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&initConfig);

    esp_event_handler_instance_t instanceId;
    esp_event_handler_instance_t instanceIp;

    esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &sta_event_handler,
            NULL,
            &instanceId
        );
    esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &sta_event_handler,
            NULL,
            &instanceIp
        );

    wifi_config_t config;
    config.sta = {
            .ssid = CONFIG_SSID,
            .password = CONFIG_PASSWD,
        };
    if (strlen(CONFIG_PASSWD))
    {
        config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }
    config.sta.pmf_cfg.capable = true;
    config.sta.pmf_cfg.required = false;

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &config);
    esp_wifi_get_mac(WIFI_IF_STA, self.mac);
    esp_wifi_start();

    DEBUG_OUT("STA started\n");

    /* Block while connecting */
    EventBits_t bits = xEventGroupWaitBits(
            staEventGroup,
            (WIFI_CONNECTED_BIT | WIFI_FAIL_BIT),
            pdFALSE,
            pdFALSE,
            portMAX_DELAY
        );

    int rc;
    if (bits & WIFI_CONNECTED_BIT)
    {
        rc = 0;
        DEBUG_OUT("Connected to SSID " << CONFIG_SSID << '\n');
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        rc = 1;
        DEBUG_OUT("Failed to connect to SSID " << CONFIG_SSID << '\n');
    }
    else
    {
        rc = 2;
        DEBUG_OUT("Unexpected event\n");
    }

    esp_event_handler_instance_unregister(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            instanceIp
        );
    esp_event_handler_instance_unregister(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            instanceId
        );
    vEventGroupDelete(staEventGroup);
    return rc;
}

void socket_client_tcp(void)
{
    DEBUG_OUT("Starting socket_client_tcp...\n");
    DEBUG_OUT("Creating socket...\n");

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    self.sock = sock;

    DEBUG_OUT("socket rc: " << sock << '\n');

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(
            AF_INET,
            TRANSMITTER_IPV4_ADDR,
            &serverAddress.sin_addr.s_addr
        );
    serverAddress.sin_port = htons(CONFIG_PORT);

    DEBUG_OUT("Connecting to server...\n");
    int rc = connect(
            self.sock,
            (struct sockaddr*)&serverAddress,
            sizeof(struct sockaddr_in)
        );
    DEBUG_OUT("connect rc: " << rc << '\n');

    if (self.socketConnected = (rc >= 0))
    {
        send(self.sock, self.mac, 6, 0);
        DEBUG_OUT("Send self mac addr: " << mac_addr_string(self.mac) << '\n');
    }

    int delayCounter(0);

    while (self.socketConnected)
    {
        if (ringBuffer.available() >= (TRANSMIT_CHUNKSIZE))
        {
            rc = recv(
                    self.sock,
                    ringBuffer.get_write_byte(),
                    (TRANSMIT_CHUNKSIZE),
                    0
                );
            if (rc < 0)
            {
                DEBUG_ERR("recv rc == " << rc << '\n');
                self.socketConnected = false;
                break;
            }

            ringBuffer.report_written_bytes(TRANSMIT_CHUNKSIZE);
        }
        delay_ticks_count(&delayCounter, 125, 1);
    }

    DEBUG_OUT("Closing socket...\n");

    rc = close(self.sock);

    DEBUG_OUT("close rc: " << rc << '\n');
    DEBUG_OUT("Socket closed\n");

    DEBUG_OUT("Exiting socket_client_tcp\n");
}

void socket_client_udp(void)
{
    DEBUG_OUT("Starting socket_client_udp...\n");

    const char* tx_buffer = "hello from socket_client_udp";
    int delayCounter(0);
    int rc;

    uint8_t rx_buffer[128];

    while (true)
    {

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(TRANSMITTER_IPV4_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(CONFIG_PORT);

        DEBUG_OUT("Creating socket...\n");

        self.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (self.sock < 0)
        {
            DEBUG_ERR("Unable to create socket\n");
            break;
        }

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        setsockopt(
                self.sock,
                SOL_SOCKET,
                SO_RCVTIMEO,
                &timeout,
                sizeof(timeout)
            );

        DEBUG_OUT("Socket created, sending to ");
        DEBUG_OUT(TRANSMITTER_IPV4_ADDR << ":" << CONFIG_PORT << '\n');

        while (true)
        {
            rc = sendto(
                    self.sock,
                    tx_buffer,
                    strlen(tx_buffer),
                    0,
                    reinterpret_cast<sockaddr*>(&dest_addr),
                    sizeof(dest_addr)
                );
                
            if (rc < 0)
            {
                DEBUG_ERR("Error occurred during sending\n");
                break;
            }
            DEBUG_OUT("Message sent\n");

            struct sockaddr_storage source_addr;
            socklen_t socklen = sizeof(source_addr);
            int length = recvfrom(
                    self.sock,
                    rx_buffer,
                    sizeof(rx_buffer) - 1,
                    0,
                    reinterpret_cast<sockaddr*>(&source_addr),
                    &socklen
                );

            if (length < 0)
            {
                DEBUG_ERR("recvfrom failed\n");
                break;
            }
            else
            {
                rx_buffer[length] = 0;
                DEBUG_OUT("Data received of length " << length);
                DEBUG_OUT(" from " << TRANSMITTER_IPV4_ADDR);
                DEBUG_OUT("\n\t" << rx_buffer << '\n');
            }

            delay_ticks_count(&delayCounter, 125, 1);
        }

        if (self.sock != -1)
        {
            DEBUG_ERR("Shutting down socket and restarting...\n");
            shutdown(self.sock, 0);
            close(self.sock);
        }
    }

    DEBUG_OUT("Exiting socket_client_udp\n");
}

extern "C" void app_main(void)
{
    DEBUG_OUT("Initializing WIFB...\n");

    /* Set buffer to have one reader initially */

    ringBuffer.set_num_readers(1);

    /* Configure button and set transmit mode */

    DEBUG_OUT("Configuring mode\n");
    button.set_hold_duration_ms(100);
    // Add a non-blocking while loop for button duration
    // txMode = button.read();
    DEBUG_OUT("Mode set to " << (txMode ? "transmit\n" : "receive\n"));
    
    /* Configure i2s */
    
    DEBUG_OUT("Configuring i2s...\n");
    i2s.set_pin_master_clock(I2S_MCK);
    i2s.set_pin_bit_clock(I2S_BCK);
    i2s.set_pin_word_select(I2S_WS);
    i2s.set_pin_data_out(I2S_DO);
    i2s.set_pin_data_in(I2S_DI);
    i2s.set_channels(NUM_CHANNELS);
    i2s.set_bit_depth(BITS_PER_SAMPLE);
    i2s.set_sample_rate(SAMPLE_RATE);
    i2s.set_buffer_length(
            ringBuffer.buffer_length(),
            ringBuffer.ring_length()
        );
    i2s.set_auto_clear(true);
    i2s.start();
    DEBUG_OUT("i2s configuration complete\n");
    
    /* Configure networking */

    DEBUG_OUT("Configuring networking...\n");

    int rc;
    if (txMode)
    {
        /* Enable soft AP mode for transmitter */
        rc = config_ap();

        // osc.set_sample_rate(SAMPLE_RATE);
        // osc.set_frequency(1000);
        // osc.scale = 0.125;
    }
    else
    {
        /* Enable STA mode for client
        to connect to transmitter AP */
        rc = config_sta();
    }
    if (rc)
    {
        DEBUG_ERR("Errors encountered\n");
        DEBUG_ERR("WIFB initialization failed\n");
        DEBUG_ERR("Rebooting...\n");
        esp_restart();
    }

    DEBUG_OUT("Networking configured\n");
    DEBUG_OUT("WIFB initialized\n");

    if (txMode)
    {
        DEBUG_OUT("Launching i2s_to_buffer_loop...\n");
        std::thread loop(i2s_to_buffer_loop);

        socket_server_tcp();
        // socket_server_udp();
    }
    else
    {
        DEBUG_OUT("Launching buffer_to_i2s_loop...\n");
        std::thread loop(buffer_to_i2s_loop);

        // socket_client_udp();
        while (true)
        {
            socket_client_tcp();

            /* Flush buffer when socket closes */
            DEBUG_ERR("Disconnected; flushing buffer...\n");
            ringBuffer.fill(0);
        }
    }
}
