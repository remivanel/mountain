typedef struct
{	PROJET *projet;
	int *img[2];
	MPL_PARA *mpl_para;
	char *chemin_key;
	char *fic_son;
	char *chemin_mplayer;
	int format,qualite;
	long nb_img;
	int freq_key;
	char *nom_fic;
	void (*Save_stack)(void);
	int (*super_alert)(int defaut, int icone, const char *str);
	void (*find_path)(char *str);
}EXT_MPL_EXP;

typedef struct
{	PROJET *projet;
	int *img[2];
	int format,qualite;
	long nb_img;
	char *nom_anim;
	char *chemin;
	void *adr_param;
	void *work_adr;
	void (*find_path)(char *str);
	EXP_INIT_PARAM *exp_init_param;
}EXT_EXP_MOD;

