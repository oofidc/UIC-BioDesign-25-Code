#include <vector> //Will need to be replaced with fixed_vector when writing for Arduino
#include <iostream>
#include <cmath>
using namespace std;

#define glcm_levels 10
#define distance 1
#define kernel_size 5
#define angle 0
#define nbits 8 //number of bits used during the cooccurance matrix calculation

//Based off of python 'fast_glcm.py' package - https://github.com/tzm030329/GLCM/blob/master/fast_glcm.py
template<int width, int height>
class fast_glcm{
private:

//Extra class for multiplication of Matrices

template <class T>
std::vector <std::vector<T>> Multiply(std::vector <std::vector<T>> &a, std::vector <std::vector<T>> &b)
{
    const int n = a.size();     // a rows
    const int m = a[0].size();  // a cols
    const int p = b[0].size();  // b cols

    std::vector <std::vector<T>> c(n, std::vector<T>(p, 0));
    for (auto j = 0; j < p; ++j)
    {
        for (auto k = 0; k < m; ++k)
        {
            for (auto i = 0; i < n; ++i)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

//minimum of 2d vector
template <class T>
static T min2dVec(vector<vector<T>> A){
    T smallest = A[0][0];
    for(vector<T> vec : A){
        for(T x: vec){
            if(x<smallest) smallest = x;
        }
    }
    return smallest;
} 

template <class T>
static T max2dVec(vector<vector<T>> A){
    T largest = A[0][0];
    for(vector<T> vec : A){
        for(T x: vec){
            if(x>largest) largest = x;
        }
    }
    return largest;
} 


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





static vector<vector<int>> create_fast_glcm(vector<vector<int>> imgVec){



    //TODO: Fix Min and Max functions
    int mi = min2dVec<int>(imgVec);
    int ma = max2dVec<int>(imgVec);
    vector<int> lines = createLinspace(mi, ma);
    if(true) return digitize(imgVec, lines);

    double dx = distance*cos(angle * (M_PI/180));
    double dy = distance*sin(-1*angle * (M_PI/180));
   
   vector<vector<int>> shiftedImg(width, vector<int>(height, 0));

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            //Possible source of error - I'm not sure if this should be + or - dx and dy
            int newX = i - dx;
            int newY = j - dy;
            if(newX >= 0 && newX < width && newY >= 0 && newY < height) {
                shiftedImg[newX][newY] = imgVec[i][j];
            }
        }
    }

    return shiftedImg;




  
    


    

}




};


int main(){

    vector<int> lines = fast_glcm<5,2>::createLinspace(2,82+1);
    vector<vector<int>> arr2d;
    arr2d.push_back({2,3,4,6,7});
    arr2d.push_back({3,5,82,34,2});
    //int arr[2][5] = {{2,3,4,6,7},{3,5,82,34,2}};
    vector<vector<int>> output = fast_glcm<2,5>::create_fast_glcm(arr2d);
    
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

