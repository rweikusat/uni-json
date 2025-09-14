/*
  serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <stdio.h>

#include "uni_json_types.h"
#include "uni_json_s_binding.h"
#include "uni_json_serializer.h"

/*  types */
typedef void serialize_func(void *val, void *sink, struct uni_json_s_binding *binds,
                            unsigned level, int fmt);

/*  prototypes */
static void ser_null(void *, void *, struct uni_json_s_binding *,
                     unsigned, int);

static void ser_bool(void *, void *, struct uni_json_s_binding *,
                     unsigned, int);

static void ser_number(void *, void *, struct uni_json_s_binding *,
                       unsigned, int);

/*  variables */
static serialize_func *serers[] = {
    [UJ_T_NULL] =	ser_null,
    [UJ_T_BOOL] =	ser_bool,
    [UJ_T_NUM] =	ser_number,
    [UJ_T_UNK] =	ser_null
};

/*  routines */
static void ser_null(void *, void *sink, struct uni_json_s_binding *binds,
                     unsigned, int)
{
    binds->output("null", 4, sink);
}

static void ser_bool(void *val, void *sink, struct uni_json_s_binding *binds,
                     unsigned, int)
{
    char *vs;
    size_t len;
    int v;

    v = binds->get_bool_value(val);
    if (v) {
        vs = "true";
        len = 4;
    } else {
        vs = "false";
        len = 5;
    }

    binds->output(vs, len, sink);
}

static void ser_number(void *val, void *sink, struct uni_json_s_binding *binds,
                       unsigned, int)
{
    struct data data;

    binds->get_num_data(val, &data);
    binds->output(data.s, data.len, sink);
    if (binds->free_num_data) binds->free_num_data(&data);
}

static void serialize_value(void *val, void *sink, struct uni_json_s_binding *binds,
                            unsigned level, int fmt)
{
    serers[binds->type_of(val)](val, sink, binds, level, fmt);
}

void uni_json_serialize(void *val, void *sink, struct uni_json_s_binding *binds,
                        int fmt)
{
    serialize_value(val, sink, binds, 0, fmt);
}
