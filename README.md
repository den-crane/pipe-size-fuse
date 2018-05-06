# pipe-size-fuse
This small app allows to limit pipe size, the same as "cstream -n" or "pv --stop-at-size --size" but it raise an error (exit 1).
Also it can raise an error on long strings (EOL).
```
$ gcc -o pfuse pipe-size-fuse.c

$ ./pfuse -?
usage: ./pfuse -s bytes -L bytes
 -s -- max pipe size [kMG],
 -L -- max line size [kMG] (eol is any cobmination of \r and \n, all other bytes don't matter). Makes a pipe much slower!.
 -v -- show pipe statistics (stderr).

$ dd if=/dev/zero bs=1m count=6000|./pfuse -s1G  >/dev/null
Error. The pipe size 1074003968 exceeded the limit 1073741824 bytes

$ echo -e "rrrrrr\n\rffff"|./pfuse -L 2
Error. Line 1 exceeded the limit 2 bytes

$ cat test.log|./pfuse -s 20G -L 5M -v >/dev/null
Pipe fuse size: 21474836480
Line fuse size: 5242880
Max line: 14410
Lines: 1292
```

-L works up to 10 times slower: 
```
$ time cat ~/Downloads/ubuntu-18.04-desktop-amd64.iso | ./pfuse -s 20G >/dev/null
real	0m0.871s
user	0m0.053s
sys	0m0.881s

$ time cat ~/Downloads/ubuntu-18.04-desktop-amd64.iso | ./pfuse -s 20G -L 10G >/dev/null
real	0m6.591s
user	0m5.082s
sys	0m1.234s
```
