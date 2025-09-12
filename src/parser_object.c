/*
  parse objects

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include "uni_json_parser.h"
#include "uni_json_p_binding.h"
#include "uni_json_types.h"
#include "pstate.h"
#include "lib.h"
#include "parser_object.h"

/*  extern declarations */
extern int no_value;
void *parse_value(struct pstate *, struct uni_json_p_binding *);

/*  routines */
static int parse_object_content(struct pstate *pstate, struct uni_json_p_binding *binds,
                                void *obj)
{
    void *k, *v;
    uint8_t *pos;
    int c, rc;

    pos = pstate->p;
    k = parse_value(pstate, binds);
    if (!k) return -1;

    if ((int *)k == &no_value) {
        c = skip_one_of(pstate, "}");
        if (c == -1) return -1;
    } else {
        do {
            if (pstate->last_type != UJ_T_STR) {
                free_obj(pstate->last_type, k, binds);

                pstate->err.code = UJ_E_INV_KEY;
                pstate->err.pos = pos;
                return -1;
            }

            c = skip_one_of(pstate, ":");
            if (c == -1) {
                binds->free_string(k);
                return -1;
            }

            v = parse_value(pstate, binds);
            if (!v || (int *)v == &no_value) {
                binds->free_string(k);

                if ((int *)v == &no_value) {
                    pstate->err.code = UJ_E_NO_VAL;
                    pstate->err.pos = pstate->p;
                }

                return -1;
            }

            rc = binds->add_2_object(k, v, obj);
            if (!rc) {
                binds->free_string(k);
                free_obj(pstate->last_type, v, binds);

                pstate->err.code = UJ_E_ADD;
                pstate->err.pos = pstate->p;
                return -1;
            }

            c = skip_one_of(pstate, ",}");
            if (c == -1) return -1;

            if (c == ',') {
                pos = pstate->p;
                k = parse_value(pstate, binds);
                if (!k) return -1;

                if ((int *)k == &no_value) {
                    pstate->err.code = UJ_E_NO_KEY;
                    pstate->err.pos = pstate->p;
                    return -1;
                }
            }
        } while (c == ',');
    }

    return 0;
}

void *parse_object(struct pstate *pstate, struct uni_json_p_binding *binds)
{
    void *obj;
    int rc;

    ++pstate->level;
    if (pstate->level > uni_json_max_nesting) {
        pstate->err.code = UJ_E_TOO_DEEP;
        pstate->err.pos = pstate->p;
        return NULL;
    }

    obj = binds->make_object();
    ++pstate->p;

    rc = parse_object_content(pstate, binds, obj);
    if (rc == -1) {
        binds->free_array(obj);
        return NULL;
    }

    pstate->last_type = UJ_T_OBJ;
    --pstate->level;
    return obj;
}
