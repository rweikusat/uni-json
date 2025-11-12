# build a docker image which can be used to build the project
#

from debian
run apt-get update
run apt-get install -y git
