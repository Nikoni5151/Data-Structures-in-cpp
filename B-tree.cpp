//B-tree feat.cpp version 2.0
#include<iostream>
#include<deque>
#include<string>

using namespace std;

#define rank 3		//B树的阶
#define NaN -2147483647	//未定义数，用于初始化value

class Node;				//结点类
typedef  Node* PNode;	//指向结点的指针
typedef PNode* BTree;	//树：指向根节点的指针
class Location;			//某一数据在节点中的位置，由指向节点的指针和偏移量构成

inline bool IsEmpty(PNode ptr);			//ptr结点不含数据 ? 1 : 0;
inline bool IsFull(PNode ptr);			//ptr结点数据已满(比标准多一个) ? 1 : 0;
inline bool IsLeaf(PNode ptr);			//ptr结点是叶节点 ? 1 : 0;
inline bool IsRoot(PNode ptr);			//ptr节点是根节点 ? 1 : 0;
inline bool HasValue(PNode ptr, int v);	//ptr节点已经存在值v ? 1 : 0;

int Insert0(PNode ptr, int v);
int Insert(PNode ptr, int v);
int Add(BTree &tree, PNode ptr, int v);	//尝试向tree中的ptr节点加入v值
int Add(BTree &tree, PNode ptr, int v, PNode newnode);	//尝试向tree中的ptr节点加入v值和newnode指针
int Divide(BTree &tree, PNode ptr);		//分裂tree的ptr节点
int Delete(BTree &tree, PNode ptr, int v);	//删除tree中v值
void Move(PNode ptr,int v);						//移动删除后的节点
int Check(BTree &tree, PNode ptr);		//检查并修正ptr节点值的数量使之符合标准
int Borrow(BTree &tree, PNode ptr);		//尝试向ptr->fa借一个节点
Location Find(PNode root, int v);		//在根为root的树中查找值v，返回值v的为止，若不存在，返回nullptr

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
		for (int i = 0; i < rank + 1; i++)
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

class Location
{
public:
	PNode pnode;
	int number;
	Location()
	{
		this->pnode = nullptr;
		this->number = 0;
	}
	void set(PNode p, int n)
	{
		this->pnode = p;
		this->number = n;
	}

};
int Insert0(PNode ptr, int v)		//简化Insert，保证ptr中一定可以插入v时使用
{
	if (HasValue(ptr, v)) return -1; //如果ptr节点已包含v值，返回-1；正常插入，返回-2；
	if (ptr->num == 0)
	{
		ptr->value[0] = v;
		ptr->num++;
		return -2;
	}
	else
	{
		for (int i = 0; i < ptr->num; i++)
		{
			if (v < ptr->value[i])
			{
				for (int j = rank; j > i; j--)
				{
					ptr->value[j] = ptr->value[j - 1];
				}
				/*for (int j = rank+1; j > i; j--)
				{
					ptr->son[j] = ptr->son[j - 1];
				}*/

				ptr->value[i] = v;
				ptr->num++;
				return -2;
			}
		}
		ptr->value[ptr->num] = v;
		ptr->num++;
	}
	return -2;
}

int Insert0(PNode ptr, int v,PNode pnnode)		//简化Insert，保证ptr中一定可以插入v时使用
{
	if (HasValue(ptr, v)) return -1; //如果ptr节点已包含v值，返回-1；正常插入，返回-2；
	if (ptr->num == 0)
	{
		ptr->value[0] = v;
		ptr->num++;
		return -2;
	}
	else
	{
		for (int i = 0; i < ptr->num; i++)
		{
			if (v < ptr->value[i])
			{
				for (int j = rank; j > i; j--)
				{
					ptr->value[j] = ptr->value[j - 1];
				}
				for (int j = rank+1; j > i; j--)
				{
					ptr->son[j] = ptr->son[j - 1];
				}

				ptr->value[i] = v;
				ptr->son[i+1] = pnnode;
				ptr->num++;
				return -2;
			}
		}
		ptr->value[ptr->num] = v;
		ptr->son[ptr->num+1] = pnnode;
		ptr->num++;
	}
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

int Add(BTree &tree, PNode ptr, int v)	//如果添加后ptr节点数据数量超限，将ptr节点分裂
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
		if (IsFull(ptr)) Divide(tree, ptr);
		return 0;
	}
	return -4;
}

int Add(BTree &tree, PNode ptr, int v, PNode newnode)
{
	int state = Insert0(ptr, v,newnode);

	if (state == -1 || state == -3)
		return state;
	if (state == -2)
	{
		if (IsFull(ptr)) Divide(tree, ptr);
		return -2;
	}
	return -4;
}

int Divide(BTree &tree, PNode ptr)
{
	int mid = rank / 2;
	int midvalue = ptr->value[mid];
	Node* newnode = new Node;
	PNode pnnode = newnode;
	
	pnnode->fa = ptr->fa;	//分裂的新节点的父亲和ptr节点的父亲相同

	//copy：将ptr节点右半部分复制到newnode节点
	for (int i = mid + 1; i < rank; i++)
	{
		pnnode->value[i - mid - 1] = ptr->value[i];
		pnnode->num++;
	}
	for (int i = mid+1; i < rank + 1; i++)
	{
		pnnode->son[i - mid -1] = ptr->son[i];
		if (!IsLeaf(ptr))ptr->son[i]->fa = pnnode;
	}
	
	//delete：将ptr节点已被复制的部分和mid删除
	for (int i = mid; i < rank; i++)
	{
		ptr->value[i] = NaN;
		ptr->num--;
	}
	for (int i = mid + 1; i < rank + 1; i++)
	{
		ptr->son[i] = nullptr;
	}

	if (IsRoot(ptr))		//如果分裂根节点，调整指向根节点的指针，使它指向新的根
	{
		Node* newroot = new Node;
		PNode pnroot = newroot;
		pnroot->num++;
		pnroot->value[0] = midvalue;

		pnroot->son[0] = ptr;
		ptr->fa = pnroot;

		pnroot->son[1] = pnnode;
		pnnode->fa = pnroot;
		*tree = pnroot;
	}
	else
	{
		Add(tree, ptr->fa, midvalue, pnnode);	//将ptr中间节点和其右儿子加入到ptr的父节点中相应位置
	}
	return 0;
}


Location Find(PNode root, int v)
{
	Location newloc;
	for (int i = 0; i < root->num;i++)	//如果在root中找到v，返回v的位置
		if (root->value[i] == v)
		{
			newloc.set(root, i);
			return newloc;
		}
	if (IsLeaf(root))					//root中没找到v且root是叶子节点，返回空位置
		return newloc;

	for (int i = 0; i < root->num + 1; i++)	//root中没找到v且root不是叶子节点，继续找v
	{
		newloc = Find(root->son[i], v);
		if (newloc.pnode != nullptr)
		{
			return newloc;
		}
	}

	return newloc;						//没啥用，为了去掉warning写的return
}

int Delete(BTree &tree, PNode ptr, int v)
{
	
	cout << "Trying to delete " << v << endl;
	Location l = Find(ptr, v); //cout << l.number << ' '; cout << l.pnode->value[0] << endl;
	//cout << l.pnode->value[l.number] << endl;
	if ((l.pnode == *tree) && l.pnode->num == 1)
	{
		ptr->num = 0;
		ptr->value[0] = NaN;
		ptr->son[0] = nullptr;
		ptr->son[1] = nullptr;
		return 0;
	}

	if (l.pnode == nullptr)
	{
		cout << "错误！未找到值" << v << endl;
		return -1;
	}
	
	if (IsLeaf(l.pnode))							//v出现在叶子节点中，删除v，检查该叶节点
	{
		/*for (int i = l.number; i < l.pnode->num+1; i++)
		{
			l.pnode->son[i] = l.pnode->son[i + 1];
		}*/
		for (int i = l.number; i < l.pnode->num; i++)
		{
			l.pnode->value[i] = l.pnode->value[i + 1];
		}
		//l.pnode->son[l.pnode->num] = nullptr;
		l.pnode->num--;
		Check(tree, l.pnode);
		return 1;
	}
	else											//v不出现在叶节点中，删除v，从v原位置的数据较多的儿子搬上来一个值和指针
	{
		PNode pleftson = l.pnode->son[l.number];
		PNode prightson = l.pnode->son[l.number + 1];
		if (pleftson->num > prightson->num)				
		{
			l.pnode->value[l.number] = pleftson->value[pleftson->num - 1];
			pleftson->son[pleftson->num - 1] = pleftson->son[pleftson->num];
			pleftson->son[pleftson->num] = nullptr;
			pleftson->num--;
			Check(tree, pleftson);
			return 2;
		}
		else
		{
			l.pnode->value[l.number] = prightson->value[0];
			/*for (int i = 0; i < prightson->num - 1; i++)
			{
				prightson->value[i] = prightson->value[i + 1];
			}
			for (int i = 0; i < prightson->num; i++)
			{
				prightson->son[i] = prightson->son[i + 1];
			}
			prightson->son[prightson->num] = nullptr;
			prightson->num--;*/
			Move(prightson,0);
			Check(tree, prightson);
			return 3;
		}

	}
}
void Move(PNode ptr,int v)					//调整删除数值之后的节点的位置 其实本来没写这个函数 后来发现这段代码多次出现 就提取出来了
{
	for (int i = v; i < ptr->num - 1; i++)
	{
		ptr->value[i] = ptr->value[i + 1];
	}
	for (int i = v; i <  ptr->num; i++)
	{
		ptr->son[i] = ptr->son[i + 1];
	}
	ptr->son[ptr->num] = nullptr;
	ptr->num--;
}

void Mover(PNode ptr, int v)					//调整删除数值之后的节点的位置 其实本来没写这个函数 后来发现这段代码多次出现 就提取出来了
{
	for (int i = v; i < ptr->num - 1; i++)
	{
		ptr->value[i] = ptr->value[i + 1];
	}
	for (int i = v+1; i < ptr->num; i++)
	{
		ptr->son[i] = ptr->son[i + 1];
	}
	ptr->son[ptr->num] = nullptr;
	ptr->num--;
}

int Check(BTree &tree, PNode ptr)		
{
	if ((ptr->num < rank / 2) && (!IsRoot(ptr)))	//如果ptr不符合标准且不是根节点，从父节点借数据
	{
		if (IsLeaf(ptr) && ptr->num != 0) return 1;
		Borrow(tree, ptr);
		return 1;
	}
	if (ptr->num == 0 && IsRoot(ptr))				//如果ptr没有数据且是根节点，将指向根节点的指针调整为指向ptr的儿子
	{
		*tree = ptr->son[0];
		ptr->son[0]->fa = ptr->son[0];
		return 2;
	}
	return 0;
}
int Borrow(BTree &tree, PNode ptr)
{
	PNode father = ptr->fa;
	
	int pos = father->num;	//pos:指向ptr的指针在father中的位置
	for (int i = 0; i < father->num+1; i++)
		if ((father->son[i])==(ptr))
		{
			pos = i;
			//cout << pos << endl;
			break;
			
		}
	
	int lsnum = 0, rsnum = 0;	//father->son[pos]左右兄弟节点的数据数量
	if (pos == 0)
	{
		rsnum = father->son[1]->num;
	}
	else if (pos == father->num)
	{
		lsnum = father->son[pos - 1]->num;
	}
	else { rsnum = father->son[pos + 1]->num;lsnum= father->son[pos - 1]->num;}

	if (rsnum > rank / 2)
	{
		int *t = &father->value[pos];
		Insert0(ptr, *t);
		ptr->son[ptr->num] = father->son[pos + 1]->son[0];
		*t = father->son[pos + 1]->value[0];
		/*for (int i = 0; i < father->son[pos + 1]->num - 1; i++)
		{
			father->son[pos + 1]->value[i] = father->son[pos + 1]->value[i + 1];
		}
		for (int i = 0; i <father->son[pos + 1]->num; i++)
		{
			father->son[pos + 1]->son[i] = father->son[pos + 1]->son[i + 1];
		}
		father->son[pos + 1]->son[father->son[pos + 1]->num] = nullptr;
		father->son[pos + 1]->num--;*/
		Move(father->son[pos + 1],0);//替代上面这一坨 
		return 0;
	}
	if (lsnum > rank / 2)
	{
		int *t = &father->value[pos-1];
		Insert0(ptr, *t);
		ptr->son[ptr->num] = ptr->son[ptr->num - 1];
		ptr->son[ptr->num - 1]=father->son[pos-1]->son[father->son[pos - 1]->num];
		*t = father->son[pos - 1]->value[father->son[pos - 1]->num-1];
		father->son[pos - 1]->son[father->son[pos - 1]->num] = nullptr;
		father->son[pos - 1]->num--;
		return 0;
	}
	//father->son[pos]的兄弟节点数据量都刚满足标准，合并son[pos]和一个兄弟节点
	if (pos == 0)		
	{
		Insert0(ptr, ptr->fa->value[0]);
		cout << ptr->num << endl;
		cout << ptr->value[0] << endl;

		for (int i = 0; i < father->son[1]->num; i++)
		{
			ptr->value[ptr->num + i] = father->son[1]->value[i];
		}
		for (int i = 0; i < father->son[1]->num+1; i++)
		{
			ptr->son[ptr->num + i] = father->son[1]->son[i];
			if (!IsLeaf(ptr)) father->son[1]->son[i]->fa = ptr;
		}
		ptr->num += father->son[1]->num;
		
		Move(father,0);
		father->son[0]->fa = nullptr;
		father->son[0] = ptr;
		
		Check(tree ,father);
		Check(tree, ptr);
	}
	else
	{
		PNode lson = father->son[pos - 1];
		Insert0(father->son[pos - 1], father->value[pos - 1]);
		for (int i = 0; i < ptr->num; i++)
		{
			father->son[pos - 1]->value[father->son[pos - 1]->num + i] =ptr->value[i];
		}
		for (int i = 0; i < ptr->num+1; i++)
		{
			father->son[pos - 1]->son[father->son[pos - 1]->num + i] = ptr->son[i];
			if (!IsLeaf(ptr)) ptr->son[i]->fa = father->son[pos - 1];
		}
		father->son[pos - 1]->num += ptr->num;
		Mover(father,pos-1);
		father->son[pos - 1] = lson;

		Check(tree, father);
		Check(tree, lson);
	}

	return 0;

}

void ShowTree(PNode node, int deep)				//打印树
{
	cout << "----------------" << endl;
	cout << "这是第" << deep << "层" << endl;
	cout << "该节点有" << node->num << "个数据" << endl;
	cout << "该节点存储的数据：" << endl;
	for (int i = 0; i < node->num; i++)
		cout << node->value[i] << ' ';
	cout << endl;


	if (IsRoot(node))
		cout << "这是根节点" << endl;
	else
		cout << "该节点的父节点是" << node->fa->value[0] << endl;
	cout << "该节点数据数量：" << node->num << endl;

	if (!IsLeaf(node))
	{
		for (int i = 0; i < node->num + 1; i++)
			cout << node->son[i]->value[0] << ' ';
		cout << endl;

		for (int i = 0; i < node->num + 1; i++)
			ShowTree(node->son[i], deep + 1);
	}
}

deque<PNode>tqueue;
int flag;
void INN(PNode node,int num)
{
	tqueue.push_front(node);
	if (IsRoot(node))
	{
		if (IsLeaf(node)) return;
		else
		{
			INN(node->son[0], num);
		}
	}
	else
	{
		++flag;
		if (flag < num)
			INN(node->fa->son[flag],num);
		else if (flag == num)
		{

		}
	}
}

void BFS(PNode root)
{
	if (IsEmpty(root))
	{
		cout << "空树" << endl;
		return;
	}
	string ss("      ");
	tqueue.push_front(root);
	int sum = root->num;int s = 0,n=5;
	while (!tqueue.empty())
	{
		for (int i = 0; i < n; i++)
			cout << ss;
		PNode node = tqueue.front();
		cout << '(';
		for (int i = 0; i < node->num; i++)
		{
			if (i == node->num - 1)
				cout << node->value[i];
			else
				cout << node->value[i] << ',';
			sum--;
		}		
		cout << ")";
		if (sum == 0) cout << endl;
		
		if (!IsLeaf(node))
		{
			for (int i = 0; i < node->num + 1; i++)
			{
				tqueue.push_back(node->son[i]);
				s += node->son[i]->num;
			}
			if (sum == 0) { sum = s; s = 0; n-=2; }
		}
		tqueue.pop_front();
	}
	cout << "结束" << endl;
}

int main()
{
	
	Node root;
	root.fa = &root;
	PNode ptroot = &root;
	BTree tree = &ptroot;
	int n=0;

	while (n == 0)
	{
		cout << "加入数值请输入1，删除数值请输入0，打印请输入-1。"<<endl;
		cin >> n;
		if (n == 1)
		{
			cout << "插入:" << endl;
			cin >> n;
			Add(tree, ptroot, n);
			cout << rank << "阶B树" << endl;
			//ShowTree(*tree, 0);
			//cout << "----------------" << endl;
		}
		else if (n == 0)
		{
			cout << "删除:" << endl;
			cin >> n;
			Delete(tree, ptroot, n);
			cout << rank << "阶B树" << endl;
			//ShowTree(*tree, 0);
			//cout << "----------------" << endl;
		}
		else
		{
			
			BFS(ptroot);
			ShowTree(*tree, 0);
			
		}
		n = 0;
	}
	
}
