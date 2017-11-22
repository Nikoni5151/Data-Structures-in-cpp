//Huffman Coding Tree feat.C
#include<iostream>
#include<fstream>
#include<map>
#include<string>
using namespace std;

struct TreeNode //树的节点
{
	char chr;	//字符 
	int value;	//对于叶子节点，value是chr出现次数;对于非叶节点，value是该子树权值
	TreeNode *left;		
	TreeNode *right;
};

struct TreeQueue	//用于存树的优先队列
{
	int Front;
	int Rear;
	int Size;
	TreeNode* queue[300];
	TreeQueue()
	{
		Front = 0;
		Rear = 0;
		Size = 0;
	}
};


int Enqueue(TreeQueue &queue, TreeNode *tree)	//入队
{
	if (queue.Size == 0)
	{
		queue.queue[queue.Front] = tree;
		queue.Rear += 1;
		queue.Size += 1;
		return 1;
	}
	else
	{
		for (int i = queue.Front; i < queue.Rear; i++)
		{
			if (queue.queue[i]->value <= tree->value)
			{
				for (int j = queue.Rear; j > i; j--)
				{
					queue.queue[j] = queue.queue[j - 1];
				}
				queue.queue[i] = tree;
				queue.Rear++;
				queue.Size++;
				return 2;
			}
			else
			{
				queue.queue[queue.Rear++] = tree;
				queue.Size++;
				return 3;
			}
		}
	}
}
TreeNode* Dequeue(TreeQueue &queue)		//出队
{
	if (queue.Size == 0)
		return nullptr;

	TreeNode* temp = queue.queue[queue.Rear - 1];
	queue.Rear--;
	queue.Size--;
	return temp;
}

TreeNode* MakeTree(char chr, int v)	//把键值对做成树
{
	TreeNode *tree = new TreeNode;
	tree->chr = chr;
	tree->value = v;
	tree->left = tree->right = nullptr;
	return tree;
}

TreeNode* MakeTree(TreeNode* t1,TreeNode* t2)	//把树t1和t2合并为tree并返回tree
{
	TreeNode *tree = new TreeNode;
	tree->value = t1->value + t2->value;
	tree->left = t1;
	tree->right = t2;
	return tree;
}

map<char, int>letter_count;

void Input1()		//读入inputfile1.txt并加入字典
{
	ifstream in("inputfile1.txt");
	char a;
	while (in.get(a))
	{
		++letter_count[a];		
	}
	in.close();
}

void Output1()	//打开outputfile1.txt，其实这个函数没啥用，懒得删了，万一改改接着用呢
{
	ofstream out("outputfile1.txt");
}

ofstream out("outputfile1.txt");
void traversal(TreeNode* tree, string s)	//遍历，输出
{
	if (tree->left!=nullptr&&tree->right != nullptr)
	{
		traversal(tree->left, s + "0");
		traversal(tree->right, s + "1");
	}
	else
	{
		if (tree->chr == ' ') out << "space";
		else
		if (tree->chr == '\n') out << "newline";
		else out << tree->chr;

		out <<  ' ' << tree->value << ' ' << s << endl;
	}
}

void Merge(TreeQueue &queue)		//合并所有树，放在队列头
{
	int i = 0;
	for (const auto &w : letter_count)
	{
		Enqueue(queue,MakeTree(w.first, w.second));
		i++;
	}
	while (queue.Size > 1)
	{
		TreeNode* t1 = Dequeue(queue);
		TreeNode* t2 = Dequeue(queue);
		Enqueue(queue, MakeTree(t1, t2));
	}
}

void Coding(TreeQueue &queue)	//打开input2和output2，将input2的二进制文件转换为文本写入到output2中，并关闭两个文件
{
	ifstream in("inputfile2.txt");
	ofstream out("outputfile2.txt");
	char a;
	TreeNode* tree = queue.queue[0];
	TreeNode* temp = tree;
	while (in.get(a))
	{
		string code = "";
		if (a=='0')
		{
			temp = temp->left;
		}
		else if (a == '1')
		{
			temp = temp->right;
		}
		if (temp->left == nullptr||temp->right == nullptr)
		{
			out << temp->chr;
			temp = tree;
		}
	}
	in.close();
	out.close();
}
int main()
{
	TreeQueue queue;
	Input1();
	Merge(queue);
	traversal(queue.queue[0],"");
	out.close();
	Coding(queue);
	return 0;
}
