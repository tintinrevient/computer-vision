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

void initBackground(const string *backgroundInputFilenames, const string *frameInputFilenames, const string *frameOutputFilenames);
void processForeground(const string &frameInputFilename, const int frameIndex, const string &backgroundInputFilename, const string &foregroundOutputFilename, bool use_preset_threshold);
int numOfSignals(const Mat &foreground);
Mat threshold(vector<Mat> &frame_hsv_channels, vector<Mat> &background_hsv_channels, int hue_threshold, int saturation_threshold, int value_threshold);

int main(int argc, char** argv)
{
    string backgroundInputFilenames[4] = {"./data/cam1/background.avi", "./data/cam2/background.avi",
                                          "./data/cam3/background.avi", "./data/cam4/background.avi"};
    string frameInputFilenames[4] = {"./data/cam1/video.avi", "./data/cam2/video.avi",
                                     "./data/cam3/video.avi", "./data/cam4/video.avi"};
    string frameOutputFilenames[4] = {"./data/cam1/frame.png", "./data/cam2/frame.png",
                                      "./data/cam3/frame.png", "./data/cam4/frame.png"};
    string foregroundOutputFilenames[4] = {"./data/cam1/foreground.png", "./data/cam2/foreground.png",
                                           "./data/cam3/foreground.png", "./data/cam4/foreground.png"};

    // play videos from 4 cameras simultaneously
    // quit will trigger the current index of the frame to be captured with the index printed on console
    // the index of the frame will be used in processForeground() as the input param "frameIndex"
//    initBackground(backgroundInputFilenames, frameInputFilenames, frameOutputFilenames);

    // the computed foreground image "foreground.png" will be stored in each camera folder as output
    // use_preset_threshold = true -> the preset global thresholds will be used
    // use_preset_threshold = false -> the thresholds will be found iteratively with the maximum signal value
    int frameIndex = 170;
    for(int i = 0; i < 4; i++)
    {
        processForeground(frameInputFilenames[i], frameIndex, backgroundInputFilenames[i], foregroundOutputFilenames[i], true);
    }

    return 0;
}

void initBackground(const string *backgroundInputFilenames, const string *frameInputFilenames, const string *frameOutputFilenames)
{
    string imageWindow_cam1 = "Image View Cam1";
    string imageWindow_cam2 = "Image View Cam2";
    string imageWindow_cam3 = "Image View Cam3";
    string imageWindow_cam4 = "Image View Cam4";

    VideoCapture cap_cam1;
    cap_cam1.open(frameInputFilenames[0]);

    VideoCapture cap_cam2;
    cap_cam2.open(frameInputFilenames[1]);

    VideoCapture cap_cam3;
    cap_cam3.open(frameInputFilenames[2]);

    VideoCapture cap_cam4;
    cap_cam4.open(frameInputFilenames[3]);

    int nframes_cam1 = (int) cap_cam1.get(CAP_PROP_FRAME_COUNT);
    int nframes_cam2 = (int) cap_cam2.get(CAP_PROP_FRAME_COUNT);
    int nframes_cam3 = (int) cap_cam3.get(CAP_PROP_FRAME_COUNT);
    int nframes_cam4 = (int) cap_cam4.get(CAP_PROP_FRAME_COUNT);
    int nframes;

    if(nframes_cam1 != nframes_cam2 || nframes_cam2 != nframes_cam3 || nframes_cam3 != nframes_cam4)
        return;
    else
        nframes = nframes_cam1;

    int step = 1;
    Mat frame_cam1;
    Mat frame_cam2;
    Mat frame_cam3;
    Mat frame_cam4;

    int i;
    for(i = 0; i < nframes; i = i + step)
    {
        cap_cam1 >> frame_cam1;
        cap_cam2 >> frame_cam2;
        cap_cam3 >> frame_cam3;
        cap_cam4 >> frame_cam4;

        if(frame_cam1.empty())
            break;
        if(frame_cam2.empty())
            break;
        if(frame_cam3.empty())
            break;
        if(frame_cam4.empty())
            break;

        imshow(imageWindow_cam1, frame_cam1);
        imshow(imageWindow_cam2, frame_cam2);
        imshow(imageWindow_cam3, frame_cam3);
        imshow(imageWindow_cam4, frame_cam4);

        if(waitKey(33) >= 0)
        {
            cout << "the index of frame is: " << i << endl;

            imwrite(frameOutputFilenames[0], frame_cam1);
            imwrite(frameOutputFilenames[1], frame_cam2);
            imwrite(frameOutputFilenames[2], frame_cam3);
            imwrite(frameOutputFilenames[3], frame_cam4);

            break;
        }
    }

    cap_cam1.release();
    cap_cam2.release();
    cap_cam3.release();
    cap_cam4.release();

    destroyWindow(imageWindow_cam1);
    destroyWindow(imageWindow_cam2);
    destroyWindow(imageWindow_cam3);
    destroyWindow(imageWindow_cam4);
}

int numOfSignals(const Mat &foreground)
{
    Mat flat = foreground.reshape(1, foreground.total() * foreground.channels());
    vector<uchar> vector = foreground.isContinuous()? flat : flat.clone();

    int count = 0;
    int num_of_signals = 0;
    bool flag = false;

    for(int i = 0; i < vector.size(); i++)
    {
        if(int(vector[i]) == 255)
        {
            if(flag == true)
                count++;

            if(flag == false)
            {
                flag = true;
                count = 0;
                count++;
            }

        } else {
            flag = false;
        }

        if(flag == false && count > signal_threshold)
        {
            num_of_signals++;
            count = 0;
        }
    }

    return num_of_signals;
}

Mat threshold(vector<Mat> &frame_hsv_channels, vector<Mat> &background_hsv_channels, int hue_threshold, int saturation_threshold, int value_threshold)
{
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

void processForeground(const string &frameInputFilename, const int frameIndex, const string &backgroundInputFilename, const string &foregroundOutputFilename, bool use_preset_threshold)
{
    string imageWindow = "Image View";

    Mat frame_image, background_image;

    VideoCapture cap;
    cap.open(frameInputFilename);
    int nframes = (int) cap.get(CAP_PROP_FRAME_COUNT);
    for(int i = 0; i < nframes; i++)
    {
        cap >> frame_image;

        if(frame_image.empty()){
            return;
        }

        if(i == frameIndex)
            break;
    }

    cap.open(backgroundInputFilename);
    cap >> background_image;
    if(background_image.empty()){
        return;
    }

    Mat frame_hsv_image;
    cvtColor(frame_image, frame_hsv_image, COLOR_BGR2HSV);  // from BGR to HSV color space

    vector<Mat> frame_hsv_channels;
    split(frame_hsv_image, frame_hsv_channels);  // Split the HSV-channels for further analysis

    Mat background_hsv_image;
    cvtColor(background_image, background_hsv_image, COLOR_BGR2HSV);  // from BGR to HSV color space

    vector<Mat> background_hsv_channels;
    split(background_hsv_image, background_hsv_channels);  // Split the HSV-channels for further analysis

    int hue_threshold_min = 0;
    int saturation_threshold_min = 0;
    int value_threshold_min = 0;

    if(use_preset_threshold)
    {
        hue_threshold_min = hue_threshold;
        saturation_threshold_min = saturation_threshold;
        value_threshold_min = value_threshold;

    } else {
        int num_of_signals = num_of_signals_threshold;

        for(int hue_threshold = 0; hue_threshold < hsv_threshold_max; hue_threshold++)
        {
            for(int saturation_threshold = 0; saturation_threshold < hsv_threshold_max; saturation_threshold++)
            {
                for(int value_threshold = 0; value_threshold < hsv_threshold_max; value_threshold++)
                {
                    cout << "hue: " << hue_threshold << ", saturation: " << saturation_threshold << ", value: " << value_threshold << endl;

                    Mat foreground = threshold(frame_hsv_channels, background_hsv_channels, hue_threshold, saturation_threshold, value_threshold);

                    if(numOfSignals(foreground) < num_of_signals)
                    {
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

    imwrite(foregroundOutputFilename, dilated_image);

    namedWindow(imageWindow, 1);
    imshow(imageWindow, dilated_image);
    waitKey(0);

    destroyWindow(imageWindow);
    cap.release();
}