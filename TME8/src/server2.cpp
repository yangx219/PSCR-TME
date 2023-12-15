#include "TCPServer.h"
#include "Socket.h"
#include "ServerSocket.h"
#include <iostream>
#include <unistd.h>

//g++ -std=c++11 server2.cpp Socket.cpp ServerSocket.cpp TCPServer.cpp -o server2
//    ./server2

class IncrementServer : public pr::ConnectionHandler {

public :
	void handleConnection (pr::Socket sc) {
		int fd = sc.getFD();

		ssize_t msz = sizeof(int);
		while (1) {
			int lu;
			auto nblu = read(fd, &lu, msz);
			if (nblu == 0) {
				std::cout << "Fin connexion par client" << std::endl;
				break;
			} else if (nblu < msz) {
				perror("read");
				break;
			}
			std::cout << "lu =" << lu << std::endl;

			if (lu == 0) {
				break;
			}
			lu++;
			if (write(fd, &lu, msz) < msz) {
				perror("write");
				break;
			}
			std::cout << "envoyé =" << lu << std::endl;
		}
	}
	ConnectionHandler * clone() const {
		return new IncrementServer();
	}
};

int main() {

	pr::TCPServer server(new IncrementServer());

	server.startServer0(1667, "localhost"); // Add the second argument for the server address

	// attend entree sur la console
	std::string s ;
	std::cin >> s ;

	std::cout << "Début fin du serveur." << std::endl ;
	// on quit
	server.stopServer();
	std::cout << "Ok fin du serveur." << std::endl;

	return 0;
}
