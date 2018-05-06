# pipe-size-fuse

$ gcc -o pfuse pipe-size-fuse.c

$ ./pfuse -?
usage: ./pfuse -s bytes -L bytes
 -s -- max pipe size [kMG],
 -L -- max line size[kMG].

$ dd if=/dev/zero bs=1m count=6000|./pfuse -s1G  >/dev/null
Error. The pipe size 1074003968 exceeded the limit 1073741824 bytes

$ echo -e "rrrrrr\n\rffff"|./pfuse -L 2
Error. The line size  3 exceeded the limit 2 bytes
