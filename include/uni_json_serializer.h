/*
  serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_serializer_h
#define uni_json_serilaizer_h

/*  includes */
#include <stdint.h>
#include <stddef.h>

/*  constants */
enum {
    UJ_FMT_FAST,
    UJ_FMT_DET,                 /* deterministic, ie, sort kv pairs by key */
    UJ_FMT_PRETTY               /* use indentation for human-readable output */
};

/*   types */
struct uni_json_s_binding;

/*  routines */
void uni_json_serialize(void *val, void *sink, struct uni_json_s_binding *binds,
                        int fmt);

#endif
