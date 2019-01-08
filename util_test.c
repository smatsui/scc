#include "scc.h"
#include <stdio.h>
#include <stdlib.h>

void expect(int line, int expected, int actual) {
  if (expected == actual) {
    return;
  }
  fprintf(stderr, "%d: %d expected, but got %d\n",
          line, expected, actual);
  exit(1);
}

typedef struct {
  int a;
  int b;
  int c;
}TestStruct;

void test_vector(){
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for(int i=0; i < 100; i++){
    vec_push(vec, (void *)i);
  }

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);

  TestStruct st0 = {.a = 1, .b = 2, .c = 3};
  TestStruct st1 = {.a = 2, .b = 3, .c = 4};
  TestStruct st2 = {.a = 3, .b = 4, .c = 5};
  vec_push(vec, &st0);
  vec_push(vec, &st1);
  vec_push(vec, &st2);

  expect(__LINE__, 1, ((TestStruct *)vec->data[100])->a);
  expect(__LINE__, 2, ((TestStruct *)vec->data[100])->b);
  expect(__LINE__, 3, ((TestStruct *)vec->data[100])->c);
}

void test_map(){
  Map *map = new_map();
  expect(__LINE__, 0, (int)map_get(map, "foo"));

  map_put(map, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_get(map, "foo"));

  map_put(map, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_get(map, "bar"));

  map_put(map, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_get(map, "foo"));
}

void runtest() {
  test_vector();
  test_map();
  printf("OK\n");
}
