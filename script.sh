#!/bin/bash

curl -X POST 'https://api.livecoinwatch.com/coins/list' -o list.txt \
  -H 'content-type: application/json' \
  -H 'x-api-key: 8df83f4c-bd84-4e83-a909-be414518d7d2' \
  -d '{"currency":"USD","sort":"rank","order":"ascending","offset":0,"limit":50,"meta":false}'
sed -i -e 's/\[//g;s/\]//g' ./list.txt


