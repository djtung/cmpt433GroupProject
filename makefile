# Change the compiler if you want to build to the BBG

CC_C= gcc
#CC_C= arm-linux-gnueabihf-gcc
CCFLAGS= -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

all: main
#cp main $(HOME)/cmpt433/public/myApps/

%.o : %.c
	$(CC_C) -c $(CCFLAGS) $<

main: main.o timeModule.o
	$(CC_C) $(CCFLAGS) -o main main.o timeModule.o

clean:
	rm main
	rm *.o