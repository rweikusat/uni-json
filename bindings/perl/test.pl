# uni-json test script
#

use Test::More tests => 27;

#**  overall tests
#
BEGIN {
    use_ok('JSON::Uni', 'parse_json');
}

eval {
    parse_json('');
};
isnt($@, '', 'empty string errors');

eval {
    parse_json("\t \r\n");
};
isnt($@, '', 'whitespace-only string errors');

for (qw(] } , :)) {
    eval {
        parse_json("$_");
    };
    isnt($@, '', "sole $_ errors");
}

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

#**  arrays
#
{
    eval {
        parse_json('[');
    };
    isnt($@, '', 'unclosed array errors');
}

{
    eval {
        parse_json('[,');
    };
    isnt($@, '', 'sep with no value in array errors');
}

{
    my $x = parse_json('[]');
    ok(ref($x) eq 'ARRAY' && @$x == 0, 'parsing an empty array works');
}

{
    my $x = parse_json('[null]');
    is_deeply($x, [undef], 'array with 1 value works');
}

{
    my $x = parse_json('[null, null]');
    is_deeply($x, [undef, undef], 'array with 2 values works');
}

{
    eval {
        parse_json('[null : null]');
    };
    isnt($@, '', 'array with wrong separator errors');
}

{
    eval {
        parse_json('[null,]');
    };
    isnt($@, '', 'array with missing value errors');
}

{
    my $x = parse_json('[ [null,null] ]');
    is_deeply($x, [[undef, undef]], 'array in array works');
}

#** numbers
#
{
    my $x = parse_json('123');
    ok($x == 123, 'parsing integer works')
}

eval {
    parse_json('1x3');
};
isnt($@, '', 'integer with garbage errors');

eval {
    parse_json('0123');
};
isnt($@, '', 'leading zeroes error');

{
    my $x = parse_json('-1234');
    ok($x == -1234, 'negative integers work');
}

{
    my $x = parse_json('1.234');
    ok($x == 1.234, 'fracs work');
}

{
    my $x = parse_json('1.23e3');
    ok($x == 1230, 'positive e-notation works');
}
