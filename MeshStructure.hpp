#ifndef MESHSTRUCTURE_H_INCLUDED
#define MESHSTRUCTURE_H_INCLUDED

#include<vector>
#include<eigen3/Eigen/Core>

using namespace std;

struct Point{
    double x;
    double y;
    
    double compute_distance(Point a,Point b);
    Point operator+(const Point &b);
    Point operator-(const Point &b);
    Point operator*(const double &b);
    Point operator/(const double &b);
};
struct Object{
    int name;
    int num_vertices;
    vector<Point> vertices;
    vector<vector<int>> faces;
    vector<int> flag_border;// 2:初期からの境界
    vector<vector<int>> origin_edge;
    vector<int> children_index;//子の添字
};
struct Mesh{
    int num_originvertex;
    vector<int> judge_origin;
    vector<Eigen::Vector3d> vertices;
    vector<Eigen::Vector3i> faces;
    vector<vector<double>> weight;
    vector<vector<int>> neighber_vertices;
    vector<Eigen::MatrixXd> rotation;
};
#endif
