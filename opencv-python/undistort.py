import numpy as np
import cv2

# Load the data with the camera matrix and distortion coefficients
data = np.load('./output/data.npy',allow_pickle='TRUE').item()

mtx = data['mtx']
dist = data['dist']

# Distortion
img = cv2.imread('./images/left12.jpg')
h,  w = img.shape[:2]
newcameramtx, roi=cv2.getOptimalNewCameraMatrix(mtx,dist,(w,h),1,(w,h))

# Undistort
dst = cv2.undistort(img, mtx, dist, None, newcameramtx)

# Crop the image
x,y,w,h = roi
dst = dst[y:y+h, x:x+w]
cv2.imwrite('./output/undistort.png',dst)