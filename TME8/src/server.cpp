#include "ServerSocket.h"
#include <iostream>
#include <unistd.h>
//g++ -std=c++11 server.cpp Socket.cpp ServerSocket.cpp -o server
//    ./server

int main00() {
	pr::ServerSocket ss(1664);

	while (1) {
		pr::Socket sc = ss.accept();

		int fd = sc.getFD();

		int lu;
		read(fd, &lu, sizeof(int));
		std::cout << "lu =" << lu << std::endl;
		lu++;
		write(fd, &lu, sizeof(int));
		sc.close();
	}
	ss.close();
	return 0;
}

int main() {
    pr::ServerSocket ss(1665);

    if (!ss.isOpen()) {
        std::cerr << "ServerSocket failed to open." << std::endl;
        return 1;
    }

    while (1) {
		//accepter une connexion
        pr::Socket sc = ss.accept();

        if (!sc.isOpen()) {
            std::cerr << "Failed to accept connection: " << std::strerror(errno) << std::endl;
            // Optionally continue to accept the next connection instead of exiting.
            continue;
        }

        int fd = sc.getFD();
        ssize_t msz = sizeof(int);

        while (1) {
            int lu;
            auto nblu = read(fd, &lu, msz);
            if (nblu == 0) {
                std::cout << "Fin connexion par client" << std::endl;
                break;
            } else if (nblu < 0) {
                std::perror("read");
                break;
            }
            std::cout << "lu =" << lu << std::endl;

            if (lu == 0) {
                break;
            }
            lu++;
            if (write(fd, &lu, msz) < 0) {
                std::perror("write");
                break;
            }
            std::cout << "envoyé =" << lu << std::endl;
        }
        sc.close();
    }

    ss.close();
    return 0;
}