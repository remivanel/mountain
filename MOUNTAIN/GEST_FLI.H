/* HEADER GEST FL[IC] */

int do_key_i_frame(char *nom_fli, char *nom_key, int frequence, OBJECT *adr_key, int prem_frame, long pos_prem_frame);
int get_fli_info(char *fic, char *dest,int *x, int *y, int *nb_pict, int *fli, int *prem_frame, long *pos_prem_frame);
int get_in_fli(int frame_to_get, int preced_frame, int hnd_flh, int hnd_key,
               int *buffer, int *img, int x_img, KEY_HEAD *key, long *flh_offset,
               long *key_offset, size_t taille_buffer);
