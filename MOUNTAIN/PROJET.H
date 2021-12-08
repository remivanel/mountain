/* projet.h */

int save_projet(char comme);
int load_projet(void);
void kill_projet(void);
int list_related_prj_obj(int num_objet, int *liste);
void efface_prj_obj(int num);
int et_ke_kon_fe_du_prj(void);
void new_prj_obj(int cur_objet, int pos_in, int piste);
int can_put_here(int cur_objet, int pos_in, int piste, int nonobstant);
int can_put_op_here(int pos_in, int pos_out, int piste, int nonobstant);
void put_objet(int num_objet);
int time_comput(int obj,int pos);
void slider_r(void);
void window_projet(int event);
int init_wprojet(int x, int y, int w, int h);
void redraw_projet(int w_handle, int x, int y, int w, int h);
void draw_obj_prj(int obj, char red);
void move_objet(int num_objet);
void copy_prj_obj(int cur_objet, int pos_in, int piste);
char *textuate_p(int piste, char *str);
int find_related_prj_ralated(int i);
void form_opta(int event);
void widen_objet(int num_objet);
void form_int(int event);
long find_anim_whole(long pos);
long real_lenght(void);
void tmp_close_all_wind(void);
void tmp_open_all_wind(void);

extern int w_prj_open;
extern int w_trk_open;

typedef struct
{	long head;
	int  x;
	int  y;
	int  taille_prev;		/* 0->1/16; 1->1/4; 2->1/1; 3->2/1 */
	char name[10];
	long  lenght;
	int  speed;
	int  zoom;
	int  nb_obj;			/* nombre d'objets inclus (de prj_obj) */
	int  preview_in;		/* premiŠre image animated preview */
	int  preview_out;		/* derni‚re */
	char chemin[300];
	char nb_sup_A;			/* nombre de pistes en superposition A */
	char nb_sup_B;			/* B */
	char nb_sup_G;			/* General */
	char mode_fond;		/* 0-> couleur 1-> objet */
	char mode_obj;			/* 0-> motif 1-> zoom‚ */
	unsigned char r,v,b;				/* couleur de fond */
	int  obj_fond;			/* objet de fond */
	long id_exp_mod;		/* l'id du module d'exp */
	long size_param;		/* tailles des parametres associes au module d'exp */
	void *adr_param;		/* l'adr des param ou les param eux mem si size_param<=4 */
}PROJET;

#define SIZE_PROJET sizeof(PROJET)