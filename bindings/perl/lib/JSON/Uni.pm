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
our @EXPORT_OK = 'parse_json';

# Ach ja
1;
