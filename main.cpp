#include "head.h"

int fd;
int nread, nwrite;
char buff[16];
char buu = 'a';
char buu2=' ';
char *dev  = "/dev/tty.BT04-A-Port"; //serial_port name

int main() {

    fd = OpenDev(dev);
    set_speed(fd,9600);
    if (set_Parity(fd,8,1,'N') == FALSE)
    {
        printf("Set Parity Error/n");
        exit (0);
    }
    sleep(1);
    for (int i = 0; i < 12; ++i) {
        read(fd, &buu2, 1);
        printf("buff: %c\n",buu2);
    }

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        return -1;
    }
    Mat frame;
    bool stop = false;
    bool map_flag= true;
    bool road_flag=true;
    bool route_flag=true;
    int new_row, new_col;
    Map my_map;
    Mat src;
    vector<Point2f> corner_set;
    cout<<"begin!"<<endl;
    while (!stop) {
        while(1){
            cap >> frame;
            src = frame.clone();
            imshow("camera",frame);
            if(waitKey(100)>0)
                break;
        }
        cap >> frame;
//        imwrite("new_puzzle.jpg", frame);
        src = frame.clone();
//        imshow("camera",frame);
//        waitKey();

//        src = imread("new_puzzle.jpg");

//        resize(src,src,Size(src.cols/2,src.rows/2));

//        src=frame.clone();
//        if (map_flag){
//        imshow("当前视频", frame);

        Mat gray_image;
        cvtColor(src,gray_image,CV_BGR2GRAY);
//        imshow("gray",gray_image);
        threshold(gray_image,gray_image,110,255,CV_THRESH_BINARY);
        imshow("threshold",gray_image);
        waitKey();
        vector<vector<Point>>contours;
        vector<Vec4i> hierarchy;
        findContours(gray_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        vector <vector<Point>>::iterator iter = contours.begin();
        for (; iter != contours.end();) {
            double g_dConArea = contourArea(*iter);
//        cout<<g_dConArea<<endl;
            if (g_dConArea < 10000 || g_dConArea>70000) {
                iter = contours.erase(iter);
            } else {
                ++iter;
            }
        }
        vector<Point2f> result;

        Mat resultImage = Mat ::zeros(gray_image.size(),CV_8U);
        drawContours(resultImage, contours, -1, Scalar(255, 255, 255));
        imshow("contour",resultImage);
        waitKey();
        corner(contours,result,resultImage);

//        perspective(result,src ,src);
//        Mat correct=src.clone();
//        imshow("correct",src);

        perspective(result,gray_image,gray_image);
        Mat correct=gray_image.clone();
        imshow("correct",gray_image);

//        int new_row, new_col;
        int** s_a = matrix(gray_image,new_row,new_col);
        my_map.Map_init(s_a,new_row,new_col);
        my_map.road_grade();
        my_map.start_end();
//        my_map.print_map();
        my_map.road_map();
//        my_map.print_road();
        my_map.print_route();
//        my_map.print_route_q();
//        my_map.sort_turning();
//        my_map.print_turning_point();
        cout<<endl;

        if(waitKey()>=0) {
            stop= true;
            cout<<"route initial finish!"<<endl;
            corner_set=result;
            destroyAllWindows();
//            break;
        }

    }
//    waitKey();
    cout<<"car_start!"<<endl;
    while(true){
        cap >> frame;
        src=frame.clone();
//        src=imread("red_green.jpg");
        perspective(corner_set,src,src);
        imshow("src",src);
//        imwrite("red_green.jpg",src);
        waitKey(30);

//        car_front(src);
//        car_back(src);
        my_map.car_run(src, nwrite, nread, fd);
//        sleep(1);
        waitKey(200);
//        usleep(200000);
    }

    return 0;
}