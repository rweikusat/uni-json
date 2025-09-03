/*
  parse objects

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_object_h
#define uni_json_parser_object_h

/*  includes */
#include "compiler.h"

/*  types */
struct pstate;
struct uni_json_p_binding;

/*  routines */
void *parse_object(struct pstate *pstate, struct uni_json_p_binding *binds);

#endif
