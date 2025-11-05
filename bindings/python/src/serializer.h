/*
   uni-json bindings for Python -- serializer flags

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/
#ifndef uj_python_serializer_h
#define uj_python_serializer_h

enum {
    UNK_TYPE_ERR =		4,
    NONSTR_KEY_ERR =		8,
    STRINGIFY_NONSTR_KEYS =	16,
    ALL = 			UNK_TYPE_ERR | NONSTR_KEY_ERR | STRINGIFY_NONSTR_KEYS
};

#endif
