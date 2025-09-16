/*
  serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <alloca.h>
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

static void ser_string(void *, void *, struct uni_json_s_binding *,
                       unsigned, int);

static void ser_array(void *, void *, struct uni_json_s_binding *,
                      unsigned, int);

static void ser_object(void *, void *, struct uni_json_s_binding *,
                       unsigned, int);

static void ser_value(void *, void *, struct uni_json_s_binding *,
                            unsigned, int);

/*  variables */
static serialize_func *serers[] = {
    [UJ_T_NULL] =	ser_null,
    [UJ_T_BOOL] =	ser_bool,
    [UJ_T_NUM] =	ser_number,
    [UJ_T_STR] =	ser_string,
    [UJ_T_ARY] =	ser_array,
    [UJ_T_IBJ] =	ser_object,
    [UJ_T_UNK] =	ser_null
};

static uint8_t escs[][8] = {
    "\\u0000",
    "\\u0001",
    "\\u0002",
    "\\u0003",
    "\\u0004",
    "\\u0005",
    "\\u0006",
    "\\u0007",
    "\\b",
    "\\t",
    "\\n",
    "\\u000b",
    "\\f",
    "\\r",
    "\\u000e",
    "\\u000f",
    "\\u0010",
    "\\u0011",
    "\\u0012",
    "\\u0013",
    "\\u0014",
    "\\u0015",
    "\\u0016",
    "\\u0017",
    "\\u0018",
    "\\u0019",
    "\\u001a",
    "\\u001b",
    "\\u001c",
    "\\u001d",
    "\\u001e",
    "\\u001f",

    ['"'] = "\\\"",
    ['\\'] = "\\\\"
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
    struct uj_data data;

    binds->get_num_data(val, &data);
    binds->output(data.s, data.len, sink);
    if (binds->free_num_data) binds->free_num_data(&data);
}

static void ser_string_data(uint8_t *s, size_t len, void *sink)
{
    typeof (binds->output) outp;
    uint8_t *p, *e, *esc;
    unsigned c;

    outp = binds->output;
    outp("\"", 1, sink);

    p = s;
    e = p + len;
    while (p < e) {
        c = *p;

        if (c < 32 || c == '"' || c == '\\') {
            if (p > s) outp(s, p - s, sink);

            esc = escs[c];
            len = esc[1] == 'u' ? 6 : 2;
            outp(esc, len, sink);

            s = p + 1;
        }

        ++p;
    }

    if (p > s) outp(s, p - s, sink);
    outp("\"", 1, sink);
}

static void ser_string(void *val, void *sink, struct uni_json_s_binding *binds,
                       unsigned, int)
{
    struct uj_data data;

    binds->get_string_data(val, &data);
    ser_string_data(data.s, data.len, sink);
    if (binds->free_string_data) binds->free_string_data(&data);
}

static void ser_array(void *ary, void *sink, struct uni_json_s_binding *binds,
                      unsigned level, int fmt)
{
    uint8_t *sep;
    unsigned sep_len;
    void *aiter, *v;
    typeof (binds->output) outp;
    typeof (binds->next_value) next_val;

    ++level;
    outp = binds->output;
    aiter = binds->start_array_traversal(ary);

    outp("[", 1, sink);

    if (fmt == UJ_FMT_PRETTY) {
        sep = alloca(level + 2);
        *sep = ',';
        sep[1] = '\n';
        sep_len = 2;
        do sep[sep_len] = '\t'; while (++sep_len < level + 2);

        outp(sep + 1, sep_len - 1, sink);
    } else {
        sep = ",";
        sep_len = 1;
    }

    next_val = binds->next_value;
    v = next_val(aiter);
    if (v) {
        ser_value(v, sink, binds, level, fmt);

        while (v = next_val(aiter), v) {
            outp(sep, sep_len, sink);
            ser_value(v, sink, binds, level, fmt);
        }
    }

    if (binds->end_array_traversal) binds->end_array_traversal(aiter);
    outp("]", 1, sink);
}

static void ser_object_fast(void *oiter, void *sink, struct uni_json_s_binding *binds)
{
    typeof (binds->output) outp;
    typeof (binds->next_kv_pair) next_kvp;
    struct uj_kv_pair kvp;

    next_kvp = binds->next_kv_pair;
    if (!next_kvp(oiter, &kvp)) return;

    ser_string_data(kvp.key.s, kvp.key.len, sink);
    outp = binds->output;
    outp(":", 1, sink);
    ser_value(kvp.val, sink, binds, 0, Uj_FMT_FAST);

    while (next_kvp(oiter, &kvp)) {
        outp(",", 1, sink);

        ser_string_data(kvp.key.s, kvp.key.len, sink);
        outp(":", 1, sink);
        ser_value(kvp.val, sink, binds, 0, Uj_FMT_FAST);
    }
}

static void ser_object(void *val, void *sink, struct uni_json_s_binding *binds,
                       unsigned level, int fmt)
{
    void *oiter;

    binds->output("{", 1, sink);
    oiter = binds->start_object_traversal(val);

    switch (fmt) {
    case UJ_FMT_FAST:
        ser_object_fast(oiter, sink, binds);
    }

    if (binds->end_object_traversal)
        binds->end_object_traversal(oiter);
    binds->output("}", 1, sink);
}

static void ser_value(void *val, void *sink, struct uni_json_s_binding *binds,
                            unsigned level, int fmt)
{
    serers[binds->type_of(val)](val, sink, binds, level, fmt);
}

void uni_json_serialize(void *val, void *sink, struct uni_json_s_binding *binds,
                        int fmt)
{
    ser_value(val, sink, binds, 0, fmt);
}
