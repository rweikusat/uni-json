/*
  serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/

/*  includes */
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uni_json_types.h"
#include "uni_json_s_binding.h"
#include "uni_json_serializer.h"

/*  types */
typedef void serialize_func(void *val, void *sink, struct uni_json_s_binding *binds,
                            unsigned level, int fmt);

struct kvp_heap {
    struct uj_kv_pair *h;
    size_t last;
};

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
    [UJ_T_OBJ] =	ser_object,
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

static void ser_string_data(uint8_t *s, size_t len, void *sink,
                            struct uni_json_s_binding *binds)
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
    ser_string_data(data.s, data.len, sink, binds);
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

    ser_string_data(kvp.key.s, kvp.key.len, sink, binds);
    outp = binds->output;
    outp(":", 1, sink);
    ser_value(kvp.val, sink, binds, 0, UJ_FMT_FAST);

    while (next_kvp(oiter, &kvp)) {
        outp(",", 1, sink);

        ser_string_data(kvp.key.s, kvp.key.len, sink, binds);
        outp(":", 1, sink);
        ser_value(kvp.val, sink, binds, 0, UJ_FMT_FAST);
    }
}

int key_cmp(struct uj_kv_pair const *kvp0, struct uj_kv_pair *kvp1)
{
    size_t kl0, kl1, cmp_len, ndx;
    int rc;

    kl0 = kvp0->key.len;
    kl1 = kvp1->key.len;
    cmp_len = kl0 < kl1 ? kl0 : kl1;

    ndx = 0;
    while (ndx < cmp_len) {
        rc = kvp0->key.s[ndx] - kvp1->key.s[ndx];
        if (rc) return rc;

        ++ndx;
    }

    if (kl0 == kl1) return 0;
    return kl0 < kl1 ? -1 : 1;
}

static void build_kvph(void *oiter,
                       typeof ((struct uni_json_s_binding){0}.next_kv_pair) next_kv_pair,
                       size_t max_kvps,
                       struct kvp_heap *kvph)
{
    struct uj_kv_pair *kvps, kvp;
    size_t last, at, pre;

    kvps = kvph->h = malloc(sizeof(*kvps) * (max_kvps + 1));
    if (!next_kv_pair(oiter, kvps + 1)) {
        free(kvps);
        return;
    }

    last = 1;
    while (next_kv_pair(oiter, &kvp)) {
        at = ++last;

        do {
            pre = at / 2;
            if (key_cmp(kvps + pre, &kvp) <= 0) break;

            kvps[at] = kvps[pre];
            at = pre;
        } while (at > 1);

        kvps[at] = kvp;
    }

    kvph->last = last;
}

static void rm_kvph_root(struct kvp_heap *kvph)
{
    size_t at, next, r_next, last;
    struct uj_kv_pair *kvps;

    kvps = kvph->h;
    last = kvph->last;
    at = 1;
    while (next = at * 2, next < last) {
        r_next = next + 1;
        if (r_next  < last && key_cmp(kvps + next, kvps + r_next) > 0)
            next = r_next;
        if (key_cmp(kvps + next, kvps + last) >= 0) break;

        kvps[at] = kvps[next];
        at = next;
    }

    kvps[at] = kvps[last];
    kvph->last = last - 1;
}

static void ser_object_det(void *oiter, size_t max_kvps, void *sink,
                           struct uni_json_s_binding *binds,
                           unsigned level, int fmt)
{
    typeof (binds->output) outp;
    struct kvp_heap kvph;
    uint8_t *kv_sep, *kvp_sep;
    size_t kv_sep_len, kvp_sep_len;

    build_kvph(oiter, binds->next_kv_pair, max_kvps,
               &kvph);
    if (!kvph.last) return;

    outp = binds->output;
    if (fmt == UJ_FMT_PRETTY) {
        kv_sep = " : ";
        kv_sep_len = 3;

        kvp_sep = alloca(level + 2);
        *kvp_sep = ',';
        kvp_sep[1] = '\n';
        kvp_sep_len = 2;
        do kvp_sep[kvp_sep_len] = '\t'; while (++kvp_sep_len < level + 2);
        outp(kvp_sep + 1, kvp_sep_len -1, sink);
    } else {
        kv_sep = ":";
        kv_sep_len = 1;

        kvp_sep = ",";
        kvp_sep_len = 1;

        outp(",", 1, sink);
    }

    ser_string_data(kvph.h[1].key.s, kvph.h[1].key.len, sink, binds);
    outp(kv_sep, kv_sep_len, sink);
    ser_value(kvph.h[1].val, sink, binds, level, fmt);

    while (rm_kvph_root(&kvph), kvph.last) {
        outp(kvp_sep, kvp_sep_len, sink);

        ser_string_data(kvph.h[1].key.s, kvph.h[1].key.len, sink, binds);
        outp(kv_sep, kv_sep_len, sink);
        ser_value(kvph.h[1].val, sink, binds, level, fmt);
    }

    free(kvph.h);
}

static void ser_object(void *val, void *sink, struct uni_json_s_binding *binds,
                       unsigned level, int fmt)
{
    void *oiter;
    size_t max_kvps;

    max_kvps = binds->max_kv_pairs(val);

    binds->output("{", 1, sink);
    oiter = binds->start_object_traversal(val);

    switch (fmt) {
    case UJ_FMT_FAST:
        ser_object_fast(oiter, sink, binds);
        break;

    case UJ_FMT_DET:
    case UJ_FMT_PRETTY:
        if (max_kvps)
            ser_object_det(oiter, max_kvps, sink, binds, level + 1, fmt);
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
