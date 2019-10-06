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

- OpenCV (prefer 3.0 or higher)
- wxwidget (prefer 3.0 or higher)
- cmake

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

### Pre-Build Version

You can download pre-build version at [here](https://github.com/SSARCandy/Coherent-Line-Drawing/releases).  
Including Windows and macOS versions.

## Screenshot

![demo](./demo/1.JPG)

![demo](./demo/2.JPG)

![demo](./demo/3.JPG)
