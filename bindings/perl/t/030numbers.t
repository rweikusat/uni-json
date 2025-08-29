# -*- perl -*-
#
# test parsing of numbers
#

use Test::More tests => 4;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('123');
is($x, 123, 'parsing integer works');

eval {
    parse_json('12x');
};
isnt($@, '', 'digit parser stops at non-digits');

$x = parse('-123');
is($x, -123, 'parsing negative integer works');

eval {
    parse_json('-');
};
isnt($@, '', 'empty sequence of digits errors');
