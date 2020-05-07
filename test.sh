#!/bin/bash

rm -rf .coverage coverage.info

time ./build/cld --src ./data/fingerprint.jpg -o output.jpg --ETF_iter 3 --CLD_iter 8 --debug_img
echo '10459bb86077235db3f85fe64d25f2d8 output.jpg' | md5sum -c - 
echo '3ea95ae0f6f92478af7fbf97e96b2763 anti-alias.jpg' | md5sum -c - 
echo '1a68133901751c29efe4febec29d014e visualize-etf.jpg' | md5sum -c - 
echo '365b29b4d15520b5c92eb78bc6c236b8 arrow-etf.jpg' | md5sum -c - 

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

# clean up
rm *.jpg