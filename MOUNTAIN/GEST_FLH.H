/* HEADER GEST FLH */


typedef struct
{	long head;
	int x;
	int y;
	int nb_img_in_flh;
	int nb_img_in_key;
	int frequence;
	int prem_frame;			/* pour sauter les frame info */
	char nom_anim[10];
	long table_offset;
}KEY_HEAD;

#define SIZE_HKEY 30
#define POS_TABLE_O 26


int do_key_frame(char *nom_flh, char *nom_key, int frequence, OBJECT *adr_key);
int get_flh_info(char *fic, char *dest,int *x, int *y, int *nb_pict);
int get_in_flh(int frame_to_get, int preced_frame, int hnd_flh, int hnd_key,
               int *buffer, int *img, int x_img, KEY_HEAD *key, long *flh_offset,
               long *key_offset, size_t taille_buffer);
