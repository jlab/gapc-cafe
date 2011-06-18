set -e
set -u

umask 022

./gap.wt --docroot='.;/app/resources,/resources,/grammar' \
  --deploy-path '/app/' \
  --http-address 127.0.0.1 \
  --http-port 8080 \
  --accesslog log/gap.log \
  > log/x.log 2>&1


