rm -rf 2>/dev/null wavtools-0.2.1
tar -xzvf wavtools-0.2.1.tar.gz
cd wavtools-0.2.1
make -j 4
cp libwav.a ..
