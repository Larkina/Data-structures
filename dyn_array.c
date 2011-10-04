#include "linear_sequence.h"
#include <string.h>

#define CONTAINER_INITIAL_SIZE 1;
#define PROPORTIONALITY_FACTOR 2;
#define LIMIT_OF_CAPACITY 0.25;

typedef struct {
	LSQ_BaseTypeT* data;
	int physical_size;
	int logical_size;
}	ArrayT, *ArrayPtrT;


typedef struct {
	ArrayPtrT handle;
	LSQ_IntegerIndexT index;
}	IteratorT, *IteratorPtrT;

static LSQ_IteratorT CreateIterator(LSQ_HandleT h, LSQ_IntegerIndexT index){
	IteratorPtrT iter = NULL;
	if (h == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	iter = (IteratorPtrT) malloc(sizeof(IteratorT));
	if (iter == NULL)
		return NULL;
	iter->handle = (ArrayPtrT)h;
	iter->index = index;
	return iter;
}

static void InsertElementAtIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index, LSQ_BaseTypeT element){
	ArrayPtrT h = (ArrayPtrT)handle;
	LSQ_BaseTypeT* PlaceOfElement = NULL;
	if(h == NULL) 
		return;
	if(h->physical_size == h->logical_size){
		h->logical_size *= PROPORTIONALITY_FACTOR;
		h->data = (LSQ_BaseTypeT*)realloc(h->data, sizeof(LSQ_BaseTypeT) * h->logical_size);
	}	
	PlaceOfElement = h->data + index;
	memmove(PlaceOfElement + 1, PlaceOfElement, sizeof(LSQ_BaseTypeT) * (h->physical_size - index));	
	*PlaceOfElement = element;
	h->physical_size++;
}

static void DeleteElementAtIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	ArrayPtrT h = (ArrayPtrT)handle;
	int For_Condition_Constant;
	LSQ_BaseTypeT* PlaceOfElement = NULL;
	if(h == NULL) 
		return;
	For_Condition_Constant = h->logical_size * LIMIT_OF_CAPACITY;
	if(h->physical_size <  For_Condition_Constant){
		h->logical_size /= PROPORTIONALITY_FACTOR;
		if (h->logical_size == 0)
			h->logical_size = CONTAINER_INITIAL_SIZE;
		h->data = (LSQ_BaseTypeT*)realloc(h->data, sizeof(LSQ_BaseTypeT) * h->logical_size);
	}
	if (h->physical_size > 0){
		PlaceOfElement = h->data + index;
		memmove(PlaceOfElement , PlaceOfElement + 1, sizeof(LSQ_BaseTypeT) * (h->physical_size - index));	
		h->physical_size--;
	}
}

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
extern LSQ_HandleT LSQ_CreateSequence(void){
	ArrayPtrT h = (ArrayPtrT)malloc(sizeof(ArrayT));
	if (h == LSQ_HandleInvalid)
		return LSQ_HandleInvalid;
	h->data = (LSQ_BaseTypeT*)malloc(sizeof(LSQ_BaseTypeT));
	h->physical_size = 0;
	h->logical_size = CONTAINER_INITIAL_SIZE; 
	return h;
}

/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
extern void LSQ_DestroySequence(LSQ_HandleT handle){
	if (handle != LSQ_HandleInvalid){
		free(((ArrayPtrT)handle)->data);
		free(handle);
	}
}

/* Функция, возвращающая текущее количество элементов в контейнере */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	return (handle != LSQ_HandleInvalid) ? ((ArrayPtrT)handle)->physical_size : -1;
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return	(iterator != NULL) ? !LSQ_IsIteratorBeforeFirst(iterator) && !LSQ_IsIteratorPastRear(iterator) : 0;
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return (iterator != NULL) ? ((IteratorPtrT)iterator)->index >= ((ArrayPtrT)((IteratorPtrT)iterator)->handle)->physical_size : 0;
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return (iterator != NULL) ? ((IteratorPtrT)iterator)->index < 0 : 0;
}

/* Функция разыменовывающая итератор. Возвращает указатель на элемент, на который ссылается данный итератор */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	IteratorPtrT iter = NULL;
	if (iterator == NULL)
		return NULL;
	iter = (IteratorPtrT)iterator;
	return ((ArrayPtrT)(iter->handle))->data + iter->index;
}

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным индексом */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	return CreateIterator(handle, index);
}

/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	return CreateIterator(handle, 0);
}

/* Функция, возвращающая итератор, ссылающийся на элемент следующий за последним контейнера */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	return (handle != LSQ_HandleInvalid) ? CreateIterator(handle, ((ArrayPtrT)handle)->physical_size ) : 0; 
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
	if (iterator == NULL)
		return;
	((IteratorPtrT)iterator)->index += shift;
}

/* Функция, устанавливающая итератор на элемент с указанным номером */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	if (iterator == NULL)
		return;
	((IteratorPtrT)iterator)->index = pos;
}

/* Функция, добавляющая элемент в начало контейнера */
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	InsertElementAtIndex(handle, 0, element);
}

/* Функция, добавляющая элемент в конец контейнера */
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	if (handle == LSQ_HandleInvalid)
		return;
	InsertElementAtIndex(handle, ((ArrayPtrT)handle)->physical_size, element);
}

/* Функция, добавляющая элемент в контейнер на позицию, указываемую в данный момент итератором. Элемент, на который  *
 * указывает итератор, а также все последующие, сдвигается на одну позицию в конец.                                  */
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){
	IteratorPtrT iter = (IteratorPtrT)iterator;
	if (iterator == NULL)
		return;
	InsertElementAtIndex(iter->handle, iter->index, newElement);
}

/* Функция, удаляющая первый элемент контейнера */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	if (handle == LSQ_HandleInvalid)
		return;
	DeleteElementAtIndex(handle, 0);
}

/* Функция, удаляющая последний элемент контейнера */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle){
	if (handle == LSQ_HandleInvalid)
		return;
	DeleteElementAtIndex(handle, ((ArrayPtrT)handle)->physical_size - 1);
}

/* Функция, удаляющая элемент контейнера, указываемый заданным итератором. Все последующие элементы смещаются на     *
 * одну позицию в сторону начала.                                                                                    */
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
	IteratorPtrT iter = (IteratorPtrT)iterator;
	if (iterator == NULL)
		return;
	DeleteElementAtIndex(iter->handle, iter->index);
}
