<!--

Introduction (?)
============

This set of freely available OpenCL exercises and solutions,
together with the [HandsOnOpenCL slides](https://github.com/HandsOnOpenCL/Lecture-Slides)
have been created by Simon McIntosh-Smith and Tom Deakin from the
University of Bristol in the UK, with financial support from the
Khronos Initiative for Training and Education ([KITE](http://kite.khronos.org/en/opencl))
to promote the use of open standards. 

[Simon McIntosh-Smith](http://www.cs.bris.ac.uk/home/simonm/) is
one of the foremost OpenCL trainers in the world, having taught
the subject since 2009. He has run many OpenCL training courses
at conferences such as SuperComputing and HiPEAC, and has provided
OpenCL training for the UK's national supercomputing service and
for the Barcelona Supercomputing Center. With OpenCL training
experience ranging from half day on-site introductions within
companies, to two-day intensive hands-on workshops for undergraduates,
Simon can provide customized OpenCL training to meet your needs.
Get in touch if you'd like to know more: <simonm at cs.bris.ac.uk>.

For more about the authors, please visit [Simon's home page](http://www.cs.bris.ac.uk/home/simonm/) or [Tom's home page](http://www.tomdeakin.com).

Source Code for the Exercises and Solutions
==========================================

These examples together with the [HandsOnOpenCL slides](https://github.com/HandsOnOpenCL/Lecture-Slides) are released under the ["attribution CC BY" creative commons license](http://creativecommons.org/licenses/by/3.0/). In other words, you can use these in any way you see fit, including commercially, but please retain an attribution for the original authors, Simon McIntosh-Smith and Tom Deakin.

Getting started
---------------

Please download a tarball from [Releases](https://github.com/HandsOnOpenCL/Exercises-Solutions/releases), or checkout the repository using git with the following command:

`git clone git://github.com/HandsOnOpenCL/Exercises-Solutions.git`

Found any issues or have some comments? Please submit a bug report in the
[Issue tab](https://github.com/HandsOnOpenCL/Exercises-Solutions/issues).

Pre-requisites
--------------

* OpenCL 1.1 (or greater)
* Python 2.7 (or greater)
* C99 compiler (we use gcc) with OpenMP support (used for timing the runs [optional])
* C++11 compiler (we use g++ or clang, also tested with Intel's icc)

Need help setting up OpenCL?
Check out the first section in the lecture slides for information
about setting up OpenCL on Linux for AMD (CPU, GPU, APU),
Intel CPUs and NVIDIA GPUs.

Building
--------

We assume here that your current working directory is the location of the source code;
e.g. `/path/to/Exercises-Solutions/Solutions/Exercise04/C`

**Python**

Just run `python source.py` to run the code.

**C**

You must first run `make` to build the binary.
We assume that your environment is set up to find the OpenCL library; if you have trouble
try `export CPATH=/path/to/OpenCL/include` and `export LD_LIBRARY_PATH=/path/to/OpenCL/lib`.

You can also run `make` in the Examples/ and Solutions/ high-level directory;
this calls all the sub-directory make files so all the examples can be built in one command.
This also builds all the C++ examples.

Define the variable `DEVICE` in the Makefiles to be one of the OpenCL device types to vary the device type the C applications use.
This can be done easily in the two global Makefiles found in the Exercises and Solutions directories.
To use a GPU, for example, change the line `DEVICE = CL_DEVICE_TYPE_DEFAULT` to `DEVICE=CL_DEVICE_TYPE_GPU`.

Note: you can also edit each of the source files to use a specific device type, but we would recommend using the global Makefile method above.

Define the variable `CC` to change the C compiler used.
By default, this is set to gcc for all platforms.

**C++**

You must first run `make` to build the binary.
We assume that your environment is set up to find the OpenCL library.

You can also run `make` in the Examples/ and Solutions/ high-level directory;
this calls all the sub-directory make files so all the examples can be built in one command.
This also builds all the C examples.

Define the variable `DEVICE` in the Makefiles to be one of the OpenCL device types to vary the device type the C++ applications use.
This can be done easily in the two global Makefiles found in the Exercises and Solutions directories.
To use a GPU, for example, change the line `DEVICE = CL_DEVICE_TYPE_DEFAULT` to `DEVICE=CL_DEVICE_TYPE_GPU`.

Note: you can also edit each of the source files to use a specific device type, but we would recommend using the global Makefile method above.

Define the variable `CPPC` to change the C compiler used.
By default, this is set to g++ on Linux, and clang++ on OS X.

Directory structure
-------------------

The Exercises directory contains all the code
needed to be handed out at the start of the
tutorial for the exercises to be completed.

The Solutions directory contains sample code
providing an example implementation which
solves the exercises in the lecture notes.

Within both of the Exercises and Solutions
directories, there is one subdirectory per
exercise. Within each exercise subdirectory,
there are further subdirectories for each
implementation: C, C++ and Python.
-->

# Some tips when configuring AMD Radeon Pro graphic cards

<br/>

## AMDGPU-Pro install with opencl

When using an AMD Radeon Pro WX2100 graphic card, run

```
./amdgpu-pro-install -y --opencl=legacy
```

Note that use a suitable AMD driver.

<br/>

## Verify OpenCL installation

Run `clinfo`

<br/>

## How to ensure that you are using amdgpu-pro

Run `glxinfo | grep "OpenGL vendor string" | cut -f2 -d":" | xargs`, or

run `glmark2`

https://wiki.archlinux.org/title/AMDGPU_PRO#How_to_ensure_you_are_using_AMDGPU-PRO_driver

<br/>

## Error: /usr/bin/ld: cannot find -lOpenCL

run ```sudo ln -s /usr/lib/x86_64-linux-gnu/libOpenCL.so.1 /usr/lib/libOpenCL.so```

https://askubuntu.com/questions/1007591/usr-bin-ld-cannot-find-lopencl


<br/>

## List package

Run `dpkg -l` (see `dpkg --help` for a reference)

<br/>

## Cannot install amdgpu drivers on Ubuntu 20.04.1 (5.4.0-56-generic)


To install 5.4.0-54 do next:

```
sudo apt install linux-image-5.4.0-54-generic linux-headers-5.4.0-54-generic linux-modules-extra-5.4.0-54-generic
```

then reboot to 5.4.0-54

then

```
sudo apt remove linux-image-5.4.0-56-generic linux-image-unsigned-5.4.0-56-generic
```

(unsigned mentioned here because it tryed to install it for me by some reason.. it just helped)

then do

```
sudo apt autoremove
```

then go to `/usr/src` and remove `linux-headers-5.4.0-56-generic` and `linux-headers-5.4.0-56` (if any of them are there)

now you can do

```
sudo ./amdgpu-install -y
```

https://community.amd.com/t5/drivers-software/can-t-install-amdgpu-drivers-on-ubuntu-20-04-1-5-4-0-56-generic/td-p/426676


<br/>

# <GL/gl.h> problem

run 'apt install mesa-common-dev'

