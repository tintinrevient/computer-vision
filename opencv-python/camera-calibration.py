import numpy as np
import cv2
import glob

criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

# 3D object points in the world
objpoints = []

# 2D image points in the image plane
imgpoints = []

# Read the images with different angles of the chessboard
images = glob.glob('./images/*.jpg')

for fname in images:
    img = cv2.imread(fname)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, (7,6),None)

    # Add the object points and the image points
    if ret == True:
        objpoints.append(objp)

        cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
        imgpoints.append(corners)

        # Draw and display the corners
        cv2.drawChessboardCorners(img, (7,6), corners,ret)
        cv2.imshow('img',img)
        cv2.waitKey(500)

ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1],None,None)

# Store the camera matrix and distortion coefficients in the dictionary
data = {
    'mtx': mtx,
    'dist': dist
}

# Save the data for the later use in the 3D reconstruction
np.save('./output/data.npy', data)

with open('./output/data.csv', 'w') as f:
    for key in data.keys():
        f.write("%s,%s\n"%(key,data[key]))

cv2.destroyAllWindows()