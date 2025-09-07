/*
  parse strings

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stddef.h>
#include <stdio.h>

#include "uni_json_parser.h"
#include "uni_json_p_binding.h"
#include "pstate.h"
#include "parser_string.h"

/*  constants */
enum {
    MIN_LEGAL =		32              /* minimum char code which may appear unescaped in a string */
};

enum {
    HI,
    LO
};

enum {
    /*
      RFC3629
      -------
      The definition of UTF-8 prohibits encoding character numbers between
      U+D800 and U+DFFF, which are reserved for use with the UTF-16
      encoding form (as surrogate pairs) and do not directly represent
      characters.
    */
    UTF8_SURR_MIN =	0xedad80, /* 0xd800 as 3-byte UTF-8 sequence */
    UTF8_SURR_MAX =	0xedbfbf,  /* ditto for 0xdfff */

    /*
      "non-characters" (0xfffe, 0xffff)
    */
    NON_CHAR_FE =	0xefbfbe,
    NON_CHAR_FF =	0xefbfbf,

    /*
      RFC3629
      -------
      In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16
      accessible range) are encoded using sequences of 1 to 4 octets.
    */
    UTF8_MAX =		0xf48fbfbf
};

enum {
    SURR_FROM =		0xd800,
    SURR_TO =		0xdfff,
    SURR_LO =		0xdc00
};

/*  types */
struct utf8_seq {
    unsigned ovmask0, ovmask1; /* bitmasks for detecting overlong encodings */
};

/*  variables */
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
    [2] = {
        .ovmask0 =	30 },

    [3] = {
        .ovmask0 =	15,
        .ovmask1 =	32 },

    [4] = {
        .ovmask0 =	7,
        .ovmask1 =	48 }
};

static uint8_t escs[256] = {
    ['"'] =		'"',
    ['\\'] =		'\\',
    ['/'] =		'/',
    ['b'] =		'\b',
    ['f'] =		'\f',
    ['n'] =		'\n',
    ['r'] =		'\r',
    ['t'] =		'\t',
    ['u'] =		'u'
};

/*  routines */
static inline int no_val_byte(unsigned c)
{
    return (c & 0xc0) != 0x80;
}

uint8_t *skip_utf8(uint8_t *p, uint8_t *e)
{
    struct utf8_seq *sp;
    unsigned maybe_long, seq_len;
    uint32_t enc_val;

    /*
      Explanation of the expression below:

      A valid UTF-8 sequences starts bit-wise either with

      110xxxxx
      1110xxxx
      11110xxx

      The value of *(int8_t *)p is *p sign-extended to the size of an
      int. The __builtin_clz function returns the number of leading 0
      bits in its operand. If the number of leading 1 bits in byte *p
      is n, the return value of __builtint_clz(~*(int8_t *)p) will
      thus be

      (sizeof(int) - 1) * 8 + n

      which means that *p is a valid start of an UTF-8 sequence if the
      value of

      __builtin_clz(~*(int8_t *)p) - (sizeof(int) - 1) * 8

      is either 2, 3 or 4.
    */
    seq_len = __builtin_clz(~*(int8_t *)p) - (sizeof(int) - 1) * 8;
    if (seq_len < 2 || seq_len > 4) return NULL;
    sp = utf8_seqs + seq_len;
    enc_val = *p << 8;

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
    if (no_val_byte(*p)) return NULL;
    if (maybe_long
        /*
          Redundant for 2-byte sequences but it won't affect the
          result and avoids a special-case.
        */
        && (*p & sp->ovmask1) == 0) return NULL;
    enc_val |= *p;

    switch (seq_len) {
    case 4:
        ++p;
        if (p == e) return NULL;
        if (no_val_byte(*p)) return NULL;
        enc_val = (enc_val << 8) | *p;

    case 3:
        ++p;
        if (p == e) return NULL;
        if (no_val_byte(*p)) return NULL;
        enc_val = (enc_val << 8) | *p;
    }

    switch (seq_len) {
    case 3:
        if (enc_val >= UTF8_SURR_MIN && enc_val <= UTF8_SURR_MAX)
            return NULL;

        if (enc_val == NON_CHAR_FE || enc_val == NON_CHAR_FF)
            return NULL;
        break;

    case 4:
        if (enc_val > UTF8_MAX) return NULL;
    }

    return p + 1;
}

static inline unsigned from_hex(unsigned c)
{
    if (c - '0' < 10) return c - '0';

    c &= ~0x20;                 /* ASCII 'toupper' */
    if (c - 'A' < 6) return c - 'A' + 10;

    return -1;
}

static uint32_t parse_4dg_hex(uint8_t *p, uint8_t *e)
{
#define un1 (unsigned)-1

    uint32_t x;
    unsigned dg;

    if (e - p < 4) return -1;

    dg = from_hex(*p++);
    if (dg == un1) return -1;
    x = dg << 12;

    dg = from_hex(*p++);
    if (dg == un1) return -1;
    x |= dg << 8;

    dg = from_hex(*p++);
    if (dg == un1) return -1;
    x |= dg << 4;

    dg = from_hex(*p);
    if (dg == un1) return -1;
    return x | dg;

#undef un1
}

static uint32_t parse_u_esc(struct pstate *pstate)
{
    uint32_t v0, v1;
    uint8_t *p, *e;

    p = pstate->p;
    e = pstate->e;

    v0 = parse_4dg_hex(p, e);
    if (v0 == (uint32_t)-1) return -1;

    /*
      RFC8529
      ------
      To escape an extended character that is not in the Basic
      Multilingual Plane, the character is represented as a
      12-character sequence, encoding the UTF-16 surrogate pair.
    */

    /*
      A surrogate pair is a number from 0xd800 - 0xdbff (high
      surrogates) paired with a number from 0xdc00 - 0xdfff (low
      surrogates). The lowest 10 bits of the first number are the
      higher ten bits of the character code, the lowest ten bits of
      the second the lower ten bits. 0x1000 needs to be added to this
      value because it's the codepoint of the first extended Unicode
      character.

      Let the first number be a and the second b. The encoded
      character code is then

      0x1000 + ((a & 0x3ff) << 10 | (b & 0x3ff))
    */
    if (v0 >= SURR_FROM && v0 <= SURR_TO) {
        if (v0 >= SURR_LO) return -1;

        p += 4;
        if (e - p < 2 || *p++ != '\\' || *p++ != 'u')
            return -1;

        v1 = parse_4dg_hex(p, e);
        if (v1 == (uint32_t)-1
            || v1 < SURR_LO || v1 > SURR_TO) return -1;
        v0 &= 0x3ff;
        v0 = (v0 << 10) | (v1 & 0x3ff);
        v0 += 0x10000;
    }

    pstate->p = p + 4;
    return v0;
}

unsigned utf8_seq_len(uint32_t c)
{
    if (c < 0x100) return 1;
    if (c < 0x1000) return 2;
    if (c < 0x10000) return 3;
    return 4;
}

unsigned utf8_encode(uint32_t c, uint8_t *p)
{
    unsigned len;

    len = utf8_seq_len(c);
    if (len > 1) {
        switch (len) {
        case 4:
            p[3] = 0x80 | (c & 0x3f);
            c >>= 6;

        case 3:
            p[2] = 0x80 | (c & 0x3f);
            c >>= 6;

        case 2:
            p[1] = 0x80 | (c & 0x3f);
            c >>= 6;
        }

        c |= 0xff << (8 - len);
    }

    *p = c;
    return len;
}

static int parse_esc(struct pstate *pstate, struct uni_json_p_binding *binds,
                     void *str)
{
    uint32_t chr;
    uint8_t utf_buf[4];
    int rc;

    if (pstate->p == pstate->e) return -1;

    chr = escs[*pstate->p++];
    switch (chr) {
    default:
        break;

    case 'u':
        chr = parse_u_esc(pstate);
        if (chr == (uint32_t)-1) return -1;
        if (chr >= UTF8_SURR_MIN && chr <= UTF8_SURR_MAX)
            return -1;
        if (chr == NON_CHAR_FE || chr == NON_CHAR_FF)
            return -1;
        break;

    case 0:
        pstate->err.code = UJ_E_INV_ESC;
        pstate->err.pos = pstate->p;
        return -1;
    }

    rc = utf8_encode(chr, utf_buf);
    rc = binds->add_2_string(utf_buf, rc, str);
    if (!rc) {
        pstate->err.code = UJ_E_ADD;
        pstate->err.pos = pstate->p - 1;
        return -1;
    }

    return 0;
}

static int parse_string_content(struct pstate *pstate, struct uni_json_p_binding *binds,
                                void *str)
{
    uint8_t *p, *pp, *e, *s;
    unsigned c;
    int rc;

    s = p = pstate->p;
    e = pstate->e;

    while (p < e && (c = *p, c != '"')) {
        if (c == '\\') {
            if (p > s) {
                rc = binds->add_2_string(s, p - s, str);
                if (!rc) {
                    pstate->err.code = UJ_E_ADD;
                    pstate->err.pos = p;
                    return -1;
                }
            }

            pstate->p = p + 1;
            rc = parse_esc(pstate, binds, str);
            if (rc == -1) return -1;

            s = p = pstate->p;
            continue;
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

    if (p == e) {
        pstate->err.code = UJ_E_EOS;
        pstate->err.pos = p;
        return -1;
    }

    if (p > s) {
        rc = binds->add_2_string(s, p - s, str);
        if (!rc) {
            pstate->err.code = UJ_E_ADD;
            pstate->err.pos = p;
            return -1;
        }
    }

    pstate->p = p + 1;
    return 0;
}

void *parse_string(struct pstate *pstate, struct uni_json_p_binding *binds)
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
