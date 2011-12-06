#include "linear_sequence_assoc.h"

#define LSQ_IteratorInvalid NULL

// АВЛ-деревья

typedef enum {
	IT_DEREFERENCABLE,
	IT_BEFOREFIRST,
	IT_PASTREAR,
} IteratorTypeT;

typedef struct ItemT {
	struct ItemT* left;
	struct ItemT* right;
	struct ItemT* parent;
	LSQ_BaseTypeT value;
	LSQ_IntegerIndexT key;

	int height;

} TreeNodeT, *TreeNodePtrT;

typedef struct {
	LSQ_BaseTypeT size;
	TreeNodePtrT root;
} TreeT, *TreePtrT;

typedef struct {
	IteratorTypeT type;
	TreePtrT tree;
	TreeNodePtrT node;
} IteratorT, *IteratorPtrT;

static IteratorPtrT CreateIterator(LSQ_HandleT h, TreeNodePtrT node, IteratorTypeT type);

static TreeNodePtrT GetNodeByKey(TreeNodePtrT node, LSQ_IntegerIndexT key);

static TreeNodePtrT GetTreeMinimum(TreeNodePtrT node);

static TreeNodePtrT GetTreeMaximum(TreeNodePtrT node);

static TreeNodePtrT CreateNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, TreeNodePtrT parent);

static void ReplaceNode(TreePtrT tree, TreeNodePtrT node, TreeNodePtrT new_node);

static void FreeNode(TreeNodePtrT node);

static int GetNodeHeight(TreeNodePtrT node);

static int NodeBalanceFactor(TreeNodePtrT node);

static void RefreshNodeHeight(TreeNodePtrT node);

static void LeftRotate(TreeNodePtrT node, TreePtrT tree);

static void RightRotate(TreeNodePtrT node, TreePtrT tree);

static void Balance(TreePtrT tree, TreeNodePtrT node, int stop_criterion);

static int max(int a, int b){
	return a > b ? a : b;
}

static TreeNodePtrT GetTreeMinimum(TreeNodePtrT node){
	while (node->left != NULL)
		node = node->left;
	return node;
}


static TreeNodePtrT GetTreeMaximum(TreeNodePtrT node){
	while (node->right != NULL) 
		node = node->right;
	return node;
}

static TreeNodePtrT CreateNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, TreeNodePtrT parent){
	TreeNodePtrT node = (TreeNodePtrT)malloc(sizeof(TreeNodeT));
	if (node == NULL)
		return NULL;
	node->key = key;
	node->value = value;
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	return node;
}

static void FreeNode(TreeNodePtrT node){
	if (node == NULL)
		return;
	FreeNode(node->left);
	FreeNode(node->right);
	free(node);
}

static TreeNodePtrT GetNodeByKey(TreeNodePtrT node, LSQ_IntegerIndexT key){
	while (node != NULL && node->key != key) 
		if (node->key < key)
			node = node->right;
		else
			node = node->left;
	return  node;
}

static IteratorPtrT CreateIterator(LSQ_HandleT h, TreeNodePtrT node, IteratorTypeT type){
	IteratorPtrT iter = (IteratorPtrT) malloc(sizeof(IteratorT));
	if (iter == LSQ_IteratorInvalid)
		return LSQ_IteratorInvalid;
	iter->node = node;
	iter->tree = (TreePtrT) h;
	iter->type = type;
	return iter;
}

static void ReplaceNode(TreePtrT tree, TreeNodePtrT node, TreeNodePtrT new_node){
    if (new_node != NULL)
        new_node->parent = node->parent;
    if (node->parent == NULL)
        tree->root = new_node;
    else 
        if (node->parent->left == node)
            node->parent->left = new_node;
        else
            node->parent->right = new_node;
}

static int GetNodeHeight(TreeNodePtrT node) {
    return node == NULL ? 0 : node->height;
}

static int NodeBalanceFactor(TreeNodePtrT node) {
    return GetNodeHeight(node->left) - GetNodeHeight(node->right);
}

static void RefreshNodeHeight(TreeNodePtrT node) {
    node->height = 1 + max(GetNodeHeight(node->left), GetNodeHeight(node->right));
}


static void LeftRotate(TreeNodePtrT node, TreePtrT tree){
    TreeNodePtrT new_root = node->right;
    ReplaceNode(tree, node, new_root);
    node->right = new_root->left;
    if (node->right != NULL)
        node->right->parent = node;
    node->parent = new_root;
    new_root->left = node;
    RefreshNodeHeight(node);
    RefreshNodeHeight(new_root);	
}

static void RightRotate(TreeNodePtrT node, TreePtrT tree){
    TreeNodePtrT new_root = node->left;
    ReplaceNode(tree, node, new_root);
    node->left = new_root->right;
    if (node->left != NULL)
        node->left->parent = node;
    node->parent = new_root;
    new_root->right = node;
    RefreshNodeHeight(node);
    RefreshNodeHeight(new_root);	
}

static void Balance(TreePtrT tree, TreeNodePtrT node, int stop_criterion) {
    TreeNodePtrT parent;
    int node_balance;
    while (node != NULL) {
        RefreshNodeHeight(node);
        node_balance = NodeBalanceFactor(node);
        parent = node->parent;
        if (abs(node_balance) == stop_criterion)
            return;
        else 
			if (node_balance == -2) {
				if (NodeBalanceFactor(node->right) > 0)
					RightRotate(node->right, tree);
            LeftRotate(node, tree);
			}
			else 
				if (node_balance == 2) {
					if (NodeBalanceFactor(node->left) < 0)
						LeftRotate(node->left, tree);
					RightRotate(node, tree);
				}
        node = parent;
    }
}

extern LSQ_HandleT LSQ_CreateSequence(void){
	TreePtrT t = (TreePtrT) malloc(sizeof(TreeT));
	if (t == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	t->root = NULL;
	t->size = 0;
	return t;
}


extern void LSQ_DestroySequence(LSQ_HandleT handle){
	FreeNode(((TreePtrT)handle)->root);
	free(handle);
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	return handle != NULL ? ((TreePtrT)handle)->size : -1;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return	iterator != NULL && !LSQ_IsIteratorBeforeFirst(iterator) && !LSQ_IsIteratorPastRear(iterator);	
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return iterator != NULL && ((IteratorPtrT)iterator)->type == IT_PASTREAR;	
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return iterator != NULL && ((IteratorPtrT)iterator)->type == IT_BEFOREFIRST;
}

extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	if (iterator == LSQ_IteratorInvalid && ((IteratorPtrT)iterator)->node == NULL)
		return NULL;
	return &(((IteratorPtrT)iterator)->node->value);
}

extern LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator){
	if (iterator == LSQ_IteratorInvalid && ((IteratorPtrT)iterator)->node == NULL)
		return -1;
	return ((IteratorPtrT)iterator)->node->key;
}
          
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	TreePtrT tree = (TreePtrT)handle;
	TreeNodePtrT node = NULL;
	if (tree == NULL)
		return NULL;
	node = GetNodeByKey(tree->root, index);
	if (node == NULL)
		return LSQ_GetPastRearElement(handle);
	return CreateIterator(handle, node, IT_DEREFERENCABLE);
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	IteratorPtrT iter = CreateIterator(handle, NULL, IT_BEFOREFIRST);
	if (iter == LSQ_IteratorInvalid)
		return LSQ_IteratorInvalid;
	LSQ_AdvanceOneElement(iter);
	return iter;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
    return CreateIterator(handle, NULL, IT_PASTREAR);
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator){
	free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
	TreeNodePtrT node = NULL, parent = NULL;
	IteratorPtrT iter = (IteratorPtrT) iterator;
	if (iter == LSQ_IteratorInvalid || iter->type == IT_PASTREAR)
		return;
	if (iter->type == IT_BEFOREFIRST) {
		if (iter->tree->root == NULL) 
			iter->type = IT_PASTREAR;
		else {
			iter->node = GetTreeMinimum(iter->tree->root);
			iter->type = IT_DEREFERENCABLE;
        }
        return;
    }
    node = iter->node;
    while (node->parent != NULL && node->right == parent) {
        parent = node;
        node = node->parent;
    }
    if (node->right == parent){
		iter->type = IT_PASTREAR;
        iter->node = NULL;
    } 
	else 
		if (node != iter->node)
			iter->node = node;
		else 
			if (node->right != NULL) 
				iter->node = GetTreeMinimum(node->right);

}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
	IteratorPtrT iter = (IteratorPtrT) iterator;
	TreeNodePtrT parent = NULL, node = NULL;
	if (iter == LSQ_IteratorInvalid || iter->type == IT_BEFOREFIRST)
		return;
	if (iter->type == IT_PASTREAR) {
		if (iter->tree->root == NULL) 
			iter->type = IT_BEFOREFIRST;
		else {
			iter->node = GetTreeMaximum(iter->tree->root);
			iter->type = IT_DEREFERENCABLE;
        }
        return;
    }
	node = iter->node;
    while (node->parent != NULL && node->left == parent){
        parent = node;
        node = node->parent;
    }
    if (node->left == parent){
		iter->type = IT_BEFOREFIRST;
        iter->node = NULL;
    }
    else 
		if (node != iter->node)
			iter->node = node;
		else 
			if (node->left != NULL)
				iter->node = GetTreeMaximum(node->left);
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
	if (iterator == LSQ_IteratorInvalid)
		return;
	for (; shift > 0; shift--, LSQ_AdvanceOneElement(iterator));
	for (; shift < 0; shift++, LSQ_RewindOneElement(iterator));
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	IteratorPtrT iter = (IteratorPtrT)iterator;
	if (iter == NULL)
		return;
	iter->type = IT_BEFOREFIRST;
	LSQ_ShiftPosition(iterator, pos + 1);
}

extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value){
	TreePtrT tree = NULL;
	TreeNodePtrT node = NULL, parent = NULL;
	if (handle == LSQ_HandleInvalid)
		return;
	tree = (TreePtrT) handle;
	if (tree->root == NULL) { 
		tree->root = CreateNode(key, value, NULL);
		tree->size++;
		return;
	} 
	parent = tree->root;
	while (1) {
		if (key < parent->key) {
			if (parent->left == NULL) 
				break;
			parent = parent->left;
		}
		else 
			if (key > parent->key) {
				if (parent->right == NULL) 
					break;
				parent = parent->right;
			}
			else {
				parent->value = value;
				return;
			}
	}
	node = CreateNode(key, value, parent);
	if (node == NULL)
		return;
	tree->size++;
	if (key < parent->key)
		parent->left = node;
	else
		parent->right = node;
	Balance(tree, parent, 0);
}

extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key){
    TreePtrT tree = (TreePtrT)handle;
    TreeNodePtrT node = NULL, left = NULL, parent = NULL;
    int new_key;
    if (tree == NULL || tree->root == NULL)
        return;
    node = GetNodeByKey(tree->root, key);
    if (node == NULL)
        return;
    parent = node->parent;
    if (node->left == NULL && node->right == NULL)
        ReplaceNode(tree, node, NULL);
    else 
		if (node->left != NULL && node->right != NULL) {
			left = GetTreeMinimum(node->right);
			new_key = left->key;
			node->value = left->value;
			LSQ_DeleteElement(handle, left->key);
			node->key = new_key;
			return;
		}
        else 
			if (node->left != NULL)
				ReplaceNode(tree, node, node->left);
			else 
				if (node->right != NULL)
					ReplaceNode(tree, node, node->right);
    free(node);
	tree->size--;
	Balance(tree, parent, 1);
}


extern void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	LSQ_IteratorT iter = LSQ_GetFrontElement(handle);
	if (handle == LSQ_HandleInvalid || iter == LSQ_IteratorInvalid)
		return;
	LSQ_DeleteElement(handle, LSQ_GetIteratorKey(iter));
	LSQ_DestroyIterator(iter);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle){
	LSQ_IteratorT iter = LSQ_GetPastRearElement(handle);
	if (handle == LSQ_HandleInvalid || iter == LSQ_IteratorInvalid)
		return;
	LSQ_RewindOneElement(iter);
	LSQ_DeleteElement(handle, LSQ_GetIteratorKey(iter));
	LSQ_DestroyIterator(iter);
}
