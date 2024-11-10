#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <syscall.h>

#include "list.h"
#include "list_dump.h"

ListDumpReturnCode Dump (List_t* list);

static ListDumpReturnCode MakePngDump       (List_t* list);
static ListDumpReturnCode MakeDotDump       (List_t* list, FILE* dot_file);
static ListDumpReturnCode DotInitSeq        (List_t* list, FILE* dot_file);
static ListDumpReturnCode DotPrintSeq       (int*    ind_arr, size_t len, FILE* dot_file);
static ListDumpReturnCode DotInitFree       (List_t* list, FILE* dot_file);
static ListDumpReturnCode DotPrintFree      (List_t* list, FILE* dot_file);
static ListDumpReturnCode MakeHtmlDump      (List_t* list);
static ListDumpReturnCode HtmlPrintInfoElem (FILE* html_file, size_t var,            const char* var_name);
static ListDumpReturnCode HtmlPrintArr      (FILE* html_file, size_t size, int* arr, const char* arr_name);
static ListDumpReturnCode HtmlPrintImg      (FILE* html_file, size_t size,           const char* img_file_name);
static int                HtmlColorPrint    (FILE* html_file, const char* color, const char* str, ...);

ListDumpReturnCode Dump(List_t* list)
{
    DO_IF(!list || !list->list_dump.dump_file || !list->data || !list->next, return LIST_DUMP_ARGS_NULL_PTR_ERROR);

    MakePngDump (list);
    MakeHtmlDump(list);

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode MakePngDump(List_t* list)
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

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode MakeDotDump(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

    fprintf(dot_file, "digraph G{\n"
                      "rankdir=LR;\n"
                      "bgcolor=\"%s\";\n", BackGroundColor);

    DotInitSeq  (list,                  dot_file);
    DotPrintSeq (list->next, list->len, dot_file);
    DotPrintSeq (list->prev, list->len, dot_file);
    DotInitFree (list,                  dot_file);
    DotPrintFree(list,                  dot_file);

    fputs("}\n", dot_file);

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode DotInitSeq(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

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

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode DotPrintSeq(int* ind_arr, size_t len, FILE* dot_file)
{
    DO_IF(!ind_arr || !dot_file, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

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

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode DotInitFree(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

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
        return LIST_DUMP_SUCCESS;
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

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode DotPrintFree(List_t* list, FILE* dot_file)
{
    DO_IF(!list || !dot_file, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

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

    return LIST_DUMP_SUCCESS;
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

ListDumpReturnCode HtmlPrintInfoElem(FILE* html_file, size_t var, const char* var_name)
{
    fprintf       (html_file,                "%s = ", var_name);
    HtmlColorPrint(html_file, InfoElemColor, "%ld", var);

    fputc         ('\n', html_file);

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode HtmlPrintArr(FILE* html_file, size_t size, int* arr, const char* arr_name)
{
    fprintf       (html_file, "%s[", arr_name);
    HtmlColorPrint(html_file, PtrColor, "%p", arr);
    fputs         ("]: ", html_file);

    for (int i = 0; i < size; i++)
    {
        HtmlColorPrint(html_file, DataElemColor, "%5d ", arr[i]);
    }

    fputc('\n', html_file);

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode HtmlPrintImg(FILE* html_file, size_t size, const char* img_file_name)
{
    fprintf(html_file, "<img src=%s width=%ld%%>\n\n", img_file_name, size);

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//

ListDumpReturnCode MakeHtmlDump(List_t* list)
{
    DO_IF(!list, return LIST_DUMP_STRUCT_NULL_PTR_ERROR);

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

    return LIST_DUMP_SUCCESS;
}

//------------------------------------------------//
