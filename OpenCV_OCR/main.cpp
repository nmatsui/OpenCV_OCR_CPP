//
//  main.cpp
//  OpenCV_OCR
//
//  Created by cube on 12/03/05.
//  Copyright (c) 2012年 Nobuyuki Matsui. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tesseract/baseapi.h>

int slider;
std::vector<std::vector<cv::Point> > contours;
std::vector<cv::Vec4i> hierarchy;
double max_area;
cv::Mat bin, dst;

void on_trackbar(int, void*);

int main (int argc, const char * argv[])
{
    cv::VideoCapture cap(0);
    cv::Mat frm;
    
    cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);
    
    while (cv::waitKey(1) != 32) {
        cap >> frm;
        cv::rectangle(frm, cv::Point(148,198), cv::Point(452,302), cv::Scalar(0,0,255), 2, 4);
        cv::imshow("Camera", frm);
    }
    cv::destroyWindow("Camera");
    
    cv::Mat src(frm, cv::Rect(150, 200, 300, 100));
    cv::namedWindow("Captured", CV_WINDOW_AUTOSIZE);
    cv::imshow("Captured", src);
    
    cv::Mat resized, gray;
    //cv::resize(src, resized, cv::Size(), 2, 2);
    cv::cvtColor(src, gray, CV_RGB2GRAY);
    
    
    cv::threshold(gray, bin, 0, 128, cv::THRESH_TOZERO_INV|cv::THRESH_OTSU);
    //cv::adaptiveThreshold(gray, bin, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 7, 8);
    
    cv::namedWindow("Binaried", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Binaried", 0, 200);
    cv::imshow("Binaried", bin);
    
    cv::findContours(bin, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    std::vector<double> areas;
    for(int i = 0; i >= 0; i = hierarchy[i][0]) {
        areas.push_back(cv::contourArea(cv::Mat(contours[i])));
    }
    max_area = *std::max_element(areas.begin(), areas.end());
    std::cout << "max" << max_area << std::endl;
    
    cv::namedWindow("Contoured", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Contoured", 0, 400);
    cv::createTrackbar("Contoured trackbar", "Contoured", &slider, 100, on_trackbar);
    on_trackbar(slider, 0);
    cv::waitKey(0);
    
    tesseract::TessBaseAPI ocr;
    ocr.Init("./", "eng", tesseract::OEM_TESSERACT_CUBE_COMBINED);
    ocr.ReadConfigFile("/Users/cube/Documents/xcode/opencv_test/opencv_test/configs", false);
    //char* text = ocr.TesseractRect(gray.data, 1, (int)gray.step1(), 0, 0, gray.cols, gray.rows);
    //ocr.SetImage(gray.data, gray.size().width, gray.size().height, gray.channels(), gray.step1());
    ocr.SetImage(dst.data, dst.size().width, dst.size().height, dst.channels(), (int)dst.step1());
    ocr.Recognize(0);
    const char* text = ocr.GetUTF8Text();
    ocr.End();
    
    std::cout << text << std::endl;
    
    cv::Mat result = cv::Mat::zeros(100, 300, CV_8UC3);
    cv::putText(result, text, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0,255,255), 2, CV_AA);
    
    cv::namedWindow("result", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("result", 0, 600);
    cv::imshow("result", result);
    
    cv::waitKey(0);
    return 0;
}

void on_trackbar(int, void*) {
    double threshold = (double) slider / 100 * max_area;
    
    dst = cv::Mat::zeros(bin.rows, bin.cols, CV_8UC3);
    cv::Scalar color(255, 255, 255);
    for (int i = 0; i >= 0; i = hierarchy[i][0]) {
        if (cv::contourArea(cv::Mat(contours[i])) > threshold) {
            cv::drawContours(dst, contours, i, color, CV_FILLED, 8, hierarchy);
        }
    }
    cv::imshow("Contoured", dst);
}

