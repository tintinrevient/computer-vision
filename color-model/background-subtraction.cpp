#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const int hue_threshold = 26;
const int saturation_threshold = 0;
const int value_threshold = 46;

const int hsv_threshold_max = 50;
const int num_of_signals_threshold = 100;
const int signal_threshold = 100;

void processForeground(Mat &frame, Mat &background, const string &fgMaskFilename, const string &frameFilename, const string imageWindow, bool use_preset_threshold);
int numOfSignals(const Mat &foreground);
Mat threshold(vector<Mat> &frame_hsv_channels, vector<Mat> &background_hsv_channels, int hue_threshold, int saturation_threshold, int value_threshold);

int main(int argc, char** argv) {
    string frameInputFilenames[4] = {"./data/cam1/video.avi", "./data/cam2/video.avi",
                                     "./data/cam3/video.avi", "./data/cam4/video.avi"};
    string backgroundInputFilenames[4] = {"./data/cam1/background.avi", "./data/cam2/background.avi",
                                          "./data/cam3/background.avi", "./data/cam4/background.avi"};
    string foregroundOutputFilenames[4] = {"./data/cam1/foreground", "./data/cam2/foreground",
                                           "./data/cam3/foreground", "./data/cam4/foreground"};
    string frameOutputFilenames[4] = {"./data/cam1/frame", "./data/cam2/frame",
                                      "./data/cam3/frame", "./data/cam4/frame"};

    // the frame video with the foreground and the background
    vector<VideoCapture> captures;
    for(int i = 0; i < 4; i++) {
        VideoCapture capture(samples::findFile(frameInputFilenames[i]));
        if (!capture.isOpened()){
            cerr << "Unable to open: " << frameInputFilenames[i] << endl;
            return 0;
        }
        captures.push_back(capture);
    }

    // the frame video with the foreground and the background
    vector<Mat> backgrounds;
    for(int i = 0; i < 4; i++) {
        VideoCapture capture(samples::findFile(backgroundInputFilenames[i]));
        if (!capture.isOpened()) {
            cerr << "Unable to open: " << frameInputFilenames[i] << endl;
            return 0;
        }
        Mat background;
        capture >> background;
        if (background.empty())
            continue;
        backgrounds.push_back(background);
    }

    Mat frame1, frame2, frame3, frame4;
    string fgMaskFilename1, fgMaskFilename2, fgMaskFilename3, fgMaskFilename4;
    string frameFilename1, frameFilename2, frameFilename3, frameFilename4;
    string imageWindow1 = "FG Mask 1";
    string imageWindow2 = "FG Mask 2";
    string imageWindow3 = "FG Mask 3";
    string imageWindow4 = "FG Mask 4";

    int index = 0;
    while (true) {
        index++;

        cout << "the index of the current frame is: " << index << endl;

        //frame1 from cam1
        captures[0] >> frame1;
        if (frame1.empty())
            break;
        fgMaskFilename1 = foregroundOutputFilenames[0] + to_string(index) + ".png";
        frameFilename1 = frameOutputFilenames[0] + to_string(index) + ".png";
        processForeground(frame1, backgrounds[0], fgMaskFilename1, frameFilename1, imageWindow1, true);

        //frame2 from cam2
        captures[1] >> frame2;
        if (frame2.empty())
            break;
        fgMaskFilename2 = foregroundOutputFilenames[1] + to_string(index) + ".png";
        frameFilename2 = frameOutputFilenames[1] + to_string(index) + ".png";
        processForeground(frame2, backgrounds[1], fgMaskFilename2, frameFilename2, imageWindow2, true);

        //frame3 from cam3
        captures[2] >> frame3;
        if (frame3.empty())
            break;
        fgMaskFilename3 = foregroundOutputFilenames[2] + to_string(index) + ".png";
        frameFilename3 = frameOutputFilenames[2] + to_string(index) + ".png";
        processForeground(frame3, backgrounds[2], fgMaskFilename3, frameFilename3, imageWindow3, true);

        //frame4 from cam4
        captures[3] >> frame4;
        if (frame4.empty())
            break;
        fgMaskFilename4 = foregroundOutputFilenames[3] + to_string(index) + ".png";
        frameFilename4 = frameOutputFilenames[3] + to_string(index) + ".png";
        processForeground(frame4, backgrounds[3], fgMaskFilename4, frameFilename4, imageWindow4, true);
    }

    return 0;
}

int numOfSignals(const Mat &foreground)
{
    Mat flat = foreground.reshape(1, foreground.total() * foreground.channels());
    vector<uchar> vector = foreground.isContinuous()? flat : flat.clone();

    int count = 0;
    int num_of_signals = 0;
    bool flag = false;

    for(int i = 0; i < vector.size(); i++) {
        if(int(vector[i]) == 255) {
            if(flag == true)
                count++;

            if(flag == false) {
                flag = true;
                count = 0;
                count++;
            }

        } else {
            flag = false;
        }

        if(flag == false && count > signal_threshold) {
            num_of_signals++;
            count = 0;
        }
    }

    return num_of_signals;
}

Mat threshold(vector<Mat> &frame_hsv_channels, vector<Mat> &background_hsv_channels, int hue_threshold, int saturation_threshold, int value_threshold) {
    // threshold the foreground image by Hue
    Mat tmp, background, foreground;
    absdiff(frame_hsv_channels[0], background_hsv_channels[0], tmp);
    threshold(tmp, foreground, hue_threshold, 255, THRESH_BINARY);

    // threshold the foreground image by Saturation
    absdiff(frame_hsv_channels[1], background_hsv_channels[1], tmp);
    threshold(tmp, background, saturation_threshold, 255, THRESH_BINARY);
    bitwise_and(foreground, background, foreground);

    // threshold the foreground image by Value
    absdiff(frame_hsv_channels[2], background_hsv_channels[2], tmp);
    threshold(tmp, background, value_threshold, 255, THRESH_BINARY);
    bitwise_or(foreground, background, foreground);

    return foreground;
}

void processForeground(Mat &frame, Mat &background, const string &fgMaskFilename, const string &frameFilename, const string imageWindow, bool use_preset_threshold) {

    Mat frame_hsv_image;
    cvtColor(frame, frame_hsv_image, COLOR_BGR2HSV);  // from BGR to HSV color space

    vector<Mat> frame_hsv_channels;
    split(frame_hsv_image, frame_hsv_channels);  // Split the HSV-channels for further analysis

    Mat background_hsv_image;
    cvtColor(background, background_hsv_image, COLOR_BGR2HSV);  // from BGR to HSV color space

    vector<Mat> background_hsv_channels;
    split(background_hsv_image, background_hsv_channels);  // Split the HSV-channels for further analysis

    int hue_threshold_min = 0;
    int saturation_threshold_min = 0;
    int value_threshold_min = 0;

    if(use_preset_threshold) {
        hue_threshold_min = hue_threshold;
        saturation_threshold_min = saturation_threshold;
        value_threshold_min = value_threshold;

    } else {
        int num_of_signals = num_of_signals_threshold;

        for(int hue_threshold = 0; hue_threshold < hsv_threshold_max; hue_threshold++) {
            for(int saturation_threshold = 0; saturation_threshold < hsv_threshold_max; saturation_threshold++) {
                for(int value_threshold = 0; value_threshold < hsv_threshold_max; value_threshold++) {
                    cout << "hue: " << hue_threshold << ", saturation: " << saturation_threshold << ", value: " << value_threshold << endl;

                    Mat foreground = threshold(frame_hsv_channels, background_hsv_channels, hue_threshold, saturation_threshold, value_threshold);

                    if(numOfSignals(foreground) < num_of_signals) {
                        num_of_signals = numOfSignals(foreground);

                        hue_threshold_min = hue_threshold;
                        saturation_threshold_min = saturation_threshold;
                        value_threshold_min = value_threshold;
                    }
                }
            }
        }

        cout << "Minimun HSV thresholds are found." << endl;
        cout << "minimum number of signals: " << num_of_signals << endl;
        cout << "minimum hue threshold: " << hue_threshold_min << endl;
        cout << "minimum saturation threshold: " << saturation_threshold_min << endl;
        cout << "minimum value threshold: " << value_threshold_min <<endl;
    }

    Mat foreground = threshold(frame_hsv_channels, background_hsv_channels, hue_threshold_min, saturation_threshold_min, value_threshold_min);

    cout << "number of signals: " << numOfSignals(foreground) << endl;

    // erode the foreground image
    Mat element = getStructuringElement(MORPH_RECT, Size(5,5));
    Mat eroded_image;
    erode(foreground, eroded_image, element);

    // dilate the foreground image
    Mat dilated_image;
    dilate(eroded_image, dilated_image, element);

    // save the fgMask image
    imwrite(fgMaskFilename, dilated_image);
    // save the original frame image
    imwrite(frameFilename, frame);

    namedWindow(imageWindow, 1);
    // show the final result
    imshow(imageWindow, dilated_image);

    //get the input from the keyboard
    int keyboard = waitKey(30);
    if (keyboard == 'q' || keyboard == 27)
        return;

    return;
}