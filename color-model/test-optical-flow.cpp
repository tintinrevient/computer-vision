#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    cv::Mat res, img1, img2, img2Original, img2OriginalC;
    cv::VideoWriter writer;

    cv::VideoCapture cap;
    // use the test video
    cap.open("./data/cam1/video.avi");
    // use the camera
//    cap.open(0);

    cv::namedWindow("preview", cv::WINDOW_AUTOSIZE);

    cap >> img1;
    cvtColor(img1, img1, COLOR_BGR2GRAY);
    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::Size imageSize((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    writer.open("./data/tracking.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imageSize);

    for (;;) {
        cap >> img2;
        if (img2.empty()) break;

        img2.copyTo(img2OriginalC);
        cvtColor(img2, img2, COLOR_BGR2GRAY);
        img2.copyTo(img2Original);

        cv::calcOpticalFlowFarneback(img1, img2, res, .4, 1, 12, 2, 8, 1.2, 0);

        int density = 15;
        for (int y = 0; y < img2.rows; y += density) {
            for (int x = 0; x < img2.cols; x += density)
            {
                // get the flow from y, x position * 3 for better visibility
                const Point2f flowatxy = res.at<Point2f>(y, x) * 1;
                // draw line at flow direction
                arrowedLine(img2OriginalC, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255, 0, 255), 3);
                // draw initial point
                circle(img2OriginalC, Point(x, y), 1, Scalar(255, 255, 255), -1);
            }
        }
        img2Original.copyTo(img1);
//        applyColorMap(img2OriginalC, img2OriginalC, COLORMAP_RAINBOW);
        imshow("preview", img2OriginalC);
        writer << img2OriginalC;
        if (cv::waitKey(1) == 27) break;
    }
    cap.release();
    return 0;
}