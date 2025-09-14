# -*- perl -*-
#
# general serializer tests
#

use Test::More tests => 8;

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

$x = json_serialize(123);
is($x, '123', 'number serializes correctly');

$x = json_serialize('abc');
is($x, '"abc"', 'plain string serializes correctly');

$x = json_serialize("ab\x01\x05de");
is($x, '"ab\u0001\u0005de"', 'string with control chars serializes correctly');

$x = json_serialize("\tbla\n\tblubb\n");
is($x, '"\tbla\n\tblubb\n"', 'short escape sequences work');
