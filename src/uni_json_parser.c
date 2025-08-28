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
    int last_type;

    struct {
        unsigned code;
        uint8_t *pos;
    } err;
};

typedef void *parse_func(struct pstate *, struct uni_json_p_binding *);

/*  constants */
enum {
    T_NULL,
    T_BOOL,
    T_NUM,
    T_STR,
    T_ARY,
    T_OBJ
};

enum {
    INT,
    FRAC,
    EXP
};

/*  prototypes */
static void *whitespace(struct pstate *, struct uni_json_p_binding *);
static void *close_char(struct pstate *, struct uni_json_p_binding *);

static void *parse_false(struct pstate *, struct uni_json_p_binding *);
static void *parse_null(struct pstate *, struct uni_json_p_binding *);
static void *parse_true(struct pstate *, struct uni_json_p_binding *);

static void *parse_number(struct pstate *, struct uni_json_p_binding *);
static void *parse_array(struct pstate *, struct uni_json_p_binding *);

static void *parse_value(struct pstate *, struct uni_json_p_binding *);

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
    ['['] =		parse_array,

    ['-'] =		parse_number,
    ['1'] =		parse_number,
    ['2'] =		parse_number,
    ['3'] =		parse_number,
    ['4'] =		parse_number,
    ['5'] =		parse_number,
    ['6'] =		parse_number,
    ['7'] =		parse_number,
    ['8'] =		parse_number,
    ['9'] =		parse_number,
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
    [UJ_E_NO_DGS] =	"no digits in number part"
};

static int no_value;

/*  routines */
/**  helpers */
static void free_obj(int type, void *obj, struct uni_json_p_binding *binds)
{
    void (**pdtor)(void *);
    void (*dtor)(void *);

    switch (type) {
    case T_NULL:
        pdtor = &binds->free_null;
        break;

    case T_BOOL:
        pdtor = &binds->free_bool;
        break;

    case T_NUM:
        pdtor = &binds->free_number;
        break;

    case T_STR:
        pdtor = &binds->free_string;
        break;

    case T_ARY:
        pdtor = &binds->free_array;
        break;

    case T_OBJ:
        pdtor = &binds->free_object;
    }

    dtor = *pdtor;
    if (dtor) dtor(obj);
}

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

static int have_one_of(struct pstate *pstate, uint8_t *set)
{
    uint8_t *p;
    int c, cs;

    p = pstate->p;
    if (p == pstate->e) {
        pstate->err.code = UJ_E_EOS;
        pstate->err.pos = p;
        return -1;
    }

    c = *p;
    while (cs = *set, cs) {
        if (c == cs) {
            pstate->p = p + 1;
            return c;
        }

        ++set;
    }

    pstate->err.code = UJ_E_INV_IN;
    pstate->err.pos = p;
    return -1;
}

/**  parser routines */
static void *whitespace(struct pstate *, struct uni_json_p_binding *)
{
    /* dummy routine to make whitespace in tok_map */
    return NULL;
}

static void *close_char(struct pstate *, struct uni_json_p_binding *)
{
    return &no_value;
}

static void *parse_false(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "false");
    if (rc == -1) return NULL;

    pstate->last_type = T_BOOL;
    return binds->make_bool(0);
}

static void *parse_null(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "null");
    if (rc == -1) return NULL;

    pstate->last_type = T_NULL;
    return binds->make_null();
}

static void *parse_true(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "true");
    if (rc == -1) return NULL;

    pstate->last_type = T_BOOL;
    return binds->make_bool(1);
}

static int parse_digits(struct pstate *pstate, size_t *len)
{
    uint8_t *s, *p, *e;

    s = p = pstate->p;
    e = pstate->e;
    while (p < e && (unsigned)*p - '0' < 10)
        ++p;

    if (s == p) {
        pstate->err.code = UJ_E_NO_DGS;
        pstate->err.pos = s;
        return -1;
    }

    *len = p - s;
    pstate->p = p;
    return 0;
}

static void *parse_number(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int neg, exp_neg;
    uint8_t *parts[3];
    size_t lens[3];
    int rc;

    parts[FRAC] = parts[EXP] = NULL;
    exp_neg = 0;
    neg = 0;

    if (*pstate->p == '-') {
        neg = 1;
        ++pstate->p;
    }

    parts[INT] = pstate->p;
    rc = parse_digits(pstate, lens + INT);
    if (rc == -1) return NULL;
    if (*parts[INT] == '0') {
        pstate->err.code = UJ_E_LEADZ;
        pstate->err.pos = parts[INT];
        return NULL;
    }

    lens[FRAC] = lens[EXP] = 0;
    if (pstate->p < pstate->e) {
        if (*pstate->p == '.') {
            ++pstate->p;
            parts[FRAC] = pstate->p;
            rc = parse_digits(pstate, lens + FRAC);
            if (rc == -1) return NULL;
            if (pstate->p == pstate->e) goto done;
        }

        switch (*pstate->p) {
        case 'e':
        case 'E':
            ++pstate->p;
            if (pstate->p == pstate->e) {
                pstate->err.code = UJ_E_EOS;
                pstate->err.pos = pstate->p;
                return NULL;
            }

            switch (*pstate->p) {
            case '-':
                exp_neg = 1;

            case '+':
                ++pstate->p;
            }

            parts[EXP] = pstate->p;
            rc = parse_digits(pstate, lens + EXP);
            if (rc == -1) return NULL;
        }
    }

done:
    return binds->make_number(neg, parts[INT], lens[INT], parts[FRAC], lens[FRAC],
                              exp_neg, parts[EXP], lens[EXP]);
}

static int parse_array_content(struct pstate *pstate, struct uni_json_p_binding *binds,
                               void *ary)
{
    void *v;
    int c, rc;

    v = parse_value(pstate, binds);
    if (!v) return -1;

    if ((int *)v == &no_value) {
        c = have_one_of(pstate, "]");
        if (c == -1) return -1;
    } else
        do {
            rc = binds->add_2_array(v, ary);
            if (!rc) {
                free_obj(pstate->last_type, v, binds);

                pstate->err.code = UJ_E_ADD;
                pstate->err.pos = pstate->p;
                return -1;
            }

            c = have_one_of(pstate, ",]");
            if (c == -1) {
                free_obj(pstate->last_type, v, binds);
                return -1;
            }

            if (c == ',') {
                v = parse_value(pstate, binds);
                if (!v) return -1;

                if ((int *)v == &no_value) {
                    pstate->err.code = UJ_E_NO_VAL;
                    pstate->err.pos = pstate->p;
                    return -1;
                }
            }
        } while (c == ',');

    return 0;
}

static void *parse_array(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    void *ary;
    int rc;

    ary = binds->make_array();
    ++pstate->p;

    rc = parse_array_content(pstate, binds, ary);
    if (rc == -1) {
        binds->free_array(ary);
        return NULL;
    }

    pstate->last_type = T_ARY;
    return ary;
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
        free_obj(pstate.last_type, v, binds);
        binds->on_error(UJ_E_GARBAGE, pstate.p - data);
        return NULL;
    }

    return v;
}
