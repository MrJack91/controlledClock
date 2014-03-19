#
# JD2XX Makefile
#

include Makefile.conf

JSRC = $(wildcard jd2xx/JD2XX*.java)
JOBJ = $(JSRC:%.java=%.class)

TARGETS = jd2xx/JD2XX.class src/jd2xx_JD2XX.h jd2xx.jar JD2XX.dll

INC = -I$(JDK)/include -I$(JDK)/include/win32 -I$(FTDI) -I$(FTDI)/$(ARCH) -I./src
DEF = -D_JNI_IMPLEMENTATION_
LIB = -L$(FTDI) -lftd2xx

JAVAH = $(JDK)/bin/javah -jni
JAVAC = $(JDK)/bin/javac
JAR = $(JDK)/bin/jar
JAVADOC = $(JDK)/bin/javadoc

JFLAGS = -O
CFLAGS = -O2 -g $(INC) $(DEF)
LDFLAGS = -shared -Wl,--kill-at

.PRECIOUS: %.class

all: $(TARGETS)

%.class: $(JSRC)
	$(JAVAC) $(JFLAGS) $^

%.h:
	$(JAVAH) -classpath . -d src $(notdir $(subst _,.,$*))

%.lst: %.o
	$(OBJDUMP) -dxStr $< > $@

%.dll: src/%.o
	$(CC) $(LDFLAGS) -o $(notdir $@) $^ $(LIB)

%.jar: $(JOBJ)
	$(JAR) cf $@ jd2xx/*

doc:
	$(JAVADOC) -d $@ jd2xx

clean:
	$(RM) $(TARGETS) jd2xx/JD2XX*.class src/jd2xx_JD2XX*.h

distclean: clean
	$(RM) jd2xx/*.bak src/*.bak *.bak
	$(RM) -r doc
