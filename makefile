#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall -fexceptions `pkg-config /usr/local/lib/pkgconfig/opencv4.pc --cflags   `
RESINC = 
LIBDIR = 
LIB = -lmosquittopp
LDFLAGS = `pkg-config /usr/local/lib/pkgconfig/opencv4.pc --libs libavformat libavcodec libavutil libswscale`

INC_DEBUG = $(INC) -Iinclude
CFLAGS_DEBUG = $(CFLAGS) -std=c++11 -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/ffmpeg

INC_RELEASE = $(INC) -Iinclude
CFLAGS_RELEASE = $(CFLAGS) -O2 -std=c++11
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/ffmpeg

OBJ_DEBUG = $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/src/base64.o $(OBJDIR_DEBUG)/src/mqtt.o $(OBJDIR_DEBUG)/src/yolodet.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/src/base64.o $(OBJDIR_RELEASE)/src/mqtt.o $(OBJDIR_RELEASE)/src/yolodet.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/src/base64.o: src/base64.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/base64.cpp -o $(OBJDIR_DEBUG)/src/base64.o

$(OBJDIR_DEBUG)/src/mqtt.o: src/mqtt.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/mqtt.cpp -o $(OBJDIR_DEBUG)/src/mqtt.o

$(OBJDIR_DEBUG)/src/yolodet.o: src/yolodet.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/yolodet.cpp -o $(OBJDIR_DEBUG)/src/yolodet.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/src

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/src/base64.o: src/base64.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/base64.cpp -o $(OBJDIR_RELEASE)/src/base64.o

$(OBJDIR_RELEASE)/src/mqtt.o: src/mqtt.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/mqtt.cpp -o $(OBJDIR_RELEASE)/src/mqtt.o

$(OBJDIR_RELEASE)/src/yolodet.o: src/yolodet.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/yolodet.cpp -o $(OBJDIR_RELEASE)/src/yolodet.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

