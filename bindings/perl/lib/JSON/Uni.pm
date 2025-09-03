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
