# -*- perl -*-
#
# general parser tests
#

use Test::More tests => 3;

BEGIN {
    use_ok('JSON::Uni', 'parse_json');
}

eval {
    parse_json('');
};
isnt($@, '', 'empty string errors');

eval {
    parse_json("\t \r\n");
};
isnt($@, '', 'whitespace-only string errors');
