#include <iostream>
#include <vector>
#include <cmath>

// Define the image type (2D vector of integers)
using Image = std::vector<std::vector<int>>;

// Function to apply affine transformation
Image warpAffine(const Image& input, const std::vector<std::vector<float>>& mat, int width, int height) {
    // Output image
    Image output(height, std::vector<int>(width, 0));

    // Inverse of the affine transformation matrix
    float det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    if (det == 0) {
        std::cerr << "Matrix is not invertible!" << std::endl;
        return output;
    }

    float inv_mat[2][3] = {
        { mat[1][1] / det, -mat[0][1] / det, (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]) / det },
        { -mat[1][0] / det, mat[0][0] / det, (mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]) / det }
    };

    // Apply the transformation
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Compute the corresponding location in the input image
            float src_x = inv_mat[0][0] * input[y][x] + inv_mat[0][1] * input[y][x] + inv_mat[0][2];
            float src_y = inv_mat[1][0] * input[y][x] + inv_mat[1][1] * input[y][x] + inv_mat[1][2];

            // Nearest-neighbor interpolation
            int src_x_int = static_cast<int>(std::round(src_x));
            int src_y_int = static_cast<int>(std::round(src_y));

            // Handle border replication
            src_x_int = std::max(0, std::min(src_x_int, static_cast<int>(input[0].size()) - 1));
            src_y_int = std::max(0, std::min(src_y_int, static_cast<int>(input.size()) - 1));

            // Assign the pixel value
            output[y][x] = input[src_y_int][src_x_int];
        }
    }

    return output;
}

int main() {
    // Example input image (3x3)
    Image input = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    // Affine transformation matrix (identity matrix with a small shift)
    float angle = 45.0f; // Rotation angle in degrees
float rad = angle * M_PI / 180.0f; // Convert to radians
float dx = 10.0f; // Translation in x
float dy = 10.0f; // Translation in y

std::vector<std::vector<float>> mat = {
    {std::cos(rad), -std::sin(rad), dx},
    {std::sin(rad),  std::cos(rad), dy}
};

    // Output image dimensions
    int width = 3;
    int height = 3;

    // Apply the transformation
    Image output = warpAffine(input, mat, width, height);

    // Print the output image
    for (const auto& row : output) {
        for (int pixel : row) {
            std::cout << pixel << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}