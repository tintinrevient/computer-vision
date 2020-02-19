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

    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }
}

int main(int argc, char** argv)
{
    Size boardSize = Size(6, 9);

    int winSize = 11;

    float squareSize = 0.025; // unit = meters

    string inputFilename = "image_list.xml";
    string outputFilename = "camera.yml";

    vector<string> imageList;
    int i, j, nframes;

    readStringList(samples::findFile(inputFilename), imageList);

    if(!imageList.empty())
        nframes = (int) imageList.size();

    Size imageSize;

    vector<vector<Point2f> > imagePoints;
    vector<vector<Point3f> > objectPoints;
    vector<Point3f> objPoints;

    for(i = 0; i < boardSize.height; i++)
    {
        for(j = 0; j < boardSize.width; j++)
        {
            objPoints.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
        }
    }

    namedWindow("Image View", 1);

    for(i = 0; i < nframes; i++)
    {
        Mat view, viewGray;

        view = imread(imageList[i], 1);

        if(view.empty())
            break;

        imageSize = view.size();

        vector<Point2f> pointbuf;
        cvtColor(view, viewGray, COLOR_BGR2GRAY);

        bool found = findChessboardCorners(view, boardSize, pointbuf,
                                           CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
        if(found) {
            cornerSubPix(viewGray, pointbuf, Size(winSize, winSize),
                         Size(-1,-1), TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001));

            imagePoints.push_back(pointbuf);
            objectPoints.push_back(objPoints);

            drawChessboardCorners(view, boardSize, Mat(pointbuf), found);
        }

        imshow("Image View", view);

        char key = (char) waitKey(500);

        if(key == 27)
            break;
    }

    vector<Mat> rvecs, tvecs;
    Mat cameraMatrix, distCoeffs;

    calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,
                    CALIB_FIX_K3 | CALIB_USE_LU, TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001));

    saveCameraParams(outputFilename, imageSize, boardSize, squareSize,
                     cameraMatrix, distCoeffs, rvecs, tvecs);

    return 0;
}