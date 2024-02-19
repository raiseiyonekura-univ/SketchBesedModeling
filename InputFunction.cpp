#include<iostream>
#include<fstream>
#include<string>
#include<regex>
#include<vector>
#include<cmath>
#include<eigen3/Eigen/Core>
#include<igl/winding_number.h>
#include"InputFunction.hpp"

using namespace std;
int num_separate=10;
vector<vector<Point>> controls;
Point current_point={0,0};
namespace myStd{
    typedef match_results<string::iterator> smatch;
}
vector<Point> compute_bezier(vector<Point> control_points){
    controls.push_back(control_points);
    vector<Point> vertices;
    for(int i=0; i<num_separate; i++){
        double n=double(i)/num_separate;
        vector<Point> tmp_point;
        tmp_point=control_points;
        for(int j=0; j<control_points.size()-1; j++){
            int tmp_p_size=tmp_point.size();
            for(int k=0; k<tmp_p_size-1; k++){
                tmp_point.push_back((tmp_point[0]*(1-n)+tmp_point[1]*n));
                tmp_point.erase(tmp_point.begin());
            }
            tmp_point.erase(tmp_point.begin());
        }
        tmp_point[0].y=-(tmp_point[0].y);
        vertices.push_back(tmp_point[0]);
    }
    return vertices;
}

vector<string> disassembly_input(string filename){
    int num_object=0;
    ifstream ifs(filename);
    regex judge_path(R"(.*\s+[d]+[=]+.*)");
    regex judge_control(R"(\s*[a-zA-Z]+((-?[0-9]+[\.]+[0-9]+[e]?[-]?[0-9]*)|-?[0-9]+)+.*)");
    string str;
    myStd::smatch m;
    smatch n;
    vector<string> path_set;
    string path_tmp="begin_path";
    while(getline(ifs,str)){
        if(regex_match(str,n,judge_path)){
            if(num_object!=0){
                path_tmp+="end";
                path_set.push_back(path_tmp);
                path_tmp="begin_path";
            }
            num_object+=1;
            path_tmp+=n[0].str();
        }
        if(regex_match(str,n,judge_control)){
            path_tmp+=n[0].str();
        }
    }
    path_tmp+="end";
    path_set.push_back(path_tmp);
    return path_set;
}

Path make_path(string path_string){
    Path path;
    myStd::smatch m;
    
    vector<string> order_kind;
    vector<int> order_place;
    vector<double> points;
    
    regex judge_control(R"(\s*[a-zA-Z]+\s*((-?[0-9]+[\.]+[0-9]+[e]?[-]?[0-9]*)|-?[0-9]+)+.*)");
    regex judge_point(R"((-?[0-9]+[\.]+[0-9]+)|-?[0-9]+)");
    regex judge_order(R"([McCsS]+)");
    regex end_point(R"([^0-9]+end)");
    regex start_point(R"(.*d=)");
    auto it=path_string.begin();
    while(it!=path_string.end()){
        if(regex_search( it, path_string.end(), m, start_point)){
            it+=m.position(0)+m.length(0);
        }
        if(regex_search( it, path_string.end(), m, judge_order)){
            it+=m.position(0)+m.length(0);
            order_kind.push_back(m[0].str());
            order_place.push_back(points.size());
        }
        while(regex_search( it, path_string.end(), m, judge_point)){
            if(regex_search( it, path_string.end(), m, judge_point)){
                it+=m.position(0)+m.length(0);
                points.push_back(stod(m[0].str()));
            }
            if(regex_match( it, path_string.end(), m, judge_control)){
                break;
            }
        }
        if(regex_match( it, path_string.end(), m, end_point)){
            break;
        }
    }
    for(int i=0; i<order_place.size(); i++){
        order_place[i]/=2;
    }
    vector<Point> vertices;
    Point tmp_vertex;
    for(int i=0; i<points.size(); i++){
        if((i+1)%2==0){
            tmp_vertex.y=points[i];
            vertices.push_back(tmp_vertex);
        }else{
            tmp_vertex.x=points[i];
        }
    }
    path.order_kind=order_kind;
    order_place.push_back(vertices.size());
    path.order_place=order_place;
    path.points=vertices;
    return path;
}
Object make_vertex(Path p){
    vector<Point> vertices;
    vector<int> flags;
    if(p.order_kind.size()>1){
        for(int i=0; i<p.order_kind.size(); i++){
            if(p.order_kind[i]=="M"){
                current_point=p.points[i];
                
            }else{
                vector<Point> control_points;
                control_points.push_back(current_point);
                for(int j=p.order_place[i]; j<p.order_place[i+1]; j++){
                    if(p.order_kind[i]=="C"){
                        current_point=p.points[j];
                    }else if(p.order_kind[i]=="c"){
                        current_point=current_point+p.points[j];
                    }else if(p.order_kind[i]=="s"){
                        if(j==p.order_place[i]){
                            Point tmp_currentpoint=current_point;
                            current_point=current_point+p.points[j];
                            control_points.push_back(current_point);
                            current_point=current_point+current_point-tmp_currentpoint;
                        }else{
                            current_point=current_point+p.points[j];
                        }
                    }
                    control_points.push_back(current_point);
                }
                vector<Point> part_path;
                //
                int num_of_bezier=control_points.size()/3;
                
                for(int k=0; k<num_of_bezier; k++){
                    vector<Point> tmp_control;
                    for(int t=3*k; t<3*k+4; t++){
                        tmp_control.push_back(control_points[t]);
                    }
                    vector<Point> tmp_part_path=compute_bezier(tmp_control);
                    for(int t=0; t<tmp_part_path.size(); t++){
                        part_path.push_back(tmp_part_path[t]);
                    }
                }
                //part_path=compute_bezier(control_points);
                vertices.insert(vertices.end(),part_path.begin(),part_path.end());
            }
            for(int i=0; i<vertices.size(); i++){
                flags.push_back(1);
            }
        }
    }else{
        for(int j=0; j<p.points.size()-1; j++){
            for(int n=0; n<10; n++){
                //cout<<endl;
                current_point=p.points[j]*(10-n)/10+p.points[j+1%p.points.size()]*n/10;
                Point tmp_point=current_point;
                tmp_point.y=-tmp_point.y;
                vertices.push_back(tmp_point);
                //print_point(current_point);
            }
        }
        for(int i=0; i<vertices.size(); i++){
            flags.push_back(2);
        }
    }
    Object ans;
    ans.vertices=vertices;
    ans.flag_border=flags;
    return ans;
}
void mergeobject(vector<Object> objects){
    int num_objects=objects.size();
    for(int i=0; i<num_objects;  i++){
        for(int j=0; j<num_objects; j++){
            if(i!=j){
                Eigen::MatrixXd i_v(objects[i].vertices.size(),2);
                Eigen::MatrixXd i_f(objects[i].vertices.size(),2);
                Eigen::MatrixXd j_v(objects[j].vertices.size(),2);
                Eigen::VectorXi w(objects[j].vertices.size());
                for(int k=0; k<objects[i].vertices.size(); k++){
                    i_v(k,0)=objects[i].vertices[k].x;
                    i_v(k,1)=objects[i].vertices[k].y;
                }
                for(int k=0; k<objects[i].vertices.size(); k++){
                    i_f(k,0)=i;
                    i_f(k,1)=(i+1)/objects[i].vertices.size();
                }
                for(int k=0; k<objects[j].vertices.size(); k++){
                    j_v(k,0)=objects[j].vertices[k].x;
                    j_v(k,1)=objects[j].vertices[k].y;
                }
                //igl::winding_number(i_v,i_f,j_v,w);
                cout<<w<<endl;
            }
        }
    }
}
vector<Object> input_master(string filename){
    vector<Object> objects;
    vector<string> path_set=disassembly_input(filename);
    for(int i=0; i<path_set.size(); i++){
        cout<<"start_make_object"<<i<<endl;
        Object object;
        Path path_i=make_path(path_set[i]);
        object=make_vertex(path_i);
        object.num_vertices=object.vertices.size();
        objects.push_back(object);
        cout<<"fin_make_object"<<i<<endl;
    }
    mergeobject(objects);
    return objects;
}
