/*
  serializer bindings

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_s_binding_h
#define uni_json_s_binding_h

/*  includes */
#include <inttypes.h>
#include <stddef.h>

/*  types */
/**  auxiliary */
struct data {
    uint8_t *s;
    size_t len;
};

struct kv_pair {
    struct data key;
    void *val;
};

/**  serializer bindings */
struct uni_json_s_binding {
    /*  general */
    void (*output)(uint8_t *data, size_t len, void *sink);
    int (*type_of)(void *p);

    /*  objects */
    void *(*start_object_traversal)(void *obj);
    void (*end_object_traversal)(void *oiter);
    int (*next_kv_pair)(void *obj, void *oiter, struct kv_pair *kvp);

    /*  arrays */
    void *(start_array_traversal)(void *ary);
    void (*end_array_traversal)(void *aiter);
    void (*next_value)(void *ary, void *aiter);

    /*  "string data" types */
    void (*get_num_data)(void *num, struct data *ndata);
    void (*free_num_data)(void *num, struct data *ndata);

    void (*get_string_data)(void *str, struct data *sdata);
    void (*free_string_data)(void *str, struct data *sdata);

    /*  bool */
    int (*get_bool_value)(void *boolean);
};

#endif
