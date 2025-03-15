// TO-DO:
//  - 2d Filtering method & Test it on its own -- DONE
//  2. Implement filtering at the end of create_fast_glcm

#include <vector> //Will need to be replaced with fixed_vector when writing for Arduino
#include <iostream>
#include <cmath>
using namespace std;

#define glcm_levels 512
#define distance 1
#define kernel_size 3 // Should be odd - almost never even, or else it will be difficult to find the center
#define angle 0
#define nbits 512 // number of bits used during the cooccurance matrix calculation
#define vector4d vector<vector<vector<vector<int>>>>

// Based off of python 'fast_glcm.py' package - https://github.com/tzm030329/GLCM/blob/master/fast_glcm.py
template <int width, int height>
class fast_glcm
{
private:
    // Extra class for multiplication of Matrices
    static void normalizeMatrix(std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return;
    
    int minVal = matrix[0][0], maxVal = matrix[0][0];
    
    // Find min and max values in the matrix
    for (const auto& row : matrix) {
        for (int val : row) {
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
        }
    }
    
    if (minVal == maxVal) return; // Avoid division by zero
    
    // Normalize values to range [0, 255]
    for (auto& row : matrix) {
        for (int& val : row) {
            val = static_cast<int>(255.0 * (val - minVal) / (maxVal - minVal));
        }
    }

}

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
    // static pad_vector(vector){}
    //  Gets the kernel from the 2D Vector and returns it; DOES NOT CHECK FOR OUT OF BOUNDS EXCEPTIONS - USE WITH CAUTION
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
    // ALWAYS ASSUMES KERNEL IS SQUARE
    // ALWAYS ASSUMES KERNEL IS A SERIES OF 1s
    // ERROR: TO GET VALUES OUT OF IT THE 2D VECTOR THE WIDTH & HEIGHT MUST BE >KERNEL_SIZE -- TODO: ADD TEST CAST TO CHECK FOR THIS
    static vector<vector<int>> filter2D(vector<vector<int>> Mat, int depth, vector<vector<int>> kernel)
    {
        vector<vector<int>> output;

        // Also denotes distance from the center to end; thus, starting and ending distance for  Iteration
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

        //cout << "KERNCEL DISTANCE: " << kernel_center << " ROWS: " << rows << " COLS: " << cols << endl; // TEST PRINT
        for (int r_i = kernel_center; r_i < rows - kernel_center; r_i++)
        {
            vector<int> row;
            //cout << "DEBUG -- ROW: " << r_i << endl; // TEST PRINT
            for (int c_i = kernel_center; c_i < cols - kernel_center; c_i++)
            {
                sum = 0;
                //cout << "DEBUG -- COL: " << c_i << endl; // TEST PRINT
                // Iterate through kernel and process convolution
                for (int i = r_i - kernel_center; i <= r_i + kernel_center; i++)
                {
                    for (int j = c_i - kernel_center; j <= c_i + kernel_center; j++)
                    {
                        sum += Mat[i][j] * 1; /*Assumption can be reasonably made that since the kernel is almost always 1 so thus it can always be multiplied by one; however, if this approach were to return it must be made so that the values for i & j stay within limits to avoid crashing */ // kernel[i][j];
                    }
                }
                //cout << "DEBUG -- SUM: " << sum << "\n\n"<< endl; // TEST PRINT
                row.push_back(sum);
            }
            output.push_back(row);
            row.clear();
        }

        // Get result of convolution with Kernel as sum

        return output;
    }
    static vector4d create_fast_glcm(vector<vector<int>> imgVec)
    {   
        //normalizeMatrix(imgVec);

        int mi = min2dVec<int>(imgVec);
        int ma = max2dVec<int>(imgVec);
        mi = 0;
        ma = 255;
        vector<int> lines = createLinspace(mi, ma);

        for (int line : lines)
        {

            cout << line << " ";
        }
 
        imgVec = digitize(imgVec, lines);
        cout << "AFTER DIGITIZE #1 " << endl;
        for (vector<int> row : imgVec)
        {
            for (int x : row)
            {
                cout << x << " ";
            }
            cout << endl;
        }
        double dx = distance * cos(angle * (M_PI / 180));
        double dy = distance * sin(-1 * angle * (M_PI / 180));

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
        cout << "AFTER DIGITIZE #2 " << endl;
        for (vector<int> row : shiftedImg)
        {
            for (int x : row)
            {
                cout << x << " ";
            }
            cout << endl;
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
        for (int i = 0; i < glcm.size(); i++)
        {
            for (int j = 0; j < glcm[i].size(); j++)
            {
                glcm[i][j] = filter2D(glcm[i][j], 1, kernel);
            }
        }

        return glcm;
    }
};
// Computes and returns contrast matrice(2d vector)
static vector<vector<int>> glcm_contrast(vector4d glcm)
{

    vector<vector<int>> contrast_matrix(glcm[0][0].size(), vector<int>(glcm[0][0][0].size(),0));
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
    return contrast_matrix;
}

void test_contrast(){
    cout << "----- LOW CONTRAST" << endl;
    vector<vector<int>> glcmLowContrast = {
        {10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11},
        {12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10},
        {11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12},
        {10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11},
        {12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10},
        {11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12},
        {10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11},
        {12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10},
        {11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12},
        {10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11},
        {12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10},
        {11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12},
        {10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11},
        {12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10},
        {11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12, 11, 10, 12}
    };
    vector<vector<int>> contrast = glcm_contrast(fast_glcm<15,15>::create_fast_glcm(glcmLowContrast));
    for (vector<int> vec : contrast)
    {
        cout << endl;
        for (int t : vec)
        {
            cout << t << " ";
        }
    }

cout << "----- MID CONTRAST" << endl;

vector<vector<int>> glcmMidContrast = {
    {50, 200, 60, 180, 70, 160, 80, 140, 90, 120, 100, 110, 105, 115, 125},
    {200, 50, 180, 60, 160, 70, 140, 80, 120, 90, 110, 100, 115, 105, 125},
    {60, 180, 50, 200, 70, 160, 80, 140, 90, 120, 100, 110, 105, 115, 125},
    {180, 60, 200, 50, 160, 70, 140, 80, 120, 90, 110, 100, 115, 105, 125},
    {70, 160, 80, 140, 50, 200, 90, 120, 60, 180, 100, 110, 105, 115, 125},
    {160, 70, 140, 80, 200, 50, 120, 90, 180, 60, 110, 100, 115, 105, 125},
    {80, 140, 90, 120, 60, 180, 50, 200, 70, 160, 100, 110, 105, 115, 125},
    {140, 80, 120, 90, 180, 60, 200, 50, 160, 70, 110, 100, 115, 105, 125},
    {90, 120, 100, 110, 105, 115, 125, 50, 200, 60, 180, 70, 160, 80, 140},
    {120, 90, 110, 100, 115, 105, 125, 200, 50, 180, 60, 160, 70, 140, 80},
    {100, 110, 105, 115, 125, 50, 200, 60, 180, 70, 160, 80, 140, 90, 120},
    {110, 100, 115, 105, 125, 200, 50, 180, 60, 160, 70, 140, 80, 120, 90},
    {105, 115, 125, 50, 200, 60, 180, 70, 160, 80, 140, 90, 120, 100, 110},
    {115, 105, 125, 200, 50, 180, 60, 160, 70, 140, 80, 120, 90, 110, 100},
    {125, 50, 200, 60, 180, 70, 160, 80, 140, 90, 120, 100, 110, 105, 115}
};

vector<vector<int>> contrastMid = glcm_contrast(fast_glcm<15,15>::create_fast_glcm(glcmMidContrast));

for (vector<int> vec : contrastMid)
{
    cout << endl;
    for (int t : vec)
    {
        cout << t << " ";
    }
}

    cout<< "------HIGH CONTRAST" << endl;

    vector<vector<int>> glcmHighContrast = {
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0},
        {255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255},
        {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0}
    };
    vector<vector<int>> contrast2 = glcm_contrast(fast_glcm<15,15>::create_fast_glcm(glcmHighContrast));

    for (vector<int> vec : contrast2)
    {
        cout << endl;
        for (int t : vec)
        {
            cout << t << " ";
        }
    }

}
int main()
{

    vector<int> lines = fast_glcm<5, 2>::createLinspace(2, 82 + 1);
    //
    // arr2d.push_back({2, 3, 4, 6, 7});
    // arr2d.push_back({3, 5, 82, 34, 2});
    vector<vector<int>> arr2d = {{2, 3, 4, 6, 7}, {3, 5, 82, 34, 2}, {2, 3, 7, 5, 8}, {15, 71, 23, 45, 67}, {2, 56, 7, 89, 1}};
    int arr[2][5] = {{2, 3, 4, 6, 7}, {3, 5, 82, 34, 2}};

    // Testing for main create_fast_glcm function
/*
    vector4d output = fast_glcm<5, 5>::create_fast_glcm(arr2d);

    cout << endl;

    for (vector<vector<vector<int>>> x : output)
    {
        for (vector<vector<int>> y : x)
        {
            for (vector<int> z : y)
            {
                for (int a : z)
                {
                    cout << a << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
        cout << endl;
    }
*/
    // Testing the contrast
    test_contrast();
    //vector<vector<int>> contrast = glcm_contrast(output);
    //cout << "CONTRAST OF THE OUTPUT MATRICE: " << contrast[0][0] << endl;
    
    
    // Other testing
    /*cout << "BEFORE CALLING FILTER2D" << endl;
    vector<vector<int>> kernel = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
         vector<vector<int>> output2 = fast_glcm<5, 5>::filter2D(output[0][0], 1, kernel);
         cout << "AFTER CALLING FILTER2D" << endl;
         for (vector<int> row : output2)
         {
             for (int x : row)
             {
                 cout << x << " ";
             }
             cout << endl;
         }
     */

    // Testing for filter2D function
    /*vector<vector<int>> kernel = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
       vector<vector<int>> Mat = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 19, 20}, {21, 22, 23, 24, 25}};
      cout << "BEFORE CALLING FILTER2D" << endl;
       vector<vector<int>> output = fast_glcm<5, 5>::filter2D(Mat, 1, kernel);
       cout << "AFTER CALLING FILTER2D" << endl;
       for (vector<int> row : output)
       {
           for (int x : row)
           {
               cout << x << " ";
           }
           cout << endl;
       } */
    return 0;
}
