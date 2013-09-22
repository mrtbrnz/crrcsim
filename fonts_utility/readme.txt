ft2texfont.c :
Generation of a textured font file (.txf) for PLIB..

showtxf.c:
Display of a file .txf

conv :
script Linux for compilation and execution of ft2texfont

-ff file TrueType. example "/usr/share/fonts/truetype/freefont/FreeSans.ttf"

-file name and path of the generated file. For CRRCSIM, it has to be "Sans_xxxx.txf" where "xxx" is the used coding. Example: "Sans_iso8859-5.txf"

-set coding of the generated file. Example: "iso8859-5"

-w , -h Dimensions of the texture. Has to be power of 2

-size Height of the characters (in pixel). See note below

-gap Spacing between the characters in the texture

-glist ABCD : List of the character to be put in the texture. By default, every character printable.


Note: 
To obtain the best quality, it would be preferable to generate the characters with the exact size used in the display. However, it seems impossible with PLIB to avoid rounding errors of the sizes of display, and we obtain bad results. A less bad solution is then to generate characters with one sharply bigger size (example: 24)

