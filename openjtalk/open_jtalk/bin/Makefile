CC=gcc
CXX=g++
CFLAGS=$(FPUHINT)

all: open_jtalkdll.a

open_jtalkdll.a:
	/bin/rm -f *.o
	/bin/rm -f *.a

	gcc -DHAVE_CONFIG_H -I. -I../mecab -I ../text2mecab -I ../mecab/src -I ../mecab2njd -I ../njd -I ../njd_set_pronunciation -I ../njd_set_digit -I ../njd_set_accent_phrase -I ../njd_set_accent_type -I ../njd_set_unvoiced_vowel -I ../njd_set_long_vowel -I ../njd2jpcommon -I ../jpcommon -I  `pwd`/../../hts_engine_API/include     -O3 -Wall -MT open_jtalk.o -MD -MP -MF .deps/open_jtalk.Tpo -c -o open_jtalkdll.o open_jtalkdll.cpp $(FPUHINT)

	ar -x ../text2mecab/libtext2mecab.a
	ar -x ../mecab/src/libmecab.a
	ar -x ../mecab2njd/libmecab2njd.a
	ar -x ../njd/libnjd.a
	ar -x ../njd_set_pronunciation/libnjd_set_pronunciation.a
	ar -x ../njd_set_digit/libnjd_set_digit.a
	ar -x ../njd_set_accent_phrase/libnjd_set_accent_phrase.a
	ar -x ../njd_set_accent_type/libnjd_set_accent_type.a
	ar -x ../njd_set_unvoiced_vowel/libnjd_set_unvoiced_vowel.a
	ar -x ../njd_set_long_vowel/libnjd_set_long_vowel.a
	ar -x ../njd2jpcommon/libnjd2jpcommon.a
	ar -x ../jpcommon/libjpcommon.a
	ar -x `pwd`/../../hts_engine_API/lib/libHTSEngine.a

	ar rcu open_jtalkdll.a *.o
	ranlib open_jtalkdll.a

clean:
	/bin/rm -f *.o
	/bin/rm -f *.a
	
	


/usr/arm-linux-gnueabi/bin/ar -x  src/alisp/.libs/libalisp.a
/usr/arm-linux-gnueabi/bin/ar -x  src/compat/.libs/libcompat.a
/usr/arm-linux-gnueabi/bin/ar -x  src/control/.libs/libcontrol.a
/usr/arm-linux-gnueabi/bin/ar -x  src/hwdep/.libs/libhwdep.a
/usr/arm-linux-gnueabi/bin/ar -x  src/mixer/.libs/libmixer.a
/usr/arm-linux-gnueabi/bin/ar -x  src/pcm/.libs/libpcm.a
/usr/arm-linux-gnueabi/bin/ar -x  src/rawmidi/.libs/librawmidi.a
/usr/arm-linux-gnueabi/bin/ar -x  src/seq/.libs/libseq.a
/usr/arm-linux-gnueabi/bin/ar -x  src/timer/.libs/libtimer.a
/usr/arm-linux-gnueabi/bin/ar -x  src/ucm/.libs/libucm.a

/usr/arm-linux-gnueabi/bin/ar rcu alsa-lib.a *.o
/usr/arm-linux-gnueabi/bin/ranlib alsa-lib.a
