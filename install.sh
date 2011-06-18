set -e
set -u

umask 022

DST=/var/www/gapapp

make

cp -u run.sh $DST
cp -u gap.wt $DST
cp -u menu.rc $DST
mkdir -p $DST/grammar
cp -u grammar/* $DST/grammar

mkdir -p $DST/pages
cp -u pages/menu.html_ $DST/pages


