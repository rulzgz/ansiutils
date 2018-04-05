#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <png.h>
#include "rawfont.h"
#include "ansicanvas.h"

png_structp png;
png_infop info;
png_bytep *row_pointers;
static png_uint_32 png_width;
static png_uint_32 png_height;

int gfx_png_export(char *pngfilename)
{
    FILE *fp = NULL;
    printf("Exporting to file '%s'...\n", pngfilename);

    fp = fopen(pngfilename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        png_width, png_height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    for(int y = 0; y < png_height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);
    return 0;
}

int gfx_png_drawglyph(BitmapFont *font, uint8_t px, uint8_t py, uint8_t glyph, uint8_t fg, uint8_t bg, uint8_t attr)
{
    RGBColour *fgc;
    RGBColour *bgc;
    uint8_t rx = 0;
    uint8_t h = 0;
    SDL_Rect r;
    //printf("gfx_png_drawglyph(%u, %u, %u, %u, '%c')\n", px, py, font->header.px, font->header.py, glyph);
    //
    fgc = canvas_displaycolour(fg + ((attr & ATTRIB_BOLD ? 8 : 0)));
    bgc = canvas_displaycolour(bg);


    for (int ii = 0; ii < font->header.py; ii++) {
        h = 0;
        /* TODO: handle big-endian */
        for (int jj = 128; jj >0; jj = jj >> 1) {

            r.x = (px*8) + (h*1);
            r.y = (py*16) + (ii*2);
            r.w = 1;
            r.h = 2;

            //printf("%u -> %u, ", r, jj);
            rx = font->fontdata[(glyph*font->header.py) + ii];
            if (rx & jj) {



//                SDL_SetRenderDrawColor( renderer, fgc->r, fgc->g, fgc->b, 255 );
//                SDL_RenderFillRect( renderer, &r );

                //SDL_RenderDrawPoint(renderer, (px*16) + (h*2), (py*16) + (ii*2));
                //printf("X");
            } else {
//                SDL_SetRenderDrawColor( renderer, bgc->r, bgc->g, bgc->b, 255 );
//                SDL_RenderFillRect( renderer, &r );

                //SDL_RenderDrawPoint(renderer, (px*16) + (h*2), (py*16) + (ii*2));
                //printf(" ");
            }
            h++;
        }
        //printf("\n");
    }
//    SDL_RenderPresent(renderer);
    return 0;
}

int gfx_png_main(uint16_t xsize, uint16_t ysize, char *WindowTitle)
{
    int posX = 100;
    int posY = 200;
    int sizeX = xsize;
    int sizeY =  ysize;
    uint32_t pngdatasize = 0;

    /* 4 bytes per pixel */

    pngdatasize = (xsize*ysize) * 4;

    printf("[PNG] allocating %lu bytes for PNG data buffer\n", (unsigned long) pngdatasize);
    assert(pngdatasize);
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * ysize);
    assert(row_pointers);
    png_width = xsize;
    png_height = ysize;

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png);
    info = png_create_info_struct(png);
    assert (info);

    for(int y = 0; y < png_height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }
    return 0;
}

int gfx_png_canvas_render(ANSICanvas *canvas, BitmapFont *myfont)
{
    ANSIRaster *r = NULL;
    uint16_t width = 0, height = 0;
    assert(canvas);
    width = canvas_get_width(canvas);
    height = canvas_get_height(canvas);
    printf("gfx_png_canvas_render(%ux%u)\n", width, height);
    for (uint16_t ii = 0; ii < height; ii++) {
        r = canvas_get_raster(canvas, ii);
        if (r) {
            for (uint16_t jj = 0; jj < r->bytes; jj++) {
                gfx_png_drawglyph(myfont, jj, ii, r->chardata[jj], r->fgcolors[jj], r->bgcolors[jj], r->attribs[jj]);
            }
        } else {
            printf("canvas data missing for raster %u\n", ii);
        }
    }
    return 0;
}
