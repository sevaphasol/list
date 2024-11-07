#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <syscall.h>

#include "list.h"

static ListReturnCode MakePngDump       (List_t* list);
static ListReturnCode MakeDotDump       (List_t* list, FILE* dot_file);
static ListReturnCode DotInitSeq        (List_t* list, FILE* dot_file);
static ListReturnCode DotPrintSeq       (int*    ind_arr, size_t len, FILE* dot_file);
static ListReturnCode DotInitFree       (List_t* list, FILE* dot_file);
static ListReturnCode DotPrintFree      (List_t* list, FILE* dot_file);
static ListReturnCode MakeHtmlDump      (List_t* list);
static ListReturnCode HtmlPrintInfoElem (FILE* html_file, size_t var,            const char* var_name);
static ListReturnCode HtmlPrintArr      (FILE* html_file, size_t size, int* arr, const char* arr_name);
static ListReturnCode HtmlPrintImg      (FILE* html_file, size_t size,           const char* img_file_name);
static int            HtmlColorPrint    (FILE* html_file, const char* color, const char* str, ...);

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

ListReturnCode Dump(List_t* list)
{
    DO_IF(!list || !list->list_dump.dump_file || !list->data || !list->next, return LIST_ARGS_NULL_PTR_ERROR);

    MakePngDump (list);
    MakeHtmlDump(list);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode MakePngDump(List_t* list)
{
    char dot_file_name[BufSize] = {};
    snprintf(dot_file_name, BufSize, "logs/dot_files/%03d.dot", list->list_dump.n_dumps);
    FILE* dot_file = fopen(dot_file_name, "w");

    MakeDotDump(list, dot_file);

    fclose(dot_file);

    char png_file_name[BufSize] = {};
    snprintf(png_file_name, BufSize, "logs/images/%03d.png", list->list_dump.n_dumps++);

    char command[4*BufSize] = {};
    snprintf(command, 4*BufSize, "touch %s; dot %s -Tpng -o %s", png_file_name, dot_file_name, png_file_name);

    system(command);

    snprintf(list->list_dump.last_png_file_name, BufSize, "%s", png_file_name);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode MakeDotDump(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "digraph G{\n"
                      "rankdir=LR;\n"
                      "bgcolor=\"%s\";\n", BackGroundColor);

    DotInitSeq  (list,                  dot_file);
    DotPrintSeq (list->next, list->len, dot_file);
    DotPrintSeq (list->prev, list->len, dot_file);
    DotInitFree (list,                  dot_file);
    DotPrintFree(list,                  dot_file);

    fputs("}\n", dot_file);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotInitSeq(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "node[style=filled, color=\"%s\", fillcolor=\"%s\", fontcolor=\"%s\", fontsize=14];\n",
                       SeqNodeBorderColor, SeqNodeBackGroundColor, SeqFontColor);

    size_t ind  = 0;
    int    iter = 0;

    while (iter++ < list->len)
    {
        fprintf(dot_file, "elem%ld["
                          "shape=\"Mrecord\", "
                          "label=\"%ld | data = %d | next = %d | prev = %d"
                          "\"];\n",
                           ind, ind , list->data[ind], list->next[ind], list->prev[ind]);

        ind = list->next[ind];

        if (!ind) break;
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotPrintSeq(int* ind_arr, size_t len, FILE* dot_file)
{
    DO_IF(!ind_arr || !dot_file, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "edge[color=\"%s\",fontsize=12, penwidth=1];\n", SeqEdgeColor);

    size_t ind  = 0;
    int    iter = 0;

    while (iter++ < len)
    {
        fprintf(dot_file, "elem%ld->", ind);

        ind = ind_arr[ind];

        if (!ind)
        {
            fputs("elem0;\n", dot_file);

            break;
        }
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotInitFree(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "node[style=filled, color=\"%s\", fillcolor=\"%s\", fontcolor=\"%s\"fontsize=14];\n",
                       FreeNodeBorderColor, FreeNodeBackGroundColor, FreeFontColor);

    size_t free_ind = list->free;

    fprintf(dot_file, "free["
                      "shape=\"Mrecord\", "
                      "label=\"free | free = %ld"
                      "\"];\n",
                      free_ind);

    if (free_ind == Poison)
    {
        return LIST_SUCCESS;
    }

    size_t iter = 0;

    while (iter++ < list->size)
    {
        fprintf(dot_file, "free_elem%ld["
                          "shape=\"Mrecord\", "
                          "label=\"%ld | data = %d | next = %d | prev = %d"
                          "\"];\n",
                          free_ind, free_ind , list->data[free_ind],
                          list->next[free_ind], list->prev[free_ind]);

        if (list->next[free_ind] == Poison)
        {
            break;
        }

        free_ind = list->next[free_ind];
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotPrintFree(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "edge[color=\"%s\",fontsize=12, penwidth=1];\n", FreeEdgeColor);

    size_t free_ind = list->free;
    size_t iter     = 0;

    fputs("free->", dot_file);

    while (iter++ < list->size)
    {
        if (list->next[free_ind] == Poison)
        {
            fprintf(dot_file, "free_elem%ld;\n", free_ind);

            break;
        }

        fprintf(dot_file, "free_elem%ld->", free_ind);

        free_ind = list->next[free_ind];
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//

int HtmlColorPrint(FILE* html_file, const char* color, const char* str, ...)
{
    va_list list;
    va_start(list, str);

	fprintf(html_file, "<font color = \"%s\">", color);

    int n_printed_chars = vfprintf(html_file, str, list);

	fprintf(html_file, "</font>");

    va_end(list);

    return n_printed_chars;
}

//------------------------------------------------//

ListReturnCode HtmlPrintInfoElem(FILE* html_file, size_t var, const char* var_name)
{
    fprintf       (html_file,                "%s = ", var_name);
    HtmlColorPrint(html_file, InfoElemColor, "%ld", var);

    fputc         ('\n', html_file);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode HtmlPrintArr(FILE* html_file, size_t size, int* arr, const char* arr_name)
{
    fprintf       (html_file, "%s[", arr_name);
    HtmlColorPrint(html_file, PtrColor, "%p", arr);
    fputs         ("]: ", html_file);

    for (int i = 0; i < size; i++)
    {
        HtmlColorPrint(html_file, DataElemColor, "%5d ", arr[i]);
    }

    fputc('\n', html_file);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode HtmlPrintImg(FILE* html_file, size_t size, const char* img_file_name)
{
    fprintf(html_file, "<img src=%s width=%ld%%>\n\n", img_file_name, size);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode MakeHtmlDump(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    FILE* dump_file = list->list_dump.dump_file;

    //------------------------------------------------//

    fputs            ("list Dump[", dump_file);
    HtmlColorPrint   (dump_file, PtrColor, "%p", list);
    fputs            ("]\n\n", dump_file);

    //------------------------------------------------//

    HtmlPrintArr     (dump_file, list->size, list->data, "data");
    HtmlPrintArr     (dump_file, list->size, list->next, "next");
    HtmlPrintArr     (dump_file, list->size, list->prev, "prev");

    fputc            ('\n', dump_file);

    //------------------------------------------------//

    HtmlPrintInfoElem(dump_file, list->free, "list->free");
    HtmlPrintInfoElem(dump_file, list->size, "list->size");
    HtmlPrintInfoElem(dump_file, list->len,  "list->len ");

    fputc            ('\n', dump_file);

    //------------------------------------------------//

    HtmlPrintImg     (dump_file, 75, list->list_dump.last_png_file_name);

    //------------------------------------------------//

    HtmlColorPrint   (dump_file, SeparatorColor, "//------------------------------------------------------------------//");

    fputs            ("\n\n", dump_file);

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

ListReturnCode UTests()
{
    List_t list = {LIST_INIT};

    if (Ctor(&list, 8) != LIST_SUCCESS)
    {
        Dtor(&list);

        return LIST_FAILURE;
    }

    for (int i = 0; i < 5; i++)
    {
        Dump(&list);

        InsertAfter(&list, 1488 + i, i);
    }

    Dump(&list);

    InsertBefore(&list, 666, 2);

    Dump(&list);

    Erase(&list, 3);

    Dump(&list);

    ListElem_t elem;
    PopBack(&list, &elem);

    Dump(&list);

    Verify(&list);

    Clear(&list);

    Dump(&list);

    if (Dtor(&list) != LIST_SUCCESS)
    {
        return LIST_FAILURE;
    }

    return LIST_SUCCESS;
}

//------------------------------------------------//
