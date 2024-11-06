#ifndef LIST_H__
#define LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define LIST_INIT .size      = 0,                  \
                  .elem_size = sizeof(ListElem_t), \
                  .len       = 1,                  \
                  .data      = nullptr,            \
                  .next      = nullptr,            \
                  .prev      = nullptr,            \
                  .free      = 0,                  \
                  .list_dump =                     \
                  {.dump_file_name    = nullptr,   \
                  .dump_file          = nullptr,   \
                  .n_dumps            = 0,         \
                  .last_png_file_name = {}}        \

/* Provided for reducing code size of checks in functions */
#define DO_IF(condition, action) if (condition)                                                            \
                                    {                                                                      \
                                        fprintf(stderr, "%s in %s:%d:%s\n",                                \
                                                        #action, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
                                        action;                                                            \
                                    }                                                                      \

const size_t  MinListSize = 8;
const size_t  BufSize     = 100;
const int     Poison      = -1;

const char* const BackGroundColor         = "#bcbddc";

const char* const SeqNodeBackGroundColor  = "#a1d99b";
const char* const SeqNodeBorderColor      = "#756bb1";
const char* const SeqEdgeColor            = "#006d2c";
const char* const SeqFontColor            = "#49006a";

const char* const FreeNodeBackGroundColor = "#9ecae1";
const char* const FreeNodeBorderColor     = "#756bb1";
const char* const FreeEdgeColor           = "#08519c";
const char* const FreeFontColor           = "#67000d";

const char* const PtrColor                = "#ff0000";
const char* const DataElemColor           = "#006837";
const char* const InfoElemColor           = "#045a8d";
const char* const SeparatorColor          = "#980043";

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
    LIST_INVALID_MODE_ERROR,
    LIST_VERIFICATION_FAILED_ERROR,
};

typedef int ListElem_t;

typedef struct ListDump
{
    const char* dump_file_name;
    FILE*       dump_file;
    int         n_dumps;
    char        last_png_file_name[BufSize];
} ListDump_t;

typedef struct List
{
    size_t      size;
    size_t      elem_size;
    size_t      len;
    ListElem_t* data;
    size_t*     next;
    size_t*     prev;
    size_t      free;
    ListDump_t  list_dump;
} List_t;

ListReturnCode Ctor         (List_t* list, size_t size);
ListReturnCode Dtor         (List_t* list);

ListReturnCode Dump         (List_t* list);
ListReturnCode Verify       (List_t* list);

ListReturnCode Front        (List_t* list, ListElem_t* ret_elem);
ListReturnCode Back         (List_t* list, ListElem_t* ret_elem);
ListReturnCode Next         (List_t* list, size_t pos, size_t* ret_pos);
ListReturnCode Prev         (List_t* list, size_t pos, size_t* ret_pos);

ListReturnCode PushFront    (List_t* list, ListElem_t elem);
ListReturnCode PushBack     (List_t* list, ListElem_t elem);
ListReturnCode Insert       (List_t* list, ListElem_t elem, size_t pos, size_t* rel_next, size_t* rel_prev);
ListReturnCode InsertAfter  (List_t* list, ListElem_t elem, size_t pos);
ListReturnCode InsertBefore (List_t* list, ListElem_t elem, size_t pos);

ListReturnCode Get          (List_t* list, size_t pos, ListElem_t* ret_elem);

ListReturnCode PopFront     (List_t* list, ListElem_t* ret_elem);
ListReturnCode PopBack      (List_t* list, ListElem_t* ret_elem);
ListReturnCode Pop          (List_t* list, size_t pos, ListElem_t* ret_elem);
ListReturnCode Erase        (List_t* list, size_t pos);
ListReturnCode Clear        (List_t* list);

ListReturnCode UTests       ();

#endif // LIST_H__
