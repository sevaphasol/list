#include <gtest/gtest.h>

#include "list.h"
#include "list_dump.h"

struct ListTest : public testing::Test {
    List_t list = {};

    void SetUp()    {Ctor(&list, 8); }
    void TearDown() {Dtor(&list); }
};

TEST_F(ListTest, DumpTest)
{
    ASSERT_TRUE(Dump(&list) == LIST_DUMP_SUCCESS);
}

TEST_F(ListTest, InsertTest)
{
    list.list_dump.n_dumps = 1;

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(InsertAfter(&list, 1488 + i, i) == LIST_SUCCESS);

        Dump(&list);
    }

    Dump(&list);

    ASSERT_TRUE(InsertBefore(&list, 666, 2) == LIST_SUCCESS);

    Dump(&list);
}

TEST_F(ListTest, EraseTest)
{
    list.list_dump.n_dumps = 8;

    InsertAfter(&list, 1488, 0);
    InsertAfter(&list, 1489, 1);

    Dump(&list);

    ASSERT_TRUE(Erase(&list, 1) == LIST_SUCCESS);

    Dump(&list);
}

TEST_F(ListTest, ClearTest)
{
    list.list_dump.n_dumps = 10;

    for (int i = 0; i < 5; i++)
    {
        InsertAfter(&list, 1488 + i, i);
    }

    Dump(&list);

    ASSERT_TRUE(Clear(&list) == LIST_SUCCESS);

    Dump(&list);
}

TEST_F(ListTest, PushPopTest)
{
    list.list_dump.n_dumps = 12;

    ASSERT_TRUE(PushFront(&list, 1488) == LIST_SUCCESS);
    ASSERT_TRUE(PushFront(&list, 1489) == LIST_SUCCESS);
    ASSERT_TRUE(PushFront(&list, 1490) == LIST_SUCCESS);

    Dump(&list);

    ListElem_t elem;
    ASSERT_TRUE(PopFront(&list, &elem) == LIST_SUCCESS);
    ASSERT_TRUE(elem == 1490);

    Dump(&list);

    ASSERT_TRUE(PopBack(&list, &elem) == LIST_SUCCESS);
    ASSERT_TRUE(elem == 1488);

    Dump(&list);
}

TEST_F(ListTest, FrontBackTest)
{
    list.list_dump.n_dumps = 15;

    PushFront(&list, 1488);
    PushBack (&list, 1489);

    Dump(&list);

    ListElem_t elem;
    ASSERT_TRUE(Front(&list, &elem) == LIST_SUCCESS);
    ASSERT_TRUE(elem == 1488);

    ASSERT_TRUE(Back(&list, &elem) == LIST_SUCCESS);
    ASSERT_TRUE(elem == 1489);
}

TEST_F(ListTest, NextPrevTest)
{
    list.list_dump.n_dumps = 16;

    PushFront(&list, 1488);
    PushFront(&list, 1489);
    PushFront(&list, 1490);

    Dump(&list);

    ListElem_t pos;
    ASSERT_TRUE(Next(&list, 3, &pos) == LIST_SUCCESS);
    ASSERT_TRUE(pos == 2);

    ASSERT_TRUE(Prev(&list, 2, &pos) == LIST_SUCCESS);
    ASSERT_TRUE(pos == 3);

    ASSERT_TRUE(Prev(&list, 1, &pos) == LIST_SUCCESS);
    ASSERT_TRUE(pos == 2);
}

TEST_F(ListTest, GetTest)
{
    list.list_dump.n_dumps = 17;

    PushFront(&list, 1488);

    Dump(&list);

    ListElem_t elem;
    ASSERT_TRUE(Get(&list, 1, &elem) == LIST_SUCCESS);
    ASSERT_TRUE(elem == 1488);
}
