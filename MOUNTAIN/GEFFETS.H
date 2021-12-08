/*  GEFFETS.H */

void form_ceff(int event);
void init_effets(void);
void unload_effects(void);
int find_effet(long id);
void form_ptra2(int event);
int para_1eff_coul(void);
int para_2eff_coul(void);
void calc_eff_prev(int draw);
void eff_set_colors(int set);
void scan_eff_param(void);

#define MAX_EFFETS 100
#define MAX_CUR_EFFETS 10

/* Para a filer au module avant calcul */

typedef struct
{	int	*img_src;
	int	*img_dst;
	int	preced_img_ok;		/* 1=il y a la precedente en img_dst, 0:sinon */
	int	larg_img;
	int	x;
	int	y;					/* la taille des images */
	TOOL_BOX	*adr_tool_box;
	TRAN_REAL_PARAM *effet_real_param;			/* addy des para multiples */
}EFFET_PARA;

typedef struct
{	int	type;			/* 0:interne 1:module */
	int	mode_para;	/* 0:pas de paramtres, 1:parametrage standart */
	char	nom[23];		/* le nom : 22 chars maxi */
	char	need_preced_img;	/* si mountain doit fournir la precedente image calcul‚e dans img_dst */
	long	id;
	int 	(*calc_effet)(EFFET_PARA *adr_para);		/* adr routine de calcul */
	TRAN_DESC_PARAM *effet_desc_param;
}EFFET;

#define EFF_NO_PARAM 0
#define EFF_STD_PARAM 1

typedef struct
{	int 	nb_effets;
	long	id[MAX_CUR_EFFETS];		/* l'id de chaque effet */
	TRAN_REAL_PARAM effet_param[MAX_CUR_EFFETS];		/* les parametres pour chaque effet */
}STOQUAG_EFF;

/* protos 2 */
void copy_sto_eff(STOQUAG_EFF *src, STOQUAG_EFF *dst);
