/*
  utility functions

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_lib_h
#define uni_json_lib_h

/*  includes */
#include <inttypes.h>
#include "compiler.h"

/*  types */
struct pstate;
struct uni_json_p_binding;

/*  routines */
void free_obj(int type, void *obj, struct uni_json_p_binding *binds) _hidden_;
int have_one_of(struct pstate *pstate, uint8_t *set) _hidden_;

#endif
