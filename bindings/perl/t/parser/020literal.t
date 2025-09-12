# -*- perl -*-
#
# parsing of literals
#

use Test::More tests => 4;
use JSON::Uni 'parse_json';

my $x;

$x = parse_json('null');
ok(!defined($x), 'null parses as undef');

$x = parse_json('true');
ok(builtin::is_bool($x) && $x, 'true parses as boolean true');

$x = parse_json('false');
ok(builtin::is_bool($x) && !$x, 'false parses as boolean true');

eval {
    parse_json('faxe');
};
isnt($@, '', 'invalid char in literal errors');
