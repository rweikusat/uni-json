/*
  parser implementation

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stddef.h>
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

struct utf8_seq {
    unsigned marker,            /* fixed bit pattern in 1st byte */
        first_val,              /* all value bits in 1st byte */
        ovmask0, ovmask1,       /* bitmasks for detecting overlong encodings */
        v_len;                   /* number of values bytes after 1st */
};

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

enum {
    MIN_LEGAL =		32              /* minimum char code which may appear unescaped in a string */
};

/*  prototypes */
static void *whitespace(struct pstate *, struct uni_json_p_binding *);
static void *close_char(struct pstate *, struct uni_json_p_binding *);

static void *parse_false(struct pstate *, struct uni_json_p_binding *);
static void *parse_null(struct pstate *, struct uni_json_p_binding *);
static void *parse_true(struct pstate *, struct uni_json_p_binding *);

static void *parse_number(struct pstate *, struct uni_json_p_binding *);
static void *parse_string(struct pstate *, struct uni_json_p_binding *);

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
};

/*
  RFC3629
  -------

   Char. number range  |        UTF-8 octet sequence
      (hexadecimal)    |              (binary)
   --------------------+---------------------------------------------
   0000 0000-0000 007F | 0xxxxxxx
   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
static struct utf8_seq utf8_seqs[] = {
    {
        .marker =	0xc0,
        .first_val =	31,
        .ovmask0 =	30,
        .v_len =	1 },
    {
        .marker =	0xe0,
        .first_val =	15,
        .ovmask0 =	15,
        .ovmask1 =	32,
        .v_len =	2 },
    {
        .marker =	0xf0,
        .first_val =	7,
        .ovmask0 =	7,
        .ovmask1 =	48,
        .v_len =	3 },

    {
        .marker =	0 }
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
    [UJ_E_INV_UTF8] =	"illegal UTF-8 sequence"
};

static size_t dtor_ofs[] = {
#define binds_ofs(m) offsetof(struct uni_json_p_binding, m)

    [T_NULL] =		binds_ofs(free_null),
    [T_BOOL] =		binds_ofs(free_bool),
    [T_NUM] =		binds_ofs(free_number),
    [T_STR] =		binds_ofs(free_string),
    [T_ARY] =		binds_ofs(free_array),
    [T_OBJ] =		binds_ofs(free_object)

#undef binds_ofs
};

static int no_value;

/*  routines */
/**  helpers */
static void free_obj(int type, void *obj, struct uni_json_p_binding *binds)
{
    void (**pdtor)(void *);
    void (*dtor)(void *);

    pdtor = (void *)((uint8_t *)binds + dtor_ofs[type]);
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

static int skip_digits(struct pstate *pstate)
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

    pstate->p = p;
    return 0;
}

static void *parse_number(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    uint8_t *s, *dig_0;
    unsigned flags;
    int rc;

    dig_0 = s = pstate->p;
    flags = UJ_NF_INT;

    if (*pstate->p == '-') {
        ++pstate->p;
        ++dig_0;
        flags |= UJ_NF_NEG;
    }

    rc = skip_digits(pstate);
    if (rc == -1) return NULL;
    if (*dig_0 == '0' && pstate->p - dig_0 > 1) {
        pstate->err.code = UJ_E_LEADZ;
        pstate->err.pos = dig_0;
        return NULL;
    }

    if (pstate->p < pstate->e) {
        if (*pstate->p == '.') {
            ++pstate->p;
            flags &= ~UJ_NF_INT;

            rc = skip_digits(pstate);
            if (rc == -1) return NULL;
            if (pstate->p == pstate->e) goto done;
        }

        switch (*pstate->p) {
        case 'e':
        case 'E':
            flags &= ~ UJ_NF_INT;

            ++pstate->p;
            if (pstate->p == pstate->e) {
                pstate->err.code = UJ_E_EOS;
                pstate->err.pos = pstate->p - 1;
                return NULL;
            }

            switch (*pstate->p) {
            case '+':
            case '-':
                ++pstate->p;
            }

            rc = skip_digits(pstate);
            if (rc == -1) return NULL;
        }
    }

done:
    pstate->last_type = T_NUM;
    return binds->make_number(s, pstate->p - s, flags);
}

static uint8_t *skip_utf8(uint8_t *p, uint8_t *e)
{
    struct utf8_seq *sp;
    unsigned c, marker, maybe_long;

    c = *p;
    sp = utf8_seqs;
    marker = sp->marker;
    do {
        /*
          The first byte of a UTF-8 sequence is bit-wise either

          110xxxxx
          1110xxxx
          11110xxx

          with x denoting a value bit. Hence, when testing from
          shortest to longest, masking out the current set of value
          bits will only result in the current marker value if the
          actual marker is the current marker.
         */
        if ((c & ~sp->first_val) == marker) break;
        ++sp;
    } while (marker = sp->marker, marker);
    if (!marker) return NULL;

    /*
      An UTF-8 sequence is said to be overlong if it uses a
      representation with more value bits than would be needed to
      encode the actual value. Valid sequences have either 7, 11, 16
      or 21 value bits. This means a 2-byte sequences is overlong if
      the highest 4 value bits are all clear and 3- and 4-byte
      sequences if the highest 5 value bits are all clear.

      Generally. this means it's overlong if all ovmask0 bits in the
      first byte are clear and all ovmask1 bits in the second byte,
      too.
    */
    maybe_long = (*p & sp->ovmask0) == 0;

    ++p;
    if (p == e) return NULL;
    if ((*p & 0xc0) != 0x80) return NULL;
    if (maybe_long
        /*
          Redundant for 2-byte sequences but it won't affect the
          result and avoids a special-case.
        */
        && (*p & sp->ovmask1) == 0) return NULL;

    switch (sp->v_len) {
    case 3:
        ++p;
        if (p == e) return NULL;
        if ((*p & 0xc0) != 0x80) return NULL;

    case 2:
        ++p;
        if (p == e) return NULL;
        if ((*p & 0xc0) != 0x80) return NULL;
    }

    return p + 1;
}

static int parse_string_content(struct pstate *pstate, struct uni_json_p_binding *binds,
                                void *str)
{
    uint8_t *p, *pp, *e, *s;
    unsigned c;
    int rc;

    s = p = pstate->p;
    e = pstate->e;
    c = 0;

    while (p < e && (c = *p, c != '"')) {
        if (c == '\\') {
            pstate->err.code = -1;
            pstate->err.pos = p;
            return -1;
        }

        if (c < MIN_LEGAL) {
            pstate->err.code = UJ_E_INV_CHAR;
            pstate->err.pos = p;
            return -1;
        }

        if (c & 0x80) {
            pp = skip_utf8(p, e);
            if (!pp) {
                pstate->err.code = UJ_E_INV_UTF8;
                pstate->err.pos = p;
                return -1;
            }

            p = pp;
        } else
            ++p;
    }

    if (c != '"') {
        pstate->err.code = UJ_E_EOS;
        pstate->err.pos = p;
        return -1;
    }

    rc = binds->add_2_string(s, p - s, str);
    if (!rc) return -1;

    pstate->p = p + 1;
    return 0;
}

static void *parse_string(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    void *str;
    int rc;

    str = binds->make_string();

    ++pstate->p;
    rc = parse_string_content(pstate, binds, str);
    if (rc == -1) {
        binds->free_string(str);
        return NULL;
    }

    pstate->last_type = T_STR;
    return str;
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
            if (c == -1) return -1;

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
