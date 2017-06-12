CC=gcc
CXX=g++
CFLAGS+=-O3

CPPFLAGS+= \
$(CFLAGS) \
-std=gnu++0x \
-Wwrite-strings \
-fpermissive \
-finput-charset=UTF-8 -fexec-charset=UTF-8 \
-I../pjproject/pjlib/include -I../pjproject/pjlib-util/include -I../pjproject/pjnath/include -I../pjproject/pjmedia/include -I../pjproject/pjsip/include -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1

LDFLAGS= \
-L../pjproject/pjlib/lib -L../pjproject/pjlib-util/lib -L../pjproject/pjnath/lib -L../pjproject/pjmedia/lib -L../pjproject/pjsip/lib -L../pjproject/third_party/lib            -lpjsua2-armv7l-unknown-linux-gnueabi -lstdc++ -lpjsua-armv7l-unknown-linux-gnueabi -lpjsip-ua-armv7l-unknown-linux-gnueabi -lpjsip-simple-armv7l-unknown-linux-gnueabi -lpjsip-armv7l-unknown-linux-gnueabi -lpjmedia-codec-armv7l-unknown-linux-gnueabi -lpjmedia-armv7l-unknown-linux-gnueabi -lpjmedia-videodev-armv7l-unknown-linux-gnueabi -lpjmedia-audiodev-armv7l-unknown-linux-gnueabi -lpjmedia-armv7l-unknown-linux-gnueabi -lpjnath-armv7l-unknown-linux-gnueabi -lpjlib-util-armv7l-unknown-linux-gnueabi  -lsrtp-armv7l-unknown-linux-gnueabi -lresample-armv7l-unknown-linux-gnueabi -lgsmcodec-armv7l-unknown-linux-gnueabi -lspeex-armv7l-unknown-linux-gnueabi -lilbccodec-armv7l-unknown-linux-gnueabi -lg7221codec-armv7l-unknown-linux-gnueabi  -lpj-armv7l-unknown-linux-gnueabi -lssl -lcrypto -luuid -lm -lrt -lpthread  -lasound

OBJ = \
FHC_SipCmd.cpp \
XLException.cpp \
XLStringUtil.cpp \
main.o

all: fhc_sipcmd
fhc_sipcmd: $(OBJ)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm *.o

