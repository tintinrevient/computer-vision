#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const int hue_threshold = 30;
const int saturation_threshold = 43;
const int value_threshold = 49;

const int hsv_threshold_max = 50;
const int num_of_signals_threshold = 200;
const int signal_threshold = 30;

void initBackground(const string &backgroundInputFilename, const string &backgroundOutputFilename);
void processForeground(const string &frameInputFilename, const string &backgroundInputFilename, const string &foregroundOutputFilename, bool use_preset_threshold);
int numOfSignals(const Mat &foreground);
Mat threshold(vector<Mat> &frame_hsv_channels, vector<Mat> &background_hsv_channels, int hue_threshold, int saturation_threshold, int value_threshold);

int main(int argc, char** argv)
{
    string camNum = argv[1];

    string backgroundInputFilename = "./data/" + camNum + "/background.avi";
    string frameInputFilename = "./data/" + camNum + "/video.avi";
    string backgroundOutputFilename = "./data/" + camNum + "/background.png";
    string foregroundOutputFilename = "./data/" + camNum + "/foreground.png";

//    initBackground(backgroundInputFilename, backgroundOutputFilename);

    processForeground(frameInputFilename, backgroundInputFilename, foregroundOutputFilename, false);

    return 0;
}

void initBackground(const string &backgroundInputFilename, const string &backgroundOutputFilename)
{
    string imageWindow = "Image View";

    VideoCapture cap;
    cap.open(backgroundInputFilename);

    int nframes = (int) cap.get(CAP_PROP_FRAME_COUNT);
    cout << "Video has " << nframes << endl;

    int step = 1;
    Mat view, temp;
    int i;

    for(i = 0; i < nframes; i = i + step)
    {
        Mat frame;
        cap >> frame;

        if(frame.empty())
            break;

        if(i == 0)
        {
            view = Mat::zeros(Size(frame.size().width, frame.size().height), CV_64FC3);
        }

        frame.convertTo(temp, CV_64FC3);
        view += temp;
    }

    view.convertTo(view, CV_8U, 1. / (i + 1));

    imwrite(backgroundOutputFilename, view);

    namedWindow(imageWindow, 1);
    imshow(imageWindow, view);
    waitKey(0);

    cap.release();
    destroyWindow(imageWindow);
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

void processForeground(const string &frameInputFilename, const string &backgroundInputFilename, const string &foregroundOutputFilename, bool use_preset_threshold)
{
    string imageWindow = "Image View";

    Mat frame_image, background_image;

    VideoCapture cap;
    cap.open(frameInputFilename);
    cap >> frame_image;
    if(frame_image.empty())
        return;

    cap.open(backgroundInputFilename);
    cap >> background_image;
    if(background_image.empty())
        return;

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