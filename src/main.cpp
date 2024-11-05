#include <stdlib.h>
#include <stdio.h>

#include "list.h"

int main()
{
    List_t list = {LIST_INIT};

    if (Ctor(&list, 8) != LIST_SUCCESS)
    {
        Dtor(&list);

        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++)
    {
        Dump(&list);

        InsertAfter(&list, 1488 + i, i);
    }

    Dump(&list);

    // InsertBefore(&list, 666, 2);
    Insert(&list, 666, 2, LIST_INSERT_BEFORE_MODE);

    Dump(&list);

    Erase(&list, 3);

    Dump(&list);

    if (Dtor(&list) != LIST_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
