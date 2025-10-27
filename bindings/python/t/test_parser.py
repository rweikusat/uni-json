#
# test python parser bindings
#

import UniJson as uj
import unittest

class TestParserBindings(unittest.TestCase):
    def test_error(self):
        with self.assertRaises(ValueError):
            uj.parse_json('')

if __name__ == '__main__':
    unittest.main()
