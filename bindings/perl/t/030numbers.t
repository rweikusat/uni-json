# -*- perl -*-
#
# test parsing of numbers
#

use Test::More tests => 13;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('123');
is($x, 123, 'parsing integer works');

$x = parse_json('0');
is($x, 0, 'parsing 0 works');

eval {
    parse_json('0123');
};
isnt($@, '', 'leading zero in integer part errors');

eval {
    parse_json('12x');
};
isnt($@, '', 'digit parser stops at non-digits');

$x = parse_json('-123');
is($x, -123, 'parsing negative integer works');

eval {
    parse_json('-');
};
isnt($@, '', 'empty sequence of digits errors');

$x = parse_json('123.978');
is($x, 123.978, 'parsing number with fraction works');

$x = parse_json('123e3');
is($x, 123000, 'parsing e exponent works');

$x = parse_json('123E3');
is($x, 123000, 'parsing E exponent works');

$x = parse_json('123e+3');
is($x, 123000, 'parsing exponent with + works');

$x = parse_json('56700e-2');
is($x, 567, 'parsing exponent with - works');

$x = parse_json('123.456e3');
is($x, 123456, 'parsing number with fraction and exponent works');

eval {
    parse_json('123e');
};
isnt($@, '', 'e with nothing following errors');
