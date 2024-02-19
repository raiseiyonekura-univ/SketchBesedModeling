#ifndef ESTIMATEHEIGHT_H_INCLUDED
#define ESTIMATEHEIGHT_H_INCLUDED
#include<vector>
#include<string>
#include<eigen3/Eigen/Core>
#include"MeshStructure.hpp"
double compute_cot(Eigen::Vector3d v0,Eigen::Vector3d v1,Eigen::Vector3d v2);
double compute_area_e(Eigen::Vector3d v0,Eigen::Vector3d v1,Eigen::Vector3d v2);
void make_volume(Mesh& m);
void make_objfile_m(vector<Mesh> m, string filename);
void EstimateHeight_master(vector<Mesh> mesh,string filename);
#endif
