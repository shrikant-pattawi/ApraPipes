#include<opencv2/core/core.hpp>
#include<opencv2/contrib/contrib.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<vector>

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_SUITE(task_test2)

BOOST_AUTO_TEST_CASE(imageoverlay)
{
    double alpha = 0.5; double beta; 
    cv::Mat dest;
    cv::Mat img = cv::imread("./data/1280x960.jpg");
    cv::Mat img2 = cv::imread("./data/1280x960.jpg");
    beta = ( 1.0 - alpha );
    addWeighted( src1, alpha, src2, beta, 0.0, dst);
    imshow( "Linear Blend", dest );
    waitKey(0);
}
BOOST_AUTO_TEST_SUITE_END()