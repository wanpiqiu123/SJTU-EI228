//
// Created by Leo on 2019/4/5.
//

#ifndef VISUAL_HEAD_H
#define VISUAL_HEAD_H

#include <iostream>
#include  "opencv2/highgui.hpp"
#include  "opencv2/imgproc.hpp"
#include <cmath>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;
using  namespace cv;

struct point{
    int x=0;
    int y=0;
    point(){}
    point(int x1, int y1){
        x=x1;
        y=y1;
    }
    bool operator==(point &pt){
        if (this->x == pt.x && this->y==pt.y)
            return true;
        else return false;
    }
    bool operator!=(point &pt){
        if (this->x == pt.x && this->y==pt.y)
            return false;
        else return true;
    }
    friend ostream &operator<<( ostream &output,
                                const point &pt )
    {
        output << "x: " << pt.x << " y: " << pt.y;
        return output;
    }
};

struct pointd{
    double x=0;
    double y=0;
    pointd(){}
    pointd(double x1, double y1){
        x=x1;
        y=y1;
    }
    bool operator==(pointd &pt){
        if (this->x == pt.x && this->y==pt.y)
            return true;
        else return false;
    }
    bool operator!=(pointd &pt){
        if (this->x == pt.x && this->y==pt.y)
            return false;
        else return true;
    }
    friend ostream &operator<<( ostream &output,
                                const pointd &pt )
    {
        output << "x: " << pt.x << " y: " << pt.y;
        return output;
    }
};

struct Car{
    pointd front;
    pointd back;
    pointd center;
    double slope;
    Car(){};
    void init(Point2d front1,Point2d back1, pointd center1, double slope1){
        front=pointd(front1.x,front1.y);
        back=pointd(back1.x,back1.y);
        center=center1;
        slope=slope1;
    }
};

int distance(Point pt1, Point pt2);
vector<Point>corner (vector<vector<Point>> vec, vector<Point2f> &result, Mat mat); //find the corner vertices
void draw_vec(vector<Point> vec, Mat &mat);
void draw_vec(vector<Point2f> vec, Mat &mat); //draw the corner vertices
void perspective(vector<Point2f> corner,Mat src, Mat &correct); //change the graph into good position
void good_position(Mat &src); // operations on the good graph
vector<Point2d> car_pos(Mat & src);
char turning(char ch);
void turnto(pointd dst, Car &car);
int** matrix(Mat src, int &new_row, int &new_col);
int distance(point pt1, point pt2);
double distance(pointd pt1, pointd pt2);
void signal(char ch);

class Map{

private:
    int** matrix;
    int** road;
    int** route;
    int row;
    int col;
    point entrance;
    point exit;
    Car car1;
    void core_processing(int grade);
    char max_grade(point pt);
    void uniwalk(char ch, point &pt, point pt2, int seq, char s_e);
    char walk_choice(point pt);
    queue<point> start_q;
    stack<point> end_s;
    queue<pointd> route_q;
public:
    Map(int** m, int new_row, int new_col){
        matrix=m;
        row=new_row;
        col=new_col;
    }
    void print_map(){
        for (int k = 0; k < row; ++k) {
            for (int i = 0; i < col; ++i) {
                cout<<matrix[k][i]<<'\t';
            }
            cout<<endl;
        }
    }
    void print_road(){
        cout<<endl;
        for (int k = 0; k < row; ++k) {
            for (int i = 0; i < col; ++i) {
                cout<<road[k][i]<<'\t';
            }
            cout<<endl;
        }

    }
    void start_end();

    void road_grade();

    void final_road(int end_point);

    int** road_map();

    void q_s_num(){
        cout<<"queue: "<<start_q.size()<<" stack: "<<end_s.size()<<endl;
    }

    void print_route(){
        cout<<endl;
        for (int k = 0; k < row; ++k) {
            for (int i = 0; i < col; ++i) {
                cout<<route[k][i]<<' ';
            }
            cout<<endl;
        }

    }

    void car_run(Mat &src);
};



#endif //VISUAL_HEAD_H
