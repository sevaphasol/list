#include <stdlib.h>
#include <stdio.h>

#include "list.h"

int main()
{
    if (UTests() != LIST_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
