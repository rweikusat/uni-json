/*
  parser

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_h
#define uni_json_parser_h

/*  includes */
#include <stdint.h>

/*  constants */
enum {
    UJ_E_INV,                    /* invalid token start char */
    UJ_E_NO_VAL                  /* missing value */
};

/*   types */
struct uni_json_p_binding;

/*  routines */
char *uni_json_ec_2_msg(unsigned);
void *uni_json_parse(uint8_t *data, size_t len, struct uni_json_p_binding *bind);

#endif
