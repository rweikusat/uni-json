# -*- perl -*-
#
# test parsing of objects
#

use Test::More tests => 3;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('{}');
is_deeply($x, {}, 'parsing empty object works');

$x = parse_json('{"emil": 43 }');
is_deeply($x, { emil => 43 }, 'parsing object with one kv-pair works');

$x = parse_json('{ "emil" : 43, "anna" : null }');
is_deeply($x, { emil => 43, anna => undef}, 'parsing object with more than one kv-pair works');
