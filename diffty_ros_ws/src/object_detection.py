#!/usr/bin/env python

import numpy as np
import cv2
import time
import rospy 
from geometry_msgs.msg import Twist

wantedArea = 16000
minimumArea = 2000
width = 640
heigh = 480
linearSpeed = 0.0
angularSpeed = 0.0
object_detected = False

def read_rgb_image(image_name, show):
    rgb_image = cv2.imread(image_name)
    if show: 
        cv2.imshow("RGB Image",rgb_image)
    return rgb_image

def filter_color(rgb_image, lower_bound_color, upper_bound_color):
    #convert the image into the HSV color space
    hsv_image = cv2.cvtColor(rgb_image, cv2.COLOR_BGR2HSV)
    #cv2.imshow("hsv image",hsv_image)

    #define a mask using the lower and upper bounds of the yellow color 
    mask = cv2.inRange(hsv_image, lower_bound_color, upper_bound_color)
    cv2.imshow("mask image",mask)
    return mask


    

def getContours(binary_image):     
    filteredContours = []
    global minimumArea
    #_, contours, hierarchy = cv2.findContours(binary_image, 
    #                                          cv2.RETR_TREE, 
    #                                           cv2.CHAIN_APPROX_SIMPLE)
    _, contours, hierarchy = cv2.findContours(binary_image.copy(), 
                                            cv2.RETR_EXTERNAL,
	                                        cv2.CHAIN_APPROX_SIMPLE)
    for c in contours:
        area = cv2.contourArea(c)
        if area > minimumArea:
            filteredContours.append(c);
    return filteredContours

def is_in_range (number):
    global wantedArea
    if (number >= (wantedArea-3000) and number <= (wantedArea+3000)):
        return True
    else :
        return False

def move_diffty (cg_x , cg_y, areaOfContour ):
    global width
    global wantedArea
    global linearSpeed , angularSpeed

    step = int(width / 3)
    angularSpeed = 0.0
    linearSpeed = 0.0 
    if (cg_x > step and cg_x < (width-step)):
        print("you are centered");
        angularSpeed = 0.0
    elif cg_x > 0.0 and cg_x < step :
        print("trun left diffty")
        angularSpeed = 1.0 
    elif cg_x > (width-step) and cg_x < width :
        print("trun right diffty")
        angularSpeed = -1.0 
    # now deciding the linear speed of the robot based on area of contour
    if is_in_range(areaOfContour):
        linearSpeed = 0.0 
        print ("stand still")
    elif areaOfContour > wantedArea :
        print("go back")
        linearSpeed = -1.0 
    elif areaOfContour < wantedArea:
        print("go forward now!")
        linearSpeed = 1.0

def stop_diffty ():
    global linearSpeed , angularSpeed
    linearSpeed = 0.0 
    angularSpeed = 0.0
    print('stop diffty')

def draw_ball_contour(binary_image, rgb_image, contours):
    black_image = np.zeros([binary_image.shape[0], binary_image.shape[1],3],'uint8')
    global wantedArea , minimumArea , object_detected 
    if (len(contours) == 0):
        object_detected = False 
        stop_diffty()
    else :
        object_detected = True
    for c in contours:
        area = cv2.contourArea(c)
        perimeter= cv2.arcLength(c, True)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        if (area > minimumArea and object_detected):
            cv2.drawContours(rgb_image, [c], -1, (150,250,150), 1)
            cv2.drawContours(black_image, [c], -1, (150,250,150), 1)
            cx, cy = get_contour_center(c)
            cv2.circle(rgb_image, (cx,cy),(int)(radius),(0,0,255),1)
            cv2.circle(black_image, (cx,cy),(int)(radius),(0,0,255),1)
            cv2.circle(black_image, (cx,cy),5,(150,150,255),-1)
            cv2.circle(rgb_image, (cx,cy),5,(150,150,255),-1)
            print ("Area: {}, Perimeter: {}".format(area, perimeter))
            print ("x: {}, y: {}".format(cx, cy))
            move_diffty (cx, cy, area)
            
    
    # print ("number of contours: {}".format(len(contours)))
    cv2.imshow("RGB Image Contours",rgb_image)
    cv2.imshow("Black Image Contours",black_image)

def get_contour_center(contour):
    M = cv2.moments(contour)
    cx=-1
    cy=-1
    if (M['m00']!=0):
        cx= int(M['m10']/M['m00'])
        cy= int(M['m01']/M['m00'])
    return cx, cy

def detect_ball_in_a_frame(image_frame):
    yellowLower =(27, 82, 54)
    yellowUpper = (92, 255, 217)
    rgb_image = image_frame
    binary_image_mask = filter_color(rgb_image, yellowLower, yellowUpper)
    contours = getContours(binary_image_mask)
    draw_ball_contour(binary_image_mask, rgb_image,contours)




def main():
    video_capture = cv2.VideoCapture(0)
    #video_capture = cv2.VideoCapture('video/tennis-ball-video.mp4')
    rospy.init_node('diffty_object_following',anonymous=True)
    global move_pup
    global linearSpeed , angularSpeed
    move_pup  = rospy.Publisher('/diffty/cmd_vel', Twist, queue_size=10)
    twist = Twist()
    while(True):
        ret, frame = video_capture.read()
        detect_ball_in_a_frame(frame)
        
        twist.linear.x = linearSpeed
        twist.angular.z = angularSpeed
        move_pup.publish(twist)
        # time.sleep(0.033)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()




