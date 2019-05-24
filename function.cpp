//
// Created by Leo on 2019/4/5.
//

#include "head.h"
#include "number.h"

int distance(Point pt1, Point pt2) {
    int dis=abs(pt1.x-pt2.x)+abs(pt1.y-pt2.y);
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
//        cout<<"col"<<mat.cols<<" rows: "<<mat.rows<<endl;
//        cout<<'('<<contour[j].x<<','<<contour[j].y<<')'<<endl;

//        if (contour[j].y/contour[j].x <result[1].y/result[1].x)
//            result[1] = contour[j];
//        if (contour[j].y/contour[j].x >result[2].y/result[2].x)
//            result[2] = contour[j];
//        if (contour[j].y/(mat.cols+2-contour[j].x) <result[0].y/(mat.cols+2-result[0].x))
//            result[0] = contour[j];
//        if (contour[j].y/(mat.cols+2-contour[j].x) >result[3].y/(mat.cols+2-result[3].x))
//            result[3] = contour[j];

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

void good_position(Mat &src) {
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
    inRange(gray_image,threshold_low,200,gray_image); //inrange has an upper bound and a lower bound
//    threshold(gray_image,gray_image,100,255,CV_THRESH_BINARY);
//    imshow("threshold",gray_image);

    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
//    dilate(gray_image,gray_image,element);
//    erode(gray_image,gray_image,element);
//    imshow("Morphology",gray_image);
    morphologyEx(gray_image,gray_image,MORPH_CLOSE,element);
//    Mat morph;
//    morphologyEx(gray_image,morph,MORPH_OPEN,element);

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
    drawContours(resultImage, contours, -1, Scalar(255, 255, 255));

    Mat dots(gray_image.size(),CV_8UC1);
    corner(contours,result,resultImage);
//    for (int i = 0; i < result.size(); ++i) {
//        cout<<'('<<result[i].x<<','<<result[i].y<<')'<<endl;
//    }
    draw_vec(result,resultImage);
//    imshow("contour",resultImage);
//    imshow("Morphology",morph);

    perspective(result,src ,src);
    Mat correct=src;
    imshow("correct",src);
//    imwrite("result.png",correct);

//    waitKey();
}

int** matrix(Mat src, int &new_row, int &new_col) {
    Mat gray;
    cvtColor(src,gray,CV_BGR2GRAY);
    threshold(gray,gray,threshold_low,255,CV_THRESH_BINARY);
//    imshow("gray",gray);
//    waitKey(0);
//    Mat drawing = gray;
    //src:600*600
//    Mat kern = (Mat_<char>(3,3) << 1,1,1,1,1,1,1,1,1);
//    Mat kern = getStructuringElement(MORPH_RECT,Size(20,20));
//    filter2D(gray,gray,-1,kern);
//    rectangle(drawing,Point(500,0),Point(540,40),Scalar(255,0,0),-1);

    int row = gray.rows;
    int col = gray.cols;
    cout<<"row: "<<row<<" col: "<<col<<endl;
    int **array = new int *[row];
    for (int i = 0; i < row; i ++){
        array[i] = new int[col];
    }


    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    vector <vector<Point>>::iterator iter = contours.begin();
    for (; iter != contours.end();) {
        double g_dConArea = contourArea(*iter);
//        cout<<g_dConArea<<endl;
        if (g_dConArea < 10000) {
            iter = contours.erase(iter);
        } else {
            ++iter;
        }
    }
    vector<Point2f> result;
    Mat resultImage = Mat ::zeros(gray.size(),CV_8U);
    drawContours(resultImage, contours, -1, Scalar(255, 255, 255),-1);
//    imshow("contour",resultImage);
    Mat car = Mat ::zeros(gray.size(),CV_8U);
    car=resultImage-gray;
//    imshow("gray",car);
    gray=resultImage;
    // change the mat into an array
    for (int i = 0; i < row; i ++){
        for (int j = 0; j < col; j ++){
            array[i][j] = gray.at<uchar>(i, j)/255;
        }
    }
//    for (int k = 0; k < 100; ++k) {
//        for (int i = 0; i < 100; ++i) {
//            cout<<array[k][i]<<'\t';
//        }
//        cout<<endl;
//    }

    int core_sz=20;
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
}

void Map::uniwalk(char ch, point &pt, int seq, char s_e) {
    int x=pt.x;
    int y=pt.y;
    switch (ch){
        case 'u':{
            while (1){
                if(y==0||(matrix[x-1][y]<matrix[x][y]&&road[x-1][y]>=0)||road[x][y]<-1){
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
                if(y==row||(matrix[x+1][y]<matrix[x][y]&&road[x+1][y]>=0)||road[x][y]<-1){
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
                if(x==0||(matrix[x][y-1]<matrix[x][y]&&road[x][y-1]>=0)||road[x][y]<-1){
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
                if(x==col||(matrix[x][y+1]<matrix[x][y]&&road[x][y+1]>=0)||road[x][y]<-1){
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
    pt.x=x;
    pt.y=y;

}

char Map::max_grade(point pt) {
    char ch=' ';
    int x=pt.x;
    int y=pt.y;
    int max1=max(road[x-1][y],road[x+1][y]);
    int max2=max(road[x][y-1],road[x][y+1]);
    int max3=max(max1,max2);
    if(max3==road[x][y-1])
        ch='l';
    else if(max3==road[x][y+1])
        ch='r';
    else if(max3==road[x-1][y])
        ch='u';
    else if(max3==road[x+1][y])
        ch='d';
    return ch;
}

char Map::walk_choice(point pt) {
    char ch='u';
    if(pt.x==0)
        ch='d';
    else if(pt.x==row-1)
        ch='u';
    else if(pt.y==0)
        ch='r';
    else if(pt.y==col-1)
        ch='l';
    else if(max_grade(pt)=='r')
        ch='r';
    else if(max_grade(pt)=='l')
        ch='l';
    else if(max_grade(pt)=='d')
        ch='d';
    else if(max_grade(pt)=='u')
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
        queue<point> tmp = start_q;
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
        start_q.pop();
    }
}

int** Map::road_map() {
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
    point p1=entrance;
    point p2=exit;
    int g1=-2;
    int g2=-10;
    while (true) {

        uniwalk(walk_choice(p1), p1,g1,'s');
        g1--;
//        print_road();
        if(road[p1.x][p1.y]<0) {
            end_point = 0;
            break;
        }

        uniwalk(walk_choice(p2),p2,g2,'e');
        g2++;
//        print_road();
        if(p1==p2||road[p2.x][p2.y]<0) {
            end_point = 1;
            break;
        }
    }
    final_road(end_point);
    return road;
}


