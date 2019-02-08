#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <vector>
#include <algorithm>
#include <math.h>
#include <map>
#include <string>
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

// Define a symbol struct here to hold the symbol (grayscale value)
// and its probability/frequency
struct Symbol {
  // Grayscale value of the symbol
	std::pair<int, int>val;
	float prb;
	bool operator<(const Symbol &rhs) const { return prb < rhs.prb; }
	int checkExists(vector<Symbol>& prob)
	{
		for (unsigned int i = 0; i < prob.size(); i++)
		{
			if (prob[i].val == (*this).val)
				return i;
		}
		return -1;
	}
	Symbol()
	{
		val = std::pair<int, int>(0, 0);
		prb = 0;
	}
  // ...
};

// Define the tree node struct here ...
struct Node {
	// ...
	Symbol sym;
	Node*left = NULL;
	Node*right = NULL;
	bool operator<(const Node &rhs) const { return  sym< rhs.sym; }
};

class Huffman {
public:
  // Constructor and destructor

  // A function to compute the entropy of the input message and
  // stores the probabilities of the different symbols in
  // the input array.
	float compute_entropy(const vector<std::pair<int, int>>& msg,
                        vector<Symbol>& prob);
  
  // A function to build the Huffman tree given the computed
  // symbol probabilities.
  void build_tree(const vector<Symbol>& prob);
  
  // A function to print the code table computed from the build_tree above.
  void print_code_table();
  
  // A function to encode the input message using the computed tree 
  // and store the output in the output array, where each entry
  // corresponds to the codeword for an input symbol in the message.
  //
  // The function should return the number of bits in the encoded 
  // message.
  int encode(const vector<std::pair<int, int>>& msg,
             vector<string> &encoded_msg);

  void decode(const vector<string>& encoded_msg,
	  vector<pair<int,int>> &msg);

  map<std::pair<int, int>, string>getHashtable() { return hashTable; };
  map<string, pair<int, int>>getdeocdeTable() { return decodeTable; };
  void populate_symbols(const string &file);
  void read_symbols(const string &file);

private:
  // Data members here ... for example to store the tree and the codeword 
  // hash table
  Node* tree;
  map<std::pair<int,int>, string> hashTable;
  map<string, pair<int,int>> decodeTable;
  void encodeSymbols(Node * root, string bin);
  // ...
};

#endif