#include "jpeg.h"

// Your implementations here
int JPEG::compress_jpeg(PPM& image, double quality)
{
	vector<vector<double>>blocks = transform.split_image(image, NUM, 8);
	vector<vector<double>>resultBlocks;
	vector<double>temp_block(64);
	vector<Symbol>rlcProb, dpcmProb;
	vector<string> rlcEncodedMsg, dpcmEncodedMsg;
	for (int i = 0; i < NUM; i++)
	{
		transform.modify_values(blocks[i],-128);
		transform.dct_block(blocks[i], &temp_block);
		transform.quantize_block(transform.getTable(), &temp_block, quality);
		rlc(temp_block);
		resultBlocks.push_back(temp_block);
	}
	dpcm(resultBlocks);
	int rlcBits = encode_rlc(rlcProb, rlcEncodedMsg);
	int dpcmBits = encode_dpcm(dpcmProb, dpcmEncodedMsg);
	rlcEncode.populate_symbols("ac_symbols.txt");
	dpcmEncode.populate_symbols("dc_symbols.txt");
	print_encoded(rlcEncodedMsg,dpcmEncodedMsg);

	return rlcBits + dpcmBits + count_bits(rlcVals) + count_bits(dpcmVals);
}

int JPEG::count_bits(const vector<string> &amps)
{
	int count = 0;
	for (unsigned int i = 0; i < amps.size(); i++)
		count += amps[i].length();
	return count;
}

void JPEG::rlc(const vector<double> &vec)
{
	vector<double> block = modify_indices(vec);
	int count, zeroCount = 0;
	int pos = 1;
	string bin;
	for (int i = 1; i < 64; i++)
	{
		if (block[i] != 0)
		{
			count = 0;
			for (int j = pos; j < i; j++)
			{
				if (block[j] == 0)
					count++;
			}

			rlcTable.push_back(std::pair<int, int>(count, get_amplitude((int)block[i], bin)));
			rlcVals.push_back(bin);
			pos = i;
			zeroCount = 0;
		}
		else
		{
			if (check_if_zeroes(block, i))
			{
				rlcTable.push_back(std::pair<int, int>(0, 0));
				rlcVals.push_back("0");
				return;
			}

			zeroCount++;
			if (zeroCount == 16)
			{
				rlcTable.push_back(std::pair<int, int>(15, 0));
				rlcVals.push_back("0");
				zeroCount = 0;
				pos = i + 1;
			}
		}
	}
	rlcTable.push_back(std::pair<int, int>(0, 0));
	rlcVals.push_back("0");
}

bool JPEG::check_if_zeroes(const vector<double> &block, int pos)
{
	for (int i = pos; i < 64; i++)
	{
		if (block[i] != 0)
			return false;
	}
	return true;
}

vector<double> JPEG::modify_indices(const vector<double> &block)
{
	vector<double>res(64);
	for (int i = 0; i < 64; i++)
	{
		res.at(indices[i]) = block[i];
	}
	return res;
}

vector<double> JPEG::invert_indices(const vector<double> &block)
{
	vector<double>res(64);
	for (int i = 0; i < 64; i++)
	{
		res.at(i) = block[indices[i]];
	}
	return res;
}

void JPEG::dpcm(const vector<vector<double>> &blocks)
{
	string bin;
	dpcmTable.push_back(std::pair<int, int>(0, get_amplitude((int)blocks[0][0],bin)));
	dpcmVals.push_back(bin);
	for (int i = 1; i < NUM; i++)
	{
		dpcmTable.push_back(std::pair<int, int>(0, get_amplitude((int)(blocks[i][0] - blocks[i - 1][0]), bin)));
		dpcmVals.push_back(bin);
	}
}

int JPEG::get_amplitude(const int &val, string &bin)
{
	bin.clear();
	bin = bitset<32>(abs(val)).to_string();
	while (bin[0] == '0')
		bin.erase(0, 1);

	if (bin == "")
	{
		bin += "0";
		return 1;
	}
	if (val > 0)
		bin = '0' + bin;
	else
		bin = '1' + bin;
	return bin.length();
}

int JPEG::encode_rlc(vector<Symbol>&prob, vector<string> &rlcEncodedMsg)
{
	rlcEncode.compute_entropy(rlcTable, prob);
	std::sort(prob.begin(), prob.end());
	rlcEncode.build_tree(prob);
	return rlcEncode.encode(rlcTable, rlcEncodedMsg);
}

int JPEG::encode_dpcm(vector<Symbol>&prob, vector<string> &dpcmEncodedMsg)
{
	dpcmEncode.compute_entropy(dpcmTable, prob);
	std::sort(prob.begin(), prob.end());
	dpcmEncode.build_tree(prob);
	return dpcmEncode.encode(dpcmTable, dpcmEncodedMsg);
}

void JPEG::print_encoded(const vector<string> &rlcEncodedMsg, const vector<string> &dpcmEncodedMsg)
{
	for (unsigned int j = 0; j < dpcmEncodedMsg.size(); j++)
	{
		cout << dpcmEncodedMsg[j] << " ";
		cout << dpcmVals[j] << endl;

	}

	for (unsigned int i = 0; i < rlcEncodedMsg.size(); i++)
	{
		cout << rlcEncodedMsg[i] << " ";
		cout << rlcVals[i] << endl;

	}
}

void JPEG::read_encoded_msg()
{
	string str1, str2;
	for (int i = 0; i < 1024; i++)
	{
		cin >> str1;
		dcRuns.push_back(str1);
		cin >> str2;
		dcVals.push_back(str2);
	}
	while (cin>>str1>>str2)
	{
		acRuns.push_back(str1);
		acVals.push_back(str2);
	}
}

void JPEG::decode(vector<pair<int, int>> &rlcPairs, vector<pair<int, int>> &dpcmPairs)
{
	read_encoded_msg();
	rlcDecode.read_symbols("ac_symbols.txt");
	dpcmDecode.read_symbols("dc_symbols.txt");
	rlcDecode.decode(acRuns, rlcPairs);
	dpcmDecode.decode(dcRuns, dpcmPairs);
}

void JPEG::decompress_jpeg(vector<unsigned char> &image, double quality)
{
	vector<pair<int, int>>rlcPairs;
	vector<pair<int, int>>dpcmPairs;
	vector<vector<double>>blocks;
	vector<double>temp_block(64);
	vector<vector<double>>res;

	decode(rlcPairs, dpcmPairs);
	blocks = inverse(rlcPairs, dpcmPairs);
	for (int i = 0; i < NUM; i++)
	{
		transform.dequantize_block(transform.getTable(), &blocks[i], quality);
		transform.idct_block(blocks[i], &temp_block);
		transform.modify_values(temp_block, 128);
		res.push_back(temp_block);
	}
	image = transform.populate_image(res, 8, NUM, 256);
	print(256, 256, image);
}



vector<vector<double>> JPEG::inverse(vector<pair<int, int>> &rlcPairs, const vector<pair<int, int>> &dpcmPairs)
{
	int count = 0;
	double temp;
	vector<vector<double>>res;
	vector<double>block;
	block.push_back(get_value(dcVals[0]));
	int index = irlc(rlcPairs, block, 0);
	int newIndex = 0;
	block = invert_indices(block);
	res.push_back(block);
	block.clear();
	for (int i = 1; i < NUM; i++)
	{
		temp = get_value(dcVals[i]);
		block.push_back(temp + res[i-1][0]);
		newIndex = irlc(rlcPairs, block, index);
		index = newIndex;
		block = invert_indices(block);
		res.push_back(block);
		block.clear();
	}
	return res;
}

double JPEG::get_value(string bin)
{
	if (bin == "0")
		return 0;
	int  num;
	bool neg = (bin[0] == '1');
	bin.erase(0, 1);
	num = stoi(bin, nullptr, 2);

	if (neg)
		num *= -1;

	return num;
}

int JPEG::irlc(vector<pair<int, int>> &rlcPairs, vector<double> &block, int start)
{
	int index = start;
	while (rlcPairs[index] != pair<int,int>(0,0))
	{
		if (rlcPairs[index] == pair<int, int>(15, 0))
		{
			for (int j = 0; j < 16; j++)
				block.push_back(0);
		}
		else
		{
			for (int j = 0; j < rlcPairs[index].first; j++)
				block.push_back(0);
			block.push_back(get_value(acVals[index]));
		}
		index++;
	}
	
	while (block.size() < 64)
		block.push_back(0);
	return index + 1;
}

void JPEG::print(const int &width, const int &height, const vector<unsigned char> &image)
{
	cout << "P2" << endl;
	cout << width << " " << height << endl;
	cout << 255 << endl;
	for (unsigned int i = 0; i < image.size(); i++)
		cout << (int)image[i] << endl;
}