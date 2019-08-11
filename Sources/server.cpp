#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>

int main() {
    
    // Create socket
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    
    int listening = socket(AF_INET, SOCK_STREAM, 0);

    if(listening == -1) {
        std::cerr << "Socket can't be created!!";
        return -1;
    }

    // Bind socket to IPV4
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(5401);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't bind to IP/Port!!";
        return -2;
    }

    // Begin listening for connections.
    if(listen(listening, SOMAXCONN) == -1) {
        std::cerr << "Can't listen!!";
    }

    // Establish client socket and connect to listening socket.
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if(clientSocket == -1) {
       std::cerr << "Issue connecting Client 1!!";
       return -3;
    }

    // Close listening socket and clean up memory. Fill registry representation with zeros.
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int resultClient = getnameinfo((sockaddr*)&client, sizeof(clientSocket), 
                &host[NI_MAXHOST], NI_MAXHOST, &svc[NI_MAXSERV], NI_MAXSERV, 0);

    if(resultClient) {
        std::cout << host << " connected on " << svc << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, &host[NI_MAXHOST], NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port);
    }

    char buf[4096];
    while(true) {
        memset(buf, 0, 4096);
        int bytesRecv = recv(clientSocket, buf, 4096, 0);

        if(bytesRecv == -1) {
            std::cerr << "There was an issue receiving data from client." << std::endl;
            break;
        }

        if(bytesRecv == 0) {
            std::cout << "The client disconnected" << std::endl;
        }

        // Display Message.
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

        // Resend Message.
        send(clientSocket, buf, bytesRecv + 1, 0);

    }

    close(clientSocket);
}