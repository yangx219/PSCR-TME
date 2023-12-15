#include "Socket.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

 namespace pr {
    void Socket::close() {
         if (fd != -1) {
             shutdown(fd,1);
             ::close(fd);
             }
    }

     void Socket::connect(const std::string & host, int port) {
         struct addrinfo * addr;
          if (getaddrinfo(host.c_str(), /* service*/ nullptr, /* hints*/ nullptr, &addr) !=
                0) {
              perror("getaddrinfo");
              return;
              }
          in_addr ipv4 = ((struct sockaddr_in *) addr->ai_addr)->sin_addr;
         freeaddrinfo(addr);
        connect(ipv4, port);
        }

 void Socket::connect(in_addr ipv4, int port) {

    sockaddr_in dest;
     dest.sin_family = AF_INET;
             dest.sin_addr = ipv4;
     dest.sin_port = htons(port); // host to network

     fd = socket(AF_INET,SOCK_STREAM, /* protocole TCP par defaut sur SOCK_STREAM*/ 0);
     if (fd <0) {
         perror("socket");
         return;
         }
     if (::connect(fd,(struct sockaddr *) &dest, sizeof dest) <0) {
         perror("connect");
         ::close(fd);
         fd=-1;
         return;
        }
     }

 std::ostream & operator<< (std::ostream & os, struct sockaddr_in * addr) {
     char hname [1024];
     if (getnameinfo((struct sockaddr *)addr,sizeof *addr, hname, 1024, nullptr, 0, 0)
           == 0) {
         os << '"' << hname << '"' << " ";
        }
     os << inet_ntoa(addr->sin_addr) << ":" << ntohs(addr->sin_port) << std::endl;
     return os;
     }
}