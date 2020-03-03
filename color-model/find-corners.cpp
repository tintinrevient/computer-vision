#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void onMouse(int event, int x, int y, int flags, void* param);

vector<Point2f> imgPoints;

int main(int argc, char** argv)
{
    string camNum = argv[1];

    string configFilename = "./data/" + camNum + "/intrinsics.xml";
    string inputFilename = "./data/" + camNum + "/extrinsics.avi";
    string cornerFilename = "./data/" + camNum + "/boardcorners.xml";
    string imageWindow = "Image View";

    int step = 1;

    Size boardSize = Size(6, 8);

    FileStorage fs_config(configFilename, FileStorage::READ);

    Mat cameraMatrix, distCoeffs;
    fs_config["camera_matrix"] >> cameraMatrix;
    fs_config["distortion_coefficients"] >> distCoeffs;

    if(!fs_config.isOpened() || cameraMatrix.empty() || distCoeffs.empty())
        return -1;
    fs_config.release();

    VideoCapture cap;
    cap.open(inputFilename);

    Mat view, viewGray, canvas;

    cap >> view;
    if(view.empty())
        return -1;

    namedWindow(imageWindow, WINDOW_AUTOSIZE);
    setMouseCallback(imageWindow, onMouse);

    cout << "Now mark the " << boardSize.area() << " interior corners of the checkerboard" << endl;

    while ((int) imgPoints.size() < boardSize.area())
    {
        canvas = view.clone();

        if (!imgPoints.empty())
        {
            for (int c = 0; c < imgPoints.size(); c++)
            {
                circle(canvas, imgPoints.at(c), 4, Scalar(255, 0, 255), 1, 8);
                if (c > 0)
                    line(canvas, imgPoints.at(c), imgPoints.at(c - 1), Scalar(255, 0, 255), 1, 8);
            }
        }

        int key = waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            break;
        }
        else if (key == 'c' || key == 'C')
        {
            imgPoints.pop_back();
        }

        imshow(imageWindow, canvas);
    }

    assert((int ) imgPoints.size() == boardSize.area());
    cout << "Marking finished!" << endl;

    cap.release();
    destroyAllWindows();

    FileStorage fs_corner(cornerFilename, FileStorage::WRITE);
    if (fs_corner.isOpened())
    {
        fs_corner << "corners_amount" << (int) imgPoints.size();
        fs_corner << "corner_points" << imgPoints;
        fs_corner.release();
    }

    return 0;
}

void onMouse(int event, int x, int y, int flags, void* param)
{
    switch (event)
    {
        case EVENT_LBUTTONDOWN:
            if (flags == (EVENT_FLAG_LBUTTON + EVENT_FLAG_CTRLKEY))
            {
                if (!imgPoints.empty())
                {
                    cout << "Removed corner " << imgPoints.size() << "... (use Click to add)" << endl;
                    imgPoints.pop_back();
                }
            } else {
                imgPoints.push_back(Point(x, y));
                cout << "Added corner " <<imgPoints.size() << "... (use CTRL+Click to remove)" << endl;
            }

            break;

        default:
            break;
    }
}