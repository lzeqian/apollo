CC=gcc
INCLIDE=.
CFLAGS= -Wall -std=c99
TARGET   = cygapollo.dll
LIBTARGET   = libapollo.dll.a
LIBPATH  = ./libs/
OBJS=global.o apolloclient.o
SRCS=global.c apolloclient.c

all:$(OBJS)
	$(CC) $(CFLAGS)  -shared -fPIC -Wl,--out-implib,$(LIBTARGET) -o $(TARGET) $(OBJS) -lcurl -ljson-c
	mkdir -p $(LIBPATH)
	mv $(TARGET) $(LIBPATH)
	mv $(LIBTARGET) $(LIBPATH)
#$^代表所有依赖也就是上面的main.o mmath.o syso.o $@代表目标也就是上面main
$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) -fPIC -I$(INCLIDE) -c $^
install:
	cp $(LIBPATH)$(TARGET) /bin
	cp $(LIBPATH)$(LIBTARGET) /lib
	mkdir -p /usr/include/apollo
	cp ./*.h /usr/include/apollo
uninstall:
	rm -rf /bin/$(TARGET)
	rm -rf /lib/$(LIBTARGET)
	rm -rf /usr/include/apollo
clean:
	rm -rf *.o
	rm -f $(LIBPATH)*
