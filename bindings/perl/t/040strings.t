# -*- perl -*-
#
# test parsing of strings
#

use Test::More tests => 5;
use JSON::Uni 'parse_json';

my $x;

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

$x = parse_json("\"\xc2\xa3\"");
is($x, "\N{U+00a3}", 'parsing 2-byte UTF-8 sequence works');
