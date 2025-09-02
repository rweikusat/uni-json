/*
  parse strings

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_string_h
#define uni_json_parser_string_h

/*  includes */
#include "compiler.h"

/*  types */
struct pstate;
struct uni_json_p_binding;

/*  routines */
void *parse_string(struct pstate *pstate, struct uni_json_p_binding *binds) _hidden_;

#endif
