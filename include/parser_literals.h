/*
  literal parsing functions

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_literals_h
#define uni_json_parser_literals_h

/*  includes */
#include "compiler.h"

/*  types */
struct uni_json_p_binding;
struct pstate;

/*  routines */
void *parse_false(struct pstate *pstate, struct uni_json_p_binding *binds) _hidden_;
void *parse_null(struct pstate *pstate, struct uni_json_p_binding *binds) _hidden_;
void *parse_true(struct pstate *pstate, struct uni_json_p_binding *binds) _hidden_;

#endif
