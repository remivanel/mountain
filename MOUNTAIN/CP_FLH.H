/* routines C de compactage FLH */
typedef unsigned long   size_t;

size_t cp_brun_tc(int *img,int *flh,int mask_pack,int x_ani,int y_ani ,int x_img );
size_t cp_delta_tc(int *old, int *img,int *flh,int mask_pack,int x_ani,int y_ani ,int x_img);
