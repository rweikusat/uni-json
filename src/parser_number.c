/*
  parse numbers

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stddef.h>

#include "uni_json_parser.h"
#include "uni_json_p_binding.h"
#include "pstate.h"
#include "parser_number.h"

/*  routines */
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

void *parse_number(struct pstate *pstate, struct uni_json_p_binding *binds)
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
