rm -rf 2>/dev/null libpng-1.6.23
tar -xzvf libpng-1.6.23.tar.gz
cd libpng-1.6.23
./configure
make -j 4
cp .libs/libpng16.a ..
