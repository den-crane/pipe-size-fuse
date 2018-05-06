#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BLOCK_SIZE 262144 // 256k buffer

#define t_ulli unsigned long long int
#define new_max(x,y) ((x) >= (y)) ? (x) : (y)

t_ulli parse_human_readable_byte_size(char *s) {
  char *endp;// = s;
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

int parseargs(int argc, char **argv, t_ulli *svalue, t_ulli *lvalue, int *vinfo) {
  int c;

  while ((c = getopt (argc, argv, "v?s:L:")) != -1)
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
      case 'v':
        *vinfo = 1;
        break;
      case '?':
        fprintf (stderr, "usage: %s -s bytes -L bytes \n", argv[0]);
        fprintf (stderr, " -s -- max pipe size [kMG], \n"
                         " -L -- max line size [kMG]"
                         " (eol is any cobmination of \\r and \\n, all other bytes"
                         " don't matter). Makes a pipe much slower!.\n"
                         " -v -- show pipe statistics (stderr).\n");
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
  int vinfo = 0;
  if (parseargs(argc, argv, &pipe_fuse_size, &line_fuse_size, &vinfo) != 0) return 1;


  char buffer[BLOCK_SIZE];

  t_ulli pipe_size = 0;
  t_ulli line_size = 0;
  t_ulli line_number = 0;

  t_ulli max_line_size = 0;
  //char last_char;

  for(;;) {
    size_t bytes = fread(buffer, 1, BLOCK_SIZE, stdin);
    pipe_size += bytes;
    if (pipe_size > pipe_fuse_size) {
      fprintf(stderr, "Error. The pipe size %llu exceeded the limit %llu bytes\n", pipe_size, pipe_fuse_size);
      return 1;
    }

    if (line_fuse_size != -1 || vinfo) {
      for (int i = 0; i < bytes; ++i) {
        line_size ++;
        if ('\r' == buffer[i] || '\n' == buffer[i]) {
          //if (! ('\r' == last_char && '\n' == buffer[i]))
          line_number ++; //if : check for two character winEOL (\r\n)
          max_line_size = new_max(max_line_size, line_size - 1);
          line_size = 0;
        }
        //last_char = buffer[i];
        if (line_size > line_fuse_size) {
          fprintf(stderr, "Error. Line %llu exceeded the limit %llu bytes\n", line_number+1, line_fuse_size);
          return 1;
        }
      }
    }

    fwrite(buffer, 1, bytes, stdout);
    fflush(stdout);
    if (bytes < BLOCK_SIZE)
      if (feof(stdin)) {
        max_line_size = new_max(max_line_size, line_size);
        break;
      }

  }
  if (vinfo) {
    fprintf(stderr, "Pipe fuse size: %llu \n", pipe_fuse_size);
    fprintf(stderr, "Line fuse size: %llu \n", line_fuse_size);
    fprintf(stderr, "Max line: %llu \n", max_line_size);
    fprintf(stderr, "Lines: %llu \n", line_number);
  }
  return 0;
}
