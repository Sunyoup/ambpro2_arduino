#include "ard_socket.h"

#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <platform_stdlib.h>
#include <platform_opts.h>

#define MAX_RECV_SIZE 1500
#define MAX_SEND_SIZE 256
#define UDP_SERVER_PORT 5002
#define TCP_SERVER_PORT 5003

//static int EXAMPLE_IPV6 = 0;

int start_client(uint32_t ipAddress, uint16_t port, uint8_t protMode) {
    int enable = 1;
    int timeout;
    int _sock;

    //create socket
    if (protMode == 0) {  // TCP
        _sock = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    } else {
        _sock = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    if (_sock < 0) {
        printf("\r\n [ERROR] Create socket failed\n");
        return -1;
    }
    printf("\r\n [INFO] Create socket successfully\n");

    // initialize structure dest
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ipAddress;
    serv_addr.sin_port = htons(port);

    //Connecting to server
    if (protMode == 0) {  //TCP MODE
        if (connect(_sock, ((struct sockaddr *)&serv_addr), sizeof(serv_addr)) == 0) {
            printf("\r\n [INFO] Connect to Server successfully!\r\n");
            timeout = 3000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            timeout = 30000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            lwip_setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
            lwip_setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
            return _sock;
        } else {
            printf("\r\n [ERROR] Connect to server failed\n");
            close_socket(_sock);
            return -1;
        }
    } else {
        //printf("\r\n Udp client setup Server's information successful!\r\n");
    }
    return _sock;
}


#if 0
int start_clientv6(uint32_t *ipv6Address, uint16_t port, uint8_t protMode) {
    int enable = 1;
    int timeout;
    int _sock;

    //create socket
    if (protMode == 0) {  // TCP
        _sock = lwip_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    } else {
        _sock = lwip_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    }
    if (_sock < 0) {
        printf("\r\n [ERROR] Create socket failed\n");
        return -1;
    }
    printf("\r\n [INFO] Create socket successfully\n");

    // initialize structure dest
    struct sockaddr_in6 serv_addr6;
    memset(&serv_addr6, 0, sizeof(serv_addr6));
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_port = htons(port);
    //inet_pton(AF_INET6, TCP_SERVER_IP_TEST, &(serv_addr6.sin6_addr));
    for (int xxx = 0; xxx < 4; xxx++) {  // IPv6 address
        serv_addr6.sin6_addr.un.u32_addr[xxx] = ipv6Address[xxx];
    }

    // connection starts
    if (protMode == 0) {  //TCP MODE
        if (connect(_sock, (struct sockaddr *)(&serv_addr6), sizeof(serv_addr6)) == -1) {
            printf("\r\n [ERROR] Connect to server failed\n");
        }
        printf("[INFO] Connect to server successfully\n");

        if (connect(_sock, (struct sockaddr *)(&serv_addr6), sizeof(serv_addr6)) == 0) {
            printf("\r\n [INFO] Connect to Server successfully!\r\n");
            timeout = 3000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            timeout = 30000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            lwip_setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
            lwip_setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
            return _sock;
        } else {
            printf("\r\n [ERROR] Connect to Server failed!\r\n");
            close_socket(_sock);
            return -1;
        }
    } else {
        //printf("\r\n Udp client setup Server's information successful!\r\n");
    }

    return _sock;
}

int start_client_v6(char ipv6Address[], uint16_t port, uint8_t protMode) {
    printf("\r\n [INFO]ard_socket.cpp  start_client_v6\n");
    int enable = 1;
    int timeout;
    int _sock;
    struct sockaddr_in6 ser_addr;

    // create socket
    if (protMode == 0) {  // TCP
        _sock = lwip_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    } else {
        _sock = lwip_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    }
    if (_sock < 0) {
        printf("\r\n [ERROR] Create socket failed\n");
        return -1;
    }
    printf("\r\n [INFO] Create socket successfully\n");

    // initialize value in dest
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin6_family = AF_INET6;
    // if (protMode == 0) {  // TCP
    // ser_addr.sin6_family = AF_INET6;
    ser_addr.sin6_port = htons(port);
    //---------------------------------------
    inet_pton(AF_INET6, ipv6Address, &(ser_addr.sin6_addr));
    // }

    // Connecting to server
    if (protMode == 0) {  //TCP MODE
        if (connect(_sock, ((struct sockaddr *)&ser_addr), sizeof(ser_addr)) == 0) {
            printf("\r\n [INFO] Connect to server successfully\n");
            timeout = 3000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            timeout = 30000;
            lwip_setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            lwip_setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
            lwip_setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
            return _sock;
        } else {
            printf("\r\n [ERROR] Connect to server failed\n");
            close_socket(_sock);
            return -1;
        }
    }
    // else {  // UDP
    // printf("\r\n [INFO] UDP client setup Server's information successful!\n");
    // }

    return _sock;
}
#endif

int set_nonblocking(int fd) {
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        return -1;
    }
    return 0;
}

int start_server(uint16_t port, uint8_t protMode) {
    int _sock;
    int timeout;
    //create socket
    if (protMode == 0) {
        timeout = 3000;
        _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        //  printf("\r\n [INFO] Create TCP socket successfudlly\n");
    } else {
        timeout = 1000;
        _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        // printf("\r\n [INFO] Create UDP socket successfully\n");
    }

    if (_sock < 0) {
        printf("\r\n ERROR opening socket\r\n");
        return -1;
    }

    // initialize structure dest
    struct sockaddr_in localHost;
    memset(&localHost, 0, sizeof(localHost));

    localHost.sin_family = AF_INET;
    localHost.sin_port = htons(port);
    localHost.sin_addr.s_addr = INADDR_ANY;

    // Assign a port number to socket
    if (bind(_sock, ((struct sockaddr *)&localHost), sizeof(localHost)) < 0) {
        printf("\r\n ERROR on binding\r\n");
        return -1;
    }
    //lwip_fcntl(_sock, F_SETFL, O_NONBLOCK);
    return _sock;
}

#if 0
int start_server_v6(uint16_t port, uint8_t protMode) {
    int _sock;
    int timeout;

    //create socket
    if (protMode == 0) {  // TCP
        timeout = 3000;
        _sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        printf("\r\n [INFO] Create TCP socket successfully\n");
    } else {  // UDP
        //timeout = 1000;
        _sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        //setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        printf("\r\n [INFO] Create UDP socket successfully\n");
    }

    if (_sock < 0) {
        printf("\r\n [ERROR] Create socket failed\n");
        return -1;
    }

    // initialize structure dest
    struct sockaddr_in6 localHost;
    memset(&localHost, 0, sizeof(localHost));
    localHost.sin6_family = AF_INET6;
    localHost.sin6_port = htons(port);
    localHost.sin6_addr = (struct in6_addr)IN6ADDR_ANY_INIT;

    // Assign a port number to socket
    if (bind(_sock, (struct sockaddr *)&localHost, sizeof(localHost)) < 0) {
        printf("\r\n [ERROR] Bind socket failed\n");
        closesocket(_sock);
        return -1;
    }
    //lwip_fcntl(_sock, F_SETFL, O_NONBLOCK);
    printf("\r\n [INFO] Bind socket successfully\n");
   
    return _sock;
}
#endif

int get_sock_errno(int sock) {
// https://www.nongnu.org/lwip/2_1_x/upgrading.html
// socket API: according to the standard, SO_ERROR now only returns asynchronous errors.
// All other/normal/synchronous errors are (and always were) available via 'errno'.

//    int so_error;
//    socklen_t len = sizeof(so_error);
//    lwip_getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
//    return so_error;
    (void)sock;
    return errno;
}


int set_sock_recv_timeout(int sock, int timeout) {
    return lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


void close_socket(int sock) {
    lwip_close(sock);
}

#if 0
int enable_ipv6(void) {
    EXAMPLE_IPV6 = 1;

    return EXAMPLE_IPV6;
}

int get_ipv6_status(void) {
    // return current ipv6 enabled status
    return EXAMPLE_IPV6;
}
#endif

// TCP
int sock_listen(int sock, int max) {
    if (listen(sock, max) < 0) {
        // printf("\r\n ERROR on listening\r\n");
        printf("\r\n [ERROR] Listen socket failed, socket closed\n");
        close_socket(sock);
        return -1;
    }
    printf("\r\n [INFO] Listen socket successfully\n");
    return 0;
}

int get_available(int sock) {
    int enable = 1;
    int timeout;
    int client_fd;
    int err;
    struct sockaddr_in cli_addr;

    socklen_t client = sizeof(cli_addr);

    do {
        client_fd = lwip_accept(sock, ((struct sockaddr *)&cli_addr), &client);
        if (client_fd < 0) {
            err = get_sock_errno(sock);
            if (err != EAGAIN) {
                break;
            }
            // Get current socket status and break if it is in non blocking mode
            if (fcntl(sock, F_GETFL, 0) & O_NONBLOCK) {
                break;
            }
        }
    } while (client_fd < 0);

    if (client_fd < 0) {
        printf("\r\n [ERROR] Accept connection failed\n");
        return -1;
    } else {
        timeout = 3000;
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        timeout = 30000;
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
        printf("\r\n [INFO] Accept connection successfully\n");
        printf("\r\n A client connected to this server :\r\n[PORT]: %d\r\n[IP]:%s\r\n\r\n", ntohs(cli_addr.sin_port), inet_ntoa(cli_addr.sin_addr.s_addr));
        return client_fd;
    }
}

#if 0
int get_available_v6(int sock) {
    int enable = 1;
    int timeout;
    int client_fd;
    int err;
    struct sockaddr_in6 cli_addr;

    socklen_t client = sizeof(struct sockaddr_in6);

    do {
        client_fd = lwip_accept(sock, ((struct sockaddr *)&cli_addr), &client);
        if (client_fd < 0) {
            err = get_sock_errno(sock);
            if (err != EAGAIN) {
                break;
            }
        }
    } while (client_fd < 0);

    printf("get_available_v6 client_fd: %d\r\n", client_fd);

    if (client_fd < 0) {
        printf("\r\n [ERROR] Accept connection failed\n");
        return -1;
    } else {
        timeout = 3000;
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        timeout = 30000;
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
        lwip_setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
        printf("\r\n [INFO] Accept connection successfully\n");
        //printf("\r\n [INFO] A client connected to this server :\r\n[PORT]: %d\r\n[IPv6]:%s\r\n\r \n", ntohl(cli_addr.sin6_port), inet6_ntoa(cli_addr.sin6_addr.un.u32_addr[4]));
        return client_fd;
    }
}
#endif

int recv_data(int sock, const uint8_t *data, uint32_t len, int flag) {
    int ret;

    ret = lwip_recv(sock, (void *)data, len, flag);

    return ret;
}

int send_data(int sock, const uint8_t *data, uint32_t len, int flag) {
    int ret;
    //printf("[info] ard_socket.c send_data()\r\n");
    ret = lwip_send(sock, data, len, flag);

    return ret;
}

// UDP

int sendto_data(int sock, const uint8_t *data, uint32_t len, uint32_t peer_ip, uint16_t peer_port) {
    int ret;
    struct sockaddr_in peer_addr;

    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = peer_ip;
    peer_addr.sin_port = htons(peer_port);

    ret = lwip_sendto(sock, data, len, 0, ((struct sockaddr *)&peer_addr), sizeof(struct sockaddr_in));

    return ret;
}

#if 0
int sendto_data_v6(int sock, const void *send_data, size_t len, uint32_t peer_ip, uint16_t peer_port) {
    int ret = 0;
    struct sockaddr_in6 peer_addr;

    peer_ip = peer_ip;
    peer_port = peer_port;

    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin6_family = AF_INET6;
    peer_addr.sin6_port = htons(UDP_SERVER_PORT);
    inet_pton(AF_INET6, (char*)peer_ip, &(peer_addr.sin6_addr));

    ret = lwip_sendto(sock, send_data, len, 0, ((struct sockaddr *)&peer_addr), sizeof(peer_addr));

    return ret;
}
#endif

int get_receive(int sock, uint8_t *data, int length, int flag, uint32_t *peer_addr, uint16_t *peer_port) {
    int ret = 0;
    struct sockaddr from;
    socklen_t fromlen;

    uint8_t backup_recvtimeout = 0;
    int backup_recv_timeout, recv_timeout;
    socklen_t len;

    if (flag & 0x01) {
        // for MSG_PEEK, we try to peek packets by changing receiving timeout to 10ms
        ret = lwip_getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &backup_recv_timeout, &len);
        if (ret >= 0) {
            recv_timeout = 10;
            ret = lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
            if (ret >= 0) {
                backup_recvtimeout = 1;
            }
        }
    }
    ret = lwip_recvfrom(sock, data, length, flag, &from, &fromlen);

    if (ret >= 0) {
        if (peer_addr != NULL) {
            *peer_addr = ((struct sockaddr_in *)&from)->sin_addr.s_addr;
        }
        if (peer_port != NULL) {
            *peer_port = ntohs(((struct sockaddr_in *)&from)->sin_port);
        }
    }

    if ((flag & 0x01) && (backup_recvtimeout == 1)) {
        // restore receiving timeout
        lwip_setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &backup_recv_timeout, sizeof(recv_timeout));
    }

    return ret;
}

#if 0
int get_receive_v6(int sock, void *recv_data, int len, int flags, uint32_t *peer_ip, uint16_t *peer_port) {
    int ret = 0;
    struct sockaddr_in6 peer_addr;
    unsigned int peer_len = sizeof(struct sockaddr_in6);

    peer_ip = peer_ip;
    peer_port = peer_port;

    ret = lwip_recvfrom(sock, recv_data, len, flags, ((struct sockaddr *)&peer_addr), &peer_len);
    return ret;
}


void ipv6_udp_server(void) {
    int server_fd;
    struct sockaddr_in6 client_addr;

    unsigned int addrlen = sizeof(struct sockaddr_in6);

    char send_data[MAX_SEND_SIZE] = "Hi client!";
    char recv_data[MAX_RECV_SIZE];

    //create socket

    server_fd = start_server_v6(UDP_SERVER_PORT, 1);

    while (1) {
        memset(recv_data, 0, MAX_RECV_SIZE);
        // if (get_receive_v6(server_fd, recv_data, MAX_SEND_SIZE, 0, UDP_SERVER_IP, UDP_SERVER_PORT) <= 0) {
        if (lwip_recvfrom(server_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen) > 0) {
            printf("\r\n [INFO] Receive data : %s\n", recv_data);
            //Send Response
            if (lwip_sendto(server_fd, send_data, MAX_SEND_SIZE, 0, (struct sockaddr *)&client_addr, addrlen) == -1) {
                printf("\r\n [ERROR] Send data failed\n");
            } else {
                printf("\r\n [INFO] Send data successfully\n");
            }
        }
    }

    closesocket(server_fd);

    return;
}
#endif
