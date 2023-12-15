#include "ServerSocket.h"
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace pr {

    ServerSocket::ServerSocket(int port) : socketfd(-1) {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            perror("create socket");
            throw std::runtime_error("Failed to create socket");
        }

        int yes = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            perror("setsockopt");
            ::close(fd);
            throw std::runtime_error("Failed to set socket options");
        }

        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sin.sin_port = htons(port);

        if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("bind");
            ::close(fd);
            throw std::runtime_error("Failed to bind");
        }

        if (listen(fd, 50) < 0) {
            perror("listen");
            ::close(fd);
            throw std::runtime_error("Failed to listen");
        }

        socketfd = fd;
    }

    Socket ServerSocket::accept() {
        struct sockaddr_in exp;
        socklen_t len = sizeof(exp);
        int scom = ::accept(socketfd, (struct sockaddr *)&exp, &len);
        if (scom < 0) {
            perror("accept");
        } else {
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &exp.sin_addr, ipstr, sizeof(ipstr));
            std::cout << "Accepted connection from " << ipstr << ":" << ntohs(exp.sin_port) << std::endl;
        }
        return scom;
    }

         
     void ServerSocket::close() {
         if (socketfd != -1) {
             ::close(socketfd);
             }
         }
    }