/*
  parser state structure definition

    Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_pstate_h
#define uni_json_pstate_h

/*  includes */
#include <inttypes.h>

/*  types */
struct pstate {
    uint8_t *p, *e;
    int last_type;
    unsigned level;

    struct {
        unsigned code;
        uint8_t *pos;
    } err;
};

#endif
