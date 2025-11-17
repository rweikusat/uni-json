# build a docker image which can be used to build the project
#

#*  basic
#
arg debver=12
from debian:$debver

#*  install build-depends
#
run <<EOF
apt-get update
apt-get install -y \
	apt-utils \
	make \
	gcc \
	pip \
	debhelper \
	cpio
dpkg -s python3-setuptools || apt-get install -y python3-setuptools
EOF

#*  copy source
#
run mkdir /tmp/uni-json
copy . /tmp/uni-json
