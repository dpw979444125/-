#pragma once

#include<queue>
using namespace std;
template<typename W>
struct HuffmanTreeNode
{
	W _w;
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;

	HuffmanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

template<typename W>
class HuffmanTree
{
	
	typedef HuffmanTreeNode<W> Node;

	struct PtrNodeCompare
	{
		bool operator()(Node* n1, Node* n2)
		{
			return n1->_w > n2->_w;
		}
	};
public:
	HuffmanTree()
		:_root(NULL)
	{}
	HuffmanTree(W* a, size_t n, const W& invalid)      //����huffman��
	{		
		//���ȼ������д�ڵ㣬PtrNodeCompare�Ƿº���
		priority_queue<Node*, vector<Node*>, PtrNodeCompare> minheap;
		for (size_t i = 0; i < n; ++i)
		{
			//ֱ�ӽ�Ȩֵ�����ɽڵ�Ž�����
			if (a[i] != invalid)
			{
				minheap.push(new Node(a[i]));
			}
		}
		while (minheap.size() > 1)     
		{
			//�ȷŽ������棬��С�ѣ��Զ���������С�������棬ÿ������С�Ľ����������ڵ�ֻ��һ��ʱ�򹹽����
			Node* left = minheap.top();
			minheap.pop();
			Node* right = minheap.top();
			minheap.pop();

			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;

			minheap.push(parent);
		}
		_root = minheap.top();
	}

	Node* GetRoot()
	{
		return _root;
	}

	~HuffmanTree()
	{
		Destroy(_root);
		_root = NULL;
	}

	void Destroy(Node* root)
	{
		if (root == NULL)
		{
			return;
		}

		Destroy(root->_left);
		Destroy(root->_right);

		delete root;
	}
private:
	HuffmanTree(const HuffmanTree<W>& t);                 //��������
	HuffmanTree<W>& operator=(const HuffmanTree<W>& t);   //��ֵ���������
protected:
	Node* _root;
};

void TestTree()
{
	int a[] = { 1,2,3,5,6,7,8,9 };
	HuffmanTree<int> t(a, sizeof(a) / sizeof(a[0]), 0);
}