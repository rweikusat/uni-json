/*
  parse arrays

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include "uni_json_parser.h"
#include "uni_json_p_binding.h"
#include "uni_json_types.h"
#include "pstate.h"
#include "lib.h"
#include "parser_array.h"

/*  extern declarations */
extern int no_value;
void *parse_value(struct pstate *, struct uni_json_p_binding *);

/*  routines */
static int parse_array_content(struct pstate *pstate, struct uni_json_p_binding *binds,
                               void *ary)
{
    void *v;
    int rc;

    v = parse_value(pstate, binds);
    if (!v) return -1;

    if ((int *)v == &no_value) {
        rc = skip_one_of(pstate, "]");
        if (rc == -1) return -1;
    } else
        do {
            rc = binds->add_2_array(v, ary);
            if (!rc) {
                free_obj(pstate->last_type, v, binds);

                pstate->err.code = UJ_E_ADD;
                pstate->err.pos = pstate->p;
                return -1;
            }

            rc = skip_one_of(pstate, ",]");
            if (rc == -1) return -1;

            if (rc == ',') {
                v = parse_value(pstate, binds);
                if (!v) return -1;

                if ((int *)v == &no_value) {
                    pstate->err.code = UJ_E_NO_VAL;
                    pstate->err.pos = pstate->p;
                    return -1;
                }
            }
        } while (rc == ',');

    return 0;
}

void *parse_array(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    void *ary;
    int rc;

    ++pstate->level;
    if (pstate->level > uni_json_max_nesting) {
        pstate->err.code = UJ_E_TOO_DEEP;
        pstate->err.pos = pstate->p;
        return NULL;
    }

    ary = binds->make_array();
    ++pstate->p;

    rc = parse_array_content(pstate, binds, ary);
    if (rc == -1) {
        binds->free_array(ary);
        return NULL;
    }

    pstate->last_type = UJ_T_ARY;
    --pstate->level;
    return ary;
}
