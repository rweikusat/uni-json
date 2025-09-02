/*
  parse arrays

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_array_h
#define uni_json_parser_array_h

/*  include */
#include "compiler.h"

/*  types */
struct pstate;
struct uni_json_p_binding;

/*  routines */
void *parse_array(struct pstate *pstate, struct uni_json_p_binding *binds) _hidden_;

#endif
