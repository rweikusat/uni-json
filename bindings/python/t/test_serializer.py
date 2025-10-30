#
# test python serializer bindings
#

import UniJson as uj
import unittest

class TestSerializerBindings(unittest.TestCase):
    def test_no_args(self):
        with self.assertRaises(TypeError):
            uj.json_serialize()

    def test_one_arg(self):
        uj.json_serialize(None)

    def test_two_args(self):
        uj.json_serialize(None, 0)

    def test_2nd_wrong(self):
        with self. assertRaises(ValueError):
            uj.json_serialize(None, 17)

    def test_null(self):
        s = uj.json_serialize(None)
        self.assertEqual(s, 'null')

    def test_true(self):
        s = uj.json_serialize(True)
        self.assertEqual(s, 'true')

    def test_false(self):
        s = uj.json_serialize(False)
        self.assertEqual(s, 'false')

    def test_string(self):
        s = uj.json_serialize('abc')
        self.assertEqual(s, '"abc"')

if __name__ == '__main__':
    unittest.main()
