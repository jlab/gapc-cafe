
set -e
set -u

umask 022

DST=/var/www/gapapp
WWW=/var/www/gap

mkdir -p $WWW $DST
chown gms:users $WWW $DST

cd $DST
mkdir -p cached output log
chown gms:gapp cached output log
chmod 775 cached output log


