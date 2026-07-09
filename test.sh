#!/bin/bash
set -e

rm -rf .coverage coverage.info

time ./build/cld --src ./data/fingerprint.jpg -o output.jpg --ETF_iter 3 --CLD_iter 8 --debug_img
echo '92757f83506131ade0ac4e9c84f755f4 output.jpg' | md5sum -c -
echo '4a1cd86bd6a52f7abe6fa594ac5b5bc6 anti-alias.jpg' | md5sum -c -
echo 'a55c4e14b375e6e0222d123d2de4db21 visualize-etf.jpg' | md5sum -c -
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