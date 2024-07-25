#include <stdio.h>
 
int main()
{           
 
    FILE *f;
    char rstring[500];
    
    f=fopen("output.txt","a");             //будем дозаписывать в файл
    fprintf(f, "%s","string from file\n"); 
    fclose(f);
    f=fopen("output.txt","r");
    fgets( rstring, 450,f);
    printf("%s\n",rstring);
    fprintf(stdout, "таже строка но через fprintf :%s\n",rstring); //или так
    fclose(f);
}


    
