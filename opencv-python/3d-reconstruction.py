import numpy as np
import cv2

# Load the data with the camera matrix and distortion coefficients
data = np.load('./output/data.npy',allow_pickle='TRUE').item()

mtx = data['mtx']
dist = data['dist']

# Draw the world 3D axes
def draw_axis(img, corners, imgpts):

    corner = tuple(corners[0].ravel())

    img = cv2.line(img, corner, tuple(imgpts[0].ravel()), (255,0,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[1].ravel()), (0,255,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[2].ravel()), (0,0,255), 5)

    return img

# Draw the 3D cube
def draw_cube(img, corners, imgpts):

    imgpts = np.int32(imgpts).reshape(-1,2)

    img = cv2.drawContours(img, [imgpts[:4]],-1,(255,255,0),3)

    for i,j in zip(range(4),range(4,8)):
        img = cv2.line(img, tuple(imgpts[i]), tuple(imgpts[j]),(255,255,0),3)

    img = cv2.drawContours(img, [imgpts[4:]],-1,(255,255,0),3)

    return img

criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

img = cv2.imread('./images/left05.jpg')
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
ret, corners = cv2.findChessboardCorners(gray, (7,6),None)

if ret == True:
    corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)

    # Find the rotation and translation vectors
    _, rvecs, tvecs, inliers = cv2.solvePnPRansac(objp, corners2, mtx, dist)

    # Project 3D object points to the image plane
    # Draw the axes
    axis = np.float32([[5, 0, 0], [0, 5, 0], [0, 0, -5]]).reshape(-1, 3)
    imgpts, jac = cv2.projectPoints(axis, rvecs, tvecs, mtx, dist)
    img = draw_axis(img, corners2, imgpts)

    # Draw the cube
    axis = np.float32([[0,0,0], [0,2,0], [2,2,0], [2,0,0],
                       [0,0,-2],[0,2,-2],[2,2,-2],[2,0,-2] ])
    imgpts, jac = cv2.projectPoints(axis, rvecs, tvecs, mtx, dist)
    img = draw_cube(img, corners2, imgpts)

    cv2.imshow('img',img)
    cv2.waitKey(500)
    cv2.imwrite('./output/cube.png', img)

cv2.destroyAllWindows()


