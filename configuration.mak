CFLAGS:=$(CFLAGS) -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
LDFLAGS:=$(LDFLAGS) -L/usr/lib -lSDL -lpthread
