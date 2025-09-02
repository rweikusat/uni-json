/*
  literal parsing functions

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include "pstate.h"
#include "uni_json_p_binding.h"
#include "uni_json_parser.h"

/*  routines */
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

void *parse_false(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "false");
    if (rc == -1) return NULL;

    pstate->last_type = T_BOOL;
    return binds->make_bool(0);
}

void *parse_null(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "null");
    if (rc == -1) return NULL;

    pstate->last_type = T_NULL;
    return binds->make_null();
}

void *parse_true(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    int rc;

    rc = skip_literal(pstate, "true");
    if (rc == -1) return NULL;

    pstate->last_type = T_BOOL;
    return binds->make_bool(1);
}
