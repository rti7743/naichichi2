all:
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -c -o base64.o base64.cpp
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -c -o HMAC_SHA1.o HMAC_SHA1.cpp
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -I../include/ -c -o liboauthcpp.o liboauthcpp.cpp
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -c -o SHA1.o SHA1.cpp
	g++ $(CFLAGS) -std=gnu++0x -D__BBL_USING_STATIC_TABLE__ -c -o urlencode.o urlencode.cpp
	ar rcv liboauthcpp.a *.o
	ranlib liboauthcpp.a
clean:
	/bin/rm *.o *.bak *~ core
	/bin/rm liboauthcpp.a
