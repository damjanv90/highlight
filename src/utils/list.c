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

#include <stdlib.h>
#include "list.h"

const List EMPTY_LST = {NULL, NULL};

int append(List* lst, BasicItem* item){
  if (lst == NULL || item == NULL){
    return -1;
  }

  if (lst->first == NULL){
    lst->first = item;
  } else {
    item->prev = lst->last;
    lst->last->next = item;
  }
  lst->last = item;

  return 0;
}

int add_before(List* lst, BasicItem* before, BasicItem* new_item){
  if (before->prev == NULL) {
    lst->first = new_item;
  } else {
    before->prev->next = new_item;
  }

  new_item->prev = before->prev;
  before->prev = new_item;
  new_item->next = before;
}

void remove_after(List* lst, BasicItem* item) {
  BasicItem* to_remove = item->next;

  if (lst->last == to_remove){
    lst->last = to_remove->prev;
  }

  to_remove->next->prev = item;
  item->next = to_remove->next;
  free(to_remove);
}

int is_empty(List* lst){
  return lst == NULL || lst->first == NULL;
}

void clear(List* lst){
  if (is_empty(lst)){
    return;
  }

  BasicItem* item = lst->first;
  while (item != NULL){
    BasicItem* tmp = item;
    if (item->next != NULL){
      item->next->prev = NULL;
    }
    item = item->next;
    free(tmp);
  }

  lst->first = NULL;
  lst->last = NULL;
}
