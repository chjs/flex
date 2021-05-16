CFLAGS = -W -Wall -fPIC -O3
LDFLAGS = -ldl -lpthread -lpmem

TARGET = libflex.so

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	
clean :
	$(RM) $(OBJECTS) $(TARGET)
