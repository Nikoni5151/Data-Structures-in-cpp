//B-tree feat.cpp
#include<iostream>

using namespace std;

#define rank 5			//B树的阶
#define NaN -2147483647	//未定义数，用于初始化value

class Node;				//结点类
typedef  Node* PNode;	//指向结点的指针
typedef PNode* BTree;	//树：指向根节点的指针

inline bool IsEmpty(PNode ptr);			//ptr结点不含数据 ? 1 : 0;
inline bool IsFull(PNode ptr);			//ptr结点数据已满(比标准多一个) ? 1 : 0;
inline bool IsLeaf(PNode ptr);			//ptr结点是叶节点 ? 1 : 0;
inline bool IsRoot(PNode ptr);			//ptr节点是根节点 ? 1 : 0;
inline bool HasValue(PNode ptr,int v);	//ptr节点已经存在值v ? 1 : 0;

int Insert0(PNode ptr, int v);
int Insert(PNode ptr,int v);
int Add(BTree tree, PNode ptr, int v);	//尝试向tree中的ptr节点加入v值
int Add(BTree tree, PNode ptr,int v,PNode newnode);	//尝试向tree中的ptr节点加入v值和newnode指针
int Divide(BTree tree, PNode ptr);		//分裂tree的ptr节点

class Node
{
public:
	int num = 0;
	PNode fa = nullptr;
	PNode* son = new PNode[rank + 1];
	int* value = new int[rank];
	Node()//构造函数
	{
		this->fa = this;
		for (int i = 0; i < rank+1; i++)
			this->son[i] = nullptr;
		for (int i = 0; i < rank; i++)
			this->value[i] = NaN;
	}
	~Node()//析构函数
	{
		delete son;
		delete value;
		son = nullptr;
		value = nullptr;
	}

};
inline bool IsEmpty(PNode ptr)
{
	return ptr->num == 0 ? 1 : 0;
}

inline bool IsFull(PNode ptr)
{
	return ptr->num >= rank ? 1 : 0;
}

inline bool IsLeaf(PNode ptr)
{
	if (ptr->son[0] == nullptr)
		return 1;
	else
		return 0;
}

inline bool IsRoot(PNode ptr)
{
	return ptr->fa == ptr ? 1 : 0;
}

inline bool HasValue(PNode ptr, int v)
{
	for (int i = 0; i < ptr->num; i++)
		if (ptr->value[i] == v)
			return 1;
	return 0;
}

int Insert0(PNode ptr, int v)
{
	if (HasValue(ptr, v)) return -1; //如果ptr节点已包含v值，返回-1；正常插入，返回-2；
	for (int i = 0; i < ptr->num; i++)
	{
		if (v < ptr->value[i])
		{
			for (int j = rank; j >i; j--)
			{
				ptr->value[j] = ptr->value[j - 1];
			}

			ptr->value[i] = v;
			ptr->num++;
			return -2;
		}
	}
	ptr->value[ptr->num] = v;
	ptr->num++;
	return -2;
}

int Insert(PNode ptr, int v)
{
	if (HasValue(ptr, v)) return -1; //如果ptr节点已包含v值，返回-1：

	if (!IsLeaf(ptr))	//如果ptr不是叶节点，返回应递归查找的son指针编号
	{
		for (int i = 0; i < ptr->num; i++)
		{
			if (v < ptr->value[i])
			{
				//Insert(ptr->son[i], v);
				return i;
			}
		}
		//Insert(ptr->son[ptr->num], v);
		return ptr->num;
	}
	else				//如果ptr是叶节点，将v插入ptr;
	{
		for (int i = 0; i < ptr->num; i++)
		{
			if (v < ptr->value[i])
			{
				for (int j = rank; j >i; j--)
				{
					ptr->value[j] = ptr->value[j - 1];
				}

				ptr->value[i] = v;
				ptr->num++;
				return -2;
			}
		}
		ptr->value[ptr->num] = v;
		ptr->num++;
		return -2;
	}

	return -3;
}

int Add(BTree tree, PNode ptr, int v)	//如果添加后ptr节点数据数量超限，将ptr节点分裂
{
	int state = Insert(ptr, v);
	if (state == -3)
	{
		cout << "Insert函数运行未知错误！" << endl;
		return -3;
	}
	if (state == -1)
	{
		cout << "添加错误！数值 " << v << " 已在节点中出现" << endl;;
		return -1;
	}
	if (state == -2)
	{
		if (IsFull(ptr)) Divide(tree, ptr);
		return -2;
	}
	if (state >= 0)
	{
		Add(tree, ptr->son[state], v);
		return 0;
	}
	return -4;
}

int Add(BTree tree, PNode ptr, int v, PNode newnode)
{
	int state = Insert0(ptr, v);
		
	if (state ==-1||state==-3)
		return state;
	if (state == -2)
	{
		ptr->son[ptr->num] = newnode;
		if (IsFull(ptr)) Divide(tree, ptr);
		return -2;
	}
	return -4;
}

int Divide(BTree tree, PNode ptr)
{
	int mid = rank / 2;
	int midvalue = ptr->value[mid];
	Node* newnode=new Node;
	PNode pnnode=newnode;
	pnnode->fa = ptr->fa;	//分裂的新节点的父亲和ptr节点的父亲相同

//copy：将ptr节点右半部分复制到newnode节点
	for (int i = mid + 1; i < rank; i++)
	{
		pnnode->value[i - mid - 1] = ptr->value[i];
		pnnode->num++;
	}
	for (int i = mid ; i < rank+1; i++)
	{
		pnnode->son[i - mid - 1] = ptr->son[i];
	}

//delete：将ptr节点已被复制的部分和mid删除
	for (int i = mid; i < rank; i++)
	{
		ptr->value[i]=NaN;
		ptr->num--;
	}
	for (int i = mid+1; i < rank + 1; i++)
	{
		ptr->son[i]=nullptr;
	}

	if (IsRoot(ptr))
	{
		Node* newroot=new Node;
		PNode pnroot=newroot;
		pnroot->num++;
		pnroot->value[0] = midvalue;
		pnroot->son[0] = ptr;
		ptr->fa = pnroot;
		pnroot->son[1] = pnnode;
		pnnode->fa = pnroot;
		*tree=newroot;
	}
	else
	{
		Add(tree, ptr->fa, midvalue, pnnode);
	}
	return 0;
}

void ShowTree(PNode tree,int deep)
{
	cout << "----------------" << endl;
	cout << "这是第" << deep << "层" << endl;	

	cout << "该节点存储的数据：" << endl;
	for (int i = 0; i < tree->num; i++)
		cout << tree->value[i] << ' ';
	cout << endl;
	
	if (IsRoot(tree))
		cout << "这是根节点" << endl;
	else
		cout << "该节点的父节点是" << tree->fa->value[0] << endl;	


	if (!IsLeaf(tree))
	{
		for (int i = 0; i < tree->num + 1; i++)
			ShowTree(tree->son[i], deep + 1);
	}
}

int main()
{
	Node root;
	root.fa = &root;
	PNode ptroot = &root;
	BTree tree = &ptroot;
	int n;
	cin >> n;
	for (int i = 0; i < n;i++)
		Add(tree, ptroot,i);

	ShowTree(*tree, 0);
	cout << "----------------" << endl;
}