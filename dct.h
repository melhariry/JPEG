#ifndef _LBG_H
#define _LBG_H
#define NUM 1024
#define PI 3.14159265
#include<vector>
#include<math.h>
#include "PPM.h"

using namespace std;

class DCT {
public:
  // Constructor and destructor
	DCT()
	{
		table.resize(64);
		At.resize(64);
		table = { 16, 11, 10, 16, 24, 40, 51, 61,
			12, 12, 14, 19, 26, 58, 60, 55,
			14, 13, 16, 24, 40, 57, 69, 56,
			14, 17, 22, 29, 51, 87, 80, 62,
			18, 22, 37, 56, 68, 109, 103, 77,
			24, 35, 55, 64, 81, 104, 113, 92,
			49, 64, 78, 87, 103, 121, 120, 101,
			72, 92, 95, 98, 112, 100, 103, 99 };
		for (int i = 0; i < 64; i++)
		{
			if (i < 8)
				A.push_back(sqrt(1.0f / 8.0f));
			else
				A.push_back(0.5f * cos((i / 8)*PI*(2.0f * (i % 8) + 1.0f) / 16.0f));
		}
		transpose(A, At);
	}
  // A function to compute the product of two 8x8 matrices represented
  // as vectors in row-major order.
  // Each matrix is represented in a row-major order i.e. 
  // A[0] -> A[7] is the first row, A[8] -> A[15] is the second row ... etc.
  // 
  // - A: the first input
  // - B: the second input
  // - C: the output where C = A B
  void mul_8x8(const vector<double>& A,
               const vector<double>& B,
               vector<double>* C);
  
  // A function to compute the DCT of an input 8x8 block of pixels.
  // - in_block: the input 8x8 pixel block. 
  // - out_block: the DCT transform of the input block.
  //
  // The DCT is performed by applying the DCT on the rows first, then
  // on the columns, or equivalently computing the matrix product:
  // Y = A X A' where Y is the DCT of the input block X, and A is the
  // DCT transform matrix.
  void dct_block(const vector<double>& in_block,
                 vector<double>* out_block);
                 
  // A function to compute the Inverse DCT of an input 8x8 block of pixels.
  // - in_block: the input 8x8 DCT block. The block is represented in
  //     a row-major order i.e. in_block[0] -> in_block[7] is the first
  //     row, in_block[8] -> in_block[15] is the second row ... etc.
  // - out_block: the inverse DCT transform of the input block.
  void idct_block(const vector<double>& in_block,
                  vector<double>* out_block);

  // A function to quantize an 8x8 block using a quantization table.
  // - table: the quantization table to use.
  // - block: the block to be quantized. The quantized value overwrites
  //   the input block. The quantization is as defined in the lectures
  //   i.e. block[i,j] = round(block[i,j] / table[i,j])
  void quantize_block(const vector<double>& table,
					  vector<double>* block, double quality);

  // A function to de-quantize an 8x8 block using a quantization table.
  // - table: the quantization table to use.
  // - block: the block to be de-quantized i.e. reconstructed. 
  //   The de-quantized value overwrites the input block. 
  //   The de-quantization is as defined in the lectures
  //   i.e. block[i,j] = block[i,j] * table[i,j]
  void dequantize_block(const vector<double>& table,
						vector<double>* block, double quality);
                      
  // A function to compress using DCT. It will peform four operations:
  // 1. Computes the DCT transform on the input image
  // 2. Quantize the DCT coefficients using the given quality factor
  // 3. Reconstruct the quantized coefficients to obtain an approximation
  //   of the DCT
  // 4. Reconstruct the image using the inverse DCT and return the 
  //   reconstructed image.
  // 
  // Note: before computing the DCT on the input blocks (before step 1), 
  // subtract 128 from each pixel, and add it back after computing the 
  // inverse DCT (after step 4).
  //
  // - image: the input image
  // - quality: an integer defining the quality of the compression
  //   to obtain the quantization matrix corresponding to the given 
  //   quality level, divide the "standard" quantization table
  //   by the "quality" factor i.e. higher quality
  //   values means lower quanitzation steps and higher quality images  e.g.
  //   for quality = 2, the quantized value for coefficient [i,j]
  //   equals round( y[i,j] * quality / q[i,j]) or the quantization
  //   step will be (q[i,j] / 2).
  //   A quality value of 1 means to use the standard table defined in the
  //   lecture slides.
  // - image_hat: The reconstructed image.
  void compress_dct(PPM& image, double quality,
					vector<unsigned char> &image_hat);

  vector<unsigned char> populate_image(vector<vector<double>>& blocks, int n, int num, int width);
  vector<vector<double>> get_n_vectors(vector<vector<double>>& blocks, int n, int index);
  vector<vector<double>> split_image(PPM &image, int num, int n);
  vector<double> getTable() { return table; }
  void modify_values(vector<double> &block, int val);

private:
  // Any data members here ... 
	vector<double>A;
	vector<double>At;
	vector<double>table;
	void transpose(const vector<double>& in, vector<double>&out);
	void initialize(vector<vector<double>> &blocks, int num);
	void print(PPM&image, vector<unsigned char>image_hat);
	double compute_snr(PPM& image, vector<unsigned char>& image_hat);
};

#endif