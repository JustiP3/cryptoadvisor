#!/bin/bash
API_KEY=$1

curl -X POST 'https://api.livecoinwatch.com/coins/list' -o list.txt \
  -H 'content-type: application/json' \
  -H  'x-api-key: '"${API_KEY}"'' \
  -d '{"currency":"USD","sort":"rank","order":"ascending","offset":0,"limit":50,"meta":false}'
sed -i -e 's/\[//g;s/\]//g' ./list.txt

./crypto.exe
