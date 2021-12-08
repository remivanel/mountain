/* MODULES.H */

void load_modules(void);
void unload_modules(void);
int get_mod_info(char *fic, int *x, int *y, int *nb_pict, long *id_mod, char *desc);
int find_module(long id);
void form_suit(int event);
int get_suit_info(int *x, int *y, int *nb_img,  long *id_mod, long *num_img);
int create_liste(char *adr_tab_nom);
int test_1st(void);
int find_next(char *str, int nb);			/* retour 0 si err */
int alpha_cmp(char *best,char *actual);
int date_cmp(unsigned int b_date,unsigned int b_time,unsigned int a_date,unsigned int a_time);
int load_sav_mod(void);

#define ME_PARAM 0x20
#define M_RELOG 0x10
#define MI_CLOSABLE 0x40

typedef struct
{	int	handle;			/* hnd fic */
	long	size_fic;		/* taille fic */
	int	retour;			/* RETOUR retour 0000 ok, FFFF erreur */
	int	x;					
	int	y;
	long	nb_img;			/* RETOUR nomdre d'img */
	long	size_buf;		/* RETOUR taille du buffer tempo */
	void *work_adr;		/* buffer de travail PERMANENT */
	char *desc;				/* RETOUR decription de l'anim */
} INIT_PARAM;

typedef struct
{	int	handle;			/* hnd fic */
	void  *work_adr;		/* buffer de travail PERMANENT */
} CLOSE_PARAM;

typedef struct
{	int	handle;			/* hnd fic */
	long	old_frame;		/* index old_frame presente sur le buffer */
	long	frame;			/* frame demandee */
	int   *buf;				/* adr buf tempo */
	int   *img;				/* buffer ecran */
	int   x_dest;			/* larg ecran dest */
	void  *work_adr;		/* buffer de travail PERMANENT */
	int	retour;			/* RETOUR retour 0000 ok, FFFF erreur */
} IMG_PARAM;

typedef struct
{	char	*name;				/* nom fic */
	int	retour;			/* RETOUR retour 0000 ok, FFFF erreur */
	int	x;					
	int	y;
	int	fps;				/* vitesse */
	long	nb_img;			/* RETOUR nomdre d'img */
	long	size_buf;		/* RETOUR taille du buffer tempo */
	void *work_adr;		/* buffer de travail PERMANENT */
	void *param_adr;		/* adr parametres */
} EXP_INIT_PARAM;

typedef struct
{	long	frame;			/* numframe demandee */
	int   *buf;				/* adr buf tempo */
	int   *old_img;		/* buffer ecran acienne image */
	int   *img;				/* buffer ecran */
	int   retour;			/* 0 ou ffff */
	void  *work_adr;		/* buffer de travail PERMANENT */
	void  *param_adr;		/* adr parametres */
} EXP_IMG_PARAM;
	
typedef struct
{	void (*init_imp)(INIT_PARAM *adr_para);		/* adr routine INIT_ANIM */
	void (*get_frame)(IMG_PARAM *adr_para);		/* adr routine GET_FRAME */
	void (*close_anim)(CLOSE_PARAM *adr_para);		/* adr routine close_anim */
	void *adr_mod;								/* Adresse du module */
	long id;										/* id de reconnaissance */
	long ver;									/* version du module */
	long size_buf_para;						/* taille du buffer contenant des info pour chaque anim */
	long flags;									/* FLAGS */
	char nom_fic[16];
	char description[32];
}IMP_MOD;
	
typedef struct
{	void (*init_exp)(EXP_INIT_PARAM *adr_para);		/* adr routine INIT_ANIM */
	void (*put_frame)(EXP_IMG_PARAM *adr_para);		/* adr routine PUT_FRAME */
	void (*mod_param)(void *ptr);			/* adr routine param */
	void (*mod_param_def)(void *ptr);	/* adr routine remplissage de param */
	void *adr_mod;								/* Adresse du module */
	long id;										/* id de reconnaissance */
	long ver;									/* version du module */
	long size_buf_perm;						/* taille du buffer contenant des info pour chaque anim */
	long size_buf_para;						/* taille de la zone de param */
	long flags;									/* FLAGS */
	char nom_fic[16];
	char description[32];
}EXP_MOD;
