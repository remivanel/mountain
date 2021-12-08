
void set_text(	WINDFORM_VAR *ptr_var, int ob,char *str);
void screen_copy(int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst);
int super_alert(int defaut, int icone, const char *str);
void text_center_x(int x, int y, char *str);
void rect(int x, int y,int w, int h, int color);
void rect_3d(int x, int y,int w, int h);
void ligne(int x, int y,int w, int h,int color);
void rect_full(int x, int y,int w, int h,int color);
void set_clip(int clip_flag, GRECT *area);
void text_vert(int x, int y, const char *str);
void text_center(int x, int y, char *str);
void find_name(char *str, char *dest);
void tnt_cpyfm(int pxy[8], MFDB *src);
void find_name_ext(char *str, char *dest);
void find_path(char *str);
int super_popup(WINDFORM_VAR *ptr_var,int objet,int index,int old,char *str,int taille_str,int nb_str);
char *strpcpy_force(char *dest, char *start, int len, char fill);
void form_mem(int event);
int conv_col(int r,int v,int b);
void conv2_col(int col,int rvb[3]);
void vire_ext(char *str);
void set_pop_zoom(int zoom);
void set_curseur(int pos);
char check_key(char *chm, char *nom, int x, int y, int nb_img);
void install_tache_fond(int tache);
void remove_tache_fond(int tache);
void launch_tache(void);
long get_cookie(long cook);
void back_memcpy(void *src, void *dst, int len);
long mem_free(void);
void barre(int cur,int tot,OBJECT *adr,const char *txt);
void ldg_mnt_init(void);
void ldg_uninstall(void);
long get_timerC(void);

#define MAX_TACHE 5
#define TACHE_APREV 0
#define TACHE_VISU 1
#define TACHE_CALCUL 2

#define UPDT_MEM 0x6000

typedef struct
{	
	int *src;
	void *dst;
	int larg_cpy;
	int x_src;
	int x_dst;
	int y;
	int pos_x_dst;			/* uniquement trm_16 */
}CPY_PARAM;
