#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "image\n");
        return 1;
    }
    
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file\n");
        return 2;
    }
    
    // allocate memory 
    unsigned char *buffer = malloc(512);
    if (buffer == NULL)
    {   
        fprintf(stderr, "error\n");
        return 3;
    }
    
    int jpg = 0;
    
    FILE *image = NULL;
    
    // read card file
    while (fread(buffer, 512, 1, file))
    {
        // check for start of jpg
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // close previous jpg
            if (jpg > 0)
            {
                fclose(image);
            }
            
            // create new jpg and name it
            char filename[8];
            sprintf(filename, "%03i.jpg", jpg); 
            image = fopen(filename, "w");
            jpg ++;
        }   
            
        if (!jpg)
        {
            continue;
        }
        
        fwrite(buffer, 512, 1, image);        
    }
    
    fclose(file);
    fclose(image);
    free(buffer);
    return 0;
}
