/*
  parse arrays

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include "uni_json_parser.h"
#include "uni_json_p_binding.h"
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

void *parse_array(struct pstate *pstate, struct uni_json_p_binding *binds)
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
