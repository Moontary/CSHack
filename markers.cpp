#include <opencv2/aruco.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>


using namespace cv;

void drawSprite(Mat img, Mat sprite, int x, int y, double a)
{
    int nx = sprite.cols*a;
    int ny = sprite.rows*a;
    for(int dx=0; dx<nx; dx++){
        for(int dy=0; dy<ny; dy++){
            int sx = dx/a;
            int sy = dy/a;
            if(sx>=sprite.cols || sy>=sprite.rows){
                continue;
            }
            if(sprite.at<Vec4b>(sy, sx)[3]>10){
                if(x+dx>=0 && y+dy>=0 && x+dx<img.cols && y+dy<img.rows){
                    img.at<Vec3b>(y+dy, x+dx)[0] = 
                        sprite.at<Vec4b>(sy, sx)[0];
                    img.at<Vec3b>(y+dy, x+dx)[1] = 
                        sprite.at<Vec4b>(sy, sx)[1];
                    img.at<Vec3b>(y+dy, x+dx)[2] = 
                        sprite.at<Vec4b>(sy, sx)[2];
                }
            }
        }
    }
}

int main(int argc, char *argv[]){
    if(argc!=3){
        std::cout<<"Usage: "<<argv[0]<<" <input_image>  <output_image>\n";
    }
    Mat img = imread(argv[1]);
    std::vector<int> markerIds; 
    std::vector<std::vector<Point2f> > markerCorners;
    std::vector<std::vector<Point2f> > rejected; 
    cv::Ptr<cv::aruco::DetectorParameters> parameters =
        aruco::DetectorParameters::create(); 
    cv::Ptr<cv::aruco::Dictionary> dictionary = 
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250); 
    cv::aruco::detectMarkers(img, dictionary, markerCorners, 
                             markerIds, parameters, rejected); 
    Mat sprite = imread("data/monster.png", IMREAD_UNCHANGED);
    for(unsigned int i=0; i<markerCorners.size(); i++){
        int minX = markerCorners[i][0].x;
        int minY = markerCorners[i][0].y;
        int maxX = minX;
        int maxY = minY;
        for(unsigned int j=0; j<markerCorners[i].size(); j++){
            if(markerCorners[i][j].x<minX){
                minX = markerCorners[i][j].x;
            }
            if(markerCorners[i][j].y<minY){
                minY = markerCorners[i][j].y;
            }
            if(markerCorners[i][j].x>maxX){
                maxX = markerCorners[i][j].x;
            }
            if(markerCorners[i][j].y>maxY){
                maxY = markerCorners[i][j].y;
            }
        }
        int size = std::max(maxX-minX, maxY-minY);
        double a = 4*((double)size)/sprite.cols;
        int x = (minX + maxX)/2 - sprite.cols*a/2;
        int y = (minY + maxY)/2 - sprite.rows*a/2;
        std::cout<<x<<"  "<<y<<"  "<<a<<"\n";
        drawSprite(img, sprite, x, y, a);
    }

    imwrite(argv[2], img);
}
