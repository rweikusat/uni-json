# -*- perl -*-
#
# test parsing of strings
#

use Test::More tests => 7;
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
#
$x = parse_json("\"\xc2\xa3\"");
is($x, "\N{U+00a3}", 'parsing 2-byte UTF-8 sequence works');

$x = parse_json("\"\xe2\x86\x93\"");
is($x, "\N{U+2193}", 'parsing 3-byte UTF-8 sequence works');

$x = parse_json("\"\xf0\x9f\x8e\xb2\"");
is($x, "\N{U+1f3b2}", 'parsing 4-byte UTF-8 sequence works');
