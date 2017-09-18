/*List with ptr  链表-指针实现*/
/*feture C*/
/*Version 0.1*/

#include<iostream>
using namespace std;

struct Node;
typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Address;

List MakeList();
int IsEmpty(List L);
int IsLast(List L, Address p);
void Delete(List L, int v);
void Insert(List L, Address p, int v);
void DeleteList(List L);
void Print(List L);
Address FindPrevious(List L, int v);
Address Find(List L, int v);
Address Header(List L);
Address First(List L);
Address Advance(Address p);
int Retrieve(Address p);



/*Show struct Node*/
struct Node
{
	int value;
	Address next;
};

/*Check if List L is empty */
int IsEmpty(List L)
{
	return (L->next == nullptr);
}

/*Check if p is the last element in List L*/
int IsLast(List L, Address p)
{
	return (p->next == nullptr);
}

/*Initialize List L*/
List MakeList()
{
	List L = new Node;
	L->value = 0;
	L->next = nullptr;
	return L;
}

/*Return the address of v*/
Address Find(List L, int v)
{
	Address t = Header(L);
	while (t->next != nullptr)
	{
		t = t->next;
		if (t->value == v)
			return t;
	}
	return nullptr;
}

/*Return the address of v's previous element*/
Address FindPrevious(List L, int v)
{
	Address t = Header(L);
	while (!IsLast(L,t))
	{
		if (t->next->value == v)
			return t;
		t = t->next;
	}
	return nullptr;
}

/*Return the Header of L*/
Address Header(List L)
{
	return L;
}

/*Return the First element in L*/
Address First(List L)
{
	return Header(L)->next;
}

/*Insert v after Address P*/
void Insert(List L, Address p, int v)
{
	Address n=new Node;
	n->next = p->next;
	n->value = v;
	p->next = n;
}

/*Delete the element whose value is v*/
void Delete(List L, int v)
{
	Address t = FindPrevious(L, v);
	Address temp = t->next;
	t->next = temp->next;
	free(temp);
}

/*Make List L Empty but keep Address L*/
void Empty(List L)
{
	while (!IsEmpty(L))
	{
		Address temp = First(L);
		Header(L)->next = temp->next;
		free(temp);
	}
}

/*Delete List L*/
void DeleteList(List L)
{
	delete L;
}

void Print(List L)
{
	Address t = Header(L);
	cout << "Head ";
	while (!IsLast(L,t))
	{
		t = t->next;
		cout << t->value << " ";
	}
	cout <<"Tail"<< endl;
}

int main()
{
	List L=MakeList();
	Insert(L, Header(L), 1);
	Insert(L, Find(L, 1), 2);
	Print(L);
	Empty(L);
	Print(L);
	int i;
	cin >> i;
	
}
