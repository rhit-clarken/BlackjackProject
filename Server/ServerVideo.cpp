//#include <iostream>
//#include <winsock2.h>
//#include <opencv2/opencv.hpp>
//#include <thread>
//#include <vector>
//#include <unordered_map>
//
//using namespace std;
//using namespace cv;
//
//#pragma comment(lib, "ws2_32.lib") // Winsock Library
//#pragma warning(disable : 4996)
//
//#define BUFLEN 64000
//#define PORT 8888
//
//struct ClientData {
//    SOCKET socket;
//    thread receiveThread;
//};
//
//class TCPServer {
//public:
//    TCPServer() {
//        // initialise winsock
//        cout << "Initialising Winsock...\n";
//        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
//            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
//            exit(0);
//        }
//        cout << "Initialised.\n";
//
//        // create a socket
//        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
//            cout << "Could not create socket: " << WSAGetLastError() << "\n";
//            exit(EXIT_FAILURE);
//        }
//        cout << "Socket created.\n";
//
//        // prepare the sockaddr_in structure
//        server.sin_family = AF_INET;
//        server.sin_addr.s_addr = INADDR_ANY;
//        server.sin_port = htons(PORT);
//
//        // bind
//        if (::bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
//            cout << "Bind failed with error code: " << WSAGetLastError() << "\n";
//            exit(EXIT_FAILURE);
//        }
//        cout << "Bind done.\n";
//
//        // listen for incoming connections
//        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
//            cout << "listen() failed with error code: " << WSAGetLastError() << "\n";
//            exit(EXIT_FAILURE);
//        }
//
//        cout << "Waiting for incoming connections...\n";
//    }
//
//    ~TCPServer() {
//        // Close all client sockets
//        for (const auto& entry : clientFrames) {
//            closesocket(entry.first);
//        }
//        closesocket(server_socket);
//        WSACleanup();
//    }
//
//    void start() {
//        while (true) {
//            // accept connection from client
//            SOCKET client_socket = accept(server_socket, NULL, NULL);
//            if (client_socket == INVALID_SOCKET) {
//                cout << "accept() failed with error code: " << WSAGetLastError() << "\n";
//                continue;
//            }
//            cout << "Connection accepted.\n";
//
//            // create a new thread to handle receiving video from the client
//            thread receiveThread(&TCPServer::receiveVideo, this, client_socket);
//            receiveThread.detach();
//        }
//    }
//
//private:
//    void receiveVideo(SOCKET client_socket) {
//        while (true) {
//            int frameSize;
//            if (recv(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
//                cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
//                break;
//            }
//
//            if (frameSize == 0) {
//                cout << "Client disconnected.\n";
//                break;
//            }
//
//            // Allocate buffer for the frame data
//            std::vector<uchar> buffer(frameSize);
//
//            // Receive the frame data
//            int bytesReceived = 0;
//            while (bytesReceived < frameSize) {
//                int bytesRemaining = frameSize - bytesReceived;
//                int bytesToReceive = min(bytesRemaining, BUFLEN);
//                int bytesRead = recv(client_socket, (char*)buffer.data() + bytesReceived, bytesToReceive, 0);
//                if (bytesRead == SOCKET_ERROR) {
//                    cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
//                    break;
//                }
//                if (bytesRead == 0) {
//                    cout << "Client disconnected.\n";
//                    break;
//                }
//                bytesReceived += bytesRead;
//            }
//
//            // Decode the frame
//            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
//
//            if (frame.empty()) {
//                std::cout << "Failed to decode frame" << std::endl;
//                continue;
//            }
//
//            // Store the frame for sending to other clients
//            {
//                lock_guard<mutex> lock(mtx);
//                clientFrames[client_socket] = frame;
//            }
//
//            // Send the collage frame to all clients
//            sendCollageFrame();
//        }
//
//        // Remove the client's frame from the map when the client disconnects
//        {
//            lock_guard<mutex> lock(mtx);
//            clientFrames.erase(client_socket);
//        }
//        closesocket(client_socket);
//    }
//
//    void sendCollageFrame() {
//        // Create a collage of frames from all clients
//        cv::Mat collage;
//
//        {
//            lock_guard<mutex> lock(mtx);
//
//            int numClients = clientFrames.size();
//            if (numClients == 0)
//                return;
//
//            int rows = ceil(sqrt(numClients));
//            int cols = ceil(static_cast<double>(numClients) / rows);
//
//            int maxWidth = 0;
//            int maxHeight = 0;
//            for (const auto& entry : clientFrames) {
//                maxWidth = max(maxWidth, entry.second.cols);
//                maxHeight = max(maxHeight, entry.second.rows);
//            }
//
//            collage.create(rows * maxHeight, cols * maxWidth, CV_8UC3);
//            collage.setTo(cv::Scalar(0, 0, 0));
//
//            int row = 0;
//            int col = 0;
//            for (const auto& entry : clientFrames) {
//                cv::Mat frame = entry.second;
//                if (!frame.empty()) {
//                    cv::Mat resizedFrame;
//                    cv::resize(frame, resizedFrame, cv::Size(maxWidth, maxHeight));
//                    resizedFrame.copyTo(collage(cv::Rect(col * maxWidth, row * maxHeight, maxWidth, maxHeight)));
//                }
//
//                col++;
//                if (col == cols) {
//                    col = 0;
//                    row++;
//                }
//            }
//        }
//
//        // Encode the collage frame
//        std::vector<uchar> buffer;
//        cv::imencode(".jpg", collage, buffer);
//
//        // Send the collage frame to all clients
//        for (const auto& entry : clientFrames) {
//            SOCKET client_socket = entry.first;
//
//            // Send the collage frame size
//            int frameSize = buffer.size();
//            if (send(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
//                cout << "send() failed with error code: " << WSAGetLastError() << "\n";
//                continue;
//            }
//
//            // Send the collage frame data
//            int bytesSent = 0;
//            while (bytesSent < frameSize) {
//                int bytesRemaining = frameSize - bytesSent;
//                int bytesToSend = min(bytesRemaining, BUFLEN);
//                if (send(client_socket, (char*)buffer.data() + bytesSent, bytesToSend, 0) == SOCKET_ERROR) {
//                    cout << "send() failed with error code: " << WSAGetLastError() << "\n";
//                    break;
//                }
//                bytesSent += bytesToSend;
//            }
//        }
//    }
//
//    WSADATA wsa{};
//    SOCKET server_socket = 0;
//    sockaddr_in server{};
//    unordered_map<SOCKET, cv::Mat> clientFrames;
//    mutex mtx;
//};
//
//int main() {
//    system("title TCP Server");
//    MoveWindow(GetConsoleWindow(), 650, 50, 500, 500, true);
//
//    TCPServer tcpServer;
//    tcpServer.start();
//}
