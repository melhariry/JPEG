// Your includes here
#include "dct.h"
// Your implementations here

void DCT::mul_8x8(const vector<double>& A, const vector<double>& B, vector<double>* C)
{
	double acc = 0;
	int index = 0;
	for (int rows = 0; rows < 8; rows++)
	{
		for (int cols = 0; cols < 8; cols ++)
		{
			for (int y = 0; y < 8; y++)
			{
				acc += A[rows * 8 + y] * B[cols + y * 8];
			}
			C->at(index) = acc;
			index++;
			acc = 0;
		}
	}
}

void DCT::dct_block(const vector<double>& in_block, vector<double>* out_block)
{
	vector <double>temp(64);
	mul_8x8(A,in_block,&temp);
	mul_8x8(temp, At, out_block);
}

void DCT::idct_block(const vector<double>& in_block, vector<double>* out_block)
{
	vector <double>temp(64);
	mul_8x8(At, in_block, &temp);
	mul_8x8(temp, A, out_block);
}

void DCT::quantize_block(const vector<double>& table, vector<double>* block, double quality)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			block->at(i * 8 + j) = round(block->at(i * 8 + j) / (table[i * 8 + j] / quality));
		}
	}
}

void DCT::dequantize_block(const vector<double>& table, vector<double>* block, double quality)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			block->at(i * 8 + j) = block->at(i * 8 + j) * (table[i * 8 + j] / quality);
		}
	}
}

void DCT::compress_dct(PPM& image, double quality, vector<unsigned char> &image_hat)
{
	vector<unsigned char> msg = image.grayToVector();
	vector<vector<double>>blocks = split_image(image, NUM, 8);
	vector<double>temp_block(64);
	for (int i = 0; i < NUM; i++)
	{
		for (int j = 0; j < 64; j++)
			blocks[i][j] -= 128;
		dct_block(blocks[i], &temp_block);
		quantize_block(table, &temp_block, quality);
		dequantize_block(table, &temp_block, quality);
		idct_block(temp_block, &blocks[i]);
		for (int j = 0; j < 64; j++)
			blocks[i][j] += 128;
	}
	image_hat = populate_image(blocks, 8, NUM, 256);
	print(image, image_hat);
}

void DCT::transpose(const vector<double>& in, vector<double>&out)
{
	vector<double> temp = in;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			out[i * 8 + j] = temp[j * 8 + i];
		}
	}
}

vector<vector<double>> DCT::split_image(PPM &image, int num, int n)
{
	vector<vector<double>>blocks;
	int index = 0;
	vector<unsigned char> msg = image.grayToVector();
	initialize(blocks, num);

	for (int x = 0; x < image.get_width(); x++)
	{
		if (x % n == 0 && x != 0)
			index += (int)sqrt(num);

		for (int y = 0; y < image.get_height(); y++)
			blocks.at((int)floor(y / n) + index).push_back((double)msg.at(x*image.get_width() + y));
	}
	return blocks;
}

void DCT::initialize(vector<vector<double>> &blocks, int num)
{
	for (int i = 0; i < num; i++)
	{
		vector<double>myVector;
		blocks.push_back(myVector);
	}
}


vector<vector<double>> DCT::get_n_vectors(vector<vector<double>>& blocks, int n, int index)
{
	vector<vector<double>>ret;
	for (int i = index; i < index + n; i++)
	{
		ret.push_back(blocks[i]);
	}
	return ret;
}

vector<unsigned char> DCT::populate_image(vector<vector<double>>& blocks, int n, int num, int width)
{
	vector<unsigned char>image_hat;
	vector<vector<double>>current;
	for (int i = 0; i < num; i += width / n)
	{
		current = get_n_vectors(blocks, width / n, i);
		for (int l = 0; l < n*n; l += n)
		{
			for (int j = 0; j < width / n; j++)
			{
				for (int k = l; k < l + n; k++)
				{
					if (current[j][k] < 0)
						current[j][k] = 0;
					image_hat.push_back((unsigned char)round(current[j][k]));
				}
			}
		}
	}
	return image_hat;
}

void DCT::print(PPM&image, vector<unsigned char>image_hat)
{
	cout << "P2" << "#SNR="<< compute_snr(image, image_hat) <<endl;
	cout << image.get_width() << " " << image.get_height() << endl;
	cout << 255 << endl;
	for (unsigned int i = 0; i < image_hat.size(); i++)
		cout << (int)image_hat[i] << endl;
}

double DCT::compute_snr(PPM& image, vector<unsigned char>& image_hat)
{
	vector<unsigned char> msg = image.grayToVector();
	double num = 0, den = 0;

	for (unsigned int i = 0; i < msg.size(); i++)
	{
		num += pow(msg[i], 2);
		den += pow((int)msg[i] - (int)image_hat[i], 2);
	}
	return 10 * log10(num / den);
}

void DCT::modify_values(vector<double> &block, int val)
{
	for (int j = 0; j < 64; j++)
		block[j] += val;
}