//TODO:

// - Make operable on Arduino(Remove cout)


#include <vector> //Will need to be replaced with fixed_vector when writing for Arduino
#include <iostream>
#include <cmath>
using namespace std;

#define glcm_levels 256
#define warp_distance 1
#define kernel_size 3 // Should be odd - almost never even, or else it will be difficult to find the center
#define angle 0
#define nbits 10 // number of bits used during the cooccurance matrix calculation
#define vector4d vector<vector<vector<vector<int>>>>

// Based off of python 'fast_glcm.py' package - https://github.com/tzm030329/GLCM/blob/master/fast_glcm.py
template <int width, int height>
class fast_glcm_ard
{
private:
    // Extra class for multiplication of Matrices

    template <class T>
    std::vector<std::vector<T>> Multiply(std::vector<std::vector<T>> &a, std::vector<std::vector<T>> &b)
    {
        const int n = a.size();    // a rows
        const int m = a[0].size(); // a cols
        const int p = b[0].size(); // b cols

        std::vector<std::vector<T>> c(n, std::vector<T>(p, 0));
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

    // minimum of 2d vector
    template <class T>
    static T min2dVec(vector<vector<T>> A)
    {
        T smallest = A[0][0];
        for (vector<T> vec : A)
        {
            for (T x : vec)
            {
                if (x < smallest)
                    smallest = x;
            }
        }
        return smallest;
    }

    template <class T>
    static T max2dVec(vector<vector<T>> A)
    {
        T largest = A[0][0];
        for (vector<T> vec : A)
        {
            for (T x : vec)
            {
                if (x > largest)
                    largest = x;
            }
        }
        return largest;
    }
    //static pad_vector(vector){}
    // Gets the kernel from the 2D Vector and returns it; DOES NOT CHECK FOR OUT OF BOUNDS EXCEPTIONS - USE WITH CAUTION
    static vector<vector<int>> cropMatrix(vector<vector<int>> kernel, int row, int col)
    {

        vector<vector<int>> output;
        int kernel_radius = kernel_size / 2;

        for (int i = row - kernel_radius; i < row + kernel_radius; i++)
        {
            vector<int> row;
            for (int j = col - kernel_radius; j < col + kernel_radius; j++)
            {
                row.push_back(kernel[i][j]);
            }
            output.push_back(row);
        }

        return output;
    }

public:
    /* Creates Linepace(bins)  by dividing range into defined variable glcm_levels and adding to min*/
    static vector<int> createLinspace(int min, int max)
    {
        vector<int> linespace;
        int diff = max - min;
        for (int i = 0; i < glcm_levels; i++)
        {
            linespace.push_back(min + i * diff / glcm_levels);
        }
        linespace.push_back(max);

        return linespace;
    }
    // Digitizes the 2D array into the bins
    // SLIGHT ERRORS - VALUES RETURNED AREN"T EXACTLY AS EXPECTED - COULD CAUSE PROBLEMS
    static vector<vector<int>> digitize(vector<vector<int>> arr2d, vector<int> bins)
    {
        vector<vector<int>> digitized;
        for (int i = 0; i < arr2d.size(); i++)
        {
            vector<int> row;
            for (int j = 0; j < arr2d[i].size(); j++)
            {
                if (arr2d[i][j] == bins[bins.size() - 1])
                {
                    row.push_back(bins.size() - 1);
                    continue;
                }
                for (int k = 0; k < bins.size(); k++)
                {
                    if (arr2d[i][j] < bins[k])
                    {
                        row.push_back(k - 1);
                        break;
                    }
                }
            }
            digitized.push_back(row);
        }

        return digitized;
    }
//ALWAYS ASSUMES KERNEL IS SQUARE
//ALWAYS ASSUMES KERNEL IS A SERIES OF 1s
//ERROR: TO GET VALUES OUT OF IT THE 2D VECTOR THE WIDTH & HEIGHT MUST BE >KERNEL_SIZE -- TODO: ADD TEST CAST TO CHECK FOR THIS
    static vector<vector<int>> filter2D(vector<vector<int>> Mat, int depth, vector<vector<int>> kernel)
    {
        vector<vector<int>> output;

        //Also denotes distance from the center to end; thus, starting and ending distance for  Iteration
        int kernel_center = kernel_size / 2; 
        int rows = Mat.size();
        int cols = Mat[0].size();
        int sum;
        
        /*
        r_i - row index
        c_i - column index
        Index through the values of the matrix and multiply with the kernel --
        Process known as convolution
        Designed to replicate cv2.filter2D() in OpenCV
         */

        //cout<< "KERNCEL DISTANCE: " << kernel_center<< " ROWS: "<< rows <<" COLS: "<<cols<<endl;//TEST PRINT
        for(int r_i = kernel_center; r_i<rows-kernel_center;r_i++){
            vector<int> row;
           // cout << "DEBUG -- ROW: " << r_i << endl;// TEST PRINT
            for(int c_i = kernel_center; c_i<cols-kernel_center; c_i++){
                sum = 0;
                //cout << "DEBUG -- COL: " << c_i << endl; //TEST PRINT
                //Iterate through kernel and process convolution
                for(int i = r_i-kernel_center; i<=r_i+kernel_center; i++){
                    for(int j = c_i-kernel_center; j<=c_i+kernel_center; j++){
                        sum += Mat[i][j] * 1 ;/*Assumption can be reasonably made that since the kernel is almost always 1 so thus it can always be multiplied by one; however, if this approach were to return it must be made so that the values for i & j stay within limits to avoid crashing */ //kernel[i][j];
                    }
                }
                //cout << "DEBUG -- SUM: " << sum << "\n\n"<< endl;//TEST PRINT
                row.push_back(sum);
            }
            output.push_back(row);
            row.clear();
        }



    //Get result of convolution with Kernel as sum
        
        
        return output;
    }
    static vector4d create_fast_glcm(vector<vector<int>> imgVec)
    {

        int mi = min2dVec<int>(imgVec);
        int ma = max2dVec<int>(imgVec);
        vector<int> lines = createLinspace(mi, ma);


        imgVec = digitize(imgVec, lines);
        
        double dx = warp_distance * cos(angle * (M_PI / 180));
        double dy = warp_distance * sin(-1 * angle * (M_PI / 180));

        vector<vector<int>> shiftedImg;
        // ASSUMES DX = -1 and DY = 0
        int temp;
        for (int i = 0; i < imgVec.size(); i++)
        {
            vector<int> row;
            temp = imgVec[i][0];
            for (int j = 1; j < imgVec[i].size(); j++)
            {
                row.push_back(imgVec[i][j]);
            }
            row.push_back(temp);
            shiftedImg.push_back(row);
        }

       
        // Only works for rectangular shaped vectors - however most imags fit this
        vector4d glcm(nbits, std::vector<std::vector<std::vector<int>>>(
                                 nbits, std::vector<std::vector<int>>(
                                            imgVec.size(), std::vector<int>(imgVec[0].size(), 0))));
        for (int i = 0; i < nbits; i++)
        {
            // cout<< i << endl; DEBUG
            for (int j = 0; j < nbits; j++)
            {
                // cout<< "\t"<<j << endl; //DEBUG
                // Ngl - ami ndiongngoke se ye nam mmi
                for (int h = 0; h < imgVec.size(); h++)
                {

                    // cout<< "\t\t"<<h << endl; //DEBUG
                    for (int w = 0; w < imgVec[0].size(); w++)
                    {
                        // cout<< "\t\t\t"<<w << endl;//DEBUG

                        glcm[i][j][h][w] = imgVec[h][w] == i & shiftedImg[h][w] == j;
                    }
                }
            }
        }

        vector<vector<int>> kernel = vector<vector<int>>(kernel_size, vector<int>(kernel_size, 1));
        for(int i = 0; i<glcm.size();i++){
            for(int j = 0; j<glcm[i].size();j++){
                glcm[i][j] = filter2D(glcm[i][j], 1, kernel);
            }
        }

            return glcm;
        }

        static vector<vector<int>> glcm_contrast(vector4d glcm)
{

    vector<vector<long long>> contrast_matrix(glcm[0][0].size(), vector<long long>(glcm[0][0][0].size(),0));
    long long total_contrast = 0;
    for (int i = 0; i < glcm.size(); i++)
    {//cout << endl;
        for (int j = 0; j < glcm[i].size(); j++)
        { //cout << endl;
            for (int y = 0; y < glcm[i][j].size(); y++)
            { //cout << endl;
                for (int x = 0; x < glcm[i][j][y].size(); x++)
                {
                     //cout << glcm[i][j][y][x] << " ";
                    
                if(abs(i-j) == y){
                    contrast_matrix[y][x] += glcm[i][j][y][x] * (i - j) * (i-j);
                    total_contrast +=  contrast_matrix[y][x];
                }
                }
            }
        }
    }
    
    cout<< " TOTAL CONTRAST OF MATRICE: " << total_contrast << endl;

    //this is so bullshit but Im runnin out of time
    contrast_matrix[0][0] = total_contrast;
    return contrast_matrix;
}
    };