#include<iostream>
#include<fstream>
#include<vector>
#include<filesystem>
#include"Triangulation.hpp"
#include"MeshStructure.hpp"
using namespace std;
void make_polyfile(Object object,string filename){
    std::filesystem::path p="TriangulatedModel/"+filename;
    cout<<p<<endl;
    ofstream outputfile(p);
    outputfile<<object.vertices.size()<<" "<<2<<" "<<1<<" "<<0<<endl;
    for(int i=0; i<object.vertices.size(); i++){
        outputfile<<i<<" "<<object.vertices[i].x<<" "<<object.vertices[i].y<<" 0"<<endl;
    }
    outputfile<<object.vertices.size()<<" "<<0<<endl;
    for(int i=0; i<object.vertices.size(); i++){
        outputfile<<i<<" "<<i<<" "<<(i+1)%object.vertices.size()<<endl;
    }
    outputfile<<"0"<<endl;
    outputfile.close();
    cout<<"fin"<<endl;
}
void execution_triangle(string filename,int num_objects){
    for(int i=0; i<num_objects; ++i){
        std::system(("./triangle -Y -a1 TriangulatedModel/"+filename + to_string(i)+".poly").c_str());
        //-Y:境界への追加の抑制 -N,-E:ノード,eleファイルを出力しない
    }
}
vector<Mesh> read_poly_to_mesh(string name,int num_objects){
    vector<Mesh> mesh;
    int sum_vertices=1;
    for(int j=0; j<num_objects; j++){
        Mesh m;
        string filename1=name+to_string(j)+".1.node";
        string filename2=name+to_string(j)+".1.ele";
        int num_vertices,num_faces;
        
        FILE* fp1;
        fp1=fopen(("TriangulatedModel/"+filename1).c_str(),"r");
        if(fp1==NULL){
            cout<<"error"<<endl;
            return mesh;
        }
        fscanf(fp1, "%d %*d %*d %*d",&num_vertices);
        for(int i=0; i<num_vertices; i++){
            double x_tmp,y_tmp;
            fscanf(fp1,"%*d  %lf %lf %*d %*d",&x_tmp, &y_tmp);
            m.vertices.push_back({x_tmp,y_tmp,0});
        }
        fclose(fp1);
        
        FILE* fp2;
        fp2=fopen(("TriangulatedModel/"+filename2).c_str(),"r");
        if(fp2==NULL){
            cout<<"error"<<endl;
            return mesh;
        }
        fscanf(fp2, "%d %*d %*d ",&num_faces);
        for(int i=0; i<num_faces; i++){
            int v0_tmp,v1_tmp,v2_tmp;
            fscanf(fp2,"%*d %d %d %d",&v0_tmp,&v1_tmp,&v2_tmp);
            m.faces.push_back({v0_tmp,v1_tmp,v2_tmp});
        }
        fclose(fp2);
        sum_vertices+=num_vertices;
        mesh.push_back(m);
    }
    return mesh;
}
vector<Mesh> triangulation_master(vector<Object> objects,string filename){
    int num_objects=objects.size();
    for(int i=0; i<num_objects; i++){
        make_polyfile(objects[i],filename+to_string(i)+".poly");
    }
    execution_triangle(filename,num_objects);
    vector<Mesh> mesh=read_poly_to_mesh(filename,num_objects);
    return mesh;
}
