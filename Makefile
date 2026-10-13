CC = gcc
CFLAGS = -Wall -Wextra -g -I./cJSON
LDLIBS = -lcurl

OBJS = src/main.o src/cli.o src/llm.o cdotenv/cdotenv.o \
       cJSON/cJSON.o cJSON/cJSON_Utils.o

commando: $(OBJS)
	$(CC) $(OBJS) $(LDLIBS) -o commando

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f chat $(OBJS)

.PHONY: clean
