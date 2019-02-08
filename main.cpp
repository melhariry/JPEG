// includes here
#include "jpeg.h"
#include <iostream>
#include <string>
#define SIZE 524288.0f

using namespace std;

int main(int argc, char* argv[])
{
	string op = argv[1];
	JPEG compressJPG;
	vector<double>block;
	if (op == "-compress")
	{
		double quality = stod(argv[2]);
		PPM image;
		image.read_stdin();
		int bits = compressJPG.compress_jpeg(image, quality);
		cout << bits << endl;
		cout << bits / SIZE;
	}
	else if (op == "-decompress")
	{
		double quality = stod(argv[2]);
		vector<unsigned char>image;
		compressJPG.decompress_jpeg(image, quality);
	}
	return 0;
}