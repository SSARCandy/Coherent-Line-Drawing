# Coherent Line Drawing

[![Build Status](https://travis-ci.org/SSARCandy/Coherent-Line-Drawing.svg?branch=master)](https://travis-ci.org/SSARCandy/Coherent-Line-Drawing)

An implementation of 'Coherent Line Drawing' by Kang et al, Proc. NPAR 2007 . 

- [Original academic paper](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.108.559&rep=rep1&type=pdf)
- [Introduction in Chinese](https://ssarcandy.tw/2017/06/26/Coherent-Line-Drawing/)

> This paper presents a non-photorealistic rendering technique that
automatically generates a line drawing from a photograph.

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
I provided a build script that can use in Linux:

```sh
#   Usage: build.sh [options]
#   Options:
#     -h, --help        Display this help and exit
#     -c, --clean       Clean build
#     -d, --debug       Build with debug mode
#     -j, --jobs        Use N cores to build
$ ./build.sh
```

### Pre-Build Version

You can download pre-build version at [here](https://github.com/SSARCandy/Coherent-Line-Drawing/releases).  
Including Windows and macOS versions.

## Screenshot

![demo](./demo/1.JPG)

![demo](./demo/2.JPG)

![demo](./demo/3.JPG)
