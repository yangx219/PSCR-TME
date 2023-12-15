#include "TCPServer.h"
#include <sys/select.h>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <dirent.h>
#include <fstream>
//#include "JobHandler.h"

 namespace pr {
     void listFiles() {}
     void uploadFile() {}
     void downloadFile() {}

     void TCPServer::handleClient(Socket sc) {
         // discussion client : read puis write.
         int lu ;
         read(sc.getFD(),&lu,sizeof(lu));
         ++lu;
         write(sc.getFD(),&lu,sizeof(lu));
         sc.close();
         }
     void TCPServer::handleClient_ftp(const std::string directory,Socket sc) {
         const int bufferSize = 1024;
         char buffer[bufferSize];
         ssize_t bytesReceived = read(sc.getFD(), buffer, bufferSize - 1);
         //erreur
         if (bytesReceived <= 0) {
             return;
         }
         buffer[bytesReceived] = '\0';
         if (strcmp(buffer, "LIST") == 0) {
             listFiles();
         } else if (strcmp(buffer, "UPLOAD") == 0) {
             uploadFile();
         } else if (strcmp(buffer, "DOWNLOAD") == 0) {
             downloadFile();
         }
         sc.close();
     }

     bool TCPServer::startServer0(int port,const std::string directory) {
         ss = new ServerSocket(port);
         if (ss->isOpen()) {
             while (1) {
                 std::cout << "En attente sur accept" << std::endl;
                 Socket sc = ss->accept();
                 if (sc.isOpen()) {
                     handleClient_ftp(directory,sc);
                     }
            }
             return true;
             }
         return false;
         }
//     bool TCPServer::startServer0(int port) {
//         ss = new ServerSocket(port);
//         if (ss->isOpen()) {
//             while (1) {
//                 std::cout << "En attente sur accept" << std::endl;
//                 Socket sc = ss->accept();
//                 if (sc.isOpen()) {
//                     handleClient(sc);
//                 }
//             }
//             return true;
//         }
//         return false;
//     }

     // version multi thread : un par client
     bool TCPServer::startServer1(int port) {
         ss = new ServerSocket(port);
         if (ss->isOpen()) {
             while (1) {
                 std::cout << "En attente sur accept" << std::endl;
                 Socket sc = ss->accept();
                 if (sc.isOpen()) {
                     threads.emplace_back(&TCPServer::handleClient,this,sc);
                     }
                 }
             return true;
             }
         return false;
         }

	void TCPServer::stopServer() {
		if (ss != nullptr) {
			ss->close();
			
		}

		// Wait for all threads to finish their current job and exit.
		for (std::thread& thread : threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
		delete ss;
		ss = nullptr;
	}




     TCPServer::~TCPServer() {
         for (auto & t: threads) {
             t.join();
             }
         }

     }