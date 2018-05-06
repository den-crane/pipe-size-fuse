#include <stdio.h>
#define BLOCK_SIZE 262144 // 256k buffer

int main() {
  char buffer[BLOCK_SIZE];
  unsigned long long int ms = 5368709120; //5Gb
  unsigned long long int pipe_size = 0;
  for(;;) {
      size_t bytes = fread(buffer, 1, BLOCK_SIZE, stdin);
      pipe_size += bytes;
      if (pipe_size > ms) {
        fprintf( stderr, "Error. The pipe size %llu exceeded the limit %llu bytes\n", pipe_size, ms);
        return 1;
      }

      fwrite(buffer, 1, bytes, stdout);
      fflush(stdout);
      if (bytes < BLOCK_SIZE)
          if (feof(stdin)) {
            break;
          }
  }
  return 0;
}
