# -*- perl -*-
#
# test object serialization
#

use Test::More tests => 4;
use JSON::Uni	qw(parse_json json_serialize);

my ($x, $y);

$x = json_serialize({});
is($x, '{}', 'empty object serializes correctly');

$x = json_serialize({a => "b"});
is($x, '{"a":"b"}', 'object with one kv pair serializes correctly');

$x = json_serialize({a => "b", "c" => 45});
ok($x eq '{"a":"b","c":45}' || $x eq '{"c":45,"a":"b"}', 'object with more than one kv pair serializes correctly');

$x = json_serialize({"a\xe4b" => 11 });
$y = parse_json("\"a\xc3\xa4b\"");
is($x, "{\"$y\":11}", 'object with latin1-char in key serializes correctly');
