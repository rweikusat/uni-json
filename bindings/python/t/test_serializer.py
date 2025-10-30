#
# test python serializer bindings
#

import UniJson as uj
import unittest

class TestSerializerBindings(unittest.TestCase):
    def test_no_args(self):
        with self.assertRaises(ValueError):
            uj.json_serialize()

if __name__ == '__main__':
    unittest.main()
