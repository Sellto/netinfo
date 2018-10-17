testing: test
test:test.c

all: netinfo
netinfo: netinfo.c

clean:
	rm -f netinfo *.o
	rm -f testing *.o
