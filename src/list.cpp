#include <string.h>

#include "list.h"
#include "list_dump.h"

//------------------------------------------------//

static ListReturnCode Insert (List_t* list, ListElem_t elem,
                              int pos, int* rel_next, int* rel_prev);

static ListReturnCode Pop    (List_t* list, int pos, ListElem_t* ret_elem);

//------------------------------------------------//

ListReturnCode Ctor(List_t* list, size_t size)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    if (size < MinListSize)
    {
        list->size = MinListSize;
    }
    else
    {
        list->size = size;
    }

    list->len  = 1;

    list->data = (ListElem_t*) calloc(size,     sizeof(list->elem_size));
    list->next = (int*)        calloc(size * 2, sizeof(size_t));
    list->prev = list->next + size;

    DO_IF(!list->data || !list->next || !list->prev, return LIST_ALLOCATE_ERROR);

    memset(list->data,     Poison,  size      * sizeof(list->elem_size));
    memset(list->prev + 1, Poison, (size - 1) * sizeof(size_t));

    for (int i = 1; i < size - 1; i++)
    {
        list->next[i] = i + 1;
    }

    list->next[size - 1] = Poison;

    list->free = 1;

    list->list_dump.dump_file_name = DumpFile;
    list->list_dump.dump_file = fopen(list->list_dump.dump_file_name, "w");
    fprintf(list->list_dump.dump_file, "<pre>\n"
                                       "<style>\n"
                                       "body {\n"
                                       "    background: %s;\n"
                                       "}\n"
                                       "</style>\n", BackGroundColor);
    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Dtor(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(list->list_dump.dump_file, "</pre>\n");
    fclose(list->list_dump.dump_file);

    list->size = 0;
    free(list->data);
    free(list->next);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Front(List_t* list, ListElem_t* ret_elem)
{
    DO_IF(!list,     return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!ret_elem, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_elem = list->data[list->next[0]];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Back(List_t* list, ListElem_t* ret_elem)
{
    DO_IF(!list,     return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!ret_elem, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_elem = list->data[list->prev[0]];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Next(List_t* list, int pos, int* ret_pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_pos = list->next[pos];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Prev(List_t* list, int pos, int* ret_pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_pos = list->prev[pos];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PushFront(List_t* list, ListElem_t elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    InsertAfter(list, elem, 0);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PushBack(List_t* list, ListElem_t elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    InsertBefore(list, elem, 0);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode InsertAfter(List_t* list, ListElem_t elem, int pos)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    Insert(list, elem, pos, list->next, list->prev);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode InsertBefore(List_t* list, ListElem_t elem, int pos)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    Insert(list, elem, pos, list->prev, list->next);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Insert(List_t* list, ListElem_t elem, int pos, int* rel_next, int* rel_prev)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    int free_pos = list->free;

    if (free_pos == Poison)
    {
        fprintf(stderr, "LIST OVERFLOW\n");

        return LIST_OVERFLOW_ERROR;
    }

    list->free              = list->next[list->free];

    list->data[free_pos]    = elem;
    rel_prev[free_pos]      = pos;
    rel_next[free_pos]      = rel_next[pos];

    rel_prev[rel_next[pos]] = free_pos;
    rel_next[pos]           = free_pos;

    if (pos == rel_prev[0])
    {
        rel_prev[0] = free_pos;
    }

    list->len++;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Get(List_t* list, int pos, ListElem_t* ret_elem)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    *ret_elem = list->data[pos];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PopFront(List_t* list, ListElem_t* ret_elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    Pop(list, list->next[0], ret_elem);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PopBack(List_t* list, ListElem_t* ret_elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    Pop(list, list->prev[0], ret_elem);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Pop(List_t* list, int pos, ListElem_t* ret_elem)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);
    DO_IF(!ret_elem,                      return LIST_ARGS_NULL_PTR_ERROR);

    *ret_elem = list->data[pos];

    list->data[pos]             = Poison;

    list->next[list->prev[pos]] = list->next[pos];
    list->prev[list->next[pos]] = list->prev[pos];

    list->next[pos]             = list->free;
    list->free                  = pos;
    list->prev[pos]             = Poison;

    list->len--;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Erase(List_t* list, int pos)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    ListElem_t dummy = 0;

    Pop(list, pos, &dummy);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Clear(List_t* list)
{
    DO_IF(!list,                      return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list->data || !list->next, return LIST_ARGS_NULL_PTR_ERROR);

    memset(list->data,     Poison,  list->size      * sizeof(list->elem_size));
    memset(list->prev + 1, Poison, (list->size - 1) * sizeof(size_t));

    for (int i = 1; i < list->size - 1; i++)
    {
        list->next[i] = i + 1;
    }

    list->next[0] = list->prev[0] = 0;

    list->len  = 1;
    list->free = 1;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Len(List_t* list, size_t* len)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!len,  return LIST_ARGS_NULL_PTR_ERROR);

    *len = list->len;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Verify(List_t* list)
{
    DO_IF(!list,                      return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list->prev || !list->next, return LIST_ARGS_NULL_PTR_ERROR);

    size_t ind  = 0;
    int    iter = 0;

    while (iter++ < list->len)
    {
        if (list->prev[list->next[ind]] != ind || list->next[list->prev[ind]] != ind)
        {
            DO_IF(true, return LIST_VERIFICATION_FAILED_ERROR);
        }

        ind = list->next[ind];

        if (!ind)
        {
            break;
        }
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//
