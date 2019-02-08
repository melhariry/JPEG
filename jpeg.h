#ifndef _JPEG_H
#define _JPEG_H

#include<vector>
#include <bitset>

#include "PPM.h"
#include "dct.h"
#include "huffman.h"

class JPEG {
public:
  // Constructor and destructor
	JPEG()
	{
		indices.resize(64);
		indices = { 0, 1, 5, 6, 14, 15, 27, 28,
					2, 4, 7, 13, 16, 26, 29, 42,
					3, 8, 12, 17, 25, 30, 41, 43,
					9, 11, 18, 24, 31, 40, 44, 53,
					10, 19, 23, 32, 39, 45, 52, 54,
					20, 22, 33, 38, 46, 51, 55, 60,
					21, 34, 37, 47, 50, 56, 59, 61,
					35, 36, 48, 49, 57, 58, 62, 63 };
	}

  // A function to compress using a JPEG-like algorithm. It will peform the
  // following operations:
  // 1. Computes the DCT transform for each 8x8 block in the input image
  // 2. Quantize the DCT coefficients using the given quality factor
  // 3. Perform RLC on the AC coefficients of each 8x8 block independently
  //    using the zig-zag ordering
  // 4. Encode the resulting triplets (runlength, size, value) using Huffman
  //    coding
  // 5. Perform DPCM on the DC coefficients of all the 8x8 blocks in the image
  //    in row-major order
  // 6. Encode resulting pairs (size, value) using Huffman coding
  // 
  // Note: before computing the DCT on the input blocks (before step 1), 
  // subtract 128 from each pixel, and add it back after computing the 
  // inverse DCT (after step 4).
  //
  // You don't need to perform any "bit packing". The output could just be
  // any ASCII or binary representation that you can easily decode. However,
  // make sure you count the number of bits required to compute the size of 
  // the output file and the compression ratio.
  //
  // You can assume you are using a standard quantization table (you don't 
  // need to include it in the output file), but you need to send the 
  // Huffman tables to (include it in the output file).
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
  // - compressed_image: the representation of the compressed image.
  //
  // returns: the number of bits in the compressed image.
  int compress_jpeg(PPM& image, double quality);
                    

  // A function to decompress an image compressed using compressed_jpeg() 
  // above. The function performs the inverse of the operations performed
  // in the encoding process:
  // 
  // 1. Decode the RLC-coded AC coefficients for the 8x8 blocks
  // 2. Decode the DPCM-coded DC coefficients
  // 3. Perform the inverse quantization with the given quality factor
  // 4. Perform the inverse DCT to reconstruct the image
  //
  // - compressed_image: the encoded image from compress_jpeg(). This will
  //     include all the information you need (quality, Huffman tables, ...)
  // - image: the reconstructed image
  //
  // returns: the number of bits in the compressed image.
  void decompress_jpeg(vector<unsigned char> &image, double quality);

  void read_encoded_msg();
private:
  // Any data members here ...
	bool check_if_zeroes(const vector<double> &block, int pos);
	void print_encoded(const vector<string> &rlcEncodedMsg, const vector<string> &dpcmEncodedMsg);
	void rlc(const vector<double> &vec);
	int encode_rlc(vector<Symbol>&prob, vector<string> &rlcEncodedMsg);
	void dpcm(const vector<vector<double>> &blocks);
	int encode_dpcm(vector<Symbol>&prob, vector<string> &dpcmEncodedMsg);
	int get_amplitude(const int &val, string &bin);
	double get_value(string bin);
	vector<double> modify_indices(const vector<double> &block);
	void decode(vector<pair<int, int>> &rlcPairs, vector<pair<int, int>> &dpcmPairs);
	vector<vector<double>> inverse(vector<pair<int, int>> &rlcPairs, const vector<pair<int,int>> &dpcmPairs);
	int irlc(vector<pair<int, int>> &rlcPairs, vector<double> &block, int start);
	vector<double> invert_indices(const vector<double> &block);
	void print(const int &width, const int &height, const vector<unsigned char> &image);
	int count_bits(const vector<string> &amps);
	DCT transform;
	Huffman rlcEncode, dpcmEncode, rlcDecode, dpcmDecode;
	vector <pair<int, int>> rlcTable, dpcmTable;
	vector <string>rlcVals, dpcmVals;
	vector<int>indices;
	vector<string>acRuns, dcRuns, acVals, dcVals;
};

#endif