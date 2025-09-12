# -*- perl -*-
#
# general serializer tests
#

use Test::More tests => 1;

BEGIN {
    use_ok('JSON::Uni', 'serialize_json');
}

serialize_json('[1,2,3]', 0);
