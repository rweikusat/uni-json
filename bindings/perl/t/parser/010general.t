# -*- perl -*-
#
# general parser tests
#

use Test::More tests => 5;

BEGIN {
    use_ok('JSON::Uni', qw(parse_json json_ec_2_msg));
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
is_deeply($@, [1, 0], 'custom perl error handler works');
