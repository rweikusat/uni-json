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
    void (*last_free)(void *);

    struct {
        unsigned code;
        uint8_t *pos;
    } err;
};

typedef void *parse_func(struct pstate *, struct uni_json_p_binding *);

/*  prototypes */
static void *whitespace(struct pstate *, struct uni_json_p_binding *);
static void *terminator(struct pstate *, struct uni_json_p_binding *);

static void *parse_false(struct pstate *, struct uni_json_p_binding *);
static void *parse_null(struct pstate *, struct uni_json_p_binding *);
static void *parse_true(struct pstate *, struct uni_json_p_binding *);

static void *parse_array(struct pstate *, struct uni_json_p_binding *);

static void *parse_value(struct pstate *, struct uni_json_p_binding *);

/*  variables */
static parse_func *tok_map[256] = {
    ['\t'] =		whitespace,
    ['\r'] =		whitespace,
    ['\n'] =		whitespace,
    [' '] =		whitespace,

    [']'] =		terminator,

    ['f'] =		parse_false,
    ['n'] =		parse_null,
    ['t'] =		parse_true,
    ['['] =		parse_array
};

static char *ec_msg_map[] = {
    [UJ_E_INV] =	"invalid token start char",
    [UJ_E_NO_VAL] =	"missing value",
    [UJ_E_INV_LIT] =	"invalid literal",
    [UJ_E_GARBAGE] =	"garbage after value",
    [UJ_E_EOS] =	"end of string in object",
    [UJ_E_INV_IN] =	"invalid char in object",
    [UJ_E_ADD] =	"failed to add value to object"
};

static int no_value;

/*  routines */
/**  helpers */
static int skip_literal(struct pstate *pstate, uint8_t *want)
{
    uint8_t *p, *e;
    unsigned c;

    p = pstate->p;
    e = pstate->e;

    while ((c = *want, c) && p < e && c == *p) {
        ++p;
        ++want;
    }

    if (c) {
        pstate->err.code = UJ_E_INV_LIT;
        pstate->err.pos = pstate->p;
        return -1;
    }

    pstate->p = p;
    return 0;
}

/**  parser routines */
static void *whitespace(struct pstate *, struct uni_json_p_binding *)
{
    /* dummy function used to mark whitespace chars in tok_map */
    return NULL;
}

static void *terminator(struct pstate *, struct uni_json_p_binding *)
{
    /* dummy function used to mark terminator chars in tok_map */
    return NULL;
}

static void *parse_false(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "false");
    if (rc == -1) return NULL;

    pstate->last_free = binds->free_bool;
    return binds->make_bool(0);
}

static void *parse_null(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "null");
    if (rc == -1) return NULL;

    pstate->last_free = binds->free_null;
    return binds->make_null();
}

static void *parse_true(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "true");
    if (rc == -1) return NULL;

    pstate->last_free = binds->free_bool;
    return binds->make_bool(1);
}

static void *parse_array(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    void *ary, *v;
    uint8_t *p;
    unsigned c;
    int rc;

    ary = binds->make_array();

    ++pstate->p;
    v = parse_value(pstate, binds);
    if (!v) goto err;

    if ((int *)v == &no_value) {
        p = pstate->p;
        if (p == pstate->e) {
            pstate->err.code = UJ_E_EOS;
            pstate->err.pos = p;
            goto err;
            }

        if (*p != ']') {
            pstate->err.code = UJ_E_INV_IN;
            pstate->err.pos = pstate->p;
            goto err;
        }

        pstate->p = p + 1;
    } else
        do {
            rc = binds->add_2_array(v, ary);
            if (!rc) {
                if (pstate->last_free) pstate->last_free(v);

                pstate->err.code = UJ_E_ADD;
                pstate->err.pos = pstate->p;
                goto err;
            }

            p = pstate->p;
            if (p == pstate->e) {
                pstate->err.code = UJ_E_EOS;
                pstate->err.pos = p;
                goto err;
            }

            c = *p;
            if (!(c == ',' || c == ']')) {
                pstate->err.code = UJ_E_INV_IN;
                pstate->err.pos = p;
                goto err;
            }

            pstate->p = p + 1;

            if (c == ',') {
                v = parse_value(pstate, binds);
                if (!v) goto err;

                if ((int *)v == &no_value) {
                    pstate->err.code = UJ_E_NO_VAL;
                    pstate->err.pos = pstate->p;
                    goto err;
                }
            }
        } while (c == ',');

    pstate->last_free = binds->free_array;
    return ary;

err:
    binds->free_array(ary);
    return NULL;
}

static void *parse_value(struct pstate *pstate, struct uni_json_p_binding *binds)
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
    if (parse == terminator) return &no_value;

    pstate->p = p;
    v = parse(pstate, binds);
    if (!v) return NULL;

    p = pstate->p;
    while (p < e && (parse = tok_map[*p], parse == whitespace))
           ++p;
    pstate->p = p;

    return v;
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
    struct pstate pstate;
    void *v;

    if (!len) {
        binds->on_error(UJ_E_NO_VAL, 0);
        return NULL;
    }

    pstate.s = pstate.p = data;
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
        if (pstate.last_free) pstate.last_free(v);
        binds->on_error(UJ_E_GARBAGE, pstate.p - data);
        return NULL;
    }

    return v;
}
