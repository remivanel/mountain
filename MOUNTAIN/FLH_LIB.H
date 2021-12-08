/* ROUTINES ASM POUR DEPACKER LE FLH */

/* conversion intel */
int intel(int mot);
long intell(long var);

/* Chunks
-  x et y largeur et hauteur de l'anim 
-  x_dest : largeur de l'ecran de destination 
-  *ptr   : buffer FLH
-  *img   : adr ecran destination */

void flh_delta(int x,int y, int x_dest, int *ptr,int *img);
void flh_brun(int x, int y, int x_dest, int *ptr,int *img);
void decomp(int x, int y, int x_dest, int *ptr,int *img);

void copy(int *scr, int *dst, long taille_en_w);
void copy_tc(int *scr, int *dst, int nb_lignes, int x , int x_to_skip);
void copy_tc2(int *scr, int *dst, int nb_lignes, int x , int x_to_skip, int x_to_skip_src);
