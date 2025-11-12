# build a docker image which can be used to build the project
#

#*  basic
#
from debian
run apt-get update

#*  install build-depends
#
run apt-get install -y git
run apt-get install -y make
run apt-get install -y gcc


