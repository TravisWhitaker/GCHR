GCHR
====

A very light means of storing graphics specific to Ghost in the Machine

====

Feel free to use this, but I've made it pretty case-specific. That said, it wouldn't be hard to adapt into something more generally useful; I may do this in the future. 

It's licensed under the GNU GPL V3, blah blah blah credit me etc. 


====

GCHR uses the Allegro 5 library. It was written and tested on linux, but should work on any OS with Allegro 5. 
For use on Windows, I recommend compiling it as C++ using Visual Studio (as VS C support is C90 only) or compiling it as-is with mingw. It has been tested in both environments without issues.

====

Basically, a PNG (or I guess a BMP, TGA, etc would work) is given to GCHR, and it will spit out a header file that defines a const char array with the name of the PNG. The const char uses one char for two pixels, or in other words, 4 bits per pixel. 

The values of each nybble can be the following corresponding pixel values:
0x00 - transparent pixel
0x01 - white pixel (255,255,255)
0x02 - light grey (160,160,160)
0x03 - dark grey (80,80,80)
0x04 - black (0,0,0)

anything else is undefined. GCHR will take a PNG and quantize pixel values into the above four tones + transparency. Needless to say, color is not preserved.

Future work: Make GCHR include a 15-color palette + transparency and have the pixel data be an indexed color lookup for more general use (Super Nintendo / Sega Genesis style). Eh, whatever.

Also included is a shred of source that won't compile on its own, but it shows how to generate an ALLEGRO_BITMAP* from a GCHR header.
