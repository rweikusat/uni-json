# uni-json Perl bindings
#
# Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net
#

#*  code
#
package JSON::Uni;

use version; our $VERSION = version->declare('v0.1.0');

require DynaLoader;
our @ISA = 'DynaLoader';
__PACKAGE__->bootstrap();

use Exporter	'import';
our @EXPORT_OK = qw(parse_json max_nesting set_max_nesting);

# Ach ja
1;

__END__

=head1 NAME

JSON::Uni - Perl interface to uni-json parser

=head1 SYNOPSIS

 use JSON::Uni	qw(parse_json max_nesting set_max_nesting);

 my $obj = parse_json('<JSON string>');
 my $nesting = max_nesting();
 set_max_nesting(<max nesting level);

=head1 DESCRIPTION

This modue provides the default Perl interface to the uni-json JSON parser.

=head2 Exportable Functions

The module can export the following functions:

=over

=item * C<parse_json>

Parses a JSON string and returns an equivalent Perl data structure based on a straight-forward
mapping of JSON value types to Perl constructs.

=item * C<max nesting>

Returns the current value of the I<max nesting> parameter (default 0xffffffff, ie
unlimited). JSON constructs which are more deeply nested than this value
permits will cause an error when trying to parse them.

=item * C<set_max_nesting>

Change the I<max nesting> parameter. This can be used to limit the maximum
depth to which the parser will descent into nested structures to guard against DoS-attacks
when processing data from untrusted sources.

=back

=head2 Error Handling

In case of any kind of parsing error, the parser routine will C<die> with a
message including the standard message for the error in question, the numeric
error code and the position in the input string where the error was encountered.

=head1 SEE ALSO

L<uni_json_parse(3)>, L<uni_json_parser_bindings(5)>

=cut
