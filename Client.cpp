#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class FileClient {
public:
    FileClient(const char* serverIp, int serverPort) : serverIp(serverIp), serverPort(serverPort) {}

    void requestFile(const char* fileName) {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            perror("Error creating socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in serverAddr;
        memset((char*)&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIp, &(serverAddr.sin_addr)) <= 0) {
            perror("Invalid address/Address not supported");
            exit(EXIT_FAILURE);
        }

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error connecting to server");
            exit(EXIT_FAILURE);
        }

        if (send(clientSocket, fileName, strlen(fileName), 0) == -1) {
            perror("Error sending file name");
            exit(EXIT_FAILURE);
        }

        receiveFile(clientSocket, fileName);

        close(clientSocket);
    }

private:
    const char* serverIp;
    int serverPort;

    void receiveFile(int clientSocket, const char* fileName) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        std::ofstream file(fileName, std::ios::binary);
        if (!file.is_open()) {
            perror("Error creating file");
            return;
        }

        while (true) {
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead == -1) {
                perror("Error receiving file content");
                break;
            }

            if (bytesRead == 0) {
                break;
            }

            file.write(buffer, bytesRead);
        }

        std::cout << "File received: " << fileName << std::endl;

        file.close();
    }
};

int main() {
    const char* SERVER_IP = "127.0.0.1";
    const int SERVER_PORT = 12345;

    FileClient fileClient(SERVER_IP, SERVER_PORT);
    fileClient.requestFile("example.txt");

    return 0;
}
