#
# Makefile.PL for uni-json Perl bindings
#

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
