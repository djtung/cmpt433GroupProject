TARGET = main

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)

# Change the compiler if you want to build to the BBG
#CC_C= gcc
CC_C= arm-linux-gnueabihf-gcc
#CCFLAGS= -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread
CCFLAGS= -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: main
	cp $(TARGET) $(OUTDIR)/$(TARGET)

%.o : %.c
	$(CC_C) -c $(CCFLAGS) $<

main: main.o timeModule.o audioModule.o
	$(CC_C) $(CCFLAGS) -o $(TARGET) main.o timeModule.o audioModule.o $(LFLAGS) -lpthread -lasound
	mkdir -p $(PUBDIR)/wav-files
	cp wav-files/* $(PUBDIR)/wav-files
	cp test.txt $(OUTDIR)

clean:
	rm $(TARGET)
	rm *.o