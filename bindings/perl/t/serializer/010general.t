# -*- perl -*-
#
# general serializer tests
#

use Test::More tests => 1;

BEGIN {
    use_ok('JSON::Uni', 'serialize_json');
}

my $d = serialize_json('[1,2,3]', 0);
print STDERR ("$d\n");
