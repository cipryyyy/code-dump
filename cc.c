#include <stdio.h>
#include <stdint.h>

void cc_fwd(char* src_path, char* dst_path, uint8_t rot)
{
    // open files
    FILE *src = fopen(src_path, "r");
    FILE *dst = fopen(dst_path, "w");

    char ch;
    char temp[2] = {'0', '\x0'}; 

    if (src == NULL)        //check if source exists
    {
        printf("%s does not exist\r\n", src_path);
    }
                
    while ((ch = fgetc(src)) != EOF)        // ᓚᘏᗢ < zzᶻᶻᶻᶻ
    {
        printf("%c", ch);
        ch += rot;          //Not a rotate but ok
        temp[0] = ch;

        fprintf(dst, "%s", temp);   //Write to destination file
    }

    fclose(src);
    fclose(dst);
    return;
}

void cc_bwd(char* path, uint8_t rot)
{
    FILE *src = fopen(path, "r");
    char ch;

    if (src == NULL)
    {
        printf("%s does not exist\r\n", path);
    }

    while ((ch = fgetc(src)) != EOF)
    {
        printf("%c", ch - rot);     //Revert edit
    }   

    fclose(src);
    return;
}

int main()
{
    cc_fwd("idk.txt", "out.txt", 20);
    printf("\n\n");
    cc_bwd("out.txt", 20);
    
    return 0;
}