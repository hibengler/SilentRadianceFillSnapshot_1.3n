
	CFLAGS = -I..
	
system:	../png_stuff.a

../png_stuff.a: png_stuff.a
	cp png_stuff.a ..	
	
png_stuff.a:	  png.o \
                                  pngerror.o \
                                  pngget.o \
                                  pngmem.o \
                                  pngpread.o \
                                  pngread.o \
                                  pngrio.o \
                                  pngrtran.o \
                                  pngrutil.o \
                                  pngset.o \
                                  pngtrans.o \
                                  pngwio.o \
                                  pngwrite.o \
                                  pngwtran.o \
                                  pngwutil.o
	ar rcs png_stuff.a	 png.o \
                                  pngerror.o \
                                  pngget.o \
                                  pngmem.o \
                                  pngpread.o \
                                  pngread.o \
                                  pngrio.o \
                                  pngrtran.o \
                                  pngrutil.o \
                                  pngset.o \
                                  pngtrans.o \
                                  pngwio.o \
                                  pngwrite.o \
                                  pngwtran.o \
                                  pngwutil.o

