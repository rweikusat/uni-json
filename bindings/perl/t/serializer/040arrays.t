# -*- perl -*-
#
# test array serialization
#

use Test::More tests => 4;
use JSON::Uni	'json_serialize';


my $x;

$x = json_serialize([]);
is($x, '[]', 'empty array serializes correctly');

$x = json_serialize(["a"]);
is($x, '["a"]', 'array with one value serializes correctly');

$x = json_serialize(["a", 123.45]);
is($x, '["a",123.45]', 'array with more than one value serializes correctly');

my $pretty = <<TT;
[
	"a",
	[
		"bc",
		"de"],
	123]
TT
$x = json_serialize(["a", ["bc", "de"], 123], 2);
is($x, substr($pretty, 0, -1), "array pretty-printer works as expected");
