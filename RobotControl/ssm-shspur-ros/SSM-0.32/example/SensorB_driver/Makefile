TARGET = SensorB_driver

CC = gcc
LIBS =  -lm -lSSM -lpthread
CFLAGS = -g -Wall -I../../include -I../ -L../../lib 

OBJS = $(TARGET).o

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET) 

clean:
	rm -f *.o *~ def/*~ core
install:
	cp $(TARGET) ../examples/
