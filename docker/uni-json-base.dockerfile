# build a docker image which can be used to build the project
#

arg debver=12
from debian:$debver

run \
	{ echo 'APT::Install-Recommends "0";' | dd of=/etc/apt/apt.conf.d/01norec; } \
	&& apt-get update \
        && apt-get install -y apt-utils \
        && apt-get install -y \
		cpio \
		debhelper \
		gcc \
		make \
		pip \
                fakeroot \
                libpython3-dev \
	&& { dpkg -s python3-setuptools || apt-get install -y python3-setuptools; }
