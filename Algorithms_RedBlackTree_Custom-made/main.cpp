#include <iostream>

struct node;
struct Tree;
void insert(Tree&, int);
void insert(Tree&, int, node*);
node* createNode(int key = 0, node* p = nullptr);
void PropFix(Tree&, node*);
void rotateLeft(Tree&, node*);
void rotateRight(Tree&, node*);

struct node {
	int key = NULL;
	node* left = nullptr;
	node* right = nullptr;
	node* p = nullptr;
	char color = 'r';
}NIL = *createNode(777);



node* createNode(int key, node* p)
{
	node* n = new node;
	if (p == nullptr) p = &NIL;
	n->key = key;
	n->left = &NIL;
	n->right = &NIL;
	n->p = p;
	n->color = 'n';
	return n;
}




struct Tree {
	node* root = &NIL;
	int h;
	int n;
};


void insert(
	Tree& T,
	int key
)
{
	if (T.root==&NIL)
	{
		T.root = createNode(key);
		T.root->color = 'b';
		return;
	}
	node* x = T.root;
	insert(T, key, x);
}


void insert(
	Tree& T,
	int key,
	node* x
)
{
	if (x->key > key && x->left==&NIL)
	{
		//insert to the left of x
		x->left = createNode(key, x);
		x->left->color = 'r';
		PropFix(T, x->left);
		++T.n;
	}
	else if (x->key <= key && x->right==&NIL)
	{
		//insert to the right of x
		x->right = createNode(key, x);
		x->right->color = 'r';
		PropFix(T, x->right);
		++T.n;
	}
	else
	{
		if (x->key > key) x = x->left;
		else x = x->right;
		insert(T, key, x);
	}

}


void PropFix(Tree& T, node* z)
{
	while (z->color == 'r' && z->p->color == 'r') //parrent is also red
	{
		if (z->p == z->p->p->right)
		{
			if (z->p->p->left->color != 'r')//can rotate to fix properties
			{
				rotateLeft(T, z);
				z->p->color = 'b';
				z->left->color = 'r';
			}
			else
			{
				z->p->color = 'b';
				z->p->p->left->color = 'b';
				z->p->p->color = 'r';
				z = z->p->p;
			}
		}
		else if (z->p == z->p->p->left)
		{
			if (z->p->p->right->color != 'r' )//can rotate to fix properties
			{
				rotateRight(T, z);
				z->p->color = 'b';
				z->right->color = 'r';
			}
			else
			{
				z->p->color = 'b';
				z->p->p->right->color = 'b';
				z->p->p->color = 'r';
				z = z->p->p;
			}
		}
	}
	T.root->color = 'b';

}


void rotateRight(Tree& T,node* z)
{
	node* p = z->p;
	if (z == p->right)
	{

		z->p = p->p;
		p->p = z;
		p->right = z->left;
		z->left = p;
		z->p->left=z;
		
	}
	z = p;
	p = z->p->p;

	z->p->p = z->p->p->p;	//parent changing
			//
	p->left = z->p->right;//doughter to previous parent
	z->p->right = p;	//prev parent make doughter
	p->p = z->p;
	if (z->p->p != &NIL)//grand parrent chaning, left or right new doughter?
	{					//|p is a previous parent of z->p|
		if (z->p->p->left == p) z->p->p->left = z->p;
		else if (z->p->p->right == p) z->p->p->right = z->p;
	}
	else {
		//if it is, then z->p is a root of the tree
		T.root = z->p;
	}
}
void rotateLeft(Tree& T, node* z)
{
	node* p = z->p;
	if (z == p->left)
	{

		z->p = p->p;
		p->p = z;
		p->left = z->right;
		z->right = p;
		z->p->right = z;
	}
	z = p;
	p = z->p->p;

	z->p->p = z->p->p->p;	//parent changing
			//
	p->right = z->p->left;//doughter to previous parent
	z->p->left = p;	//prev parent make doughter
	p->p = z->p;
	if (z->p->p != &NIL)//grand parrent chaning, left or right new doughter?
	{					//|p is a previous parent of z->p|
		if (z->p->p->right == p) z->p->p->right = z->p;
		else if (z->p->p->left == p) z->p->p->left = z->p;
	}
	else {
		//if it is, then z->p is a root of the tree
		T.root = z->p;
	}
}



int main()
{
	Tree TheTree;

	


	std::cout << '\n';
	return 0;
}

	
	