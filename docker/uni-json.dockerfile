# build a docker image which can be used to build the project
#

arg img
from $img

copy . /tmp/uni-json

entrypoint cd /tmp/uni-json && make && make deb
