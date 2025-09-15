# -*- perl -*-
#
# test string serialization
#

use Test::More tests => 3;
use JSON::Uni	'json_serialize';

$x = json_serialize('abc');
is($x, '"abc"', 'plain string serializes correctly');

$x = json_serialize("ab\x01\x05de");
is($x, '"ab\u0001\u0005de"', 'string with control chars serializes correctly');

$x = json_serialize("\tbla\n\tblubb\n");
is($x, '"\tbla\n\tblubb\n"', 'short escape sequences work');
