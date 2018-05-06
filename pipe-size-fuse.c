#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#define BLOCK_SIZE 262144 // 256k buffer

#define t_ulli unsigned long long int


t_ulli parse_human_readable_byte_size(char *s) {
  char *endp = s;
  int sh;
  int errno = 0;
  t_ulli x = strtoull(s, &endp, 10);
  //if (errno || endp == s) goto error;
  switch(*endp) {
    case 'k': sh=10; break;
    case 'M': sh=20; break;
    case 'G': sh=30; break;
    case 0: sh=0; break;
    //default: goto error;
  }
  //if (x > SIZE_MAX>>sh) goto error;
  x <<= sh;
  return x;
}

int parseargs(int argc, char **argv, t_ulli *svalue, t_ulli *lvalue) {
  int c;

  while ((c = getopt (argc, argv, "?s:L:")) != -1)
    switch (c)
      {
      case 's':
        *svalue = parse_human_readable_byte_size(optarg);
        //*svalue = atoll(optarg);
        break;
      case 'L':
        *lvalue = parse_human_readable_byte_size(optarg);
        //*lvalue = atoll(optarg);
        break;
      case '?':
        fprintf (stderr, "usage: %s -s bytes -L bytes \n ", argv[0]);
        fprintf (stderr, "-s -- max pipe size [kMG], \n -L -- max line size[kMG].\n");
        return 1;
      default:
        abort ();
      }

  return 0;

}


int main(int argc, char **argv)
{
  t_ulli pipe_fuse_size = -1;
  t_ulli line_fuse_size = -1;

  if (parseargs(argc, argv, &pipe_fuse_size, &line_fuse_size) != 0) return 1;


  char buffer[BLOCK_SIZE];

  t_ulli pipe_size = 0;
  t_ulli line_size = 0;

  for(;;) {
    size_t bytes = fread(buffer, 1, BLOCK_SIZE, stdin);
    pipe_size += bytes;
    if (pipe_size > pipe_fuse_size) {
      fprintf( stderr, "Error. The pipe size %llu exceeded the limit %llu bytes\n", pipe_size, pipe_fuse_size);
      return 1;
    }

    if (line_fuse_size != -1) {
      int i;
      for (i = 0; i < bytes; ++i) {
        line_size ++;
        if (line_size > line_fuse_size) {
          fprintf( stderr, "Error. The line size  %llu exceeded the limit %llu bytes\n", line_size, line_fuse_size);
          return 1;
        }
        if ('\r' == buffer[i] || '\n' == buffer[i]) line_size = 0;
      }
    }

    fwrite(buffer, 1, bytes, stdout);
    fflush(stdout);
    if (bytes < BLOCK_SIZE)
      if (feof(stdin)) break;

  }
  return 0;
}
