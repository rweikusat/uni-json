# -*- perl -*-
#
# test parsing of arrays
#

use Test::More tests => 7;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('[]');
is_deeply($x, [], 'parsing empty array works');

eval {
    $x = parse_json('[}');
};
isnt($@, '', 'empty array with wrong terminator errors');

$x = parse_json('[123]');
is_deeply($x, [123], 'parsing array with one value works');

$x = parse_json('[123, 779]');
is_deeply($x, [123, 779], 'parsing array with more than one value works');

eval {
    parse_json('[123}');
};
isnt($@, '', 'non-empty array with wrong terminator errors');

eval {
    parse_json('[123 % 456]');
};
isnt($@, '', 'array with wrong separator errors');

eval {
    parse_json('[456,]');
};
isnt($@, '', 'missing value in array errors');
