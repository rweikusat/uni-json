#
# test python parser bindings
#

import UniJson as uj
import unittest

class TestParserBindings(unittest.TestCase):
    def test_error(self):
        with self.assertRaises(ValueError):
            uj.parse_json('')

    def test_null(self):
        x = uj.parse_json('null')
        self.assertIsNone(x)

    def test_bool(self):
        x = uj.parse_json('true')
        self.assertIs(x, True)
        x = uj.parse_json('false')
        self.assertIs(x, False)

    def test_num(self):
        x = uj.parse_json('1234');
        self.assertEqual(x, 1234)
        x = uj.parse_json('1.23E2')
        self.assertEqual(x, 1.23E2)

if __name__ == '__main__':
    unittest.main()
