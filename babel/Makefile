all:
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -c -o babel.o babel.cpp
	ar rcv babel.a babel.o
	ranlib babel.a
clean:
	/bin/rm *.o *.bak *~ core
	/bin/rm babel.a
