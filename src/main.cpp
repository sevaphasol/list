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

    InsertAfter(&list, 1, 0);

    Dump(&list);

    InsertAfter(&list, 2, 1);

    Dump(&list);

    InsertAfter(&list, 3, 2);
    InsertAfter(&list, 4, 3);
    InsertAfter(&list, 5, 4);

    Dump(&list);

    InsertAfter   (&list, 7, 3);

    Dump(&list);

    if (Dtor(&list) != LIST_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
