#!/bin/bash
export x=/tmp/x$$
rm -rf 2>/dev/null $x
find /var/www -type d -name radio >$x
awk <$x '{print "mount -t ramfs ramfs " $0}' | bash
awk <$x '{print "chmod 777 " $0}' | bash
awk <$x '{print "echo 404 > " $0 "/index.html" }'
