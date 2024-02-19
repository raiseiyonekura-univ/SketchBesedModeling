#ifndef INPUTFUNCTION_H_INCLUDED
#define INPUTFUNCTION_H_INCLUDED

#include<string>
#include<vector>
#include"MeshStructure.hpp"
using namespace std;

struct Path{
    vector<string> order_kind;
    vector<int> order_place;
    vector<Point> points;
};
vector<string> disassembly_input(string filename);
Path make_path(string path_string);
Object make_vertex(Path p);
void mergeobject(vector<Object> objects);
vector<Object> input_master(string filename);
#endif
