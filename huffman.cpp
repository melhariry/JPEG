#include "huffman.h"

float Huffman::compute_entropy(const vector<std::pair<int,int>>& msg, vector<Symbol>& prob)
{
	float entropy = 0;
	int count = msg.size();
	Symbol init;
	init.val = msg[0];
	init.prb++;
	prob.push_back(init);
	for (int i = 1; i < count; i++)
	{
		Symbol next;
		next.val = msg[i];
		next.prb++;
		int pos = next.checkExists(prob);
		if (pos != -1)
			prob[pos].prb++;
		else
			prob.push_back(next);
	}
	unsigned int x = 0;
	while (x < prob.size())
	{
		if (prob[x].prb != 0)
		{
			float prb = prob[x].prb;
			prb = prb / (float)count;
			entropy -= prb*log2(prb);
			x++;
		}
		else
			prob.erase(prob.begin() + x);
	}
	return entropy;
}

void Huffman::print_code_table()
{
	for (map<std::pair<int,int>, string >::const_iterator it = hashTable.begin(); it != hashTable.end(); ++it)
	{
		std::cout << it->first.first << " " << it->first.second << " " << it->second << std::endl;
	}
}

void Huffman::encodeSymbols(Node * root, string bin)
{
	if (root->left == NULL && root->right == NULL)
		hashTable.insert(std::pair<std::pair<int,int>, string >(root->sym.val, bin));
	if (root->left != NULL)
		encodeSymbols(root->left, bin + '1');
	if (root->right != NULL)
		encodeSymbols(root->right, bin + '0');
}


int Huffman::encode(const vector<std::pair<int,int>>& msg, vector<string> &encoded_msg)
{
	map<std::pair<int,int>, string>::iterator lookup;
	int count = 0;

	for (unsigned int i = 0; i < msg.size(); i++)
	{
		lookup = hashTable.find(msg[i]);

		if (lookup != hashTable.end())
		{
			count += lookup->second.length();
			encoded_msg.push_back(lookup->second);
		}
	}
	return count;
}

void Huffman::build_tree(const vector<Symbol>& prob)
{
	vector<Node>nodes(prob.size());

	for (unsigned int i = 0; i<prob.size(); i++)
	{
		nodes[i].sym = prob[i];
	}

	while (nodes.size() > 1)
	{
		Node l, r, parent;
		l = nodes[0];
		r = nodes[1];

		parent.left = new Node();
		parent.right = new Node();
		memcpy(parent.left, &l, sizeof(l));
		memcpy(parent.right, &r, sizeof(r));
		parent.sym.prb = l.sym.prb + r.sym.prb;
		nodes[0] = parent;

		if (nodes.size() > 1)
		{
			nodes.erase(nodes.begin() + 1);
			std::sort(nodes.begin(), nodes.end());
		}
	}
	tree = &nodes[0];
	encodeSymbols(tree, "");
}


void Huffman::populate_symbols(const string &file)
{
	ofstream out;
	out.open(file);
	for (map<std::pair<int, int>, string >::const_iterator it = hashTable.begin(); it != hashTable.end(); ++it)
	{
		out << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
	out.close();
}

void Huffman::read_symbols(const string &file)
{
	ifstream in;
	in.open(file);
	int val1, val2;
	string str;
	while (!in.eof())
	{
		in >> val1;
		in >> val2;
		in >> str;
		decodeTable.insert(pair<string, pair<int, int>>(str,pair<int,int>(val1,val2)));
	}
	in.close();
}

void Huffman::decode(const vector<string>& encoded_msg, vector<pair<int, int>> &msg)
{
	map<string, pair<int, int>>::iterator lookup;
	int count = 0;

	for (unsigned int i = 0; i < encoded_msg.size(); i++)
	{
		lookup = decodeTable.find(encoded_msg[i]);

		if (lookup != decodeTable.end())
			msg.push_back(lookup->second);
	}
}