#!/bin/bash

rm -rf .coverage coverage.info

./build/cld --src ./data/fingerprint.jpg -o output.jpg --ETF_iter 3 --CLD_iter 8
echo '10459bb86077235db3f85fe64d25f2d8 output.jpg' | md5sum -c - 

if ! which lcov; then
    exit 0
fi

lcov \
    --capture \
    --directory build/CMakeFiles/cld.dir/src \
    --output-file coverage.info \
    --test-name coverageHtml

lcov --remove coverage.info '/usr/*' -o coverage.info > /dev/null

genhtml -o .coverage coverage.info