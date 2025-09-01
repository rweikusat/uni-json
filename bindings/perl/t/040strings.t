# -*- perl -*-
#
# test parsing of strings
#

use Test::More tests => 24;
use JSON::Uni 'parse_json';

my $x;

#*  simple strings tests
#
$x = parse_json('"abc"');
is($x, 'abc', 'parsing a simple string works');

$x = parse_json('""');
is($x, '', 'parsing an empty string works');

eval {
    parse_json('"');
};
isnt($@, '', 'parsing an unterminated string errors');

eval {
    parse_json("\"\x00\"");
};
isnt($@, '', 'unescaped control char errors');

#*  UTF-8 tests
#**  valid sequences
#
$x = parse_json("\"\xc2\xa3\"");
is($x, "\N{U+00a3}", 'parsing 2-byte UTF-8 sequence works');

$x = parse_json("\"\xe2\x86\x93\"");
is($x, "\N{U+2193}", 'parsing 3-byte UTF-8 sequence works');

$x = parse_json("\"\xf0\x9f\x8e\xb2\"");
is($x, "\N{U+1f3b2}", 'parsing 4-byte UTF-8 sequence works');

#**  invalid marker
#
eval {
    parse_json("\"\xff\"");
};
isnt($@, '', 'invalid UTF-8 marker errors');

#**  invalid value bytes
#
eval {
    parse_json("\"\xc2\xc0\"");
};
isnt($@, '', 'invalid 1st value byte errors');

eval {
    parse_json("\"\xe2\x86\x00\"");
};
isnt($@, '', 'invalid 2nd value byte errors');

eval {
    parse_json("\"\xf0\x9f\x8e\x40\"");
};
isnt($@, '', 'invalid 3rd value byte errors');

#**  overlong sequences
#
eval {
    # 1-byte sequences have 7 value bits while all other have at most 6
    # value bits per byte
    #
    parse_json("\"\xc1\x80\"");
};
isnt($@, '', 'overlong 2-byte sequence errors');

eval {
    parse_json("\"\xe0\x80\""); # only first two bytes matter for overlength detection
};
isnt($@, '', 'overlong 3-byte sequence errors');

$x = parse_json("\"\xe0\xa0\x80\"");
is($x, "\N{U+800}", 'shortest valid 3-byte sequence works');

eval {
    parse_json("\"\xf0\x80\"");
};
isnt($@, '', 'overlong 4-byte sequence errors');

$x = parse_json("\"\xf0\x90\x80\x80\"");
is($x, "\N{U+10000}", 'shortest valid 4-byte sequence works');

#**  encoded surrogates
#
eval {
    parse_json("\"\xed\xad\x80\"");
};
isnt($@, '', 'encoded minimum surrogate errors');

eval {
    parse_json("\"xed\xbf\xbf\"");
};
isnt($@, '', 'encoded maxium surrogate errors');

#**  embedded sequences
#
$x = parse_json("\"ab\xc2\xa3ba\"");
is($x, "ab\N{U+a3}ba", 'embedded 2-byte sequence works');

$x = parse_json("\"cd\xe2\x86\x93dc\"");
is($x, "cd\N{U+2193}dc", 'embedded 3-byte sequence works');

$x = parse_json("\"efg\xf0\x9f\x8e\xb2gfe\"");
is($x, "efg\N{U+1f3b2}gfe", 'embedded 4-byte sequence works');

#*  escape sequences
#
$x = parse_json('"ab\\"cd"');
is($x, 'ab"cd', 'simple escape sequence works');

eval {
    parse_json('"\\y"');
};
isnt($@, '', 'invalid simple escape sequence errors');

eval {
    parse_json('"\\');
};
isnt($@, '', 'end of data in escape sequence errors');
