//
// Created by Leo on 2019/4/5.
//

#include "head.h"
#include "number.h"
bool lr=false;

int distance(Point pt1, Point pt2) {
    int dis=abs(pt1.x-pt2.x)+abs(pt1.y-pt2.y);
    return dis;
}

int distance(point pt1, point pt2) {
    int dis=abs(pt1.x-pt2.x)+abs(pt1.y-pt2.y);
    return dis;
}

double distance(pointd pt1, pointd pt2) {
    double dis=abs(pt1.x-pt2.x)+abs(pt1.y-pt2.y);
    return dis;
}

vector<Point> corner(vector<vector<Point>> vec, vector<Point2f> &result, Mat mat) {
//    leftup,rightup,leftdown,rightdown
    vector<Point2f> contour ;
    Point p0(0,0),p1(mat.cols,0),p2(0,mat.rows),p3(mat.cols,mat.rows);
    for (int i = 1; i < vec.size(); ++i) {
        vec[0].insert(vec[0].end(),vec[i].begin(),vec[i].end());
    }
    for (int i = 0; i < vec[0].size(); ++i) {
        contour.push_back(Point2d(float(vec[0][i].x),float(vec[0][i].y)));
    }
    for (int i = 0; i < 4; ++i) {
        result.push_back(contour[i]);
    }

    for (int j = 0; j < contour.size(); ++j) {
        if(distance(contour[j],p0)<distance(result[0],p0))
            result[0]=contour[j];
        if(distance(contour[j],p1)<distance(result[1],p1))
            result[1]=contour[j];
        if(distance(contour[j],p2)<distance(result[2],p2))
            result[2]=contour[j];
        if(distance(contour[j],p3)<distance(result[3],p3))
            result[3]=contour[j];
    }
    return vector<Point>();
}

void draw_vec(vector<Point> vec, Mat &mat) {
    for (int i = 0; i < vec.size(); ++i) {
        circle(mat, vec[i],3,Scalar(255,0,0),-1,CV_AA);
    }
}

void draw_vec(vector<Point2f> vec, Mat &mat) {
    for (int i = 0; i < vec.size(); ++i) {
        circle(mat, vec[i],3,Scalar(255,0,0),-1,CV_AA);
    }
}

void perspective(vector<Point2f> corner, Mat src, Mat &correct) {
    vector<Point2f> src_vec;
    src_vec.push_back(Point2d(0,0));
    src_vec.push_back(Point2d(src.cols,0));
    src_vec.push_back(Point2d(0,src.rows));
    src_vec.push_back(Point2d(src.cols,src.rows));
    Mat warpMatrix = getPerspectiveTransform(corner,src_vec);
    warpPerspective(src, correct, warpMatrix, src.size(), INTER_LINEAR, BORDER_CONSTANT);
    resize(correct,correct,Size(600,600));
}

vector<Point2f> good_position(Mat &src) {
    Mat mask;
    Mat bgr;
    Mat hsv;
    src.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    //颜色空间转换
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(hmin, smin / float(smin_Max), vmin / float(vmin_Max)), Scalar(hmax, smax / float(smax_Max), vmax / float(vmax_Max)), mask);
//    imshow("mask",mask);


    //只保留
    Mat dst;
    dst = Mat(src.size(), CV_32FC3,Scalar(255,255,255));
    for (int r = 0; r < bgr.rows; r++)
    {
        for (int c = 0; c < bgr.cols; c++)
        {
            if (mask.at<uchar>(r, c) == 255)
            {
                dst.at<Vec3f>(r, c) = bgr.at<Vec3f>(r, c);
            }
        }
    }
//    imshow("dst", dst);

    Mat gray_image(src.size(),CV_8UC1);
    cvtColor(dst,gray_image,CV_HSV2BGR);
    dst.convertTo(gray_image, CV_8UC1,255); //should first transfer it to CV_8UC1 for "find_contour"
    cvtColor(gray_image,gray_image,CV_BGR2GRAY);
//    inRange(gray_image,threshold_low,200,gray_image); //inrange has an upper bound and a lower bound
    threshold(gray_image,gray_image,230,255,CV_THRESH_BINARY);
//    imshow("threshold",gray_image);
    Mat element = getStructuringElement(MORPH_RECT, Size(5,5));
    dilate(gray_image,gray_image,element);
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(gray_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    vector <vector<Point>>::iterator iter = contours.begin();
    for (; iter != contours.end();) {
        double g_dConArea = contourArea(*iter);
//        cout<<g_dConArea<<endl;
        if (g_dConArea < 20000) {
            iter = contours.erase(iter);
        } else {
            ++iter;
        }
    }
    vector<Point2f> result;

    Mat resultImage = Mat ::zeros(gray_image.size(),CV_8U);
//    drawContours(resultImage, contours, -1, Scalar(255, 255, 255));

    Mat dots(gray_image.size(),CV_8UC1);
    corner(contours,result,resultImage);
    perspective(result,src ,src);
    Mat correct=src.clone();
    imshow("correct",src);
//    imwrite("result_car1.png",correct);

//    waitKey();
    return result;
}

Point2d car_front(Mat src) {
    Mat green=src.clone();

//  GREEN(FRONT)

    Mat mask;
    Mat bgr;
    Mat hsv;
    src.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    //颜色空间转换
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(hmin_g, smin_g / float(smin_Max), vmin / float(vmin_Max)), Scalar(hmax, smax_g / float(smax_Max), vmax / float(vmax_Max)), mask);
//    imshow("mask",mask);


    //只保留
    Mat dst;
    dst = Mat(src.size(), CV_32FC3,Scalar(0,0,0));
    for (int r = 0; r < bgr.rows; r++)
    {
        for (int c = 0; c < bgr.cols; c++)
        {
            if (mask.at<uchar>(r, c) == 255)
            {
                dst.at<Vec3f>(r, c) = bgr.at<Vec3f>(r, c);
            }
        }
    }
    Mat gray_image(src.size(),CV_8UC1);
    cvtColor(dst,gray_image,CV_HSV2BGR);
    dst.convertTo(gray_image, CV_8UC1,255); //should first transfer it to CV_8UC1 for "find_contour"
//    imshow("gray_image",gray_image);
//    waitKey();
    cvtColor(gray_image,gray_image,CV_BGR2GRAY);
    threshold(gray_image,gray_image,110,255,CV_THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
    dilate(gray_image,gray_image,element);
    erode(gray_image,gray_image,element);
//    imshow("green", gray_image);
//    waitKey();
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(gray_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    vector <vector<Point>>::iterator iter = contours.begin();
    for (; iter != contours.end();) {
        double g_dConArea = contourArea(*iter);
//        cout<<g_dConArea<<endl;
        if (g_dConArea < 200) {
            iter = contours.erase(iter);
        } else {
            ++iter;
        }
    }
    Mat resultImage = Mat ::zeros(gray_image.size(),CV_8U);
    drawContours(resultImage, contours, -1, Scalar(255, 255, 255));
//    imshow("contour",resultImage);
    RotatedRect bound_rect=minAreaRect(contours[0]);
//    circle(gray_image,bound_rect.center,3,Scalar(0,0,0),-1,LINE_AA);
//    imshow("draw",gray_image);
//    waitKey();
    destroyWindow("draw");
    Point2d center(0.0,0.0);
    center= Point2d(bound_rect.center.y,bound_rect.center.x);
    cout<<"Front: "<<center<<endl;
    return center;
}

Point2d car_back(Mat src) {
    Mat red=src.clone();

//  RED(BACK)

    Mat mask;
    Mat bgr;
    Mat hsv;
    src.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    //颜色空间转换
    cvtColor(bgr, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(hmin, smin_r / float(smin_Max), vmin / float(vmin_Max)), Scalar(hmax_r, smax / float(smax_Max), vmax_r / float(vmax_Max)), mask);
//    imshow("mask",mask);
//    waitKey();

    //只保留
    Mat dst;
    dst = Mat(src.size(), CV_32FC3,Scalar(0,0,0));
    for (int r = 0; r < bgr.rows; r++)
    {
        for (int c = 0; c < bgr.cols; c++)
        {
            if (mask.at<uchar>(r, c) == 255)
            {
                dst.at<Vec3f>(r, c) = bgr.at<Vec3f>(r, c);
            }
        }
    }
    Mat gray_image(src.size(),CV_8UC1);
    cvtColor(dst,gray_image,CV_HSV2BGR);
//    imshow("cvcolor",dst);
    dst.convertTo(gray_image, CV_8UC1,255); //should first transfer it to CV_8UC1 for "find_contour"
    cvtColor(gray_image,gray_image,CV_BGR2GRAY);
    threshold(gray_image,gray_image,50,255,CV_THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
    dilate(gray_image,gray_image,element);
    erode(gray_image,gray_image,element);
//    imshow("red", gray_image);
//    waitKey();
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(gray_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    vector <vector<Point>>::iterator iter = contours.begin();
    for (; iter != contours.end();) {
        double g_dConArea = contourArea(*iter);
//        cout<<g_dConArea<<endl;
        if (g_dConArea < 200) {
            iter = contours.erase(iter);
        } else {
            ++iter;
        }
    }
    Mat resultImage = Mat ::zeros(gray_image.size(),CV_8U);
    drawContours(resultImage, contours, -1, Scalar(255, 255, 255));
//    imshow("contour",resultImage);
    RotatedRect bound_rect=minAreaRect(contours[0]);
//    circle(gray_image,bound_rect.center,3,Scalar(0,0,0),-1,LINE_AA);
//    imshow("draw2",gray_image);
//    waitKey();
    destroyWindow("draw2 ");
    Point2d center( 0.0,0.0);
    center= Point2d(bound_rect.center.y,bound_rect.center.x);
    //    cout<<bound_rect.size;
    cout<<"Back: "<<center<<endl;
    return center;
}

int** matrix(Mat src, int &new_row, int &new_col) {
    Mat gray;
    gray=src.clone();
    imshow("gray",gray);
    int row = gray.rows;
    int col = gray.cols;
//    cout<<"row: "<<row<<" col: "<<col<<endl;
    int **array = new int *[row];
    for (int i = 0; i < row; i ++){
        array[i] = new int[col];
    }

    for (int i = 0; i < row; i ++){
        for (int j = 0; j < col; j ++){
            array[i][j] = gray.at<uchar>(i, j)/255;
        }
    }


    new_row=row/core_sz;
    new_col=row/core_sz;
    int **small_array = new int *[new_row];
    for (int i = 0; i < new_col; i ++){
        small_array[i] = new int[new_col];
    }
    for (int i = 0; i < row; i+=core_sz) {
        for (int j = 0; j < col; j+=core_sz) {
            int score=0;
            for (int k = i; k < i+core_sz; ++k) {
                for (int l = j; l < j+core_sz; ++l) {
                    score+=array[k][l];
                }
            }
            if (score>=core_sz*core_sz/2)
                small_array[i/core_sz][j/core_sz]=1;
            else small_array[i/core_sz][j/core_sz]=0;
        }
    }

//    for (int k = 0; k < new_row; ++k) {
//        for (int i = 0; i < new_col; ++i) {
//            cout<<small_array[k][i]<<' ';
//        }
//        cout<<endl;
//    }

//    waitKey(0);
    return small_array;
}

void Map::start_end() {
//  Up
    point point1;
    vector<point> pv;
    int start=-1, end=-1;
    for (int i = 0; i < col; ++i) {
        if (matrix[0][i]!=0) {
            start = end = i;
            point1.x=0;
            break;
        }
    }
    if(start!=-1) {
        for (int j = start; j < col; ++j) {
            if (matrix[0][j] == 0) {
                end = j - 1;
                break;
            }
        }
    }
    if(start!=-1&&end!=-1) {
        matrix[0][(end + start) / 2] = -1;
        point1.y=(end + start) / 2;
        pv.push_back(point1);
        start = end = -1;
    }

//  Left
    for (int i = 0; i < row; ++i) {
        if (matrix[i][0]!=0){
            start = end = i;
            point1.y=0;
            break;
        }
    }
    if(start!=-1) {
        for (int j = start; j < row; ++j) {
            if (matrix[j][0] == 0){
                end = j - 1;
                break;
            }
        }
    }
    if(start!=-1&&end!=-1) {
        matrix[(end + start) / 2][0] = -1;
        point1.x=(end + start) / 2;
        pv.push_back(point1);
        start = end = -1;
    }
//  Down
    for (int i = 0; i < col; ++i) {
        if (matrix[row-1][i]!=0){
            start = end = i;
            point1.x=row-1;
            break;
        }
    }
    if(start!=-1) {
        for (int j = start; j < col; ++j) {
            if (matrix[row - 1][j] == 0){
                end = j - 1;
                break;
            }
        }
    }
    if(start!=-1&&end!=-1) {
        matrix[row-1][(end + start) / 2] = -1;
        point1.y=(end + start) / 2;
        pv.push_back(point1);
        start = end = -1;
    }
//  Right
    for (int i = 0; i < row; ++i) {
        if (matrix[i][col-1]!=0){
            start = end = i;
            point1.y=col-1;
            break;
        }
    }
    if(start!=-1) {
        for (int j = start; j < row; ++j) {
            if (matrix[j][col - 1] == 0) {
                end = j - 1;
                break;
            }
        }
    }
    if(start!=-1&&end!=-1) {
        matrix[(end + start) / 2][col-1] = -1;
        point1.x=(end + start) / 2;
        pv.push_back(point1);
        start = end = -1;
    }
    exit=pv[0];
    entrance=pv[1];
//    cout<<exit.x<<' '<<exit.y<<endl;
}

void Map::core_processing(int grade) {
//    cout<<matrix[7][23]<<endl;
    for (int i = 1; i < row-1; ++i) {
        for (int j = 1; j < col-1; ++j) {
            if(matrix[i][j]==grade){ //eight blocks nearby are all the same
                if(matrix[i-1][j-1]>=grade&&matrix[i][j-1]>=grade&&matrix[i+1][j-1]>=grade
                &&matrix[i-1][j]>=grade&&matrix[i+1][j]>=grade
                &&matrix[i-1][j+1]>=grade&&matrix[i][j+1]>=grade&&matrix[i+1][j+1]>=grade)
                    matrix[i][j]++;
            }
        }
    }
}

void Map::road_grade() {
    core_processing(1);
    core_processing(2);
    core_processing(3);
    core_processing(4);
    core_processing(5);
//    core_processing(6);
}

void Map::uniwalk(char ch, point &pt, point pt2, int seq, char s_e) {
    int x=pt.x;
    int y=pt.y;
    switch (ch){
        case 'u':{
            while (1){
                if(y==0||(matrix[x-1][y]<matrix[x][y]&&road[x-1][y]>=0)||road[x][y]<-1||point(x,y)==pt2){
                    break;
                }
                road[x][y]=seq;
                if(s_e=='s')
                    start_q.push(point(x,y));
                else if(s_e=='e')
                    end_s.push(point(x,y));
                x--;
            }
            if(s_e=='s')
                start_q.push(point(x,y));
            else if(s_e=='e')
                end_s.push(point(x,y));
            break;
        }
        case 'd':{
            while (1){
                if(y==row||(matrix[x+1][y]<matrix[x][y]&&road[x+1][y]>=0)||road[x][y]<-1||point(x,y)==pt2){
                    break;
                }
                road[x][y]=seq;
                if(s_e=='s')
                    start_q.push(point(x,y));
                else if(s_e=='e')
                    end_s.push(point(x,y));
                x++;
            }
            if(s_e=='s')
                start_q.push(point(x,y));
            else if(s_e=='e')
                end_s.push(point(x,y));
            break;
        }
        case 'l':{
            while (1){
                if(x==0||(matrix[x][y-1]<matrix[x][y]&&road[x][y-1]>=0)||road[x][y]<-1||point(x,y)==pt2){
                    break;
                }
                road[x][y]=seq;
                if(s_e=='s')
                    start_q.push(point(x,y));
                else if(s_e=='e')
                    end_s.push(point(x,y));
                y--;
            }
            if(s_e=='s')
                start_q.push(point(x,y));
            else if(s_e=='e')
                end_s.push(point(x,y));
            break;
        }
        case 'r':{
            while (1){
                if(x==col||(matrix[x][y+1]<matrix[x][y]&&road[x][y+1]>=0)||road[x][y]<-1||point(x,y)==pt2){
                    break;
                }
                road[x][y]=seq;
                if(s_e=='s')
                    start_q.push(point(x,y));
                else if(s_e=='e')
                    end_s.push(point(x,y));
                y++;
            }
            if(s_e=='s')
                start_q.push(point(x,y));
            else if(s_e=='e')
                end_s.push(point(x,y));
            break;
        }
    }
//    road[x][y]=seq;
    pt.x=x;
    pt.y=y;

}

char Map::max_grade(point pt) {
    char ch=' ';
    int x=pt.x;
    int y=pt.y;
    int u,d,l,r;
    if (x==0)
        u=-100;
    else u=road[x-1][y];
    if (x==row)
        d=-100;
    else d=road[x+1][y];
    if (y==0)
        l=-100;
    else l=road[x][y-1];
    if (y==col)
        r=-100;
    else r=road[x][y+1];
    int max1=max(u,d);
    int max2=max(l,r);
    int max3=max(max1,max2);
    vector<char> choice;
    if(max3==l)
        choice.push_back('l');
    if(max3==r)
        choice.push_back('r');
    if(max3==u)
        choice.push_back('u');
    if(max3==d)
        choice.push_back('d');
    int sz=choice.size();
    int pos=rand()%sz;
//    cout<<pos<<endl;
    return choice[pos];
}

char Map::walk_choice(point pt) {
    char ch='u';
    char choice=max_grade(pt);
    if(pt.x==0)
        ch='d';
    else if(pt.x==row-1)
        ch='u';
    else if(pt.y==0)
        ch='r';
    else if(pt.y==col-1)
        ch='l';
    else if(choice=='r')
        ch='r';
    else if(choice=='l')
        ch='l';
    else if(choice=='d')
        ch='d';
    else if(choice=='u')
        ch='u';
    return ch;
}

void Map::final_road(int end_point) {

//    q_s_num();
    if(end_point==0){

        while(end_s.top()!=start_q.back()){
//            cout<<end_s.top()<<start_q.back()<<endl;
            end_s.pop();
        }//delete unnecessary points
        end_s.pop();
        while(!end_s.empty()){
            start_q.push(end_s.top());
            end_s.pop();
        }
    }

    if(end_point==1){
        queue<point> tmp;
        while(end_s.top()!=start_q.front()){
            tmp.push(start_q.front());
            start_q.pop();
        }//delete unnecessary points
        while(!end_s.empty()){
            tmp.push(end_s.top());
            end_s.pop();
        }
        start_q=tmp;
    }
//    generate the route
    int i,j;
    while (!start_q.empty()){
        i=start_q.front().x;
        j=start_q.front().y;
        route[i][j]=1;
        route_q.push(pointd(i,j));
        start_q.pop();
    }
}

int** Map::road_map() {
    srand(time(NULL));
    int ep;
    bool flag=true;
    while(flag) {
        road = new int*[row];
        for (int i = 0; i < row; ++i) {
            road[i] = new int[col];
            for (int j = 0; j < col; ++j) {
                road[i][j]=matrix[i][j];
            }
        }
        route = new int*[row];
        for (int i = 0; i < row; ++i) {
            route[i] = new int[col];
            for (int j = 0; j < col; ++j) {
                route[i][j]=0;
            }
        }
        int end_point; // 0:entrance 1:exit
        point p1 = entrance;
        point p2 = exit;
        int g1 = -2;
        int g2 = -20;
        int count = 0;
        while(!start_q.empty())
            start_q.pop();
        while(!end_s.empty())
            end_s.pop();
        while(!route_q.empty())
            route_q.pop();
        turning_point.clear();
        while (count++ <= 6) {

            uniwalk(walk_choice(p1), p1,p2, g1, 's');
            g1--;
            turning_point.push_back(pointd(p1.x,p1.y));
//        print_road();
            if (-20>=road[p1.x][p1.y]&&road[p1.x][p1.y]>-10) {
                end_point = 0;
                break;
            }

            uniwalk(walk_choice(p2), p2,p1, g2, 'e');
            turning_point.push_back(pointd(p2.x,p2.y));
            g2++;
//        print_road();
            if (p1 == p2 || (-10<=road[p2.x][p2.y]&&road[p2.x][p2.y]<0)) {
                end_point = 1;
                flag=false;
                break;
            }
        }
//        print_road();
        ep=end_point;
    }
//    delete same elements
    if(turning_point[turning_point.size()-2]==turning_point[turning_point.size()-1])
        turning_point.erase(turning_point.end()-1);
    for (int k = 0; k < turning_point.size(); ++k) {
        turning_point[k]=pointd(turning_point[k].x,turning_point[k].y);
    }
    final_road(ep);
    return road;
}

void Map::sort_turning() {
    turning_point.clear();
    cout<<turning_point.size()<<endl;
    queue<pointd> tmp2=route_q;
    pointd pt;
    int i=0;
    while(!tmp2.empty()){
        i++;
        pt=tmp2.front();
        tmp2.pop();
        if(i>10&&i%6==0)
            turning_point.push_back(pointd(pt.x*core_sz,pt.y*core_sz));
        }
    turning_point.erase(turning_point.end()-1);
    turning_point.erase(turning_point.end()-1);
    turning_point.push_back(pointd(route_q.back().x*core_sz,route_q.back().y*core_sz));

}

void signal(char ch, int nwrite, int nread, int fd) {
    char receive,stop;
    stop='i';
    if(ch!='i') {
        if(ch!='a'&&ch!='c') {
            nwrite = write(fd, &ch, 1);
            printf("nwrite: %d\n write char: %c\n", nwrite, ch);
            nread = read(fd, &receive, 1);
            printf("nread: %d\n read char: %c\n", nread, receive);
            usleep(150000);
        }
        else {
            nwrite = write(fd, &ch, 1);
            printf("nwrite: %d\n write char: %c\n", nwrite, ch);
            nread = read(fd, &receive, 1);
            printf("nread: %d\n read char: %c\n", nread, receive);
            usleep(100000);
        }
//        waitKey(100);
    }

    nwrite = write(fd, &stop, 1);
    printf("nwrite: %d\n write char: %c\n", nwrite, stop);
    nread = read(fd, &receive, 1);
    printf("nread: %d\n read char: %c\n", nread, receive);
    usleep(10000);
}

int turnto(pointd dst,Car &car, int nwrite, int nread, int fd) {
    bool flag=false;
//    signal('i', nwrite, nread, fd);
    pointd Vd=pointd((dst.x-car.center.x),(dst.y-car.center.y));//vector car->destination
    cout<<"Vd: "<<Vd<<endl;
    pointd Vc=pointd(car.front.x-car.back.x,car.front.y-car.back.y);//vector back->front
    cout<<"Vc: "<<Vc<<endl;
    double angle = (Vd.x*Vc.x+Vd.y*Vc.y)/sqrt(Vd.x*Vd.x+Vd.y*Vd.y)/sqrt(Vc.x*Vc.x+Vc.y*Vc.y);//cos theta
    cout<<angle<<endl;
    if(!(angle>0.92)){
        flag=true;
        if(lr) {
            signal('a', nwrite, nread, fd);
//            lr=false;
            cout<<"turn left!"<<endl;
        }
        else{
            signal('c', nwrite, nread, fd);
//            lr=true;
            cout<<"turn right!"<<endl;
        }
//        usleep(100000);

        pointd Vd=pointd((dst.x-car.center.x),(dst.y-car.center.y));//vector car->destination
        pointd Vc=pointd(car.front.x-car.back.x,car.front.y-car.back.y);//vector back->front
    }
    return flag;
}

int nearest(pointd pt, vector<pointd> vec) {
    int i=0;
    double min=10000;
    vector<double> dis;
    cout<<vec.size()<<endl;
    for (int j = 0; j < vec.size(); ++j) {
        dis.push_back(distance(pt,vec[j]));
        cout<<vec[j]<<distance(pt,vec[j])<<endl;
    }
    for (int j = 0; j < dis.size(); ++j) {
        if(dis[j]<min){
           i=j;
           min=dis[j];
        }
    }
    return i;
}

bool Map::turning(pointd ch) {
    bool flag=false;
    for (int i = 0; i < turning_point.size(); ++i) {
        if(ch==turning_point[i]){
            flag=true;
            break;
        }
    }
    return flag;
}

void Map::car_run(Mat src, int nwrite, int nread, int fd) {
    Point2d front = car_front(src);
    Point2d back = car_back(src);
    pointd center((front.x+back.x)/2,(front.y+back.y)/2);
    cout<<center<<endl;
    bool flag=false;
    double route_x = center.x;
    double route_y = center.y;
//    destroyAllWindows();
    double slope;
//    cout<<"initial begin!"<<endl;
    car1.init(front,back,center,slope);
    if(front.x-back.x==0)
        slope=9999;
    else slope= (front.y-back.y)/(front.x-back.x);
//    cout<<"slope: "<<slope<<endl;
    int pos=nearest(center,turning_point);
//    int pos=0;
    pointd pointd1 = turning_point[pos];
    pointd current = pointd(pointd1.x,pointd1.y);
    circle(src,Point2d(current.y,current.x),2,Scalar(0,0,0),-1);
//    imshow("current",src);
//    waitKey();
    cout<<"current: "<<current<<endl;
    if(distance(current,center)<=50){
        turning_point.erase(turning_point.begin()+pos);
        cout<<"next!"<<endl;
        return;
    }
    else if(distance(current,center)>=50)
        flag=turnto(current,car1, nwrite, nread, fd);
    if(!flag) {
        signal('e', nwrite, nread, fd);
        lr=(!lr);
//        usleep(100000);//0.1s
        cout << "Advance!" << endl;
    }

}

