MARCO = -DCL_TARGET_OPENCL_VERSION=220

CC  = gcc
CPP = g++

LIBS = -lOpenCL

oclBandwidthTest: oclBandwidthTest.cpp
	$(CPP) $^ $(LIBS) -o $@