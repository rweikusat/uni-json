/*
  utility functions

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include "uni_json_p_binding.h"
#include "uni_json_parser.h"
#include "pstate.h"
#include "lib.h"

/*  variables */
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

/*  routines */
void free_obj(int type, void *obj, struct uni_json_p_binding *binds)
{
    void (**pdtor)(void *);
    void (*dtor)(void *);

    pdtor = (void *)((uint8_t *)binds + dtor_ofs[type]);
    dtor = *pdtor;
    if (dtor) dtor(obj);
}


int skip_one_of(struct pstate *pstate, uint8_t *set)
{
    /*
      Consume a character provided it's in 'set'.

      set	null-terminated string of characters to look for

      If there was a character at the current position and it was in
      set, it's returned and the position advanced. Otherwise, returns
      -1 and sets an error state.
    */
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
