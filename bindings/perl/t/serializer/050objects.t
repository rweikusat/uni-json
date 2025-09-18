# -*- perl -*-
#
# test object serialization
#

use Test::More tests => 6;
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

my $d = { zloty => [1,2,3, { d => 4 }], alpha => "beta", alpert => 12, gamma => "delta", epsilon => 1};
my $ser = <<TT;
{
	"alpert" : 12,
	"alpha" : "beta",
	"epsilon" : 1,
	"gamma" : "delta",
	"zloty" : [
		1,
		2,
		3,
		{
			"d" : 4}]}
TT
$ser = substr($ser, 0, -1);
$y = 1;
for (0 .. 49) {
    $x = json_serialize($d, 2);
    $y = 0, last unless $x eq $ser;
}
is($y, 1, 'pretty-printing a complex object worked 50 times in a row');

$x = json_serialize({z => 0, b => 1, k => 2}, 1);
is($x, '{"b":1,"k":2,"z":0}', 'deterministic serializer works');
