/* ANIM.H */


int open_anim(int num_objet);
int get_frame(int canal,int frame,int old_frame, int *img, int x_img);
void close_anim(int canal);
int get_one_frame(int num_objet, int frame, int *img, int x_img);
void libere_buffer(void);
int new_buffer(long taille);

#define MAX_ANIM_PAR 20

typedef struct
{	int objet;
	int handle;
	int hnd_key;
	long *offset_flh;		/* ou fli */
	long *offset_key;
	int palette[256];			/* pour les fl[ic] */
	KEY_HEAD key;
	char used;
	void *work_adr;			/* buffer permanent de travail */
	int mod;			/* numero du mod en mem */
}ANIM_PAR;
