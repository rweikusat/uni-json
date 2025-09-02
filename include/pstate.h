/*
  parser state structure definition

    Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_pstate_h
#define uni_json_pstate_h

/*  includes */
#include <inttypes.h>

/*  constants */
enum {
    T_NULL,
    T_BOOL,
    T_NUM,
    T_STR,
    T_ARY,
    T_OBJ
};

/*  types */
struct pstate {
    uint8_t *p, *e;
    int last_type;

    struct {
        unsigned code;
        uint8_t *pos;
    } err;
};

#endif
