MARCO = -DCL_TARGET_OPENCL_VERSION=210

ifndef CC
	CC = gcc
endif

CCFLAGS=-O3 -lm

LIBS = -lOpenCL

COMMON_DIR = ../../C_common

# OpenGL paths
glad_dir = /home/bmezhou/Downloads/glfw-3.3.4/deps/
glfw_dir = /home/bmezhou/Downloads/glfw-3.3.4/include/

glfw_lib = /home/bmezhou/Downloads/glfw-3.3.4/glfw_build/src

LDFLAGS="-Wl,-rpath,/home/bmezhou/Downloads/glfw-3.3.4/glfw_build/src"

# Change this variable to specify the device type
# to the OpenCL device type of choice. You can also
# edit the variable in the source.
ifndef DEVICE
	DEVICE = CL_DEVICE_TYPE_DEFAULT
endif

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)

ifeq ($(PLATFORM), Darwin)  
	LIBS = -framework OpenCL
endif

CCFLAGS += -D DEVICE=$(DEVICE)

vadd: vadd_c.c $(COMMON_DIR)/wtime.c $(COMMON_DIR)/device_info.c
	$(CC) $^ $(MARCO)  $(CCFLAGS) $(LIBS) -I $(COMMON_DIR) -I $(glad_dir) -I $(glfw_dir) -L $(glfw_lib) -lGL  $(LDFLAGS) -lglfw -o $@

vadd_pin: vadd_cPin.c $(COMMON_DIR)/wtime.c $(COMMON_DIR)/device_info.c
	$(CC) $^ $(CCFLAGS) $(LIBS) -I $(COMMON_DIR)   -o $@

clean:
	rm -f vadd
