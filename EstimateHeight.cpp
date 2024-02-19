#include<iostream>
#include<fstream>
#include<filesystem>
#include<cmath>
#include<vector>
#include<cstdlib>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Geometry>
#include<eigen3/Eigen/LU>
#include<eigen3/Eigen/Sparse>
#include"EstimateHeight.hpp"
#include"MeshStructure.hpp"

using namespace std;
using  Matrix = Eigen::MatrixXd;
using  Vector = Eigen::VectorXd;
using  RowVector = Eigen::RowVectorXd;
using  Solver = Eigen::FullPivLU<Matrix>;
using SparseMatrix = Eigen:: SparseMatrix<double>;
using  SparseSolver = Eigen::SparseLU<SparseMatrix>;
using Triplet = Eigen::Triplet<double>;

double compute_cot(Eigen::Vector3d v0,Eigen::Vector3d v1,Eigen::Vector3d v2){
    double cos,sin,cot;
    Eigen::Vector3d a=v1-v0;
    Eigen::Vector3d b=v2-v0;
    cos=a.dot(b)/(a.norm()*b.norm());
    sin=sqrt(1-cos*cos);
    if(sin<0.01){
        cot=1;
    }else{
        cot=cos/sin;
    }
    return cot/2;
}
double compute_area_e(Eigen::Vector3d v0,Eigen::Vector3d v1,Eigen::Vector3d v2){
    Eigen::Vector3d v3=v2-v1;
    Eigen::Vector3d v4=v0-v1;
    
    Eigen::Vector3d normal_vector=v3.cross(v4);
    return normal_vector.norm()/2;
}
void make_volume(Mesh& m){
    cout<<"start"<<endl;
    Vector weight=Vector::Zero(m.vertices.size());
    vector<vector<int>> neighber_vertices;
    for(int i=0; i<m.vertices.size(); i++){
        vector<int> tmp;
        neighber_vertices.push_back(tmp);
    }
    m.neighber_vertices=neighber_vertices;
    //コタンジェント重み
    vector<vector<double>> c_w(m.vertices.size(),vector<double>(m.vertices.size(),0));
    for(int i=0; i<m.faces.size(); i++){
        c_w[m.faces[i](0)][m.faces[i](1)]+=compute_cot(m.vertices[m.faces[i](2)],m.vertices[m.faces[i](0)],m.vertices[m.faces[i](1)]);
        c_w[m.faces[i](1)][m.faces[i](2)]+=compute_cot(m.vertices[m.faces[i](0)],m.vertices[m.faces[i](1)],m.vertices[m.faces[i](2)]);
        c_w[m.faces[i](2)][m.faces[i](0)]+=compute_cot(m.vertices[m.faces[i](1)],m.vertices[m.faces[i](2)],m.vertices[m.faces[i](0)]);
    }
    m.weight=c_w;
    cout<<"success:cot"<<endl;
    for(int i=0; i<m.faces.size(); i++){
        int v0=m.faces[i](0);
        int v1=m.faces[i](1);
        int v2=m.faces[i](2);
        double f_area=compute_area_e(m.vertices[v0],m.vertices[v1],m.vertices[v2]);
        f_area/=3.0;
        weight[v0]+=f_area;
        weight[v1]+=f_area;
        weight[v2]+=f_area;
        int flag0=0;
        int flag1=0;
        int flag2=0;
        for(int i=0; i<neighber_vertices[v0].size(); i++){
            if(neighber_vertices[v0][i]==v1) flag1+=1;
            if(neighber_vertices[v0][i]==v2) flag2+=1;
        }
        if(flag1==0) neighber_vertices[v0].push_back(v1);
        if(flag2==0) neighber_vertices[v0].push_back(v2);
        
        flag0=0;
        flag1=0;
        flag2=0;
        for(int i=0; i<neighber_vertices[v1].size(); i++){
            if(neighber_vertices[v1][i]==v0) flag0+=1;
            if(neighber_vertices[v1][i]==v2) flag2+=1;
        }
        if(flag0==0) neighber_vertices[v1].push_back(v0);
        if(flag2==0) neighber_vertices[v1].push_back(v2);
        
        flag0=0;
        flag1=0;
        flag2=0;
        for(int i=0; i<neighber_vertices[v2].size(); i++){
            if(neighber_vertices[v2][i]==v1) flag1+=1;
            if(neighber_vertices[v2][i]==v0) flag0+=1;
        }
        if(flag1==0) neighber_vertices[v2].push_back(v1);
        if(flag2==0) neighber_vertices[v2].push_back(v0);
        
    }
    for(int i=0; i<weight.size(); i++){
        if(m.judge_origin[i]==0){
            weight[i]*=3;
        }else{
            weight[i]*=0;
        }
    }
    cout<<"success:weight"<<endl;
    SparseMatrix laplacian(m.vertices.size(),m.vertices.size());
    vector<Triplet> triplets;
    for(int i=0; i<m.vertices.size(); i++){
        double cot_sum=0;
        if(m.judge_origin[i]==0){
            for(int j=0; j<neighber_vertices[i].size(); j++){
                //triplets.push_back(Triplet(i,neighber_vertices[i][j],-(c_w[i][neighber_vertices[i][j]])));//-1
                triplets.push_back(Triplet(i,neighber_vertices[i][j],-1));
                cot_sum+=c_w[i][neighber_vertices[i][j]];
            }
            triplets.push_back(Triplet(i,i,neighber_vertices[i].size()));
            //triplets.push_back(Triplet(i,i,cot_sum));//neighber_vertices[i].size()
        }else{
            triplets.push_back(Triplet(i,i,1));
        }
        
    }
    cout<<"success:trip"<<endl;
    laplacian.setFromTriplets(triplets.begin(),triplets.end());
    SparseSolver solver;
    solver.compute(laplacian);
    Eigen::VectorXd height=solver.solve(weight);
    for(int i=0; i<m.vertices.size(); i++){
        m.vertices[i](2)=sqrt(abs(height[i]));
    }
    cout<<"success:solve"<<endl;
    //表面だけ
    map<int,int> v_pair;
    int num_vertices=m.vertices.size();
    for(int i=0; i<num_vertices; i++){
        if(m.vertices[i](2)!=0){
            v_pair[i]=m.vertices.size();
            m.vertices.push_back({m.vertices[i](0),m.vertices[i](1),-m.vertices[i](2)});
            m.weight.push_back(m.weight[i]);
        }
    }
    m.neighber_vertices.resize(m.vertices.size());
    int num_faces=m.faces.size();
    for(int i=0; i<num_faces; i++){
        int a=m.faces[i](0);
        int b=m.faces[i](1);
        int c=m.faces[i](2);
        if(m.vertices[a](2)!=0) a=v_pair[a];
        if(m.vertices[b](2)!=0) b=v_pair[b];
        if(m.vertices[c](2)!=0) c=v_pair[c];
        if(a>=num_vertices || b>=num_vertices || c>=num_vertices){
            m.faces.push_back({c,b,a});
            m.neighber_vertices[a].push_back(b);
            m.neighber_vertices[a].push_back(c);
            m.neighber_vertices[b].push_back(a);
            m.neighber_vertices[b].push_back(c);
            m.neighber_vertices[c].push_back(a);
            m.neighber_vertices[c].push_back(b);
        }
    }
}
void make_objfile_m(vector<Mesh> m, string filename){
    std::filesystem::path p="VolumedModel/"+filename;
    ofstream outputfile(p);
    for(int i=0; i<m.size(); i++){
        for(int j=0; j<m[i].vertices.size(); j++){
            //cout<<m[i].vertices[j](2)<<endl;
            outputfile<<"v "<<m[i].vertices[j](0)<<" "<<m[i].vertices[j](1)<<" "<<m[i].vertices[j](2)<<" "<<endl;
        }
    }
    int v_size=1; //これまでの物体の頂点数＋１（1index)
    for(int i=0; i<m.size(); i++){
        for(int j=0; j<m[i].faces.size(); j++){
            outputfile<<"f "<<m[i].faces[j](0)+v_size<<" "<<m[i].faces[j](1)+v_size<<" "<<m[i].faces[j](2)+v_size<<" "<<endl;
        }
        v_size+=m[i].vertices.size();
    }
    outputfile.close();
}
void EstimateHeight_master(vector<Mesh> mesh,string filename){
    for(int i=0; i<mesh.size(); i++){
        make_volume(mesh[i]);
    }
    make_objfile_m(mesh,filename+"_volume.obj");
    system(("open VolumedModel/" + filename + "_volume.obj").c_str());
}
