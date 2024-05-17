#include <iostream>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996)

#define SERVER "127.0.0.1"  // or "localhost" - ip address of TCP server
#define BUFLEN 65536  // max length of answer
#define PORT 8888  // the port on which to listen for incoming data

class TCPClient {
public:
    TCPClient() {
        // initialise winsock
        cout << "Initialising Winsock...\n";
        if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Initialised.\n";

        // create socket
        if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // setup address structure
        memset((char*)&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.S_un.S_addr = inet_addr(SERVER);

        // connect to server
        if (connect(client_socket, (sockaddr*)&server, sizeof(server)) < 0) {
            cout << "connect() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    ~TCPClient() {
        closesocket(client_socket);
        WSACleanup();
    }

    void streamVideo() {
        VideoCapture cap("hillbillies.mp4");

        // Check if video opened successfully
        if (!cap.isOpened()) {
            cout << "Error opening video stream or file" << endl;
            exit(EXIT_FAILURE);
        }

        while (cap.isOpened()) {
            Mat frame;
            // Capture frame-by-frame
            cap >> frame;
            // If the frame is empty, break immediately
            if (frame.empty())
                break;
            // Compress the frame
            vector<uchar> buffer;
            imencode(".jpg", frame, buffer);

            // Send the frame size
            int frameSize = buffer.size();
            if (send(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
                cout << "send() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            // Send the frame data
            int bytesSent = 0;
            while (bytesSent < frameSize) {
                int bytesRemaining = frameSize - bytesSent;
                int bytesToSend = min(bytesRemaining, BUFLEN);
                if (send(client_socket, (char*)buffer.data() + bytesSent, bytesToSend, 0) == SOCKET_ERROR) {
                    cout << "send() failed with error code: " << WSAGetLastError() << "\n";
                    exit(EXIT_FAILURE);
                }
                bytesSent += bytesToSend;
            }
            // Press ESC on keyboard to exit
            char c = (char)waitKey(25);
            if (c == 27)
                break;
        }
        // When everything done, release the video capture object
        cap.release();
        // Closes all the frames
        destroyAllWindows();
    }

    void receiveVideo() {
        while (true) {
            int frameSize;
            if (recv(client_socket, (char*)&frameSize, sizeof(int), 0) == SOCKET_ERROR) {
                cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
                break;
            }

            if (frameSize == 0) {
                cout << "Server disconnected.\n";
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
                    cout << "Server disconnected.\n";
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
            cv::imshow("Received Video", frame);
            if (cv::waitKey(1) == 27) {
                break;
            }
        }
    }

    void start() {
        thread receiveThread(&TCPClient::receiveVideo, this);
        receiveThread.detach();
        streamVideo();
    }

private:
    WSADATA ws;
    SOCKET client_socket;
    sockaddr_in server;
};

int main() {
    system("title TCP Client");
    MoveWindow(GetConsoleWindow(), 50, 50, 500, 500, true);

    TCPClient tcpClient;
    tcpClient.start();
}