#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

using namespace cv ;
using namespace std;

const float square_length = 0.0249f; //meters
const Size board_dimensin =Size(9,6);

void createKnownBoardPosition(Size board_size, float sqr_length, vector<Point3f>& corners){
    /*
     We get the know square positions on the board
     board_size: is the dimension of the board
     sqr_length : length of one side of the square
     corners: stores x, y, z co-ordinates of where expect each square to appear(the intersections between 2 squares) but z here is 0 since we have 2D images
     */

    for (int x=0; x < board_size.height; x++) {

        for (int y=0;  y < board_size.width; y++) {

            corners.push_back(Point3f(y * sqr_length, x * sqr_length, 0.0f));

        }
    }

}

bool saveCameraCalibration(String file_name, Mat camera_matrix, Mat distortion_coeff){
    /*
     This function will save details about the camera matrix to a txt file'
     Details include:
     1. The camera matrix
     2. The distortion co-efficient matrix
     */
    ofstream outstream(file_name);

    if (outstream) {
        uint16_t rows = camera_matrix.rows;
        uint16_t cols = camera_matrix.cols;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                double value = camera_matrix.at<double>(r, c);
                outstream << value <<endl ;
            }
        }

        rows = distortion_coeff.rows;
        cols = distortion_coeff.cols;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                double value = distortion_coeff.at<double>(r, c);
                outstream << value <<endl ;
            }
        }
        return true;
    }

    return false;
}

void cameraCalibrate(){
    vector<Mat> imgs;
    vector<string> tmp;
    vector<Point2f> points_found;
    vector< vector<Point3f> > realworld_corner_points(1);
    vector< vector<Point2f> > get_corners;
    vector<Mat> r_vec, t_vec ;
    Mat camera_matrix = Mat::eye(3, 3, CV_64F);
    Mat distortion_coeff = Mat::zeros(8, 1, CV_64F);




    int count = 0;
    std::ifstream filename("imageList.txt");
    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(filename, str))
    {
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0){
            //cout<<str<<endl;
            count += 1;
            Mat tmp = imread(str);
            imgs.push_back(tmp);
        }
    }



    for(int i = 0; i < imgs.size(); i++){
        bool found = findChessboardCorners(imgs[i], board_dimensin, points_found, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
        if(found){
            get_corners.push_back(points_found);
            cout<<found<<endl;
        }
    }
    createKnownBoardPosition(board_dimensin, square_length, realworld_corner_points[0]);

    realworld_corner_points.resize(get_corners.size(), realworld_corner_points[0]);



    calibrateCamera(realworld_corner_points, get_corners, board_dimensin, camera_matrix, distortion_coeff, r_vec, t_vec);
    saveCameraCalibration("output.txt", camera_matrix, distortion_coeff);

    cout<< distortion_coeff<<endl;
    cout<<"below is camera m"<<endl;
    cout<<camera_matrix<<endl;
}


int main(int argc, const char * argv[]) {
    cameraCalibrate();
    return 0;
}