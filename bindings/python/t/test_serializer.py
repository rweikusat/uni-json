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

    def test_unk_null(self):
        s = uj.json_serialize([1, (1,2), 3])
        self.assertEqual(s, '[1,null,3]')

    def test_unk_err(self):
        with self.assertRaises(ValueError):
            uj.json_serialize([1, (1,2), 3], 4)

    def test_string_key_dict(self):
        x = uj.json_serialize({ "a" : 1, "b" : 2 })
        self.assertTrue(x == '{"b":1,"a":2}' or x == '{"a":1,"b":2}')

    def test_nsk_dict_skip_some(self):
        x = uj.json_serialize({ 12 : 14, "emil" : "jannings", (1,2) : 11, "johnny" : "potato"}, 1)
        self.assertEqual(x, '{"emil":"jannings","johnny":"potato"}')

    def test_nsk_dict_skip_all(self):
        x = uj.json_serialize({ 12 : 14, (1,2) : 11}, 1)
        self.assertEqual(x, '{}')

    def test_nsk_dict_err(self):
        with self.assertRaises(ValueError):
            uj.json_serialize({ "a" : 3, (8,9) : 1 }, 8)

    def test_nsk_dict_str(self):
        x = uj.json_serialize({"a" : "b", (8,9) : 12, 13: "alpha"}, 17)
        self.assertEqual(x, '{"(8, 9)":12,"13":"alpha","a":"b"}')

if __name__ == '__main__':
    unittest.main()
