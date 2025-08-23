# uni-json Perl bindings
#
# Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net
#

#*  code
#
package JSON::Uni;

use version; our $VERSION = version->declare('v0.1.0');

require DynaLosder;
our @ISA = 'DynaLoader';
__PACKAGE__->bootstrap();

# Ach ja
1;
