# -*- perl -*-
#
# test parsing of objects
#

use Test::More tests => 9;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('{}');
is_deeply($x, {}, 'empty object works');

$x = parse_json('{"emil": 43 }');
is_deeply($x, { emil => 43 }, 'object with one kv-pair works');

$x = parse_json('{ "emil" : 43, "anna" : [1, 2, 3] }');
is_deeply($x, { emil => 43, anna => [1, 2, 3]}, 'object with more than one kv-pair works');

eval {
    parse_json('{');
};
isnt($@, '', 'unclosed object errors');

eval {
    parse_json('{ [1, 2] : null }');
};
isnt($@, '', 'non-string key errors');

eval {
    parse_json('{ "key" }');
};
isnt($@, '', 'key-only object errors');

eval {
    parse_json('{"key" : }');
};
isnt($@, '', 'object with key and separator errors');

eval {
    parse_json('{ "key" , "blubb" }');
};
isnt($@, '', 'wrong kv separator errors');

eval {
    parse_json('{ "key" : "blubb", }');
};
isnt($@, '', 'missing key errors');
