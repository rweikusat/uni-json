#
# test python serializer bindings
#

import UniJson as uj
import unittest

class TestSerializerBindings(unittest.TestCase):
    def test_no_args(self):
        with self.assertRaises(TypeError):
            uj.json_serialize()

    def test_two_args(self):
        uj.json_serialize(None, 0)

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

    def test_num(self):
        s = uj.json_serialize(1234)
        self.assertEqual(s, '1234')

    def test_array(self):
        s = uj.json_serialize([1, 2, ['affe']])
        self.assertEqual(s, '[1,2,["affe"]]')

    def test_string_key_dict(self):
        s = uj.json_serialize({ "b" : 1, "a" : 7 }, 1)
        self.assertEqual(s, '{"a":7,"b":1}')

    def test_non_string_key_dict(self):
        s = uj.json_serialize({ (1,2) : 3, 4711 : "Emil", None : 'bla' }, 1)
        self.assertEqual(s, '{"(1, 2)":3,"4711":"Emil","None":"bla"}')

    def test_unk_null(self):
        s = uj.json_serialize([1, (1,2), 3])
        self.assertEqual(s, '[1,null,3]')

    def test_unk_err(self):
        with self.assertRaises(ValueError):
            uj.json_serialize([1, (1,2), 3], 4)

if __name__ == '__main__':
    unittest.main()
