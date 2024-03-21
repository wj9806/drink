CC = gcc
CFLAGS = -M -g -DDEBUG -lm -Wall -I vm -I include -I parser -I cli -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers -Wunused-parameter -fgnu89-inline
TARGET = target
DIRS =  include vm parser cli
CFILES = $(foreach dir, $(DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o,$(CFILES))
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)
clean:
	-$(RM) $(TARGET) $(OBJS)
r: clean $(TARGET)