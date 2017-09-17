/*List with ptr  链表-指针实现*/
/*妈的 崩了 不好用 明天重写*/
#include<iostream>
using namespace std;
class Node
{
public:
	Node *next;
	int value;
};

class List
{
public:	
	int Empty();
	int IsEmpty();
	int IsLast(Node n);
	int Insert(int v,Node &n);
	int Delete(Node &n);
	Node Find(int v);
	Node FindPre(Node n);
	Node header;
};
int List::Empty() //会导致内存泄漏，暂时不会改orz
{
	this->header.next = nullptr;
	return 1;
}
int List::IsEmpty()
{
	if (this->header.next == nullptr)
		return 1;
	return 0;
}
int List::IsLast(Node n)
{
	if (n.next == nullptr)
		return 1;
	return 0;
}
Node List::FindPre(Node n)
{
	Node m = this->header;
	while (m.next != nullptr)
	{
		if (m.next == &n)
			return m;
		else (m=*m.next);
	}
	return header ;//没有找到节点N，返回header
}
int List::Delete(Node &n) //会导致内存泄漏，暂时不会改orz
{
	Node temp=FindPre(n);
	temp.next = n.next;
	n.next = nullptr;
	return 1;
}
Node List::Find(int v)
{
	Node t = header;
	while (t.next != nullptr)
	{
		t = *t.next;
		if (t.value == v)
			return t;
	}
	return header;//没有找到节点N，返回header
}
int List::Insert(int v, Node &n)
{
	if (&n == &header)
	{
		Node t;
		t.value = v;
		t.next = nullptr;
		header.next = &t;
	}
	else
	{
		Node t;
		t.value = v;
		t.next = n.next;
		n.next = &t;
	}
	return 1;
}

int main()//test
{
	List L;
	L.Insert(1, L.header);
	cout << &L.Find(1) << endl;
}
