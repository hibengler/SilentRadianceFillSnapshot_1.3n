rm -rf 2>/dev/null libbpg-0.9.8
tar -xzvf libbpg-0.9.8.tar.gz
cd libbpg-0.9.8
sed -i "s/CFLAGS+=-I\.$/ CFLAGS\+\=-I\. -I\.\.\/libpng-1.6.23/g" Makefile
sed -i "s/CFLAGS+=-I\.\/x265\/source/CFLAGS+=-I\.\/x265\/source -I\.\.\/libpng-1.6.23/g" Makefile
sed -i "s/-lpng/\.\.\/libpng16.a -lz/g" Makefile
make
# make -j 4 fails
echo install the binaries if you have root
sudo make install
cp bpgdec bpgenc bpgview ../..
cp libbpg.a libbpg.d bpgdec bpgenc bpgview ..
