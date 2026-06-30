#include "net.h"
#include "kernel/heap.h"

static uint8_t mac_addr[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static uint8_t ip_addr[4] = {192, 168, 1, 100};

void net_init(void) {
}

void net_send_eth(const uint8_t *dst_mac, uint16_t ethertype, const void *data, uint16_t len) {
    (void)dst_mac;
    (void)ethertype;
    (void)data;
    (void)len;
}

void net_handle_rx(const uint8_t *frame, uint16_t len) {
    (void)frame;
    (void)len;
}

void net_set_ip(const uint8_t *ip) {
    for (int i = 0; i < 4; i++)
        ip_addr[i] = ip[i];
}

void net_get_mac(uint8_t *mac) {
    for (int i = 0; i < 6; i++)
        mac[i] = mac_addr[i];
}