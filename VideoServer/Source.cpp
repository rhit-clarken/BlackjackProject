#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888




int main() {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Failed to create socket" << std::endl;
        return -1;
    }

    // Set the server address
    struct sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Failed to bind socket" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(sockfd, 1) < 0) {
        std::cout << "Failed to listen on socket" << std::endl;
        return -1;
    }

    // Accept a client connection
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientfd < 0) {
        std::cout << "Failed to accept client connection" << std::endl;
        return -1;
    }

    // Receive and display frames
    cv::namedWindow("Live Camera Feed", cv::WINDOW_NORMAL);
    while (true) {
        // Receive the frame size
        int frameSize;
        recv(clientfd, &frameSize, sizeof(int), 0);

        // Allocate buffer for the frame data
        std::vector<uchar> buffer(frameSize);

        // Receive the frame data
        recv(clientfd, buffer.data(), frameSize, 0);

        // Decode the frame
        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
        if (frame.empty()) {
            std::cout << "Failed to decode frame" << std::endl;
            break;
        }
        // Display the frame
        cv::imshow("Live Camera Feed", frame);
        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    // Clean up
    close(clientfd);
    close(sockfd);
    cv::destroyAllWindows();

    return 0;
}