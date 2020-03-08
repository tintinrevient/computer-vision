#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    string frameInputFilenames[4] = {"./data/cam1/video.avi", "./data/cam2/video.avi",
                                     "./data/cam3/video.avi", "./data/cam4/video.avi"};
    string foregroundOutputFilenames[4] = {"./data/cam1/foreground", "./data/cam2/foreground",
                                           "./data/cam3/foreground", "./data/cam4/foreground"};
    string frameOutputFilenames[4] = {"./data/cam1/frame", "./data/cam2/frame",
                                           "./data/cam3/frame", "./data/cam4/frame"};

    Ptr<BackgroundSubtractor> backgroundSubtractor = createBackgroundSubtractorKNN();
//    Ptr<BackgroundSubtractor> backgroundSubtractor = createBackgroundSubtractorMOG2();
ff
    vector<VideoCapture> captures;
    for(int i = 0; i < 4; i++) {
        VideoCapture capture(samples::findFile(frameInputFilenames[i]));
        if (!capture.isOpened()){
            cerr << "Unable to open: " << frameInputFilenames[i] << endl;
            return 0;
        }
        captures.push_back(capture);
    }

    Mat frame1, fgMask1;
    Mat frame2, fgMask2;
    Mat frame3, fgMask3;
    Mat frame4, fgMask4;
    Mat element = getStructuringElement(MORPH_RECT, Size(5,5));

    int index = 0;
    while (true) {
        index++;

        //frame1 from cam1
        captures[0] >> frame1;
        if (frame1.empty())
            break;
        backgroundSubtractor->apply(frame1, fgMask1);
        //show the fg masks
        imshow("FG Mask 1", fgMask1);

        //frame2 from cam2
        captures[1] >> frame2;
        if (frame2.empty())
            break;
        backgroundSubtractor->apply(frame2, fgMask2);
        //show the fg masks
        imshow("FG Mask 2", fgMask2);

        //frame3 from cam3
        captures[2] >> frame3;
        if (frame3.empty())
            break;
        backgroundSubtractor->apply(frame3, fgMask3);
        //show the fg masks
        imshow("FG Mask 3", fgMask3);

        //frame4 from cam4
        captures[3] >> frame4;
        if (frame4.empty())
            break;
        backgroundSubtractor->apply(frame4, fgMask4);
        //show the fg masks
        imshow("FG Mask 4", fgMask4);

        cout << "the index of the current frame is: " << index << endl;

        //save the fg masks
        string filename1 = foregroundOutputFilenames[0] + to_string(index) + ".png";
        string filename2 = foregroundOutputFilenames[1] + to_string(index) + ".png";
        string filename3 = foregroundOutputFilenames[2] + to_string(index) + ".png";
        string filename4 = foregroundOutputFilenames[3] + to_string(index) + ".png";
        imwrite(filename1, fgMask1);
        imwrite(filename2, fgMask2);
        imwrite(filename3, fgMask3);
        imwrite(filename4, fgMask4);

        //save the frames
        filename1 = frameOutputFilenames[0] + to_string(index) + ".png";
        filename2 = frameOutputFilenames[1] + to_string(index) + ".png";
        filename3 = frameOutputFilenames[2] + to_string(index) + ".png";
        filename4 = frameOutputFilenames[3] + to_string(index) + ".png";
        imwrite(filename1, frame1);
        imwrite(filename2, frame2);
        imwrite(filename3, frame3);
        imwrite(filename4, frame4);

        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }

    return 0;
}