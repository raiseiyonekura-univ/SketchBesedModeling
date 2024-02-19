#include<cmath>
#include"MeshStructure.hpp"
Point Point::operator+(const Point &b){
    Point a;
    a.x=x+b.x;
    a.y=y+b.y;
    return a;
}
Point Point::operator-(const Point &b){
    Point a;
    a.x=x-b.x;
    a.y=y-b.y;
    return a;
}
Point Point::operator*(const double &b){
    Point a;
    a.x=x*b;
    a.y=y*b;
    return a;
}
Point Point::operator/(const double &b){
    Point a;
    a.x=x/b;
    a.y=y/b;
    return a;
}
double Point::compute_distance(Point a,Point b){
    Point c;
    c=a-b;
    return sqrt((c.x*c.x)+(c.y*c.y));
}
