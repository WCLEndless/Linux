#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include"test.h"




/*NODE* init_link()
{
	NODE*phead = (NODE*)malloc(sizeof(NODE));
	assert(phead != NULL);
	phead->next = NULL;
	phead->data = NULL;
	return phead;
}
static NODE* alloc_node(void *e, int length)
{
	NODE*tmp = (NODE*)malloc(sizeof(NODE));
	tmp->data = (void*)malloc(length);
	tmp->next = NULL;
	memcpy(tmp->data, e, length);
	return tmp;
}
bool insert_head_link(NODE*phead, void *e, int length)
{
	if (phead == NULL)
	{
		return false;
	}
	NODE*tmp = alloc_node(e, length);
	tmp->next=phead->next;
	phead->next = tmp;
	return true;
}
bool del_head_link(NODE*phead)
{
	if (phead == NULL||phead->next==NULL)
	{
		return false;
	}
	NODE*tmp = phead->next;
	phead->next = tmp->next;
	free(tmp->data);
	free(tmp);
	return true;
}
bool show(NODE*phead,void(*pfunc)(void *e))
{
	if (phead == NULL)
	{
		return false;
	}
	NODE*tmp = phead->next;
	while (tmp)
	{
		pfunc(tmp->data);
		tmp = tmp->next;
	}
	printf("\n");
	return true;

}
bool destory_link(NODE*phead)
{
	if (phead == NULL)
	{
		return false;
	}
	while (phead->next != NULL)
	{
		del_head_link(phead);
	}
	free(phead);
	return true;
}*/





NODE_EX *alloc_node()
{
	NODE_EX *phead = (NODE_EX*)malloc(sizeof(NODE_EX));
	assert(phead!=NULL);
	phead->next = NULL;
	return phead;
}
bool link_insert_head_ex(NODE_EX*phead, void(*pfunc)(NODE_EX*, void *), void *e)
{
	if (phead == NULL )
	{
		return false;
	}

	/*NODE_EX *tmp = phead->next;
	phead->next = new_node;
	new_node->next = tmp;*/
	pfunc(phead,e);
	return true;
}

bool link_insert_head(NODE_EX*phead, NODE_EX *new_node)
{
	if (phead == NULL||new_node==NULL)
	{
		return false;
	}

	NODE_EX *tmp = phead->next;
	phead->next = new_node;
	new_node->next = tmp;
	return true;
}

bool link_insert_tail_ex(NODE_EX*phead,void(*pfunc)(NODE_EX*,void *),void *e)
{
	if (phead == NULL )
	{
		return false;
	}

	NODE_EX*p = phead;
	while (p->next != NULL)
	{
		p = p->next;
	}
	pfunc(p,e);
	//p->next = new_node;
	//new_node->next = NULL;
	return true;
}

bool link_insert_tail(NODE_EX*phead, NODE_EX *new_node)
{
	if (phead == NULL||new_node==NULL)
	{
		return false;
	}

	NODE_EX*p = phead;
	while (p->next != NULL)
	{
		p = p->next;
	}
	p->next = new_node;
	new_node->next = NULL;
	return true;
}

bool link_del(NODE_EX*phead)
{
	if (phead == NULL||phead->next==NULL)
	{
		return false;
	}
	NODE_EX*p = phead->next;

	phead->next = p->next;
	//free(p);
	return true;
}

bool link_del_tail(NODE_EX*phead,void(*pfunc)(NODE_EX*))
{
	if (phead == NULL||phead->next==NULL)
	{
		return false;
	}
	NODE_EX*p = phead;
	NODE_EX*tmp=p->next;
	while (tmp->next != NULL)
	{
		p = p->next;
		tmp = p->next;
	}
	//free(tmp);
	pfunc(tmp);
	p->next = NULL;
	return true;
}
bool link_del_head(NODE_EX*phead,void (*pfunc)(NODE_EX*))
{
	if (phead == NULL || phead->next == NULL)
	{
		return false;
	}
	NODE_EX*p = phead->next;

	phead->next = p->next;
	pfunc(p);
	return true;
}


bool show_ex(NODE_EX*phead,void(*pfunc)(NODE_EX*))
{
	if (phead == NULL)
	{
		return false;
	}
	NODE_EX*p = phead->next;
	while (p != NULL)
	{
		pfunc(p);
		p = p->next;
	}
	printf("\n");
	return true;
}

bool is_empty(NODE_EX*phead)
{
	return phead->next == NULL;
}


bool destory_link_ex(NODE_EX*phead, void(*pfunc)(NODE_EX*))
{
	if (phead == NULL)
	{
		return false;
	}
	NODE_EX*p = phead->next;
	while (p != NULL)
	{
		p = p->next;
		link_del_head(phead,pfunc);
	}
	free(phead);
	return true;
}
