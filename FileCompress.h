#pragma once
#pragma warning(disable:4996)
#include <string>
#include <fstream>
#include<algorithm>
#include<assert.h>
#include<iostream>
using namespace std;

#include "HuffmanTree.h"

typedef long long LongType;

struct CharInfo
{
	char _ch;
	LongType _count;
	string _code;
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}

	// w1 > w2
	bool operator>(const CharInfo& info)
	{
		return _count > info._count;
	}

	bool operator!=(const CharInfo& info)
	{
		return _count != info._count;
	}

};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	struct TmpInfo
	{
		char _ch;
		LongType _count;
	};
public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_hashInfos[i]._ch = i;
			_hashInfos[i]._count = 0;
		}
	}
	size_t c = 0;
	void Compress(const char* file)
	{
		//1.统计文件中字符出现的次数
	/*	ifstream ifs(file);
		char ch;
		while (ifs.get(ch))
		{
			_hashInfos[ch]._count++;
		}*/
		
		FILE* fout = fopen(file, "r");
		assert(fout);
		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_hashInfos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
	

		//2.构建huffman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);

		//3.生成code,因为解压缩需要遍历树，所以不用哈夫曼树生成code,直接放到_hashInfos
		GenerateHuffmanCode(tree.GetRoot());

		//4.压缩
		string compressFile = file;
		compressFile += ".huffman";
		FILE* fin = fopen(compressFile.c_str(), "wb");
		//assert(fin);
		/*fseek(fout, 0, SEEK_SET);*/
		//ofstream ofs(compressFile.c_str());

		//写入字符出现的信息,  为了让解压缩单独运行，不用将哈夫曼编码写入Tmpinfo
		for (size_t i = 0; i < 256; ++i)
		{
			if (_hashInfos[i]._count > 0)
			{
				TmpInfo info;
				info._ch = _hashInfos[i]._ch;
				info._count = _hashInfos[i]._count;
				fwrite(&info, sizeof(TmpInfo), 1, fin);

			}
		}
		TmpInfo info;
		info._count = -1;
		fwrite(&info, sizeof(TmpInfo), 1, fin);
		rewind(fout);
		//ifs._Seekbeg;
		ch = fgetc(fout);
		//ifs.seekg(0);
		char value = 0;
		int pos = 0;
	
		while (ch != EOF)
		{
		
			string& code = _hashInfos[(unsigned char)ch]._code;    //code是每个ch的code
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}
				else if(code[i] == '1')
				{
					value |= (1 << pos);
				}
				else
				{
					assert(false);
				}
				++pos;
				if (pos == 8)
				{
					fputc(value, fin);
					value = 0;
					pos = 0;
				}
				c++;
				
			}
			ch = fgetc(fout);
		}
		cout << c << endl;
		//剩余位以0补齐
		if (pos > 0)
		{
			fputc(value, fin);
		}
		fclose(fout);
		fclose(fin);
	}

	//生成huffman code，前序
	void GenerateHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;
		
		if (root->_left == NULL && root->_right == NULL)  //叶子节点
		{
			string code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (cur == parent->_left)
				{
					code.push_back('0');
				}
				else
				{
					code.push_back('1');
				}
				//走上父节点
				cur = parent;
				parent = cur->_parent;
			}
			//因为从下往上数，编码是反的，需要翻转
			reverse(code.begin(), code.end());
			_hashInfos[(unsigned char)root->_w._ch]._code = code;     //用unsigned char 是为了写入汉字
				
			return;
		}
		GenerateHuffmanCode(root->_left);
		GenerateHuffmanCode(root->_right);
	}

	void Uncompress(const char* file)
	{
		size_t n = 0;
		assert(file);
		string uncompressFile = file;
		size_t pos = uncompressFile.rfind('.');    //倒序查找.
		assert(pos != string::npos);         //没有找到返回npos(最大的无符号整形)
		uncompressFile.erase(pos, uncompressFile.size() - pos);
		uncompressFile += ".unhuffman";

		FILE* fout = fopen(file, "rb");
		assert(fout);
		FILE* fin = fopen(uncompressFile.c_str(), "w");
		assert(fin);

		//读取压缩文件中的数据，解压缩
		TmpInfo info;
		fread(&info, sizeof(TmpInfo), 1, fout);
		while (info._count != -1)
		{
			_hashInfos[(unsigned char)info._ch]._ch = info._ch;
			_hashInfos[(unsigned char)info._ch]._count = info._count;
			fread(&info, sizeof(TmpInfo), 1, fout);
		}

		//1.重建huffman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		Node* root = tree.GetRoot();
		Node* cur = root;
		long long fileSize = root->_w._count;
		cout << fileSize<<endl;
		//2.解压缩
		char value = fgetc(fout);
		while (1)
		{
			for (size_t pos = 0; pos < 8; ++pos)
			{
				if (value & (1 << pos))
					cur = cur->_right;
				else
					cur = cur->_left;
				if (NULL == cur->_left && NULL == cur->_right)
				{
					fputc(cur->_w._ch, fin);        //还原完一个回到根节点，继续
					cur = root;

					if (--fileSize == 0)
					{
						break;
					}
				}
				n++;	
			}
			if (fileSize == 0)
			{
				break;
			}
			value = fgetc(fout);
		}
		cout << n << endl;
		fclose(fout);
		fclose(fin);
	}


protected:
	CharInfo _hashInfos[256];
};

void Test()
{
	FileCompress fc;
	fc.Compress("input.txt");
	fc.Uncompress("input.txt.huffman");
	//通过beyond compare工具验证压缩是否正确
}