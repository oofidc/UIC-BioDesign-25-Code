#include <vector> //Will need to be replaced with fixed_vector when writing for Arduino
#include <iostream>
#include <cmath>
using namespace std;

#define glcm_levels 10
#define distance 1
#define kernel_size 5
#define angle 0

//Based off of python 'fast_glcm.py' package - https://github.com/tzm030329/GLCM/blob/master/fast_glcm.py
template<int width, int height>
class fast_glcm{
public:

/* Creates Linespace(bins)  by dividing range into defined variable glcm_levels and adding to min*/
static vector<int> createLinspace(int min, int max){
    vector<int> linespace;
    int diff = max - min;
    for(int i = 0; i < glcm_levels; i++){
        linespace.push_back(min + i*diff/glcm_levels);
    }
    linespace.push_back(max);

    return linespace;
}
//Digitizes the 2D array into the bins
//SLIGHT ERRORS - VALUES RETURNED AREN"T EXACTLY AS EXPECTED - COULD CAUSE PROBLEMS
static vector<vector<int>> digitize(vector<vector<int>> arr2d, vector<int> bins){
    vector<vector<int>> digitized;
    for(int i = 0; i < arr2d.size(); i++){
        vector<int> row;
        for(int j = 0; j < arr2d[i].size(); j++){
            for(int k = 0; k < bins.size(); k++){
                if(arr2d[i][j] < bins[k]){
                    row.push_back(k);
                    break;
                }
            }
        }
        digitized.push_back(row);
    }

    return digitized;
}




fast_glcm(int img[width][height]){}
fast_glcm(){
    return;
}



};


int main(){

    vector<int> lines = fast_glcm<100,100>::createLinspace(2,82+1);
    vector<vector<int>> arr2d;
    arr2d.push_back({2,3,4,6,7});
    arr2d.push_back({3,5,82,34,2});
    vector<vector<int>> output = fast_glcm<100,100>::digitize(arr2d, lines);
    
    for(int x :fast_glcm<100,100>::createLinspace(2,82+1)){
        cout << x << " ";
    }

    cout<<endl;

    for(vector<int> x: output){
        for(int y: x){
            cout << y << " ";
        }
        cout << endl;
    }

    return 0;
}

