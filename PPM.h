#ifndef _PPM_H
#define _PPM_H

// Put your includes here ...
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
using namespace std;
class PPM {
private:
	unsigned char**colorImage;
	unsigned char*grayImage;
	int width, height, intensity;
	string type;
    string adjustType();

public:
  // Consturctors and destructors here
  // ...
	PPM()
	{
		width=0;
		height=0;
    type="";
	}

  // Read in a PPM file from stdin into the class
  void read_stdin();
  void readType();
  // Write the contents of this PPM to stdout
  void write_stdout();
  void print(vector<unsigned char> msg, int width, int height, int intensity);
  
  // Substitute a color (r0, g0, b0) and put in its place another
  // color (r1, g1, b1) in the whole image. The operation changes
  // the current PPM object.
  void sub_color(unsigned char r0,
                 unsigned char g0, 
                 unsigned char b0, 
                 unsigned char r1, 
                 unsigned char g1, 
                 unsigned char b1);
  
  // Converts this PPM image to gray scale. Note that the input format
  // starts with P3 (color image) while the output format starts with
  // P2 (grayscale image). The operation changes the current PPM object.
  void rgb_to_gray();
  
  vector<unsigned char> grayToVector();
  vector<int> grayToIntVector();

  int get_width() { return width; }
  int get_height() { return height; }
};

#endif