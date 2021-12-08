/* Mouvement.H */

void form_mou(int event);
void init_mvt(void);
int find_free_mvt(void);
void copy_mvt(int src, int dst);
void mou_set_ass_pos(int objc, int ofon, int val);
void mou_set_ass_zoom(int objc, int ofon, int val);
void mou_set_ass_rot(int objc, int ofon, int val);
int new_mvt(void);
void move_ass_pos(int objc, int ofon, int *val);
void move_ass_zoom(int objc, int ofon, int *val);
void clear_pt_mvt(int i);
void set_pt_mvt(int src);
int cdecl draw_mvt(PARMBLK *parmblock);
int cdecl draw_preview(PARMBLK *parmblock);
void cacl_n_put_mvt(int *src, int *dst, int src_x, int src_y, int src_tx, int dst_x, int dst_y, int dst_tx, int num_mvt, int time, int transp, int tr,int tv,int tb, int intens);
void update_mou(void);
void mou_put_coul(void);
void mou_scan_coul(void);
int cdecl preview_color(PARMBLK *parmblock);
int cdecl pic_coul(PARMBLK *parmblock);
void draw_marqueur(int x, int y, char fill);
int cdecl draw_time(PARMBLK *parmblock);
void new_pt(void);
void updt_num_pt(void);
void click_temps(void);
int trouve_pt(int x);
void aff_time_pt(int val);
void anim_prev(void);
void kill_pt(int pt);
void save_mvt(void);
void load_mvt(void);
int duplique_mvt(int src);



#define MAX_ZOOM 3000
#define MAX_POS 2000
#define TEMP_MVT 0
#define MAX_USE_MVT 15
#define MAX_MVT 100
#define MAX_PT_MVT 1000
#define MOU_W 200
#define MOU_H 200
#define MOU_PREV_W 120
#define MOU_PREV_H 120
#define MOU_PIC_W 64
#define MOU_PIC_H 64

typedef struct
{	int	zoom_x;			/* 0 -> rien, 1000 -> largeur projet, >1000 -> zoom grossissant, <1000 -> interieur */
	int	zoom_y;
	int	rot;
	int	pos_x;
	int	pos_y;
	int	time;				/* de 0 … 1000 */
}PT_MVT;

typedef struct
{	int nb_mvt_used;
	int img_used;
	int time;					/* time spline, voir pistes.h */
	unsigned char r,v,b;
	char transparent;
	char used;
	int time_zx;		/* timeSpline reservee a zoom x */
	int time_zy;
	int time_px;
	int time_py;
}MVT;
