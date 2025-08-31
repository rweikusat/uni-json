# -*- perl -*-
#
# test parsing of strings
#

use Test::More tests => 13;
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
isnt($@, '', 'parsing an untermiated string errors');

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
    parse_json("\"\xc0\x80\"");
};
isnt($@, '', 'overlong 2-byte sequence errors');

eval {
    parse_json("\"\xe0\x80\"");
};
isnt($@, '', 'overlong 3-byte sequence errors');
