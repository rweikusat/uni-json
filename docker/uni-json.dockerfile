# build a docker image which can be used to build the project
#

arg img
from $img

copy . /tmp/uni-json
