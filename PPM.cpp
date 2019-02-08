#include "PPM.h"

// Read in a PPM file from stdin into the class

void PPM::read_stdin() {
	cin>>type>>width>>height>>intensity;
	int temp;
	type = adjustType();
	
	if (type == "P2")
	{
		grayImage = new unsigned char[width*height];
		for (int i=0; i<width*height; ++i)
		{
			cin>>temp;
			grayImage[i] = temp;
		}
	}
	else if (type == "P3")
	{
		colorImage = new unsigned char*[width*height];
		for(int i = 0; i < width*height; ++i)
		{
			colorImage[i] = new unsigned char[3];
			for (int k = 0; k < 3; k++)
			{
				cin>>temp;
				colorImage[i][k]=temp;
			}
		}
	}
	

}
  
// Write the contents of this PPM to stdout
void PPM::write_stdout() {

	cout<<type<<endl;
	cout<<width<<" "<<height<<endl;
	cout<<intensity<<endl;
	if (type == "P2")
	{
		for (int i=0; i<width*height; ++i)
		{
			cout<<(int)grayImage[i]<<" ";
			if (i%9==0)
				cout<<endl;
		}
		cout<<endl;
	}
	else if (type == "P3")
	{
		for(int i = 0; i < width*height; ++i)
		{
			cout << (int)colorImage[i][0] << " " << (int)colorImage[i][1] << " " << (int)colorImage[i][2] << " " << endl;

		}
		cout<<endl;
	}
}
  
// Substitute a color (r0, g0, b0) and put in its place another
// color (r1, g1, b1) in the whole image. The operation changes
// the current PPM object.
void PPM::sub_color(unsigned char r0,
                    unsigned char g0, 
                    unsigned char b0, 
                    unsigned char r1, 
                    unsigned char g1, 
                    unsigned char b1) {


		for(int i = 0; i < width*height; ++i)
		{
			if (colorImage[i][0] == r0&&colorImage[i][1] == g0&&colorImage[i][2] == b0)
			{
				colorImage[i][0] = r1;
				colorImage[i][1] = g1;
				colorImage[i][2] = b1;
			}
		}
}
  
// Converts this PPM image to gray scale. Note that the input format
// starts with P3 (color image) while the output format starts with
// P2 (grayscale image). The operation changes the current PPM object.
void PPM::rgb_to_gray() {

	cout<<"P2"<<endl;
	cout<<width<<" "<<height<<endl;
	cout<<intensity<<endl;
	for(int i = 0; i < width*height; ++i)
	{
		cout<<(int)(0.298839*colorImage[i][0] + 0.586811*colorImage[i][1] + 0.114350*colorImage[i][2])<<endl;
	}
}

vector<unsigned char> PPM::grayToVector()
{
	vector<unsigned char> img(width*height);
	for (int i = 0; i < width*height; i++)
	{
		img[i] = grayImage[i];
	}
	return img;
}

vector<int> PPM::grayToIntVector()
{
	vector<int> img(width*height);
	for (int i = 0; i < width*height; i++)
	{
		img[i] = grayImage[i];
	}
	return img;
}

string PPM::adjustType()
{
	string res;
	int len = type.length();
	if (len>2)
	{
		if (type[len-1] == '2')
			res="P2";
		else if (type[len-1] == '3')
			res="P3";
		return res;
	}
	return type;
}

void PPM::print(vector<unsigned char> msg, int width, int height, int intensity)
{

	cout<<"P2\n"<<width<<" "<<height<<endl<<intensity<<endl;
	for (unsigned int i = 0; i<msg.size(); i++)
		cout << (int)msg[i] << endl;
}