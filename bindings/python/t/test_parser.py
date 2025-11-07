#
# test python parser bindings
#

import UniJson as uj
import unittest
from sys import stderr

class MyExcept(BaseException):
    pass

def my_on_error(code, pos):
    print(f'error {code} at {pos}', file=stderr)
    raise MyExcept("bla")

class TestParserBindings(unittest.TestCase):
    def test_error(self):
        with self.assertRaises(ValueError):
            uj.parse_json('')

    def test_null(self):
        x = uj.parse_json('null')
        self.assertIsNone(x)

    def test_true(self):
        x = uj.parse_json('true')
        self.assertIs(x, True)

    def test_false(self):
        x = uj.parse_json('false')
        self.assertIs(x, False)

    def test_int(self):
        x = uj.parse_json('1234');
        self.assertEqual(x, 1234)

    def test_float(self):
        x = uj.parse_json('1.23E2')
        self.assertEqual(x, 1.23E2)

    def test_string_with_esc(self):
        x = uj.parse_json('"123456789012345\\n67890123456789012345678äx"')
        self.assertEqual(x,  '123456789012345\n67890123456789012345678äx')

    def test_string_without_esc(self):
        x = uj.parse_json('"abcdefg"')
        self.assertEqual(x, 'abcdefg')

    def test_array(self):
        x = uj.parse_json('[1, 2, [1, 2]]')
        self.assertEqual(x, [1, 2, [1, 2]])

    def test_object(self):
        x = uj.parse_json('{"a" : 1, "b" : 2 }')
        self.assertEqual(x, {'a' : 1, 'b' : 2})

    def test_max_nesting(self):
        with self.assertRaises(ValueError):
            uj.parse_json('[1,2,[3,4,[5]]]', None, 2)

    def test_my_error(self):
        with self.assertRaises(MyExcept):
            uj.parse_json('[', my_on_error)

if __name__ == '__main__':
    unittest.main()
