# -*- perl -*-
#
# test simple types
#

use Test::More tests => 4;
use JSON::Uni	'json_serialize';

my $x;

$x = json_serialize(undef);
is($x, 'null', 'undef serializes to null');

$x = json_serialize(!1);
is($x, 'false', 'boolean false serializes correclty');

$x = json_serialize(!0);
is($x, 'true', 'boolean true serializes correclty');

$x = json_serialize(123);
is($x, '123', 'number serializes correctly');
