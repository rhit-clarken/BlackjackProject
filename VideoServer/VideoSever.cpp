#include <iostream>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;
using namespace cv;

#pragma comment(lib, "ws2_32.lib") // Winsock Library
#pragma warning(disable : 4996)

#define BUFLEN 64000
#define PORT 8888

class TCPServer {
public:
    TCPServer() {
        // initialise winsock
        cout << "Initialising Winsock...\n";
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
            exit(0);
        }
        cout << "Initialised.\n";

        // create a socket
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            cout << "Could not create socket: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Socket created.\n";

        // prepare the sockaddr_in structure
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PORT);

        // bind
        if (::bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
            cout << "Bind failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Bind done.\n";

        // listen for incoming connections
        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
            cout << "listen() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        cout << "Waiting for incoming connections...\n";
    }

    ~TCPServer() {
        closesocket(server_socket);
        WSACleanup();
    }

    void start() {
        while (true) {
            // accept connection from client
            SOCKET client_socket = accept(server_socket, NULL, NULL);
            if (client_socket == INVALID_SOCKET) {
                cout << "accept() failed with error code: " << WSAGetLastError() << "\n";
                continue;
            }
            cout << "Connection accepted.\n";

            // add the client to the list
            {
                lock_guard<mutex> lock(clients_mutex);
                clients.push_back(client_socket);
            }

            // create a new thread to handle the client
            thread client_thread(&TCPServer::handleClient, this, client_socket);
            client_thread.detach();
        }
    }

private:
    void handleClient(SOCKET client_socket) {
        receiveAndBroadcastVideo(client_socket);

        // remove the client from the list
        {
            lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
        }

        closesocket(client_socket);
    }

    void receiveAndBroadcastVideo(SOCKET client_socket) {
        string window_name = "Live Camera Feed - Client " + to_string(client_socket);
        cv::namedWindow(window_name, cv::WINDOW_NORMAL);

        while (true) {
            int frameSize;
            if (recv(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
                cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
                break;
            }

            if (frameSize == 0) {
                cout << "Client disconnected.\n";
                break;
            }

            // Allocate buffer for the frame data
            std::vector<uchar> buffer(frameSize);

            // Receive the frame data
            int bytesReceived = 0;
            while (bytesReceived < frameSize) {
                int bytesRemaining = frameSize - bytesReceived;
                int bytesToReceive = min(bytesRemaining, BUFLEN);
                int bytesRead = recv(client_socket, (char*)buffer.data() + bytesReceived, bytesToReceive, 0);
                if (bytesRead == SOCKET_ERROR) {
                    cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
                    break;
                }
                if (bytesRead == 0) {
                    cout << "Client disconnected.\n";
                    break;
                }
                bytesReceived += bytesRead;
            }

            // Decode the frame
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

            if (frame.empty()) {
                std::cout << "Failed to decode frame" << std::endl;
                continue;
            }

            // Display the frame
            cv::imshow(window_name, frame);
            if (cv::waitKey(1) == 27) {
                break;
            }

            // Broadcast the frame to all other clients
            broadcastFrame(client_socket, buffer);
        }

        cv::destroyWindow(window_name);
    }

    void broadcastFrame(SOCKET sender_socket, const vector<uchar>& buffer) {
        lock_guard<mutex> lock(clients_mutex);

        int frameSize = buffer.size();
        for (const auto& client_socket : clients) {
            if (client_socket != sender_socket) {
                if (send(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
                    cout << "send() failed with error code: " << WSAGetLastError() << "\n";
                    continue;
                }

                int bytesSent = 0;
                while (bytesSent < frameSize) {
                    int bytesRemaining = frameSize - bytesSent;
                    int bytesToSend = min(bytesRemaining, BUFLEN);
                    if (send(client_socket, (char*)buffer.data() + bytesSent, bytesToSend, 0) == SOCKET_ERROR) {
                        cout << "send() failed with error code: " << WSAGetLastError() << "\n";
                        break;
                    }
                    bytesSent += bytesToSend;
                }
            }
        }
    }

    WSADATA wsa{};
    SOCKET server_socket = 0;
    sockaddr_in server{};
    vector<SOCKET> clients;
    mutex clients_mutex;
};

int main() {
    system("title TCP Server");
    MoveWindow(GetConsoleWindow(), 650, 50, 500, 500, true);

    TCPServer tcpServer;
    tcpServer.start();
}