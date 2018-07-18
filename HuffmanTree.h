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
	HuffmanTree(W* a, size_t n, const W& invalid)      //构建huffman树
	{		
		//优先级队列中存节点，PtrNodeCompare是仿函数
		priority_queue<Node*, vector<Node*>, PtrNodeCompare> minheap;
		for (size_t i = 0; i < n; ++i)
		{
			//直接将权值构建成节点放进堆里
			if (a[i] != invalid)
			{
				minheap.push(new Node(a[i]));
			}
		}
		while (minheap.size() > 1)     
		{
			//先放进堆里面，是小堆，自动调整，最小的在上面，每次拿最小的建树，当树节点只有一个时候构建完成
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
	HuffmanTree(const HuffmanTree<W>& t);                 //拷贝构造
	HuffmanTree<W>& operator=(const HuffmanTree<W>& t);   //赋值运算符重载
protected:
	Node* _root;
};

void TestTree()
{
	int a[] = { 1,2,3,5,6,7,8,9 };
	HuffmanTree<int> t(a, sizeof(a) / sizeof(a[0]), 0);
}