# build a docker image which can be used to build the project
#

#*  basic
#
arg debver=12
from debian:$debver

#*  install build-depends
#
run \
	apt-get update \
        && apt-get install -y \
		apt-utils \
		make \
		gcc \
		pip \
		debhelper \
		cpio \
	&& { dpkg -s python3-setuptools || apt-get install -y python3-setuptools; }

#*  copy source
#
copy . /tmp/uni-json/
