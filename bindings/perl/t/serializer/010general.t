# -*- perl -*-
#
# general serializer tests
#

use Test::More tests => 4;

BEGIN {
    use_ok('JSON::Uni', 'json_serialize');
}

my $x;

$x = json_serialize(undef);
is($x, 'null', 'undef serializes to null');

$x = json_serialize(!1);
is($x, 'false', 'boolean false serializes correclty');

$x = json_serialize(!0);
is($x, 'true', 'boolean true serializes correclty');
