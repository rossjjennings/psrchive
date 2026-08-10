export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
xargs -a Management/packages.txt apt-get install -y -qq
