#
# Makefile.PL for uni-json Perl bindings
#

package MY;

sub const_loadlibs
{
    local $_[0]{LD_RUN_PATH};
    $_[0]->SUPER::const_loadlibs();
}

package main;

use ExtUtils::MakeMaker;


WriteMakefile(
              #** meta
              #
              NAME =>		'JSON::Uni',
              VERSION_FROM =>	'lib/JSON/Uni.pm',
              AUTHOR =>		['Rainer Weikusat <rweikusat@talktalk.net>'],

              #**  build info
              #
              INC =>		'-I../../include',
              LIBS =>		['-L../../bin -luni-json'],
              OBJECT =>		'parser$(OBJ_EXT) Uni$(OBJ_EXT)'
             );
