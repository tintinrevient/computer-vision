#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

static bool readStringList(const string& filename, vector<string>& l)
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    size_t dir_pos = filename.rfind('/');
    if (dir_pos == string::npos)
        dir_pos = filename.rfind('\\');
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
    {
        string fname = (string)*it;
        if (dir_pos != string::npos)
        {
            string fpath = samples::findFile(filename.substr(0, dir_pos + 1) + fname, false);
            if (fpath.empty())
            {
                fpath = samples::findFile(fname);
            }
            fname = fpath;
        }
        else
        {
            fname = samples::findFile(fname);
        }
        l.push_back(fname);
    }
    return true;
}

static void saveCameraParams( const string& filename,
                              Size imageSize, Size boardSize, float squareSize,
                              const Mat& cameraMatrix, const Mat& distCoeffs,
                              const vector<Mat>& rvecs, const vector<Mat>& tvecs)
{
    FileStorage fs(filename, FileStorage::WRITE);

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;

    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << boardSize.width;
    fs << "board_height" << boardSize.height;
    fs << "square_size" << squareSize;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
}

int main(int argc, char** argv)
{
    string camNum = argv[1];

    Size boardSize = Size(6, 8);
    int winSize = 11;
    float squareSize = 0.115; // unit = meters -> 115 millimeters

    Size imageSize;
    string inputFilename = "./data/" + camNum + "/intrinsics.avi";
    string outputFilename = "./data/" + camNum + "/intrinsics.xml";
    string imageWindow = "Image View";

    int step = 2;

    vector<vector<Point2f> > imgPointsOfAllFrames;
    vector<vector<Point3f> > objPointsOfAllFrames;
    vector<Point3f> objPointsOfOneFrame;

    for(int i = 0; i < boardSize.height; i++)
    {
        for(int j = 0; j < boardSize.width; j++)
        {
            objPointsOfOneFrame.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
        }
    }

//    namedWindow(imageWindow, 1);

    VideoCapture cap;
    cap.open(inputFilename);

    int nframes = (int) cap.get(CAP_PROP_FRAME_COUNT);

    cout << "Video has " << nframes << endl;

    for(int i = 0; i < nframes; i = i + step)
    {
        Mat view, viewGray;

        cap >> view;

        if(view.empty())
            break;

        cvtColor(view, viewGray, COLOR_BGR2GRAY);
        imageSize = view.size();

        vector<Point2f> imgPointsOfOneFrame;

        bool found = findChessboardCorners(view, boardSize, imgPointsOfOneFrame,
                                           CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
        if(found) {
            cornerSubPix(viewGray, imgPointsOfOneFrame, Size(winSize, winSize),
                         Size(-1,-1), TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001));

            imgPointsOfAllFrames.push_back(imgPointsOfOneFrame);
            objPointsOfAllFrames.push_back(objPointsOfOneFrame);

            drawChessboardCorners(view, boardSize, Mat(imgPointsOfOneFrame), found);
        }

//        imshow(imageWindow, view);

//        if(waitKey(33) >= 0)
//            break;
    }

    vector<Mat> rvecs, tvecs;
    Mat cameraMatrix, distCoeffs;

    calibrateCamera(objPointsOfAllFrames, imgPointsOfAllFrames, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,
                    CALIB_FIX_K3 | CALIB_USE_LU, TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001));

    saveCameraParams(outputFilename, imageSize, boardSize, squareSize,
                     cameraMatrix, distCoeffs, rvecs, tvecs);

    cap.release();
//    destroyWindow(imageWindow);

    return 0;
}
