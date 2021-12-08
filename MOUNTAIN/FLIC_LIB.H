/* ROUTINES ASM POUR DEPACKER LE FL[IC] */

/* Chunks
-  x et y largeur et hauteur de l'anim 
-  x_dest : largeur de l'ecran de destination 
-  *ptr   : buffer FLH
-  *pal   : adr palette destination 
-  *img   : adr ecran destination */

void fli_delta(int x,int y, int x_dest, int *ptr,int *img);
void fli_brun(int x, int y, int x_dest, int *ptr,int *img);
void fli_lc(int x, int y, int x_dest, int *ptr,int *img);
void fli_color_64(int *ptr);
void fli_color_256(int *ptr);
void decompi(int x, int y, int x_dest, int *ptr,int *img);
