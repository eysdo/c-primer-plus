//tree.c -- 树类型的实现接口

#include <stdio.h>
#include <stdlib.h>							//malloc(), free()
#include <stdbool.h>						//bool类型向C++兼容
#include <string.h>							//strcmp()
#include <Windows.h>						//Sleep()
#include "tree.h"

//局部数据类型
typedef struct pair
{
	Node * parent;
	Node * child;
} Pair;


//局部函数原型
static Node * MakeNode(const Item * pi);
static bool ToLeft(const Item * pi1, const Item * pi2);
static bool ToRight(const Item * pi1, const Item * pi2);
static void AddNode(Node * pnew_node, Node * root);
static void InOrder(const Node * root, void(*pfun)(Item item));
static Pair SeekItem(const Item * pi, const Tree * ptree);
static void DeleteNode(Node ** ptr);
static void DeleteAllNodes(Node * root);


//函数定义
void InitializeTree(Tree * ptree)
{
	ptree->root = NULL;
	ptree->size = 0;
}

bool TreeIsEmpty(const Tree * ptree)
{
	if (ptree->root == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TreeIsFull(const Tree * ptree)
{
	if (ptree->size == MAXITEMS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int TreeItemCount(const Tree * ptree)
{
	return ptree->size;
}

bool AddItem(const Item * pi, Tree * ptree)
{
	Node  * pnew_node;
	Pair look;
	int insert = -1;

	//检查树是否已满
	if (TreeIsFull(ptree))
	{
		fprintf(stderr, "Tree is full.\n");
		return false;
	}

	//检查欲添加项是否已存在
	look = SeekItem(pi, ptree);
	if (look.child != NULL)
	{
		for (int i = 0; i < MAXKINDS; i++)
		{
			if (strcmp(look.child->item.petkind[i], "") == 0)
			{
				insert = i;
				break;
			}
		}
		if (insert == -1)
		{
			printf("The list of pet kind named %s is full.\n", pi->petname);
		}
		else
		{
			strcpy_s(look.child->item.petkind[insert], 20, pi->petkind[0]);
			ptree->size++;
		}
		//fprintf(stderr, "Attempted to add duplicate item.\n");
		return true;
	}

	//尝试创建新节点
	pnew_node = MakeNode(pi);
	if (pnew_node == NULL)
	{
		fprintf(stderr, "Couldn't create node.\n");
		return false;
	}
	//创建成功，更新树结构
	ptree->size++;
	if (ptree->root == NULL)
	{
		ptree->root = pnew_node;
	}
	else
	{
		AddNode(pnew_node, ptree->root);
	}
	return true;
}

bool InTree(Item * pi, const Tree * ptree)
{
	Pair look = SeekItem(pi, ptree);
	
	if (look.child != NULL)
	{
		*pi = look.child->item;				//如果找到目标，将目标的名字和种类列表包括在内写入到*pi
		return true;
	}

	return false;
}


bool DeleteItem(const Item * pi, Tree * ptree)
{
	Pair look;

	look = SeekItem(pi, ptree);
	if (look.child == NULL)
	{
		return false;											//不存在，返回
	}

	//检查种类列表
	for (int i = 0; i < MAXKINDS; i++)
	{
		if (strcmp(look.child->item.petkind[i], pi->petkind[0]) == 0)		//待删宠物的种类是宠物列表中的第一个
		{
			strcpy_s(look.child->item.petkind[i], 20, "");		//清除该宠物种类					
			for (int j = i; j < MAXKINDS - 1; j++)
			{													//后续的宠物种类依次前移
				strcpy_s(look.child->item.petkind[j], 20, 
					look.child->item.petkind[j + 1]);
			}
			break;
		}
	}
	if (look.child->item.petkind[0] != "")
	{
		ptree->size--;
		return true;											//删除目标宠物后，如果该名字的宠物种类不为空，则返回
	}

	if (look.parent == NULL)
	{
		DeleteNode(&ptree->root);
	}
	else if (look.parent->left == look.child)
	{
		DeleteNode(&look.parent->left);
	}
	else
	{
		DeleteNode(&look.parent->right);
	}
	ptree->size--;
	return true;
}

void Traverse(const Tree * ptree, void(*pfun)(Item item))
{
	if (ptree->root != NULL)
	{
		InOrder(ptree->root, pfun);
	}
}

void EmptyAll(Tree * ptree)
{
	if (ptree->root != NULL)
	{
		DeleteAllNodes(ptree->root);
	}
	ptree->root = NULL;
	ptree->size = 0;
}


//局部函数定义

Node * MakeNode(const Item * pi)
{
	Node * pnew_node;

	pnew_node = (Node *)malloc(sizeof(Node));		//分配节点内存
	if (pnew_node != NULL)							//分配到时进行节点的初始化
	{
		pnew_node->item = *pi;
		for (int i = 1; i < MAXKINDS; i++)			//种类列表中，第二个开始依次设为空字符串
			strcpy_s(pnew_node->item.petkind[i], 20, "");
		pnew_node->left = NULL;
		pnew_node->right = NULL;
	}
	return pnew_node;
}

bool ToLeft(const Item * pi1, const Item * pi2)
{
	int comp1;

	if ((comp1 = strcmp(pi1->petname, pi2->petname)) < 0)
	{
		return true;
	}
	/*else if (comp1 == 0 && strcmp(pi1->petkind, pi2->petkind) < 0)
	{
		return true;
	}*/
	else
	{
		return false;
	}
}

bool ToRight(const Item * pi1, const Item * pi2)
{
	int comp1;

	if ((comp1 = strcmp(pi1->petname, pi2->petname)) > 0)
	{
		return true;
	}
	/*else if (comp1 = 0 && strcmp(pi1->petkind, pi2->petkind) > 0)
	{
		return true;
	}*/
	else
	{
		return false;
	}
}

void AddNode(Node * pnew_node, Node * root)					//递归
{
	int dest = -1;

	if (ToLeft(&pnew_node->item, &root->item))
	{
		if (root->left == NULL)
		{
			root->left = pnew_node;
		}
		else
		{
			AddNode(pnew_node, root->left);
		}
	}
	else if (ToRight(&pnew_node->item,&root->item))
	{
		if (root->right == NULL)
		{
			root->right = pnew_node;
		}
		else
		{
			AddNode(pnew_node, root->right);
		}
	}
	else
	{	
		fprintf(stderr, "location error in AddNode().\n");
		Sleep((unsigned long)3000);
		exit(EXIT_FAILURE);
	}
}

void InOrder(const Node * root, void(*pfun)(Item item))		//递归
{
	//递归调用访问非空的子树，遍历顺序：中序遍历
	if (root != NULL)
	{
		InOrder(root->left, pfun);
		(*pfun)(root->item);
		InOrder(root->right, pfun);
	}
}

Pair SeekItem(const Item * pi, const Tree * ptree)			//递归
{
	Pair look;

	look.parent = NULL;
	look.child = ptree->root;

	if (look.child == NULL)
	{
		return look;										//空树返回
	}
	while (look.child != NULL)
	{
		if (ToLeft(pi, &look.child->item))
		{
			look.parent = look.child;
			look.child = look.child->left;
		}
		else if (ToRight(pi, &look.child->item))
		{
			look.parent = look.child;
			look.child = look.child->right;
		}
		else
		{
			break;				//如果前两种情况都不满足，必定为相等的情况，中断循环，look.child等于目标节点的地址
		}
	}
	return look;
}

void DeleteNode(Node ** ptr)
{
	Node * temp;

	puts((*ptr)->item.petname);
	if ((*ptr)->left == NULL)
	{
		*ptr = (*ptr)->right;	//只有右支时，让待删节点的父节点中指向该节点的指针，修改为指向待删节点的右子树
	}
	else if ((*ptr)->right == NULL)
	{
		*ptr = (*ptr)->left;	//同上
	}
	else						//待删节点有2个子节点
	{
								//在左子树的右支查找空的右子树
		for (temp = (*ptr)->left; temp->right == NULL; temp = temp->right)
		{
			continue;
		}
		temp->right = (*ptr)->right;
		temp = *ptr;
		*ptr = temp->left;
		free(temp);
	}
}

void DeleteAllNodes(Node * root)							//递归
{
	Node *pright;

	if (root != NULL)
	{
		//中序遍历删除非空的子树
		pright = root->right;
		DeleteAllNodes(root->left);
		free(root);
		DeleteAllNodes(pright);
	}
}