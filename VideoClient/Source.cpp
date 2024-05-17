#include <iostream>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma comment (lib, "opencv_world490d.lib")
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

    void receiveText() {
        // receive a reply and print it
        char answer[BUFLEN] = {};

        // try to receive some data
        int answer_length;
        if ((answer_length = recv(client_socket, answer, BUFLEN, 0)) == SOCKET_ERROR) {
            cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        if (answer_length == 0) {
            cout << "Server disconnected.\n";
            exit(EXIT_FAILURE);
        }

        cout << "Server's response: " << answer << "\n";
    }

    void sendText() {
        char message[BUFLEN];
        cout << "Enter message: ";
        cin.getline(message, BUFLEN);

        // send the message
        if (send(client_socket, message, strlen(message), 0) == SOCKET_ERROR) {
            cout << "send() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    void streamVideo() {
        VideoCapture cap("hillbillies.mp4");
        // Check if video opened successfully
        if (!cap.isOpened()) {
            cout << "Error opening video stream or file" << endl;
            exit(EXIT_FAILURE);
        }

        double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); // get the width of frames of the video
        double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); // get the height of frames of the video

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

    void start() {
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