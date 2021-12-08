/* TIME_SPL.H */


/* nombre de timesplines affichees dans la fenetre de choix */
#define NB_CHX_TIM 12

/* le nombre de points de controle, 16 devrait suffire */
#define MAX_POINT	16  


extern int *retour_index;
extern WINDFORM_VAR *retour_wform;
extern int retour_objet;

/* le systeme d'annotations */
extern int etim_nb_anno;
extern int etim_pos_anno[3];
extern char etim_text_anno[3][10];


typedef struct 
{	char nom[30];
	int  mode; 			/* 1 : Manuel 
								2 : Lignes
								3 : besier	... */
	int interpolee;							/* si on interpole le calcul */
	unsigned char valeur[256];				/* toutes les valeurs manuelles */
	int nb_points;								/* mini 2 ! */
	unsigned char point[MAX_POINT][2];	/* les points de controle */
	void *next_time_spline;
}TIME_SPLINE;


/* proto */

void form_ctim(int event);
int new_time_spline(void);
void reset_time_spline(TIME_SPLINE *la_spline);
TIME_SPLINE *find_time_spline(int indice);
void init_chx_time(void);
long spline_comput(int index,TIME_SPLINE *adr_spline, long time, long max_abscisse, long max_ordonee);
void copy_spline_name(int index, char *str);
void form_etim(int event);
void delete_spline(int index);
void save_spline(TIME_SPLINE *la_spline);
int load_spline(TIME_SPLINE *la_spline);
void add_loaded_spline(void);
