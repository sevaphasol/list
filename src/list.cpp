#include <stdio.h>
#include <string.h>
#include <syscall.h>

#include "list.h"

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
    list->next = (size_t*)     calloc(size * 2, sizeof(size_t));
    list->prev = list->next + size;

    DO_IF(!list->data || !list->next || !list->prev, return LIST_ALLOCATE_ERROR);

    memset(list->prev + 1, -1, (size - 1) * sizeof(size_t));

    for (int i = 1; i < size - 1; i++)
    {
        list->next[i] = i + 1;
    }

    list->next[size - 1] = Poison;

    list->free = 1;

    list->list_dump.dump_file_name = DumpFile;
    list->list_dump.dump_file = fopen(list->list_dump.dump_file_name, "w");
    fprintf(list->list_dump.dump_file, "<pre>\n");

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

ListReturnCode ListVerify(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Dump(List_t* list)
{
    DO_IF(!list || !list->list_dump.dump_file || !list->data || !list->next, return LIST_ARGS_NULL_PTR_ERROR);

    MakePngDump(list);
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
    fputs("digraph G{\n"
          "rankdir=LR;\n"
          "node[color=\"red\",fontsize=14];\n"
          "edge[color=\"blue\",fontsize=12, penwidth=3];\n",
          dot_file);

    DotPrintData(list, dot_file);
    DotPrintNext(list, dot_file);
    DotPrintFree(list, dot_file);

    fputs("}\n", dot_file);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotPrintData(List_t* list, FILE* dot_file)
{
    for (int i = 0; i < list->len; i++)
    {
        fprintf(dot_file, "elem%d["
                          "shape=\"Mrecord\", "
                          "label=\"%d | data = %d | next = %ld | prev = %ld"
                          "\"];\n",
                           i, i , list->data[i], list->next[i], list->prev[i]);
    }

    for (int i = 0; i < list->len; i++)
    {
        fprintf(dot_file, "elem%d->", i);
    }

    fprintf(dot_file, "elem%d;\n", 0);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotPrintNext(List_t* list, FILE* dot_file)
{
    fputs("edge[color=\"red\",fontsize=12, penwidth=1];\n", dot_file);

    fputs("elem0->", dot_file);

    for (int i = 0; i < list->len - 1; i++)
    {
        fprintf(dot_file, "elem%ld->", list->next[i]);
    }

    fprintf(dot_file, "elem%ld;", list->next[list->len - 1]);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode DotPrintFree(List_t* list, FILE* dot_file)
{
    fputs("edge[color=\"green\",fontsize=12, penwidth=1];\n", dot_file);

    fprintf(dot_file, "free["
                      "shape=\"Mrecord\", "
                      "label=\"free | free = %ld"
                      "\"];\n",
                      list->free);

    size_t free_ind = list->free;

    while (list->next[free_ind] != -1)
    {
        fprintf(dot_file, "elem%ld["
                    "shape=\"Mrecord\", "
                    "label=\"%ld | data = %d | next = %ld | prev = %ld"
                    "\"];\n",
                    free_ind, free_ind , list->data[free_ind],
                    list->next[free_ind], list->prev[free_ind]);

        free_ind = list->next[free_ind];
    }

    fprintf(dot_file, "elem%ld["
                "shape=\"Mrecord\", "
                "label=\"%ld | data = %d | next = %ld | prev = %ld"
                "\"];\n",
                free_ind, free_ind , list->data[free_ind],
                list->next[free_ind], list->prev[free_ind]);

    free_ind = list->free;

    fprintf(dot_file, "free->");

    while (list->next[free_ind] != -1)
    {
        fprintf(dot_file, "elem%ld->", free_ind);

        free_ind = list->next[free_ind];
    }

    fprintf(dot_file, "elem%ld;\n", free_ind);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode MakeHtmlDump(List_t* list)
{
    FILE* dump_file = list->list_dump.dump_file;

    fprintf(dump_file, "list Dump[%p]\n\n", list);

    //------------------------------------------------//

    fprintf(dump_file, "data[%p]: ", list->data);

    for (int i = 0; i < list->size - 1; i++)
    {
        fprintf(dump_file, "%3d ", list->data[i]);
    }

    fprintf(dump_file, "%3d\n", list->data[list->size - 1]);

    //------------------------------------------------//

    fprintf(dump_file, "next[%p]: ", list->next);

    for (int i = 0; i < list->size - 1; i++)
    {
        fprintf(dump_file, "%3ld ", list->next[i]);
    }

    fprintf(dump_file, "%3ld\n", list->next[list->size - 1]);

    //------------------------------------------------//

    fprintf(dump_file, "prev[%p]: ", list->prev);

    for (int i = 0; i < list->size - 1; i++)
    {
        fprintf(dump_file, "%3ld ", list->prev[i]);
    }

    fprintf(dump_file, "%3ld\n\n", list->prev[list->size - 1]);

    //------------------------------------------------//

    fprintf(dump_file, "list->free: %ld\n", list->free);
    fprintf(dump_file, "list->size: %ld\n", list->size);
    fprintf(dump_file, "list->len:  %ld\n\n", list->len);

    //------------------------------------------------//

    fprintf(dump_file, "<img src=%s width=75%%>\n\n", list->list_dump.last_png_file_name);

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Front(List_t* list, size_t* ret_pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_pos = list->data[list->next[0]];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Back(List_t* list, size_t* ret_pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_pos = list->data[list->prev[0]];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Next(List_t* list, size_t pos, size_t* ret_pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_pos = list->next[pos];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Prev(List_t* list, size_t pos, size_t* ret_pos)
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

    list->data[list->next[0]] = elem;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PushBack(List_t* list, ListElem_t elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    list->data[list->prev[0]] = elem;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode InsertAfter(List_t* list, ListElem_t elem, size_t pos)
{
    DO_IF(!list,                          return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    size_t free_pos      = list->free;
    list->free           = list->next[list->free];

    list->data[free_pos] = elem;
    list->prev[free_pos] = pos;
    list->next[free_pos] = list->next[pos];

    list->next[pos]      = free_pos;

    list->len++;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode InsertBefore(List_t* list, ListElem_t elem, size_t pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!(0 < pos || pos < list->size), return LIST_INVALID_POS_ERROR);

    size_t free_pos      = list->free;
    list->free           = list->next[list->free];

    list->data[free_pos] = elem;
    list->next[free_pos] = pos;
    list->prev[free_pos] = list->prev[pos];

    list->prev[pos]      = free_pos;

    list->len++;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Get(List_t* list, size_t pos, ListElem_t* ret_elem)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);
    DO_IF(!list, return LIST_ARGS_NULL_PTR_ERROR);

    *ret_elem = list->data[pos];

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PopFront(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    // list->data--;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode PopBack(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    // list->data--;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Erase(List_t* list, size_t pos)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

    list->data[pos]             = Poison;

    list->next[list->prev[pos]] = list->next[pos];
    list->prev[list->next[pos]] = list->prev[pos];

    list->next[pos]             = list->free;
    list->free                    = pos;
    list->prev[pos]             = Poison;

    list->data--;

    return LIST_SUCCESS;
}

//------------------------------------------------//

ListReturnCode Clear(List_t* list)
{
    DO_IF(!list, return LIST_STRUCT_NULL_PTR_ERROR);

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
