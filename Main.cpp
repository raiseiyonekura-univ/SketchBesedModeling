#include<iostream>
#include<string>
#include<vector>
#include<regex>
#include"EstimateHeight.hpp"
#include"InputFunction.hpp"
//#include"MeshDeformation.hpp"
#include"MeshStructure.hpp"
#include"Triangulation.hpp"
using namespace std;
int main(int argc, char* argv[]){
    if(argc !=2){
        cout<<"error"<<endl;
        return 0;
    }
    string filename=argv[1];
    vector<Object> objects=input_master("sketch/"+filename);
    filename=regex_replace(filename,regex("\.svg"),"");
    vector<Mesh> mesh=triangulation_master(objects,filename);
    int num_object=objects.size();
    for(int i=0; i<num_object; i++){
        vector<int> tmp_judge_origin;
        tmp_judge_origin.resize(mesh[i].vertices.size());
        for(int j=0;  j<objects[i].num_vertices; j++){
            tmp_judge_origin[j]=1;
        }
        mesh[i].judge_origin=tmp_judge_origin;
    }
    EstimateHeight_master(mesh,filename);
}
//g++ -std=c++20 -I /Eigen  -I /igl  Main.cpp -o main EstimateHeight.cpp InputFunction.cpp MeshDeformation.cpp MeshStructure.cpp Triangulation.cpp
