/* 
adapted for FreeType library by Joel Lienard 2012
from gentexfont.c, Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ft2build.h> 
#include FT_FREETYPE_H
#include <math.h>
#include <iconv.h>
#include <errno.h>
#include "TexFont.h"


typedef struct {
  short width;
  short height;
  short xoffset;
  short yoffset;
  short advance;
  unsigned char *bitmap;
} PerGlyphInfo, *PerGlyphInfoPtr;


//globales
FT_Face face;
iconv_t iconvd;
int format = TXF_FORMAT_BITMAP;
int gap = 1;




void
getMetric(FT_Face face, int cc, TexGlyphInfo * tgi)
  {
  int error;
  int charcode;
  unsigned char *bitmapData;
  char c = cc;
  char* ac = &c;
  char oc [8];
  oc[0]=oc[1]=oc[2]=oc[3];
  char* aoc = &oc[0];
  size_t n=1;
  size_t on=8;
  tgi->c = cc;
  size_t nconv=iconv (iconvd, &ac, &n,
         &aoc, &on);
  if(nconv==(size_t)(-1)){
    int  error = errno;
     printf(" erreur iconv(%d)\n",error);
     if(error==EBADF)printf("The cd argument is not a valid open conversion descriptor\n");
     exit(0);
    }
  charcode= (unsigned char)(oc[3]);
  charcode = charcode<<8 | (unsigned char)(oc[2]);
  charcode = charcode<<8 | (unsigned char)(oc[1]);
  charcode = charcode<<8 | (unsigned char)(oc[0]);
  FT_UInt glyph_index = FT_Get_Char_Index( face, charcode );
  //printf("car: %c utf32 :%8X \n",c,charcode);
  error = FT_Load_Glyph(
            face, /* handle to face object */
            glyph_index, /* glyph index */
            FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP ); /* load flags, see below */
  if(error){printf("erreur dans FT_Load_Glyph\n");exit(0);}
  error = FT_Render_Glyph(
            face->glyph, /* glyph slot */
            FT_RENDER_MODE_NORMAL ); /* render mode */
  if(error){printf("erreur dans Render_Glyph\n");exit(0);}
  
  FT_GlyphSlot g = face->glyph;
  bitmapData = g->bitmap.buffer;
  if (bitmapData) {
    tgi->width = g->bitmap.width+2 ;
    tgi->height = g->bitmap.rows+2 ;
    tgi->xoffset = g->bitmap_left +1;
    tgi->yoffset =  -g->bitmap.rows + g->bitmap_top;// TODO
  } else {
    tgi->width = 0;
    tgi->height = 0;
    tgi->xoffset = 0;
    tgi->yoffset = 0;
  }
  tgi->dummy = 0;
  tgi->advance = g->advance.x/64;
}

int
glyphCompare(const void *a, const void *b)
{
  unsigned char *c1 = (unsigned char *) a;
  unsigned char *c2 = (unsigned char *) b;
  TexGlyphInfo tgi1;
  TexGlyphInfo tgi2;

  getMetric(face, *c1, &tgi1);
  getMetric(face, *c2, &tgi2);
  return tgi2.height - tgi1.height;
}

int
getFontel(unsigned char *bitmapData, int spanLength, int i, int j)
{
  return bitmapData[i * spanLength + j / 8] & (1 << (j & 7)) ? 255 : 0;
}

void
placeGlyph( int c, unsigned char *texarea, int stride, int x, int y)
{
  unsigned char *bitmapData;
  int width, height, spanLength;
  int i, j;

  bitmapData = face->glyph->bitmap.buffer;
  if (bitmapData) {
    width = face->glyph->bitmap.width;
    spanLength = (width + 7) / 8;
    height = face->glyph->bitmap.rows;
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        texarea[stride * (y + i) + x + j] =
                  bitmapData[(height-i-1) * width + j];
      }
    }
  }
}

char *
nodupstring(char *s)
{
  int len, i, p;
  char *newc;

  len = strlen(s);
  newc = (char *) calloc(len + 1, 1);
  p = 0;
  for (i = 0; i < len; i++) {
    if (!strchr(newc, s[i])) {
      newc[p] = s[i];
      p++;
    }
  }
  newc = (char*)realloc(newc, p + 1);
  return newc;
}

int 
main(int argc, char *argv[])
{
  int texw, texh;
  unsigned char *texarea, *texbitmap;
  FILE *file;
  int len, stride;
  char *glist;
  int width, height;
  int px, py, maxheight;
  TexGlyphInfo tgi;
  int usageError = 0;
  char *fontname, *filename;
  int endianness;
  int i, j;
  int error;
  //defaults values
  char *codeset = (char*)"ISO_8859-15";
  int charsize = 24;//on pixels 
  texw =256;//textureWidth 
  texh = 256;// textureHeight
  format = TXF_FORMAT_BYTE; 
  char *fontfile = (char*)"font.ttf";//input font file
  filename = (char*)"default.txf";//output textured font
  //init glist to all printables chars
  unsigned char allc[256];
  j=0;
  for (i = 0x20; i<0x7F; i++) allc[j++] = i;
  for (i = 0xA0; i<=0xFF; i++) allc[j++] = i;
  allc[j]=0;
  glist = (char*)allc;
  //read paremeters
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-w")) {
      i++;
      texw = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-h")) {
      i++;
      texh = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-gap")) {
      i++;
      gap = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-byte")) {
      format = TXF_FORMAT_BYTE;
    } else if (!strcmp(argv[i], "-bitmap")) {
      format = TXF_FORMAT_BITMAP;
    } else if (!strcmp(argv[i], "-glist")) {
      i++;
      glist = ( char *) argv[i];
    } else if (!strcmp(argv[i], "-ff")) {
      i++;
      fontfile = argv[i];
    } else if (!strcmp(argv[i], "-file")) {
      i++;
      filename = argv[i];
    } else if (!strcmp(argv[i], "-set")) {
      i++;
      codeset = argv[i];
    } else if (!strcmp(argv[i], "-size")) {
      i++;
      charsize = atoi(argv[i]);
    } else {
      usageError = 1;
    }
  }
printf("codeset: %s\n",codeset);
  if (usageError) {
    putchar('\n');
    printf("usage: texfontgen [options] txf-file\n");
    printf(" -w #          textureWidth (def=%d)\n", texw);
    printf(" -h #          textureHeight (def=%d)\n", texh);
    printf(" -gap #        gap between glyphs (def=%d)\n", gap);
    printf(" -byte         use a byte encoding (less compact)\n", gap);
    printf(" -glist ABC    glyph list (def= all chars)\n", glist);
    printf(" -ff name      font filename (def=%s)\n", fontfile);
    printf(" -set name     codeset name (def=%s)\n", codeset);
    printf(" -file name    output file for textured font (def=%s)\n", fontname);
    putchar('\n');
    exit(1);
  }
  //alloc texture
  texarea = (unsigned char *)calloc(texw * texh, sizeof( char));
  glist = ( char *) nodupstring((char *) glist);
  //init iconv lib
  char tocode[] = "UTF-32LE";
  iconvd = iconv_open (tocode, codeset);
  if(iconvd == ( (iconv_t)(-1))){
    printf("erreur iconv_open\n");
    exit(0);
    }

  //init FT lib
  FT_Library library;
  error = FT_Init_FreeType( &library );
  if ( error ) { 
    printf("an error occurred during FT library initialization \n");
    exit(0);
    } 
  error = FT_New_Face( library, fontfile, 0, &face );
  if ( error == FT_Err_Unknown_File_Format ) {
    printf("New_Face: the font file could be opened and read, but it appears ... that its font format is unsupported\n");
    exit(0);
    }
  else if ( error ) {
    printf("New_Face: another error code means that the font file could not ... be opened or read, or simply that it is broken\n");
    exit(0);
    }

    error = FT_Select_Charmap(
        face, /* target face object */
        FT_ENCODING_UNICODE ); /* encoding */
    if ( error ) {
      printf("error FT_Select_CharMap %d\n",error);
      exit(0);
      }      
    error = FT_Set_Pixel_Sizes(
      face, /* handle to face object */
      0, /* pixel_width */
      charsize /* pixel_height */
      ); 
    if ( error ) {
      printf("error Set _Pixel_Sizes\n");
      exit(0);   
      }
  len = strlen((char *) glist);
  printf("%s   %d\n",glist,len);
  qsort(glist, len, sizeof(unsigned char), glyphCompare);

  file = fopen(filename, "wb");
  fwrite("\377txf", 1, 4, file);
  endianness = 0x12345678;
  assert(sizeof(int) == 4);  /* Ensure external file format size. */
  fwrite(&endianness, sizeof(int), 1, file);
  fwrite(&format, sizeof(int), 1, file);
  fwrite(&texw, sizeof(int), 1, file);
  fwrite(&texh, sizeof(int), 1, file);
  int max_ascent = charsize;
  int max_descent = charsize/3;///? not used
  fwrite(&max_ascent, sizeof(int), 1, file);
  fwrite(&max_descent, sizeof(int), 1, file);
  fwrite(&len, sizeof(int), 1, file);
  
  px = gap;
  py = gap;
  maxheight = 0;
  for (i = 0; i < len; i++) {
    if (glist[i] != 0) {  /* If not already processed... */

      /* Try to find a character from the glist that will fit on the
         remaining space on the current row. */

      int foundWidthFit = 0;
      int c;

      getMetric(face, glist[i], &tgi);   
      width = tgi.width;
      height = tgi.height;
      if (height > 0 && width > 0) {
        for (j = i; j < len;) {
          if (height > 0 && width > 0) {
            if (px + width + gap < texw) {
              foundWidthFit = 1;
	              if (j != i) {
		                i--;  /* Step back so i loop increment leaves us at same character. */
	              }
              break;
              }
	          }
          j++;
          getMetric(face, glist[j], &tgi);

          width = tgi.width;
          height = tgi.height;
          }

        /* If a fit was found, use that character; otherwise, advance a line
           in  the texture. */
        if (foundWidthFit) {
          if (height > maxheight) {
            maxheight = height;
          }
          c = j;
          } else {
            getMetric(face, glist[i], &tgi);
            width = tgi.width;
            height = tgi.height;

            py += maxheight + gap;
            px = gap;
            maxheight = height;
            if (py + height + gap >= texh) {
              printf("Overflowed texture space.\n");
              exit(1);
              }
            c = i;
            }

        /* Place the glyph in the texture image. */
        placeGlyph( glist[c], texarea, texw, px+1, py+1);/////JL 

        /* Assign glyph's texture coordinate. */
        tgi.x = px;
        tgi.y = py;

	/* Advance by glyph width, remaining in the current line. */
        px += width + gap;
      } else {
	/* No texture image; assign invalid bogus texture coordinates. */
        tgi.x = -1;
        tgi.y = -1;
      }
      glist[c] = 0;     /* Mark processed; don't process again. */
      assert(sizeof(tgi) == 12);  /* Ensure external file format size. */
      fwrite(&tgi, sizeof(tgi), 1, file);
     }

  }
  //write texture
  fwrite(texarea, texw * texh, 1, file);

  free(texarea);
  fclose(file);
  return(0);
}
