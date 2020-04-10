CC=gcc
INCLIDE=.
CFLAGS= -Wall -std=c99
TARGET   = libapollo.so
LIBPATH  = ./libs/
OBJS=global.o apolloclient.o apollo.o
SRCS=global.c apolloclient.c apollo.c

all:$(OBJS)
	$(CC) $(CFLAGS)  -shared -fPIC  -o $(TARGET) $(OBJS) -lcurl -ljson-c
	mkdir -p $(LIBPATH)
	mv $(TARGET) $(LIBPATH)
#$^代表所有依赖也就是上面的main.o mmath.o syso.o $@代表目标也就是上面main
$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) -fPIC -I$(INCLIDE) -c $^
install:
	cp $(LIBPATH)$(TARGET) /lib64
	mkdir -p /usr/include/apollo
	cp ./*.h /usr/include/apollo
uninstall:
	rm -rf /lib64/$(TARGET)
	rm -rf /usr/include/apollo
clean:
	rm -rf *.o
	rm -f $(LIBPATH)*
