/*
  parser implementation

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stdio.h>

#include "uni_json_p_binding.h"
#include "uni_json_parser.h"

/*  types */
struct pstate {
    uint8_t *s, *p, *e;

    struct {
        unsigned code;
        uint8_t *pos;
    } err;
};

typedef void *parse_func(struct pstate *, struct uni_json_p_binding *);

/*  prototypes */
static void *whitespace(struct pstate *, struct uni_json_p_binding *);

/*  variables */
static parse_func *tok_map[256] = {
    ['\t'] =		whitespace,
    ['\r'] =		whitespace,
    ['\n'] =		whitespace,
    [' '] =		whitespace
};

static char *ec_msg_map[] = {
    [UJ_E_INV] =	"invalid token start char"
};

/*  routines */
/**  parser routines */
static void *whitespace(struct pstate *, struct uni_json_p_binding *)
{
    return NULL;
}

/**  interface */
char *uni_json_ec_2_msg(unsigned ec)
{
    if (ec < sizeof(ec_msg_map) / sizeof(*ec_msg_map))
        return ec_msg_map[ec];
    return NULL;
}

void *uni_json_parse(uint8_t *data, size_t len, struct uni_json_p_binding *binds)
{
    fputs("I won't parse!\n", stderr);
    return NULL;
}
