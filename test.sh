#!/bin/bash

./build/cld --src ./data/fingerprint.jpg -o output.jpg --ETF_iter 3 --CLD_iter 8
echo '10459bb86077235db3f85fe64d25f2d8 output.jpg' | md5sum -c - 