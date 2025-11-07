# -*- perl -*-
#
# test max nesting parameter
#

use Test::More tests => 1;
use JSON::Uni 'parse_json';

eval {
    parse_json('[[[]]]', undef, 2);
};
isnt($@, '', 'max nesting exceeded errors');
