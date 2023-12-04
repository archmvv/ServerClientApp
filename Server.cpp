#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

class FileServer {
public:
    FileServer(int port) : port(port) {}

	void start(){
		int serverSocket, clientSocket;
		struct sockaddr_in serverAddr, clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(serverSocket == -1){
		perror("Error creating the socket");
		exit(EXIT_FAILURE);
		}

	memset((char*)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
		perror("Error binding socket");
		exit(EXIT_FAILURE);
	}

	if(listen(serverSocket, 5) == -1) {
		perror("Error listening for connections");
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on port " << port << std:: endl;

	while(true){
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if(clientSocket == -1){
			perror("Error accepting connection");
			continue;
		}

	std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	handleClient(clientSocket);
	close(clientSocket);
	std::cout << "Connection closed\n";
	}

	close(serverSocket);
	}

private:
	int port;

	void handleClient(int clientSocket){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	if(recv(clientSocket, buffer, sizeof(buffer), 0) == -1){
		perror("Error receiving filename");
		return;
	}

	std::string fileName(buffer);

	std::ifstream file(fileName, std::ios::binary);
	if(!file.is_open()) {
		perror("Error opening file");
		return;
	}

	while(!file.eof()) {
	file.read(buffer, sizeof(buffer));
	if (send(clientSocket, buffer, file.gcount(), 0) == -1) {
		perror("Error sending file content");
		break;
	}
	}

	std::cout << "File sent:" << fileName << std::endl;

	file.close(); 
 }
};

int main(){
	const int PORT = 12345;
	FileServer fileServer(PORT);
	fileServer.start();

	return 0;
}




