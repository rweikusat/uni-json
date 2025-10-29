/*
   uni-json bindings for Python -- work_string

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/
#ifndef uj_python_work_string_h
#define uj_python_work_string_h

/*  includes */
#include <inttypes.h>

/*  types */
struct work_string {
    uint8_t *s, *p, *e;
};

/*  routines */
void *make_work_string(void);
void free_work_string(void *ws);
int add_2_work_string(uint8_t *data, size_t len, void *ws);

#endif
