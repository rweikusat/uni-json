# uni-json test script
#

use Test::More tests => 9;

#**  overall tests
#
BEGIN {
    use_ok('JSON::Uni', 'parse_json');
}

{
    my $x = parse_json('');
    ok(!defined($x), 'empty string parses as null');
}

eval {
    parse_json(' ');
};
isnt($@, '', 'non-empty string with no value returns an error');

#**  literal parsing
#
{
    my $x = parse_json('null');
    ok(!defined($x), 'null parses as undef');
}

{
    my $x = parse_json('true');
    ok(builtin::is_bool($x) && $x, 'parsing true returns a boolean true value');
}

{
    my $x = parse_json('false');
    ok(builtin::is_bool($x) && !$x, 'parsing false returns a boolean false value');
}

{
    my $x;
    eval {
        $x = parse_json("\t\r\n   null\r\r\t\n");
    };
    ok($@ eq '' && !defined($x), 'values can have leading and trailing whitespace');
}

{
    eval {
        parse_json('flase');
    };
    isnt($@, '', 'wrong chars in literals error');
}

{
    eval {
        parse_json('nuller');
    };
    isnt($@, '', 'additional chars in literals error');
}
