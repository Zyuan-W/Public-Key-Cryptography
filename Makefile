CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra $(shell pkg-config -- cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)
COMMON_OBJS = rsa.o randstate.o numtheory.o

all : keygen encrypt decrypt

keygen: keygen.o $(COMMON_OBJS)
		$(CC) -o keygen keygen.o $(COMMON_OBJS) $(LFLAGS)

encrypt: encrypt.o $(COMMON_OBJS)
		$(CC) -o encrypt encrypt.o $(COMMON_OBJS) $(LFLAGS)

decrypt: decrypt.o $(COMMON_OBJS)
		$(CC) -o decrypt decrypt.o $(COMMON_OBJS) $(LFLAGS)

keygen.o: keygen.c numtheory.h randstate.h rsa.h
		$(CC) $(CFLAGS) -c keygen.c

rsa.o: rsa.c rsa.h numtheory.h randstate.h
		$(CC) $(CFLAGS) -c rsa.c

randstate.o: randstate.c randstate.h
		$(CC) $(CFLAGS) -c randstate.c

numtheory.o: numtheory.c numtheory.h randstate.h
		$(CC) $(CFLAGS) -c numtheory.c

clean:
		rm -f *.o keygen encrypt decrypt

format:
		clang-format -i -style=file *.c *.h

