#ifndef LIST_H__
#define LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "list_dump.h"

/* Provided for reducing code size of checks in functions */
#define DO_IF(condition, action) if (condition)                                                            \
                                    {                                                                      \
                                        fprintf(stderr, "%s in %s:%d:%s\n",                                \
                                                        #action, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
                                        action;                                                            \
                                    }                                                                      \

const size_t  MinListSize = 8;
const int     Poison      = -1;

const char* const DumpFile = "ListDump.html";

enum ListReturnCode
{
    LIST_SUCCESS,
    LIST_FAILURE,
    LIST_ALLOCATE_ERROR,
    LIST_STRUCT_NULL_PTR_ERROR,
    LIST_ARGS_NULL_PTR_ERROR,
    LIST_INVALID_POS_ERROR,
    LIST_OVERFLOW_ERROR,
    LIST_VERIFICATION_FAILED_ERROR,
};

typedef int ListElem_t;

typedef struct List
{
    size_t      size;
    size_t      elem_size;
    size_t      len;
    ListElem_t* data;
    int*        next;
    int*        prev;
    int         free;
    ListDump_t  list_dump;
} List_t;

extern ListDumpReturnCode Dump (List_t* list);

ListReturnCode Ctor         (List_t* list, size_t size);
ListReturnCode Dtor         (List_t* list);

ListReturnCode Verify       (List_t* list);

ListReturnCode Front        (List_t* list, ListElem_t* ret_elem);
ListReturnCode Back         (List_t* list, ListElem_t* ret_elem);
ListReturnCode Next         (List_t* list, int pos, int* ret_pos);
ListReturnCode Prev         (List_t* list, int pos, int* ret_pos);

ListReturnCode PushFront    (List_t* list, ListElem_t elem);
ListReturnCode PushBack     (List_t* list, ListElem_t elem);
ListReturnCode Insert       (List_t* list, ListElem_t elem, int pos, int* rel_next, int* rel_prev);
ListReturnCode InsertAfter  (List_t* list, ListElem_t elem, int pos);
ListReturnCode InsertBefore (List_t* list, ListElem_t elem, int pos);

ListReturnCode Get          (List_t* list, int pos, ListElem_t* ret_elem);

ListReturnCode PopFront     (List_t* list, ListElem_t* ret_elem);
ListReturnCode PopBack      (List_t* list, ListElem_t* ret_elem);
ListReturnCode Pop          (List_t* list, int pos, ListElem_t* ret_elem);
ListReturnCode Erase        (List_t* list, int pos);
ListReturnCode Clear        (List_t* list);

ListReturnCode UTests       ();

#endif // LIST_H__
