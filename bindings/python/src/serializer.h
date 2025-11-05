/*
   uni-json bindings for Python -- serializer flags

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/
#ifndef uj_python_serializer_h
#define uj_python_serializer_h

enum {
    PY_UJ_UNK_ERR =		4,
    PY_UJ_NSK_ERR =		8,
    PY_UJ_NSK_STR =		16,
    PY_UJ_ALL =			PY_UJ_UNK_ERR | PY_UJ_NSK_ERR | PY_UJ_NSK_STR
};

#endif
