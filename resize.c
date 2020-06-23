// resize a bmp

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // check resize factor
    float f = atof(argv[1]);
    if (f < 0.0 || f > 100.0)
    {
        fprintf(stderr, "resize factor must be positive and less than or equal to 100\n");
        return 1;
    }
    
    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    BITMAPFILEHEADER resized_bf;
    BITMAPINFOHEADER resized_bi;

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER orig_bf;
    fread(&orig_bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER orig_bi;
    fread(&orig_bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (orig_bf.bfType != 0x4d42 || orig_bf.bfOffBits != 54 || orig_bi.biSize != 40 ||
        orig_bi.biBitCount != 24 || orig_bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // access file contents
    resized_bf = orig_bf;
    resized_bi = orig_bi;

    // resize info header height and width values
    resized_bi.biWidth = orig_bi.biWidth * f;
    resized_bi.biHeight = orig_bi.biHeight * f;

    // determine padding
    int orig_padding = (4 - (orig_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int resized_padding = (4 - (resized_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // alter biSizeImage
    resized_bi.biSizeImage = ((resized_bi.biWidth * sizeof(RGBTRIPLE)) + resized_padding) * abs(resized_bi.biHeight);

    // alter bfSize
    resized_bf.bfSize = resized_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&resized_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&resized_bi, sizeof(BITMAPINFOHEADER), 1, outptr);



    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(orig_bi.biHeight); i < biHeight; i++)
    {
        // vertical resizing
        for (int j = 0; j < f; j++)
        {
            // iterate over pixels in scanline
            for (int k = 0; k < orig_bi.biWidth; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // horizontal resizing
                for (int l = 0; l < f; l++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }

            }

            // skip over padding, if any
            fseek(inptr, orig_padding, SEEK_CUR);

            // add resized padding if needed
            for (int m = 0; m < resized_padding; m++)
            {
                fputc(0x00, outptr);
            }

            // recopy
            fseek(inptr, -((orig_bi.biWidth * sizeof(RGBTRIPLE)) + orig_padding), SEEK_CUR);
        }

        fseek(inptr, (orig_bi.biWidth * sizeof(RGBTRIPLE)) + orig_padding, SEEK_CUR);

    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
