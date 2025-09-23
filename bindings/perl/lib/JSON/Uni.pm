# uni-json Perl bindings
#
# Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net
#

#*  code
#
package JSON::Uni;


BEGIN {
    use version; our $VERSION = version->declare('v0.1.0');

    require DynaLoader;
    our @ISA = 'DynaLoader';
    __PACKAGE__->bootstrap();

    my %h;

    %h = unpack('(pQ)*', err_consts());
    require constant;
    constant->import(\%h);

    %h = unpack('(pQ)*', fmt_consts());
    require constant;
    constant->import(\%h);
}

use Exporter	'import';
our @EXPORT_OK = qw(parse_json max_nesting set_max_nesting json_serialize json_ec_2_msg

                    UJ_E_INV UJ_E_NO_VAL UJ_E_INV_LIT
                    UJ_E_GARBAGE UJ_E_EOS UJ_E_INV_IN
                    UJ_E_ADD UJ_E_LEADZ UJ_E_NO_DGS
                    UJ_E_INV_CHAR UJ_E_INV_UTF8 UJ_E_INV_ESC
                    UJ_E_INV_KEY UJ_E_NO_KEY UJ_E_TOO_DEEP

                    UJ_FMT_FAST UJ_FMT_DET UJ_FMT_PRETTY
                  );

# Ach ja
1;

__END__

=head1 NAME

JSON::Uni - Perl interface to uni-json parser and serializer

=head1 SYNOPSIS

 use JSON::Uni	qw(parse_json max_nesting set_max_nesting json_serialize

                   UJ_E_INV UJ_E_NO_VAL UJ_E_INV_LIT
                   UJ_E_GARBAGE UJ_E_EOS UJ_E_INV_IN
                   UJ_E_ADD UJ_E_LEADZ UJ_E_NO_DGS
                   UJ_E_INV_CHAR UJ_E_INV_UTF8 UJ_E_INV_ESC
                   UJ_E_INV_KEY UJ_E_NO_KEY UJ_E_TOO_DEEP

                   UJ_FMT_FAST UJ_FMT_DET UJ_FMT_PRETTY);

 my $obj = parse_json(<JSON string>[, <error handler>]);

 my $nesting = max_nesting();
 set_max_nesting(<max nesting level);

 my $str = json_serialize(<perl object>[, <format spec>]);

=head1 DESCRIPTION

This module provides the default Perl interface to the uni-json JSON
parser and serializer.

=head2 Exportable Functions

The module can export the following functions:

=over

=item * C<parse_json>

Parses a JSON string and returns an equivalent Perl data structure
based on a straight-forward mapping of JSON value types to Perl
constructs.

The optional I<error handler> argument should either be the name of a
subroutine or a refence to one. In case of a parsing error, it'll be
invoked with the error code as first argument and the position of the
error in the input string as second. The various C<UJ_E_...> constants are
the symbolic names of these error codes, a detailed description of them
is in L<uni-json(3)>.

It's safe to call C<die> or an equivalent from this handler as all
memory allocated during parsing will have been free before it's
invoked.

=item * C<max_nesting>

Returns the current value of the I<max nesting> parameter (default 0xffffffff, ie
unlimited).

=item * C<set_max_nesting>

Change the I<max nesting> parameter. JSON constructs which are more deeply nested than this value
permits will cause an error when trying to parse them. This can be used to limit the maximum
depth to which the parser will descend into nested structures to guard against DoS-attacks
when processing data from untrusted sources.

=item * C<json_serialize>

Serialize a Perl object to a JSON string. The optional second argument can be one of
C<UJ_FMT_FAST>, C<UJ_FMT_DET> or C<UJ_FMT_PRETTY> to select I<fast>, I<deterministic>
or I<pretty-printed> output format, with I<fast> being the default if no second argument
was provided.

The first two affect only serialization of objects/
hashes. When using the I<fast> format, key-value pairs will be serialized in
the order they're returned by the L<perlapi(1)> equivalent of C<each> (C<hv_iternext>). In
particular, this means the order of keys in the ouput will be random for each individual
object and very likely different for different objects containing the same keys. For the
I<deterministic> output format, keys will appear sorted by locale-blind codepoint
comparisons.

The I<pretty-printed> output format will include redundant whitespace in arrays and
objects to make the resulting text easier to read for humans. For objects, it uses the same
key-sorting algorithm as the I<deterministic> format.

B<Example Output>

The output C<< json_serialize([{a => b, c => d}, {a => b, c => d}]) >> can either be
C<[{"c":"d","a":"b"},{"c":"d","a":"b"}]> or C<[{"a":"b","c":"d"},{"c":"d","a":"b"}]> (or
any other possible combination).

For C<< json_serialize([{a => b, c => d}, {a => b, c => d}], UJ_FMT_DET) >>, it will always
be C<[{"a":"b","c":"d"},{"a":"b","c":"d"}]>.

Using C<< json_serialize([{a => b, c => d}, {a => b, c => d}], UJ_FMT_PRETTY) >> will produce

C<[
        {
                "a" : "b",
                "c" : "d"},
        {
                "a" : "b",
                "c" : "d"}]>

using tabs (ASCII 09) for indentation. This is to avoid creating extremely huge output strings
containing mostly space characters when serializing large structures.

=back

=head2 Default Parser Error Handling

In case of a parsing error, when the optional second argument to C<parse_json> wasn't provided,
the parser will C<die> with a message including the standard message for error which occurred, the
numeric error code and the position in the input string where the error was
encountered.

=head1 SEE ALSO

L<uni-json(3)>, L<uni-json-parser-bindings(3)>, L<uni-json-serializer-bindings(3)>

=cut
