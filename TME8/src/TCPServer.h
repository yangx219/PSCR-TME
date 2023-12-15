#ifndef SRC_TCPSERVER_H_
#define SRC_TCPSERVER_H_

#include <thread>
#include "ServerSocket.h"
#include "ConnectionHandler.h"

namespace pr {

// un serveur TCP, la gestion des connections est déléguée
class TCPServer {
	ServerSocket * ss; // la socket d'attente si elle est instanciee
	ConnectionHandler * handler; // le gestionnaire de session passe a la constru
    std::vector<std::thread> threads;
	// a completer
public :
	TCPServer(ConnectionHandler * handler): ss(nullptr),handler(handler) {}
    TCPServer(): ss(nullptr) {}
    void handleClient(Socket scom);
    void handleClient_ftp(const std::string directory,Socket scom);
	// Tente de creer une socket d'attente sur le port donné
	bool startServer0 (int port,const std::string directory);
    bool startServer1 (int port);
	// stoppe le serveur
	void stopServer () ;
    ~TCPServer();
};

} // ns pr

#endif /* SRC_TCPSERVER_H_ */