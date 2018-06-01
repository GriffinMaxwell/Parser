#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE (1024)

static FILE *stream;
static char buf[BUF_SIZE];

void main(int argc, char *argv[])
{
   if (argc > 2)
   {
      printf("Expected 1 or 0 arguments.\n");
      printf("Usage: %s [filename]\n", argv[0]);
   }
   else if (argc == 2)
   {
      stream = fopen(argv[1], "r");
   }
   else
   {
      stream = stdin;
   }

   while(fgets(buf, BUF_SIZE, stream))
   {
      printf("> %s", buf);
      if(stream == stdin)
      {
         fflush(stdout);
      }

      // Parse the buffer and return the list of tokens
      // Print the list of tokens
   }

   if(stream != stdin)
   {
      fclose(stream);
   }
}
