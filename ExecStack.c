//
// Created by Sines on 2021/3/14.
//
#include <memory.h>
#include <stdlib.h>
#include "ExecStack.h"

/// Stack

static void m_es_realloc(ExecStack *es) {
    int *data = NULL;
    int cap;
    if (es->cap == 0) {
        cap = 10;
    } else {
        cap = 2 * es->cap;
    }
    data = (int*)malloc(cap * sizeof(int));
    es->sp = data + (es->sp - es->data);
    memcpy(data, es->data, sizeof(int) * es->cap);
    es->data = data;
    es->cap = cap;
}

ExecStack *es_new() {
    ExecStack *res = (ExecStack *)malloc(sizeof(ExecStack));
    res->size = 0;
    res->sp = res->data;
    m_es_realloc(res);
    return res;
}

void es_destroy(ExecStack *es) {
    es->sp = NULL;
    free(es->data);
    free(es);
    es = NULL;
}

void es_push(ExecStack *es, int arg) {
    es->size++;
    if (es->size > es->cap) {
        m_es_realloc(es);
    }
    *es->sp = arg;
    es->sp++;
}

int es_pop(ExecStack *es) {
    int top = *(es->sp - 1);
    es->size--;
    es->sp--;
    return top;
}

int es_peek(ExecStack *es) {
    return *(es->sp - 1);
}

bool es_empty(ExecStack *es) {
    return es->size == 0;
}

/// End Stack