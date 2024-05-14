#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace cv;
using namespace std;

#define PORT 8888
#define SERVER_IP "127.0.0.1"

int main() {
    // Open the default camera
    cout << "gabab" << endl;
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Failed to open the camera" << endl;
        return -1;
    }

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cout << "Failed to create socket" << endl;
        return -1;
    }

    // Set the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Failed to connect to the server" << endl;
        return -1;
    }
    double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;

    // string window_name = "My Camera Feed";
    // namedWindow(window_name); //create a window called "My Camera Feed"
    // Capture and send frames
    while (true) {
        // Capture a frame from the camera
        Mat frame;

        bool bSuccess = cap.read(frame); // read a new frame from video 
        if (bSuccess == false) {
            cout << "Video camera is disconnected" << endl;
            cin.get(); //Wait for any key press
            break;
        }
        if (frame.empty()) {
            cout << "Failed to capture frame" << endl;
            break;
        }

        //show the frame in the created window
        // imshow(window_name, frame);

        // Compress the frame
        vector<uchar> buffer;
        imencode(".jpg", frame, buffer);

        // Send the frame size
        int frameSize = buffer.size();
        send(sockfd, &frameSize, sizeof(int), 0);

        // Send the frame data
        send(sockfd, buffer.data(), buffer.size(), 0);
    }

    // // Clean up
    // close(sockfd);
    cap.release();

    return 0;
}