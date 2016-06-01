/*
The MIT License (MIT)

Copyright (c) 2016 Damjan Vukovic (email:damjanvu@gmail.com, github:damjanv90)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/utils/list.h"
#include "../src/utils/bool.h"

void test_add_before_first(){
  List lst = EMPTY_LST;
  BasicItem first ={NULL, NULL};
  append(&lst, &first);

  BasicItem before_first={NULL, NULL};
  add_before(&lst, &first, &before_first);
  assert(lst.first == &before_first);
  assert(lst.last == &first);
  assert(lst.first->next == &first);
  assert(first.prev == &before_first);
  assert(before_first.prev == NULL);
  assert(first.next == NULL);
}

void test_add_before_second(){
  List lst = EMPTY_LST;
  BasicItem first={NULL, NULL};
  append(&lst, &first);

  BasicItem second={NULL, NULL};
  append(&lst, &second);

  BasicItem before_second={NULL, NULL};
  add_before(&lst, &second, &before_second);

  assert(lst.last == &second);
  assert(lst.first == &first);
  assert(lst.first->next == &before_second);
  assert(before_second.prev == &first);
  assert(before_second.next == &second);
  assert(first.prev == NULL);
  assert(second.prev == &before_second);
  assert(second.next == NULL);
}

void test_add_before(){
  test_add_before_first();
  test_add_before_second();
}

void test_append_empty(){
  List lst = EMPTY_LST;
  BasicItem first ={NULL, NULL};
  append(&lst, &first);
  assert(is_empty(&lst) == FALSE);
  assert(lst.first == &first);
  assert(lst.last == &first);
  assert(lst.first->next == NULL);
  assert(lst.first->prev == NULL);
}

void test_append_one_item(){
  List lst = EMPTY_LST;
  BasicItem first={NULL, NULL};
  append(&lst, &first);

  BasicItem second={NULL, NULL};
  append(&lst, &second);

  assert(is_empty(&lst) == FALSE);
  assert(lst.first == &first);
  assert(lst.last == &second);
  assert(lst.first->next == &second);
  assert(lst.first->prev == NULL);
  assert(lst.last->prev == &first);
  assert(lst.last->next == NULL);
}

void test_append(){
  test_append_empty();
  test_append_one_item();
}

void test_remove_last(){
  List lst = EMPTY_LST;
  BasicItem first={NULL, NULL};
  append(&lst, &first);

  BasicItem* second = (BasicItem*)calloc(1, sizeof(BasicItem));
  append(&lst, second);

  remove_after(&lst, &first);
  assert(is_empty(&lst) == FALSE);
  assert(lst.first == &first);
  assert(lst.last == &first);
  assert(lst.first->next == NULL);
  assert(lst.first->prev == NULL);
}

void test_remove_after_last(){
  List lst = EMPTY_LST;
  BasicItem first={NULL, NULL};
  append(&lst, &first);

  BasicItem second={NULL, NULL};
  append(&lst, &second);

  remove_after(&lst, &second);
  assert(is_empty(&lst) == FALSE);
  assert(lst.first == &first);
  assert(lst.last == &second);
  assert(lst.first->next == &second);
  assert(lst.first->prev == NULL);
  assert(lst.last->prev == &first);
  assert(lst.last->next == NULL);
}

void test_remove_after(){
  test_remove_after_last();
  test_remove_last();
}

void test_is_empty_empty(){
    List lst = EMPTY_LST;
    assert(is_empty(&lst) == TRUE);
}

void test_is_empty_null(){
  List* lst = NULL;
  assert(is_empty(lst) == TRUE);
}

void test_is_empty_one_item(){
  List lst = EMPTY_LST;
  BasicItem first={NULL, NULL};
  append(&lst, &first);
  assert(is_empty(&lst) == FALSE);
}

void test_is_empty(){
  test_is_empty_empty();
  test_is_empty_null();
  test_is_empty_one_item();
}

void test_clear_empty_list(){
  List lst = EMPTY_LST;
  clear(&lst);
  assert(is_empty(&lst)==TRUE);
}

void test_clear_null_list(){
  List* lst = NULL;
  clear(lst);
  assert(is_empty(lst)==TRUE);
}

void test_clear_one_item_list(){
  List lst = EMPTY_LST;
  BasicItem* first = (BasicItem*)calloc(1, sizeof(BasicItem));
  append(&lst, first);
  clear(&lst);
  assert(is_empty(&lst)==TRUE);
}

void test_clear_two_item_list(){
  List lst = EMPTY_LST;
  BasicItem* first = (BasicItem*)calloc(1, sizeof(BasicItem));
  append(&lst, first);

  BasicItem* second = (BasicItem*)calloc(1, sizeof(BasicItem));
  append(&lst, second);

  clear(&lst);
  assert(is_empty(&lst)==TRUE);
}

void test_clear_list(){
  test_clear_empty_list();
  test_clear_null_list();
  test_clear_one_item_list();
  test_clear_two_item_list();
}

int main(int argc, char** argv){
  printf("Started testing list...\n");

  test_add_before();
  test_remove_after();
  test_append();
  test_is_empty();
  test_clear_list();

  printf("Finished testing list SUCCESSFULY!\n\n");
}
