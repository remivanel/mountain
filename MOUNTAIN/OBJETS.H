/* OBJETS.H */

typedef struct
{	char type;
	int  x;
	int  y;
	char nom[10];
	char desc[20];
	int  nb_img;
	int  def_in;		/* premiŠre image par d‚faut */
	int  def_out;		/* derni‚re */
	char chemin[150];
/* FLI FLC FLH : */
	char chemin_key[150];
/* PAR MODULE EXTERNE */
	long module_id;			/* id du module utilis‚ */
/*  SUITES D'IMG */
	long num_img;				/* image a prendre ds chaque fichers */
	char *tab_noms;
	MFDB preview;
	int img_preview;
}OBJET;

#define SIZE_OBJ sizeof(OBJET)
#define MAX_OBJET 300
#define OBJ_H 56

/* Les types d'objet */

#define OBJ_FLH 0
#define OBJ_FLI 1
#define OBJ_FLC 2
#define OBJ_SUIT 3
#define OBJ_MOD 4		/* description dans chemin_key */


void new_objet(char type, char nom[10], char chm[150], char chm_key[150]);
int init_wobjet(int x, int y, int w, int h);
void window_objet(int event);
int make_preview(int num_objet);
void efface_objet(int num_objet);
void slider_o(void);
void redraw_objet(int w_handle, int x, int y, int w, int h);
void redraw_1objet(int num_objet);
