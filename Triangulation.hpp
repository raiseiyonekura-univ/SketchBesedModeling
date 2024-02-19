#ifndef TRIANGULATION_H_INCLUDED
#define TRIANGULATION_H_INCLUDED

#include<vector>
#include<string>
#include"MeshStructure.hpp"
using namespace std;
void make_polyfile(Object object,string filename);
void execution_triangle(string filename,int num_objects);
vector<Mesh> read_poly_to_mesh(string name,int num_objects);
vector<Mesh> triangulation_master(vector<Object> objects, string filename);
#endif
