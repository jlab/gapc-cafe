set -e
set -u

umask 022

WT=$1

cd /var/www/gap

mkdir -p app
cd app
cp -r ${WT}/share/resources .
cd ..

ln -s app/resources

