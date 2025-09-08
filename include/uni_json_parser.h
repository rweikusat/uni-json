/*
  parser

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_parser_h
#define uni_json_parser_h

/*  includes */
#include <stdint.h>
#include <stddef.h>

/*  constants */
enum {
    UJ_E_INV,                    /* invalid token start char */
    UJ_E_NO_VAL,                 /* missing value */
    UJ_E_INV_LIT,                /* invalid literal */
    UJ_E_GARBAGE,                /* garbage after value */
    UJ_E_EOS,                    /* end of string in value */
    UJ_E_INV_IN,                 /* invalid char in value */
    UJ_E_ADD,                    /* failed to add value */
    UJ_E_LEADZ,                  /* leading zero in int part of number */
    UJ_E_NO_DGS,                 /* no digits in number part */
    UJ_E_INV_CHAR,               /* illegal char in string */
    UJ_E_INV_UTF8,               /* illegal UTF-8 sequence */
    UJ_E_INV_ESC,                /* illegal escape sequence */
    UJ_E_INV_KEY,                /* object key is no string */
    UJ_E_NO_KEY,                 /* missing key in object */
    UJ_E_TOO_DEEP                /* too many levels of nesting */
};

/*   types */
struct uni_json_p_binding;

/*  variables */
extern unsigned uni_json_max_nesting;

/*  routines */
char *uni_json_ec_2_msg(unsigned ec);
void *uni_json_parse(uint8_t *data, size_t len, struct uni_json_p_binding *binds);

#endif
