/* PISTES.H */

void redraw_pistes(int w_handle, int x, int y, int w, int h);
void window_pistes(int event);
int init_wpistes(int x, int y, int w, int h);
void init_buffer(void);
void close_buffer(void);
int make_reduc(int cur_objet, int pos, int *img_dest, int *img, int *canal, int *old_frame);
void draw_piste(int piste, int x_base, int y_base, int x,int y,int w,int h);
int find_anim(int pos,int piste);
int find_tran(int pos);
void draw_prj_obj(int cur_obj_prj, int x_base, int y_base, int x,int y,int w,int h);
void draw_courb_int(int cur_prj_obj, int x_base, int y_base,int modulo);
void slider_ph(void);
void slider_pv(void);
int calc_hpiste(void);
int find_pos_piste(int mx,int my,int *piste,int *pos_in);
void aff_prev(int piste,int pos_in, int pos_out);
void aff_pos_preview(int pos);
int calc_nb_aff_pistes(void);
void draw_bar_prev(int in, int out);
void move_bar_prev(int mousex);
void redraw_une_piste(int piste);
void scrool_left(long old);
void scrool_right(long old);
int img_to_pix(long pos,int xbase);
void form_supi(int event);
char *img_to_smpte(int img, int speed, char *str);

#define MAX_PRJ_OBJ 200
#define MAX_BUF_OBJ 500

typedef struct
{	int  objet;
	int  piste;
	int  pos_in;		/* dans le projet */
	int  pos_out;
	int  obj_in;		/* dans l'objet */
	int  obj_out;
	int  mode_replay; /* 0->une fois 1-> boucle 2-> ping pong */
	int  time;			/* TIME SPLINE 0-> lineaire */
	int  mvt;			/* 0-> mode ki rempli tout en zoomant TEMP_MVT resrve*/
	int  trans;			/* -1 -> pas de trans >=0 -> tolerence en pour mille */
	int  r,v,b;			/* couleur de transp */
	int  intens;		/* val&0x8000 si pas time, val&0x0fff == 0 -> time spline , val#f000 0..4 -> intensite fixe 0,25,50,75,100% */
	STOQUAG_EFF *stoquag_eff; /* = 0 si pas d'effets */
}PRJ_OBJ;

#define SIZE_PRJ_OBJ sizeof(PRJ_OBJ)

typedef struct
{	int objet;
	int pos;
}BUF_OBJ;

/* Les pistes :
	0 -> A
	1 -> B
	2 -> Tran (pas pour les objets !)
	50+ -> SA
	100+ -> SB
	150+ -> SG
*/