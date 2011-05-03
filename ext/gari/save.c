
#include "gari.h"
#define GARI_INTERN_ONLY
#include "gari_intern.h"

#include <stdlib.h>
#include <png.h>
#include <jpeglib.h>

/* Saves a GariImage as a to a file in filename in BMP format. 
Return NULL if it failed, img on success. */
GariImage * gari_image_savebmp(GariImage * img, const char * filename) {
  SDL_Surface * surf = gari_image_surface(img);
  int res            = SDL_SaveBMP(surf, filename);
  if (res < 0) return NULL;
  return img;
}

/* PNG Color type for SDL_Surace */
static int sdl_surface_pngcolortype(SDL_Surface *surface) {
  int colortype = PNG_COLOR_MASK_COLOR;

  if (surface->format->palette) { 
    colortype |= PNG_COLOR_MASK_PALETTE; 
  } else if (surface->format->Amask) {
    colortype |= PNG_COLOR_MASK_ALPHA;
  }
  
  return colortype;
}

static int savepng_done(png_bytep * rows, FILE * fout, png_structp png,
                        png_infop info, SDL_Surface * temp, 
                        png_colorp  pale, Uint8  * trns) {
  if(rows) gari_free(rows);
  if(pale) gari_free(pale);
  if(trns) gari_free(trns);
  if(temp) SDL_FreeSurface(temp);
  if(fout) fclose(fout);
  if(info) {
    png_destroy_write_struct(&png, &info); 
  } else if (png) {
    png_destroy_write_struct(&png, NULL);
  }  
  return 0;
}  

static int save_fail(const char * msg) {
  if(msg) { SDL_SetError(msg); }
  return -1;
}

/* Saves an SDL surface to a png file.
* TODO: support screen formats. 
*/
int sdl_surface_savepng(SDL_Surface * surf, const char * filename) {
  png_bytep       * rows  = NULL;
  png_colorp        pale  = NULL; 
  FILE            * fout  = NULL;
  png_structp       png   = NULL;
  png_infop         info  = NULL;
  int               colortype;
  int               i     = 0;
  Uint8           * tran  = NULL;
  SDL_PixelFormat * fmt   = NULL;
  SDL_Surface     * temp  = NULL;
  
  if (!surf) { return save_fail("Surface was NULL."); }
  if (!filename) { return save_fail("Filename was NULL."); }
    
  rows  =  gari_malloc(sizeof(png_bytep)*surf->h);
  if (!rows) { return save_fail("Out of memory."); }
  
  for (i = 0; i < surf->h; i++) { 
    rows[i] = ((Uint8 *)surf->pixels) + i*surf->pitch;
  }
  
  /* Create palette and transparency table if needed. */
  fmt = surf->format;
  if (fmt->palette) {
    pale = malloc(fmt->palette->ncolors * sizeof(png_color));
    if (!pale) {
      savepng_done(rows, fout, png, info, temp, pale, tran);
      return save_fail("Out of memory in saving palette.");
    }    
    for (i = 0; i < fmt->palette->ncolors; i++) {
      pale[i].red    = fmt->palette->colors[i].r;
      pale[i].green  = fmt->palette->colors[i].g;
      pale[i].blue   = fmt->palette->colors[i].b;
    }
    
    if (surf->flags & SDL_SRCCOLORKEY) {
      tran = malloc(fmt->palette->ncolors * sizeof(Uint8));
      if(!tran) {
        savepng_done(rows, fout, png, info, temp, pale, tran);
        return save_fail("Out of memory in saving palette transparency.");
      }
      for (i = 0; i < fmt->palette->ncolors; i++) {
        tran[i] = (((unsigned)i) == fmt->colorkey) ? 255 : 0;
      }
    }
  }
    
  fout = fopen(filename, "wb");
  if(!fout) {
    savepng_done(rows, fout, png, info, temp, pale, tran);    
    return save_fail("Couldn't open file for writing."); 
  }
  
  png  = png_create_write_struct(PNG_LIBPNG_VER_STRING,  NULL, NULL, NULL);
  if (!png) {
    savepng_done(rows, fout, png, info, temp, pale, tran);
    return save_fail("Couldn't create png_structp");
  }  
  
  info = png_create_info_struct(png);
  if (!info) {
    savepng_done(rows, fout, png, info, temp, pale, tran);
    return save_fail("Couldn't create png_infop");
  }
  
  /* libpng rudely uses longjump to terminate on errors. 
    This in turns causes warning of pissbli clobered variables. Could it suck 
    more? */
  if (setjmp(png->jmpbuf)) {
    /* This give a warning, but it seems to be unavoidable. */
    savepng_done(rows, fout, png, info, temp, pale, tran);
    return save_fail("Error writing png file.");
  }
  
  /* Set file pointer. */
  png_init_io(png, fout);
  
  colortype = sdl_surface_pngcolortype(surf);
  png_set_IHDR(png, info, surf->w, surf->h, 8, colortype, PNG_INTERLACE_NONE, 
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  
  /* Write palette if needed. */
  if(pale) { 
    png_set_PLTE(png, info, pale, fmt->palette->ncolors);
  }
    
  /* Write transparency table if needed. */
  if (tran) {  
    png_set_tRNS(png, info, tran, fmt->palette->ncolors, NULL);
  }
  
  /* Write the image. */
  png_write_info(png, info);
  png_set_packing(png);
  if(SDL_MUSTLOCK(surf)) {  SDL_LockSurface(surf);  }
  png_write_image(png, rows);
  if(SDL_MUSTLOCK(surf)) { SDL_UnlockSurface(surf); }
  png_write_end(png, info);
  
  
  /* Clean up. */
  savepng_done(rows, fout, png, info, temp, pale, tran);
  return 0;
}



/* Saves a GariImage as a to a file in filename in PNG format. 
Return NULL if it failed, img on success. */
GariImage * gari_image_savepng(GariImage * img, const char * filename) {
  SDL_Surface * surf = gari_image_surface(img);
  int res            = sdl_surface_savepng(surf, filename);
  if (res < 0) return NULL;
  return img;
}

int savejpg_done(FILE * fout) {
/*  if(rows) gari_free(rows);
  if(pale) gari_free(pale);
  if(trns) gari_free(trns);
  if(temp) SDL_FreeSurface(temp); */
  if(fout) fclose(fout);
/*  if(info) {
    png_destroy_write_struct(&png, &info); 
  } else if (png) {
    png_destroy_write_struct(&png, NULL);
  } */  
  return 0;
}



/* Saves an SDL surface to a png file.
* TODO: Getting grayscaled and warped images somehow. Support screen formats. 
*/

int sdl_surface_savejpg(SDL_Surface * surf, char *filename)
{
  FILE                       * fout = NULL;
  struct jpeg_compress_struct  jpeg;
  struct jpeg_error_mgr        jerr;
  JSAMPROW                     row[1];
  
  if (!surf)     { return save_fail("Surface was NULL.");  }
  if (!filename) { return save_fail("Filename was NULL."); }
  
  fout = fopen(filename, "wb");
  
  if (!fout) return save_fail("Could not open output jpeg file for writing.");
    
  jpeg.err                = jpeg_std_error(&jerr);
  jpeg_create_compress(&jpeg);
  jpeg_stdio_dest(&jpeg, fout);  
  jpeg.image_width        = surf->w;
  jpeg.image_height       = surf->h;
  jpeg.input_components   = 3;
  jpeg.in_color_space     = JCS_RGB;
  jpeg_set_defaults(&jpeg);
  jpeg_start_compress(&jpeg, TRUE );  
  while ( jpeg.next_scanline < jpeg.image_height )  {
    row[0] =  ((Uint8 *)surf->pixels) + (jpeg.next_scanline * surf->pitch); 
    jpeg_write_scanlines( &jpeg, row, 1);
  }
    
  jpeg_finish_compress( &jpeg );
  jpeg_destroy_compress( &jpeg );
  fclose(fout);  
  return 0;
}


/* Saves a GariImage as a to a file in filename in JPG format. 
Return NULL if it failed, img on success. */
GariImage * gari_image_savejpg(GariImage * img, const char * filename) {
  SDL_Surface * surf = gari_image_surface(img);
  int res            = sdl_surface_savejpg(surf, filename);
  if (res < 0) return NULL;
  return img;
}



