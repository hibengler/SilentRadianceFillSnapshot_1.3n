rm -rf 2>/dev/null opus-1.1
tar -xzvf opus-1.1.tar.gz
cd opus-1.1
./configure
make -j 4
sudo make install
cp .libs/libopus.a ..
