#include <stdio.h>

#define LINELEN 20

int main(int argc, char *argv[])
{
  int ch;
  int pos=1;

  if(argc != 4) {
    printf("%s - convert binary file to C code\n\nusage: %s variable inputfile outputfile\n",argv[0],argv[0]);
    return -1;
  }

  FILE *fi = fopen(argv[2],"rb");
  FILE *fo = fopen(argv[3],"wt");
  if(!fi ||!fo) {
    printf("Error opening files\n");
    return -1;
  }
  fprintf(fo, "unsigned char v_mpg_%s[] = \n   \"", argv[1]);
  while(EOF != (ch = fgetc(fi))) {
    fprintf(fo, "\\x%02x", ch);
    if(pos++ > LINELEN) {
      fprintf(fo, "\"\n   \"");
      pos=1;
    }
  }
  fprintf(fo, "\";\n\nint v_mpg_%s_length = sizeof(v_mpg_%s);\n\n", argv[1], argv[1]);

  fclose(fi);
  fclose(fo);

  return 0;
}
