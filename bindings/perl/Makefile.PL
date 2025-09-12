#
# Makefile.PL for uni-json Perl bindings
#

package MY;

our $LPATH = '../../bin';

sub const_loadlibs
{
    local $_[0]{LD_RUN_PATH};   # don't put $LPATH into binaries
    $_[0]->SUPER::const_loadlibs();
}

sub test
{
    my $self = shift;
    my $sec;

    $sec = $self->SUPER::test(@_);
    $sec =~ s|(PERL_DL_NONLAZY)|LD_LIBRARY_PATH=$LPATH $1|g;
    return $sec;
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
              LIBS =>		["-L$MY::LPATH -luni-json"],
              OBJECT =>		'parser$(OBJ_EXT) serializer$(OBJ_EXT) Uni$(OBJ_EXT)',
              depend =>		{
                                 'parser$(OBJ_EXT)' => '../../include/uni_json_p_binding.h ../../include/uni_json_parser.h',
                                 'serializer$(OBJ_EXT)' => '../../include/uni_json_s_binding.h ../../include/uni_json_serializer.h ../../include/uni_json_types.h',
                                 'Uni$(OBJ_EXT)' => '../../include/uni_json_parser.h', },

              test =>		{ TESTS => 't/parser/*.t t/serializer/*.t' }
             );
