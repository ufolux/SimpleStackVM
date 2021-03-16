//
// Created by Sines on 2021/3/14.
//

#ifndef ASMER_EXECSTACK_H
#define ASMER_EXECSTACK_H

#include <stdbool.h>

typedef struct ExecStack {
    int cap;
    int size;
    int *data;
    int *sp;
    int *pc;
} ExecStack;

ExecStack *es_new();

void es_destroy(ExecStack *es);

void es_push(ExecStack *es, int arg);

int es_pop(ExecStack *es);

int es_peek(ExecStack *es);

bool es_empty(ExecStack *es);

#endif //ASMER_EXECSTACK_H
