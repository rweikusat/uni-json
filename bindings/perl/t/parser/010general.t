# -*- perl -*-
#
# general parser tests
#

use Test::More tests => 5;

BEGIN {
    use_ok('JSON::Uni', qw(parse_json json_ec_2_msg UJ_E_NO_VAL));
}

eval {
    parse_json('');
};
isnt($@, '', 'empty string errors');

eval {
    parse_json("\t \r\n");
};
isnt($@, '', 'whitespace-only string errors');

eval {
    parse_json('@');
};
isnt($@, '', 'non-start-char errors');

eval {
    parse_json('', sub { die([@_]) });
};
is_deeply($@, [UJ_E_NO_VAL, 0], 'custom perl error handler works');
