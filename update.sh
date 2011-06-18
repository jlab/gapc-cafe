

set -e
set -u

umasl 022

make -C web

make -f web/web.mf SRC=pages DST=/var/www/gap INSERT=web/repl

cp -u pages/*_bib_bib.html /var/www/gap

cp -u resources/gap.css /var/www/gap/resources
cp -u resources/favicon.ico /var/www/gap/resources
cp -u resources/header_noTitle.png /var/www/gap/resources

mkdir -p /var/www/gap/grammar
cp -u grammar/* /var/www/gap/grammar


