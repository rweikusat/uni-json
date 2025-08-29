# -*- perl -*-
#
# test high-level value parser
#

use Test::Nore tests => 4;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('\t\n\r 123');
is($x, 123, 'whitespace before value is ignored');

$x = parse_json('122  \n ');
is($x, 122, 'whitespace after value is ignored');

eval {
    parse_json('123 123');
};
isnt($@, '', 'data after top-level value errors');
