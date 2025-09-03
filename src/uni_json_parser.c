/*
  parser implementation

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stddef.h>
#include <stdio.h>
#include "compiler.h"

#include "uni_json_p_binding.h"
#include "uni_json_parser.h"
#include "pstate.h"
#include "lib.h"
#include "parser_array.h"
#include "parser_literals.h"
#include "parser_number.h"
#include "parser_object.h"
#include "parser_string.h"

/*  types */
typedef void *parse_func(struct pstate *, struct uni_json_p_binding *);

/*  prototypes */
static void *whitespace(struct pstate *, struct uni_json_p_binding *);
static void *close_char(struct pstate *, struct uni_json_p_binding *);

void *parse_value(struct pstate *, struct uni_json_p_binding *) _hidden_;

/*  variables */
static parse_func *tok_map[256] = {
    ['\t'] =		whitespace,
    ['\r'] =		whitespace,
    ['\n'] =		whitespace,
    [' '] =		whitespace,

    [']'] =		close_char,
    ['}'] =		close_char,

    ['f'] =		parse_false,
    ['n'] =		parse_null,
    ['t'] =		parse_true,

    ['-'] =		parse_number,
    ['0'] =		parse_number,
    ['1'] =		parse_number,
    ['2'] =		parse_number,
    ['3'] =		parse_number,
    ['4'] =		parse_number,
    ['5'] =		parse_number,
    ['6'] =		parse_number,
    ['7'] =		parse_number,
    ['8'] =		parse_number,
    ['9'] =		parse_number,

    ['"'] =		parse_string,

    ['['] =		parse_array,
    ['{'] =		parse_object
};

static char *ec_msg_map[] = {
    [UJ_E_INV] =	"invalid token start char",
    [UJ_E_NO_VAL] =	"missing value",
    [UJ_E_INV_LIT] =	"invalid literal",
    [UJ_E_GARBAGE] =	"garbage after value",
    [UJ_E_EOS] =	"end of string in object",
    [UJ_E_INV_IN] =	"invalid char in object",
    [UJ_E_ADD] =	"failed to add value to object",
    [UJ_E_LEADZ] =	"leading zero in integer part of number",
    [UJ_E_NO_DGS] =	"no digits in number part",
    [UJ_E_INV_CHAR] =	"illegal char in string",
    [UJ_E_INV_UTF8] =	"illegal UTF-8 sequence",
    [UJ_E_INV_ESC] =	"illegal escape sequence",
    [UJ_E_INV_KEY] =	"object key is no string",
    [UJ_E_NO_KEY] =	"missing key in object"
};

int no_value _hidden_;
unsigned uni_json_max_nesting = -1;

/*  routines */
static void *whitespace(struct pstate *, struct uni_json_p_binding *)
{
    /* dummy routine to make whitespace in tok_map */
    return NULL;
}

static void *close_char(struct pstate *, struct uni_json_p_binding *)
{
    return &no_value;
}

void *parse_value(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    parse_func *parse;
    uint8_t *p, *e;
    void *v;

    p = pstate->p;
    e = pstate->e;
    while (p < e && (parse = tok_map[*p], parse == whitespace))
        ++p;
    if (p == e) return &no_value;

    if (!parse) {
        pstate->err.code = UJ_E_INV;
        pstate->err.pos = p;
        return NULL;
    }

    pstate->p = p;
    v = parse(pstate, binds);
    if (!v) return NULL;

    p = pstate->p;
    while (p < e && (parse = tok_map[*p], parse == whitespace))
           ++p;
    pstate->p = p;

    return v;
}

char *uni_json_ec_2_msg(unsigned ec)
{
    if (ec < sizeof(ec_msg_map) / sizeof(*ec_msg_map))
        return ec_msg_map[ec];
    return "not implemented";
}

void *uni_json_parse(uint8_t *data, size_t len, struct uni_json_p_binding *binds)
{
    struct pstate pstate;
    void *v;

    if (!len) {
        binds->on_error(UJ_E_NO_VAL, 0);
        return NULL;
    }

    pstate.p = data;
    pstate.e = data + len;

    v = parse_value(&pstate, binds);

    if (!v) {
        binds->on_error(pstate.err.code, pstate.err.pos - data);
        return NULL;
    }

    if ((int *)v == &no_value) {
        binds->on_error(UJ_E_NO_VAL, 0);
        return NULL;
    }

    if (pstate.p != pstate.e) {
        free_obj(pstate.last_type, v, binds);
        binds->on_error(UJ_E_GARBAGE, pstate.p - data);
        return NULL;
    }

    return v;
}
