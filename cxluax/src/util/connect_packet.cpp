#include <cstring>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

#include "connect_packet.hpp"

#define ASIO_KCP_CONNECT_PACKET "asio_kcp_connect_package get_conv"
#define ASIO_KCP_SEND_BACK_CONV_PACKET "asio_kcp_connect_back_package get_conv:"
#define ASIO_KCP_DISCONNECT_PACKET "asio_kcp_disconnect_package"

namespace asio_kcp {

std::string making_connect_packet(void)
{
    return std::string(ASIO_KCP_CONNECT_PACKET, sizeof(ASIO_KCP_CONNECT_PACKET));
}

bool is_connect_packet(const char* data, size_t len)
{
    return (len == sizeof(ASIO_KCP_CONNECT_PACKET) &&
        memcmp(data, ASIO_KCP_CONNECT_PACKET, sizeof(ASIO_KCP_CONNECT_PACKET) - 1) == 0);
}

bool is_send_back_conv_packet(const char* data, size_t len)
{
    return (len > sizeof(ASIO_KCP_SEND_BACK_CONV_PACKET) &&
        memcmp(data, ASIO_KCP_SEND_BACK_CONV_PACKET, sizeof(ASIO_KCP_SEND_BACK_CONV_PACKET) - 1) == 0);
}

std::string making_send_back_conv_packet(uint32_t conv)
{
    char str_send_back_conv[256] = "";
    size_t n = snprintf(str_send_back_conv, sizeof(str_send_back_conv), "%s %u", ASIO_KCP_SEND_BACK_CONV_PACKET, conv);
    return std::string(str_send_back_conv, n);
}

uint32_t grab_conv_from_send_back_conv_packet(const char* data, size_t len)
{
    uint32_t conv = atol(data + sizeof(ASIO_KCP_SEND_BACK_CONV_PACKET));
    return conv;
}




std::string making_disconnect_packet(uint32_t conv)
{
    char str_disconnect_packet[256] = "";
    size_t n = snprintf(str_disconnect_packet, sizeof(str_disconnect_packet), "%s %u", ASIO_KCP_DISCONNECT_PACKET, conv);
    return std::string(str_disconnect_packet, n);
}

bool is_disconnect_packet(const char* data, size_t len)
{
    return (len > sizeof(ASIO_KCP_DISCONNECT_PACKET) &&
        memcmp(data, ASIO_KCP_DISCONNECT_PACKET, sizeof(ASIO_KCP_DISCONNECT_PACKET) - 1) == 0);
}

uint32_t grab_conv_from_disconnect_packet(const char* data, size_t len)
{
    uint32_t conv = atol(data + sizeof(ASIO_KCP_DISCONNECT_PACKET));
    return conv;
}


} // namespace asio_kcp
