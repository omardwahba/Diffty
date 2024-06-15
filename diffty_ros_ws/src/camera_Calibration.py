#!/usr/bin/env python

import cv2
import numpy as np
#setting up the selector to select the upper and lower bond
def nothing(X):
    pass

#turnning on the video camera
cap = cv2.VideoCapture(0)
#setting up the tuner
cv2.namedWindow('the Tuner')
cv2.createTrackbar('L_H','the Tuner', 0, 255, nothing)
cv2.createTrackbar('L_S','the Tuner', 0, 255, nothing)
cv2.createTrackbar('L_V','the Tuner', 0, 255, nothing)
cv2.createTrackbar('U_H','the Tuner', 255, 255, nothing)
cv2.createTrackbar('U_S','the Tuner', 255, 255, nothing)
cv2.createTrackbar('U_V','the Tuner', 255, 255, nothing)

# frame = cv2.imread('smarties.png')
# hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)


while True:
    if cv2.waitKey(1) & 0xff == ord('q'): break
    _, frame = cap.read()
    hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
    #getting the upper and lower bound values from the Tuner
    LH = cv2.getTrackbarPos('L_H','the Tuner')
    LS = cv2.getTrackbarPos('L_S', 'the Tuner')
    LV = cv2.getTrackbarPos('L_V', 'the Tuner')
    UH = cv2.getTrackbarPos('U_H', 'the Tuner')
    US = cv2.getTrackbarPos('U_S', 'the Tuner')
    UV = cv2.getTrackbarPos('U_V', 'the Tuner')
    #PUTTING THE upper and lower values in from of np.arrays
    lowerBound = np.array([LH, LS, LV])
    upperBound = np.array([UH, US, UV])
    #creating mask to do object detection
    mask = cv2.inRange(hsv,lowerBound,upperBound)
    #applying the mask on the original frame
    result = cv2.bitwise_and(frame,frame, mask=mask)
    #do some plotting
    cv2.imshow("original",frame)
    cv2.imshow("mask", mask)
    cv2.imshow("result", result)
cap.release()
cv2.destroyAllWindows()

