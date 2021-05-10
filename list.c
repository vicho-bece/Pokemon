
#include <string.h>
#include "list.h"

/*
  ants
*/
#define ARRAYLIST_INITIAL_CAPACITY 10
#define ARRAYLIST_CAPACITY_DELTA 10

/*
  constants
*/
#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0


/*
  type definitions
*/
#undef Boolean
#define Boolean short unsigned int

#undef Object
#define Object void*

typedef struct Arraylist_Struct *Arraylist;
typedef struct Arraylist_Struct List;

static  size_t object_size = sizeof(Object);


/*
  structures
*/
struct Arraylist_Struct {
  int _current_capacity;
  Object *_data;
  int _size;
   Boolean (*_equals)();
  int _current_index;
};


/*
  private function declarations
*/
static void *checked_malloc( size_t size);

int arraylist_size( Arraylist list)
{
  return list->_size;
}

void arraylist_free( Arraylist list)
{
  free(list->_data);
  free(list);
}

Arraylist arraylist_create()
{
  Arraylist list;

#ifdef GOK_DEBUG
  list = malloc(sizeof(struct Arraylist_Struct));
#else
  list = checked_malloc(sizeof(struct Arraylist_Struct));
#endif
  list->_current_capacity = ARRAYLIST_INITIAL_CAPACITY;
#ifdef GOK_DEBUG
  list->_data = malloc(object_size * list->_current_capacity);
#else
  list->_data = checked_malloc(object_size * list->_current_capacity);
#endif
  list->_size = 0;
  //list->_equals = equals;

  return list;
}

Boolean arraylist_add( Arraylist list, Object object)
{
  int old_size = arraylist_size(list);
  int new_capacity;
  Object *new_data;

  (list->_size)++;
  if (old_size == list->_current_capacity)
    {
      new_capacity = list->_current_capacity + ARRAYLIST_CAPACITY_DELTA;
#ifdef GOK_DEBUG
      new_data = malloc(object_size * new_capacity);
#else
      new_data = checked_malloc(object_size * new_capacity);
#endif
      memcpy(new_data, list->_data, object_size * old_size);
      free(list->_data);
      (list->_data) = new_data;
      list->_current_capacity = new_capacity;
    }
  (list->_data)[old_size] = object;
  return TRUE;
}

Boolean arraylist_remove( Arraylist list,  int index)
{
  int length = arraylist_size(list);
  int last_index = length - 1;
  int new_size, new_capacity;

  (list->_size)--;
  if (index < last_index)
    {
      memmove(list->_data + index, list->_data + index + 1, object_size * (last_index - index));
      new_size = list->_size;
      new_capacity = list->_current_capacity - ARRAYLIST_CAPACITY_DELTA;
      if (new_capacity > new_size)
    {
      list->_data = realloc(list->_data, object_size * new_capacity);
      list->_current_capacity = new_capacity;
    }
    }
  return TRUE;


}




Boolean arraylist_is_empty( Arraylist list)
{
  return (0 == arraylist_size(list));
}


Object arraylist_get( Arraylist list,  int index)
{
  list->_current_index = index;
  return list->_data[index];
}

void arraylist_clear( Arraylist list)
{
  list->_data = realloc(list->_data, object_size * ARRAYLIST_INITIAL_CAPACITY);
  list->_current_capacity = ARRAYLIST_INITIAL_CAPACITY;
  list->_size = 0;
}

void arraylist_sort( Arraylist list,  int (*compare)( Object object_1,  Object object_2))
{
  qsort(list->_data,
	arraylist_size(list),
	sizeof(Object),
	(int (*)())compare);
}

static void *checked_malloc( size_t size)
{
  void *data;

  data = malloc(size);
  if (data == NULL)
    {
      fprintf(stderr, "\nOut of memory.\n");
      fflush(stderr);
      exit(EXIT_FAILURE);
    }

  return data;
}

//void arraylist_free(List* list);
List* create_list(){
    return arraylist_create();
}

void push_back(List* L, void* object){
    arraylist_add(L, object);
}

void pop_front(List* L){
    arraylist_remove(L, 0);
}

void pop_back(List* L){
    arraylist_remove(L, L->_size-1);
}

void pop_current(List* L){
    arraylist_remove(L, L->_current_index);
}

int is_empty(List* L){
   return arraylist_is_empty(L);
}

int size(List* L){
    return arraylist_size(L);
}

void* first(List* L){
    return arraylist_get(L, 0);
}

void* next(List* L){
    L->_current_index++;
    if(L->_current_index >= L->_size) return NULL;
    return arraylist_get(L, L->_current_index);
}

void clear(List* L){
   arraylist_clear(L);
}
