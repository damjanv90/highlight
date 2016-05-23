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

#include "range.h"

Range* Range_new(int start, int end, color _color, int priority){
  Range* range = (Range*)calloc(1, sizeof(Range));
  range->start = start;
  range->end = end;
  range->_color = _color;
  range->priority = priority;

  return range;
}

void expand_range_right(List* lst, Range* range, int expand_to, color _color, int priority){
  if (priority > range->priority){
    range->priority = priority;
    range->_color = _color;
  }
  range->end = expand_to;
  while (range->item.next != NULL){
    Range* next = (Range*)range->item.next;
    if (expand_to > next->start){
      // range is expanding over next so we will either merge them or shrink one
      if (range->priority < next->priority){
        // range is lesser priority so we will shrink it
        range->end = next->start - 1;
      } else {
        if (range->priority > next->priority && range->end < next->end) {
          // range is higher priority so we will shrink next
          next->start = range->end + 1;
        } else {
          // ranges are either of same priority or range is eating the whole next
          // in either case we should remove next, and repeat iteration
          remove_after(lst, (BasicItem*)range);
          // let's go to next range and see if we can expand more
          continue;
        }
      }
    }
    break;
  }
}

void expand_range(List* lst, Range* range, int start, int end, color _color, int priority){
  if (start < range->start){
    if (priority >= range->priority) {
      // expand range to left
      range->start = start;
      range->_color = _color;
      range->priority = priority;
    } else {
      add_before(lst, (BasicItem*)range, (BasicItem*)Range_new(start, range->start - 1, _color, priority));
    }
  }
  if (priority >= range->priority && end > range->end){
    expand_range_right(lst, range, end, _color, priority);
  }
}

void process_range(List* lst, int start, int end, color _color, int priority){
  if (is_empty(lst)){
    append(lst, (BasicItem*)Range_new(start, end, _color, priority));
  } else {
    Range* range;
    for (range = (Range*)lst->first; range != NULL; range = (Range*)range->item.next) {
      if (end < range->start){
        add_before(lst, (BasicItem*)range, (BasicItem*)Range_new(start, end, _color, priority));
      } else if (start <= range->end) {
        expand_range(lst, range, start, end, _color, priority);
      }

      if (end > range->end){
        // there are leftovers
        start = start > range->end ? start : range->end + 1;
        if (range->item.next == NULL){
          append (lst, (BasicItem*)Range_new(start, end, _color, priority));
        } else {
          // there is next range, so check if that can be expanded
          continue;
        }
      }
      break;
    }
  }
}
