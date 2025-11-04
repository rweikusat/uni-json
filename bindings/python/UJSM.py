# enum with serialization mode constants
#

from enum import IntFlag

class SM(IntFlag):
    UJ_FMT_FAST = 0
    UJ_FMT_DET = 1
    UJ_FMT_PRETTY = 2
    PY_UJ_UNK_ERR = 4
    PY_UJ_NSK_ERR = 8
    PY_UJ_NSK_STR = 16
