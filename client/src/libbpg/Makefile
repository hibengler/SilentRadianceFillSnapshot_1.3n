# libbpg Makefile
# 
# Compile options:
#
prefix=/usr/local


#################################

CROSS_PREFIX:=
EXE:=

CC=$(CROSS_PREFIX)gcc
CXX=$(CROSS_PREFIX)g++
AR=$(CROSS_PREFIX)ar

PWD:=$(shell pwd)

CFLAGS:=-Os -Wall -MMD -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -fno-math-errno -fno-signed-zeros -fno-tree-vectorize -fomit-frame-pointer
CFLAGS+=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT
CFLAGS+=-I. -I../../libpng
CFLAGS+=-DCONFIG_BPG_VERSION=\"$(shell cat VERSION)\"


LDFLAGS=-g
ifdef CONFIG_APPLE
LDFLAGS+=-Wl,-dead_strip
else
LDFLAGS+=-Wl,--gc-sections
endif
CFLAGS+=-g
CXXFLAGS=$(CFLAGS)

PROGS=bpgdec$(EXE) 

all: $(PROGS) ../libbpg.h

LIBBPG_OBJS:=$(addprefix libavcodec/, \
hevc_cabac.o  hevc_filter.o  hevc.o         hevcpred.o  hevc_refs.o\
hevcdsp.o     hevc_mvs.o     hevc_ps.o   hevc_sei.o\
utils.o cabac.o golomb.o videodsp.o )
LIBBPG_OBJS+=$(addprefix libavutil/, mem.o buffer.o log2_tab.o frame.o pixdesc.o md5.o )
LIBBPG_OBJS+=libbpg.o

LIBBPG_JS_OBJS:=$(patsubst %.o, %.js.o, $(LIBBPG_OBJS)) tmalloc.js.o

LIBBPG_JS8_OBJS:=$(patsubst %.o, %.js8.o, $(LIBBPG_OBJS)) tmalloc.js8.o

LIBBPG_JS8A_OBJS:=$(patsubst %.o, %.js8a.o, $(LIBBPG_OBJS)) tmalloc.js8a.o

$(LIBBPG_OBJS): CFLAGS+=-D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -DHAVE_AV_CONFIG_H -std=c99 -D_GNU_SOURCE=1 -DUSE_VAR_BIT_DEPTH -DUSE_PRED

$(LIBBPG_JS_OBJS): EMCFLAGS+=-D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -DHAVE_AV_CONFIG_H -std=c99 -D_GNU_SOURCE=1 -DUSE_VAR_BIT_DEPTH

$(LIBBPG_JS8_OBJS): EMCFLAGS+=-D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -DHAVE_AV_CONFIG_H -std=c99 -D_GNU_SOURCE=1

$(LIBBPG_JS8A_OBJS): EMCFLAGS+=-D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 -DHAVE_AV_CONFIG_H -std=c99 -D_GNU_SOURCE=1 -DUSE_PRED

../libbpg.h: libbpg.h
	cp libbpg.h ..

ifdef CONFIG_APPLE
LIBS:=
else
LIBS:=-lrt
endif # !CONFIG_APPLE 
LIBS+=-lm -lpthread

BPGDEC_LIBS:= $(LIBS)
BPGENC_LIBS+= -ljpeg $(LIBS)


bpgenc.o: CFLAGS+=-Wno-unused-but-set-variable

libbpg.a: $(LIBBPG_OBJS) 
	$(AR) rcs $@ $^

bpgdec$(EXE): bpgdec.o libbpg.a
	$(CC) $(LDFLAGS) -o $@ $^ $(BPGDEC_LIBS)

size:
	strip bpgdec
	size bpgdec libbpg.o libavcodec/*.o libavutil/*.o | sort -n
	gzip < bpgdec | wc


CLEAN_DIRS=libavcodec libavutil 
     

clean: 
	rm -f $(PROGS) *.o *.a *.d *~ $(addsuffix /*.o, $(CLEAN_DIRS)) \
          $(addsuffix /*.d, $(CLEAN_DIRS)) $(addsuffix /*~, $(CLEAN_DIRS)) \
          $(addsuffix /*.a, $(CLEAN_DIRS))

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.js.o: %.c
	$(EMCC) $(EMCFLAGS) -c -o $@ $<

%.js8.o: %.c
	$(EMCC) $(EMCFLAGS) -c -o $@ $<

%.js8a.o: %.c
	$(EMCC) $(EMCFLAGS) -c -o $@ $<

-include $(wildcard *.d)
-include $(wildcard libavcodec/*.d)
-include $(wildcard libavutil/*.d)
-include $(wildcard jctvc/*.d)
-include $(wildcard jctvc/TLibEncoder/*.d)
-include $(wildcard jctvc/TLibVideoIO/*.d)
-include $(wildcard jctvc/TLibCommon/*.d)
-include $(wildcard jctvc/libmd5/*.d)
