#ifndef LIST_DUMP_H__
#define LIST_DUMP_H__

#include "list.h"

const size_t      BufSize                 = 100;

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

enum ListDumpReturnCode
{
    LIST_DUMP_SUCCESS,
    LIST_DUMP_STRUCT_NULL_PTR_ERROR,
    LIST_DUMP_ARGS_NULL_PTR_ERROR,
};

typedef struct ListDump
{
    const char* dump_file_name;
    FILE*       dump_file;
    int         n_dumps;
    char        last_png_file_name[BufSize];
} ListDump_t;

#endif // LIST_DUMP_H__
