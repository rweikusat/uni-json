/*
   uni-json bindings for Python -- work_string

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "work_string.h"

/*  constants */
enum {
    WS_MIN =	32
};

/*  routines */
void _hidden_ *make_work_string(void)
{
    struct work_string *ws;

    ws = malloc(sizeof(*ws));
    if (!ws) return NULL;
    ws->s = ws->p = ws->e = NULL;

    return ws;
}

int _hidden_ add_2_work_string(uint8_t *data, size_t len, void *str)
{
    struct work_string *ws;
    uint8_t *tmp;
    size_t in_ws, have, want;

    ws = str;
    if (!ws->s || ws->e - ws->p < (ptrdiff_t)len) {
        if (ws->s) {
            in_ws = ws->p - ws->s;

            have = ws->e - ws->s;
            if (have > len) want = have * 2;
            else want = in_ws + len * 2;
        } else {
            in_ws = 0;

            want = len * 2;
            if (want < WS_MIN) want = WS_MIN;
        }

        tmp = realloc(ws->s, want);
        if (!tmp) return -1;
        ws->s = tmp;
        ws->p = tmp + in_ws;
        ws->e = tmp + want;
    }

    memcpy(ws->p, data, len);
    ws->p += len;
    return 0;
}

void _hidden_ free_work_string(void *str)
{
    struct work_string *ws;

    ws = str;
    if (ws->s) free(ws->s);
    free(ws);
}
