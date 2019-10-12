# Coherent Line Drawing

[![Build Status](https://travis-ci.org/SSARCandy/Coherent-Line-Drawing.svg?branch=master)](https://travis-ci.org/SSARCandy/Coherent-Line-Drawing)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/SSARCandy/Coherent-Line-Drawing/blob/master/LICENSE)


This project implemented a non-photorealistic rendering technique presented by Kang et al, that can automatically generates a line drawing from a photograph. This project provide an easy-to-use, real-time interactive graphic user interface system.


- [Original academic paper](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.108.559&rep=rep1&type=pdf)
- [Introduction in Chinese](https://ssarcandy.tw/2017/06/26/Coherent-Line-Drawing/)

![demo](./demo/4.JPG)

## Workflow(youtube video)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=48fTXKUTM-8
" target="_blank"><img src="http://img.youtube.com/vi/48fTXKUTM-8/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="800" border="10" /></a>


## Build

### Requirement

- OpenCV (3.0 or higher)
- WxWidget (not required if you only interested in cmd application)
- CMake

It should work fine in Windows/Linux/MacOS.  
I provided some scripts that can use in Linux:

```sh
# Usage: build.sh [options]
# Options:
#   -c, --clean       Clean build
#   -d, --debug       Build with debug mode
#   -j, --jobs        Use N cores to build
$ ./build.sh

# Usage: linter.sh [options]
# Check code style
# Options:
#   -i                In-place format
$ ./linter.sh
```

### Command Line Version

I provide another command line application that can directly use without graphic interface, the entry point is `src/cmd.cpp`. The options is list as following:

```sh
# Coherent-Line-Drawing Options:
#   -h [ --help ]         Help message
#   -s [ --src ] arg      Source image path
#   -o [ --output ] arg   Output image path
#   --ETF_kernel arg      ETF kernel size, default kernal = 5
#   --ETF_iter arg        Refining n times ETF, default is 1 iteration
#   --CLD_iter arg        Iterate n times FDoG, default is 1 iteration
$ ./build/cld \
    --src ./data/fingerprint.jpg \
    --output 123.jpg \
    --ETF_kernel 6 \
    --ETF_iter 3 \
    --CLD_iter 8
```

### Pre-Build GUI Version

You can download pre-build version at [here](https://github.com/SSARCandy/Coherent-Line-Drawing/releases).  
Including Windows and macOS versions.

## Screenshot

![demo](./demo/1.JPG)

![demo](./demo/2.JPG)

![demo](./demo/3.JPG)
