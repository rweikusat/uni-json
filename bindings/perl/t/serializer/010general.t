# -*- perl -*-
#
# general serializer tests
#

use Test::More tests => 1;

BEGIN {
    use_ok('JSON::Uni', 'json_serialize');
}

my $d = json_serialize('[1,2,3]', 0);
print STDERR ("$d\n");
