#include "head.h"

int main() {

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        return -1;
    }
    Mat frame;
    bool stop = false;
    while (!stop) {
        cap >> frame;
//        imwrite("origin.jpg",frame);
        imshow("当前视频", frame);
//        Mat src = imread("figure1.png");
        Mat src=frame.clone();
//        resize(src,src,Size(src.cols/2,src.rows/2));
        good_position(src);
        if (waitKey(30) >= 0)
            stop = true;
    }


//    Mat src = imread("origin.jpg");
//    resize(src,src,Size(src.cols/2,src.rows/2));
//    good_position(src);
//
//
//    int new_row, new_col;
//    int** s_a = matrix(src,new_row,new_col);
//    Map my_map(s_a,new_row,new_col);
//    my_map.road_grade();
//    my_map.start_end();
//    my_map.print_map();
//    my_map.road_map();
//    my_map.print_road();
//    my_map.q_s_num();
//    my_map.print_route();
    return 0;
}