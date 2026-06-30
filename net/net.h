#pragma once

#include <stdint.h>
#include <stdbool.h>

#define NET_MTU 1500
#define NET_ETH_TYPE_IP 0x0800
#define NET_ETH_TYPE_ARP 0x0806

typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t payload[NET_MTU];
    uint16_t payload_len;
} __attribute__((packed)) net_eth_frame_t;

typedef struct {
    uint8_t hw_len;
    uint8_t hw_type;
    uint16_t proto_type;
    uint8_t proto_len;
    uint16_t opcode;
    uint8_t src_mac[6];
    uint8_t src_ip[4];
    uint8_t dst_mac[6];
    uint8_t dst_ip[4];
} __attribute__((packed)) net_arp_packet_t;

typedef struct {
    uint8_t ver_ihl;
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t payload[NET_MTU - 20];
    uint16_t payload_len;
} __attribute__((packed)) net_ip_packet_t;

void net_init(void);
void net_send_eth(const uint8_t *dst_mac, uint16_t ethertype, const void *data, uint16_t len);
void net_handle_rx(const uint8_t *frame, uint16_t len);
void net_set_ip(const uint8_t *ip);
void net_get_mac(uint8_t *mac);