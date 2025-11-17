# build a docker image which can be used to build the project
#

#*  basic
#
arg debver=12
from debian:$debver
run apt-get update

#*  install build-depends
#
run apt-get install apt-utils
run apt-get install -y git
run apt-get install -y make
run apt-get install -y gcc
run apt-get install -y pip
run apt-get install -y debhelper
run dpkg -s python3-setuptools || apt-get install -y python3-setuptools

#*  copy source
#
run mkdir /tmp/uni-json
copy . /tmp/uni-json
