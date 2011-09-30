#include "linear_sequence.h"

typedef struct ListItemT {
	LSQ_BaseTypeT data;
	struct ListItemT* next;
	struct ListItemT* prev;
} ListElementT, *ListElementPtrT;

typedef struct {
	int size;
	ListElementPtrT before_first;
	ListElementPtrT past_rear;
} ListT, *ListPtrT;

typedef struct {
	ListPtrT handle;
	ListElementPtrT element;
} ListIteratorT, *ListIteratorPtrT;

static LSQ_IteratorT CreateIterator(LSQ_HandleT handle,  ListElementPtrT element){
	ListIteratorPtrT iter = NULL;
	if (handle == LSQ_HandleInvalid || element == NULL)
		return NULL;
	iter = (ListIteratorPtrT)malloc(sizeof(ListIteratorT));
	if (iter == NULL)
		return NULL;
	iter->element = element;
	iter->handle = (ListPtrT)handle;
	return iter;
}

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
extern LSQ_HandleT LSQ_CreateSequence(void){
	ListPtrT handle = (ListPtrT)malloc(sizeof(ListT));
	if (handle == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	handle->before_first = (ListElementPtrT)malloc(sizeof(ListElementT));
	if (handle->before_first == NULL){
		free(handle);
		return LSQ_HandleInvalid;
	}
	handle->past_rear = (ListElementPtrT)malloc(sizeof(ListElementT));
	if (handle->past_rear == NULL){
		free(handle->before_first);
		free(handle);
		return LSQ_HandleInvalid;		
	}
	handle->size = 0;
	handle->before_first->next = handle->past_rear;
	handle->before_first->prev = NULL;
	handle->past_rear->next = NULL;
	handle->past_rear->prev = handle->before_first;
	return handle;
}

/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
extern void LSQ_DestroySequence(LSQ_HandleT handle){
	ListIteratorPtrT iter = (ListIteratorPtrT)LSQ_GetFrontElement(handle);
	if (iter == NULL)
		return;
	while (!LSQ_IsIteratorPastRear(iter)){
		free(iter->element->prev);
		LSQ_AdvanceOneElement(iter);
	}
	free(iter->element->prev);
	free(iter->element);
	free(iter->handle);
	LSQ_DestroyIterator(iter);
}

/* Функция, возвращающая текущее количество элементов в контейнере */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	return (handle != LSQ_HandleInvalid) ? ((ListPtrT)handle)->size : -1;
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return	(iterator != NULL) ? !LSQ_IsIteratorBeforeFirst(iterator) && !LSQ_IsIteratorPastRear(iterator) : 0;	
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return (iterator != NULL) ? ((ListIteratorPtrT)iterator)->handle->past_rear == ((ListIteratorPtrT)iterator)->element : 0; 
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return (iterator != NULL) ? ((ListIteratorPtrT)iterator)->handle->before_first == ((ListIteratorPtrT)iterator)->element : 0; 
}

/* Функция разыменовывающая итератор. Возвращает указатель на элемент, на который ссылается данный итератор */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	ListIteratorPtrT iter = (ListIteratorPtrT)iterator;
	if (iter == NULL || iter->handle == LSQ_HandleInvalid || !LSQ_IsIteratorDereferencable(iter))
		return NULL;
	return &(iter->element->data);
}

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */

/* Функция, возвращающая итератор, ссылающийся на элемент с указанным индексом */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	ListIteratorPtrT iter = (ListIteratorPtrT)LSQ_GetFrontElement(handle);
	if(iter == NULL) 
		return NULL;
	LSQ_ShiftPosition(iter, index);
	return iter;
}

/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	ListIteratorPtrT iter = NULL;
	if(handle == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	iter = (ListIteratorPtrT)CreateIterator((ListPtrT)handle, ((ListPtrT)handle)->before_first);
	LSQ_AdvanceOneElement(iter);
	return iter;
}
/* Функция, возвращающая итератор, ссылающийся на последний элемент контейнера */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	return (handle != LSQ_HandleInvalid) ? CreateIterator(handle, ((ListPtrT)handle)->past_rear) : LSQ_HandleInvalid;
}

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator){
	free(iterator);
}

/* Функция, перемещающая итератор на один элемент вперед */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	LSQ_ShiftPosition(iterator, 1);	
}

/* Функция, перемещающая итератор на один элемент назад */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator){
	LSQ_ShiftPosition(iterator, -1);
}

/* Функция, перемещающая итератор на заданное смещение со знаком */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
	ListIteratorPtrT iter = (ListIteratorPtrT)iterator;
	int i;
	if (iter == NULL)
		return;
	if (shift > 0)
		for (i = 0; i < shift; i++){
			if (iter->element == NULL || LSQ_IsIteratorPastRear(iter))
				break;
			iter->element = iter->element->next;
		}
	else
		for (i = 0; i < abs(shift); i++){
			if (iter->element == NULL || LSQ_IsIteratorBeforeFirst(iter))
				break;
			iter->element = iter->element->prev;
		}
}

/* Функция, устанавливающая итератор на элемент с указанным номером */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	ListIteratorPtrT iter = (ListIteratorPtrT)iterator;
	if(iter == NULL) 
		return;
	iter = (ListIteratorPtrT)LSQ_GetFrontElement(iter->handle);	
	LSQ_ShiftPosition(iter, pos);
	((ListIteratorPtrT)iterator)->element = iter->element;
	LSQ_DestroyIterator(iter);
}

/* Функция, добавляющая элемент в начало контейнера */
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	LSQ_IteratorT iter = LSQ_GetFrontElement(handle);
	if(iter == NULL) 
		return;
	LSQ_InsertElementBeforeGiven(iter, element);
	LSQ_DestroyIterator(iter);
}

/* Функция, добавляющая элемент в конец контейнера */
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	LSQ_IteratorT iter = LSQ_GetPastRearElement(handle);
	if(iter == NULL) 
		return;
	LSQ_InsertElementBeforeGiven(iter, element);
	LSQ_DestroyIterator(iter);
}
/* Функция, добавляющая элемент в контейнер на позицию, указываемую в данный момент итератором. Элемент, на который  *
 * указывает итератор, а также все последующие, сдвигается на одну позицию в конец.                                  */
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){
	ListElementPtrT e = NULL;
	ListIteratorPtrT iter = NULL;
	if(iterator == NULL || LSQ_IsIteratorBeforeFirst(iterator)) 
		return;
	iter = (ListIteratorPtrT)iterator;
	e = (ListElementPtrT)malloc(sizeof(ListElementT));
	if (e == NULL)
		return;
	e->next = iter->element;
	e->prev = iter->element->prev;
	e->data = newElement;
	iter->element->prev->next = e;
	iter->element->prev = e;
	iter->element = e;
	iter->handle->size++;
}

/* Функция, удаляющая первый элемент контейнера */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	LSQ_IteratorT iter = LSQ_GetFrontElement(handle);
	if(iter == NULL) 
		return;
	LSQ_DeleteGivenElement(iter);
	LSQ_DestroyIterator(iter);
}

/* Функция, удаляющая последний элемент контейнера */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle){
	LSQ_IteratorT iter = LSQ_GetPastRearElement(handle);
	if(iter == NULL) 
		return;
	LSQ_RewindOneElement(iter);
	LSQ_DeleteGivenElement(iter);
	LSQ_DestroyIterator(iter);
}
/* Функция, удаляющая элемент контейнера, указываемый заданным итератором. Все последующие элементы смещаются на     *
 * одну позицию в сторону начала.                                                                                    */
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
	ListElementPtrT l = NULL, r = NULL;
	ListIteratorPtrT iter = (ListIteratorPtrT)iterator;
	if(iter == NULL || !LSQ_IsIteratorDereferencable(iter)) return;
	l = iter->element->prev;
	r = iter->element->next;
	l->next = r;
	r->prev = l;
	free(iter->element);
	iter->element = r;
	iter->handle->size--;
}