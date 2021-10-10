GCC = gcc
CC  = g++

glad_dir = /home/bmezhou/Downloads/glfw-3.3.4/deps/
glfw_dir = /home/bmezhou/Downloads/glfw-3.3.4/include/

glfw_lib = /home/bmezhou/Downloads/glfw-3.3.4/glfw_build/src

LDFLAGS="-Wl,-rpath,/home/bmezhou/Downloads/glfw-3.3.4/glfw_build/src"

glTest: glTest.c 
	$(CC) -O3  $^ -I $(glad_dir) -I $(glfw_dir) -L $(glfw_lib) $(LDFLAGS) -lglfw -o $@