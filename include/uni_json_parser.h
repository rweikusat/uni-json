/*
  parser

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_h
#define uni_json_parser_h

/*  includes */
#include <stddef.h>

/*   types */
struct uni_json_p_binding;

/*  routines */
void *uni_json_parse(uint8_t *data, size_t len, struct uni_json_p_binding *bind);

#endif
