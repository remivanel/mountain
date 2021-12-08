/********************MOUTAIN********************/
/*****          GESTION DES MODULES         ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "some_fnc.h"
#include "modules.h"
#include "export.h"
#include "projet.h"
#include "transit.h"
#include "geffets.h"
#include "select.h"
#include "mountain.h"
#include "objets.h"
#include "txts.h"
#include "sav_mod\sav_mod.h"


/* sav mod */
extern void (*ext_mpl_export)(EXT_MPL_EXP *ext_mpl_exp);
extern void (*ext_mod_export)(EXT_EXP_MOD *ext_exp_mod);
long trouve_text(void *adr_mod);
long boot_serial(void);
void decode_text(char *pos_text,long code);
extern WINDFORM_VAR binfo_var;
long	code_text='…e82';

extern char chemin_modules[150];
IMP_MOD imp_mod[20];
EXP_MOD exp_mod[10];
int nb_imp_mod=0;
int nb_exp_mod=0;
int mnt_registered;

static void make_fichier(long *fichier);
static long make_cle(char *fic);
long trouve_debut(void *adr_mod);
long checkcode(char *adr, long taille_en_long);
void uncrypt(char *adr,long cle, long size_code);

/* suite img */
extern WINDFORM_VAR suit_var;		/* import_suite img */
extern char import_new_suit;
static int mode_chm=1;		/* 1: repertoire 2: liste num */
static int mode_tri=1;		/* 1: nom , 2: date , 3: pas de tri */
static CHEMIN src;
static int in_nb,in_start, in_x ,in_y;
static long in_id_mod;
static char in_ext[5];			/* .EXT */
static char fic_in[200];		
static char zeros = 4;
static char mask_ajt[20];		/* masque d'ajout a fic_in */
static char prem_nom[14];		/* premier nom */
DTA fic_dta;

typedef struct
{	char nom[14];
	unsigned int date;
	unsigned int time;
	char used;
} LISTE;
LISTE *liste_nom;


/* def de fnc */
static void updt_mod(void);
void set_popup(OBJECT *adr_form, int button, int option, int index);
static char read_img(CHEMIN *src, char *nom);
void create_bss(void *adr_mod);

void load_modules(void)
{	int handle,retour;
	char chemin[150];
	char str[100];
	char tampon[100];
	DTA *fic_dta = Fgetdta();
	size_t taille_fic,taille_max,taille_bss, taille_mem;
	
	strcpy(chemin, chemin_modules);
	strcat(chemin,"*.IMP");
	retour=Fsfirst(chemin,0);
	
	while(retour == 0)
	{	strcpy(chemin, chemin_modules);
		strcat(chemin,fic_dta->d_fname);
		handle = (int)Fopen(chemin,FO_READ);
		if(handle>=0)
		{	taille_fic = fic_dta->d_length;
			
			/* RESERVE TOUTE LA MEMOIRE POUR LE MODULE */
			taille_mem = (size_t)Malloc(-1);
			taille_mem -= 128000L;
			imp_mod[nb_imp_mod].adr_mod =  Malloc(taille_mem);
			if(imp_mod[nb_imp_mod].adr_mod)
			{	
				Fread(handle,44+28,tampon);
				Fseek(0,handle,0);
				
				strcpy(imp_mod[nb_imp_mod].nom_fic,fic_dta->d_fname);
				/* PREND LES INFO DE L'ENTETE */
				imp_mod[nb_imp_mod].id = *(long *)(tampon + 28);
				imp_mod[nb_imp_mod].ver = *(long *)(tampon + 32);
				imp_mod[nb_imp_mod].size_buf_para = *(long *)(tampon + 36);
				imp_mod[nb_imp_mod].flags = *(long *)(tampon + 40);

				if(imp_mod[nb_imp_mod].flags & M_RELOG) /* A RELOGER */
				{	taille_bss = *(long *)(tampon + 10);
					taille_max = taille_fic + 256 - 28 + taille_bss;
					if(taille_max > taille_mem)
					{	sprintf(str,T_NO_MEM_TO_LOAD_MOD,fic_dta->d_fname);
						super_alert(1,1,str);
						goto module_suivant;
					}
					
					/* charge le module ou il faut*/
					Fread(handle,taille_fic,(char *)imp_mod[nb_imp_mod].adr_mod + 228);
					
					create_bss(imp_mod[nb_imp_mod].adr_mod); /* creation de la base page, intit bss et relocation */
					
					Mshrink(0,imp_mod[nb_imp_mod].adr_mod,taille_max); /* REDUIT LA MEMOIRE AU FICHIER + BSS (+ basepage) */
					
					strncpy(imp_mod[nb_imp_mod].description,tampon + 44,28);
					(long)imp_mod[nb_imp_mod].init_imp = (long)imp_mod[nb_imp_mod].adr_mod + 72 + 228;
					(long)imp_mod[nb_imp_mod].get_frame = (long)imp_mod[nb_imp_mod].adr_mod + 72 + 232;
					(long)imp_mod[nb_imp_mod].close_anim = (long)imp_mod[nb_imp_mod].adr_mod + 72 + 236;
				}else /* MODULE AUTO RELOGEABLE */
				{	Fread(handle,taille_fic,imp_mod[nb_imp_mod].adr_mod);
					Mshrink(0,imp_mod[nb_imp_mod].adr_mod,taille_fic); /* REDUIT LA MEMOIRE AU FICHIER */
					strncpy(imp_mod[nb_imp_mod].description,(char *)imp_mod[nb_imp_mod].adr_mod,28);
					(long)imp_mod[nb_imp_mod].init_imp = (long)imp_mod[nb_imp_mod].adr_mod + 44;
					(long)imp_mod[nb_imp_mod].get_frame = (long)imp_mod[nb_imp_mod].adr_mod + 48;
					(long)imp_mod[nb_imp_mod].close_anim = (long)imp_mod[nb_imp_mod].adr_mod + 52;
				}
								
				nb_imp_mod++;
			}
			else
			{	sprintf(str,T_NO_MEM_TO_LOAD_MOD,fic_dta->d_fname);
				super_alert(1,1,str);
			}
			Fclose(handle);
		}else
		{	sprintf(str,T_IMPOS_TO_OPEN_MOD,fic_dta->d_fname);
			super_alert(1,1,str);
		}
		module_suivant:;
		retour = Fsnext();
	}

	strcpy(chemin, chemin_modules);
	strcat(chemin,"*.EXP");
	retour=Fsfirst(chemin,0);
	
	while(retour == 0)
	{	strcpy(chemin, chemin_modules);
		strcat(chemin,fic_dta->d_fname);
		handle = (int)Fopen(chemin,FO_READ);
		if(handle>=0)
		{	exp_mod[nb_exp_mod].adr_mod =  Malloc(fic_dta->d_length);
			if(exp_mod[nb_exp_mod].adr_mod)
			{	Fread(handle,fic_dta->d_length,exp_mod[nb_exp_mod].adr_mod);

				(long)exp_mod[nb_exp_mod].init_exp = (long)exp_mod[nb_exp_mod].adr_mod + 48;
				(long)exp_mod[nb_exp_mod].put_frame = (long)exp_mod[nb_exp_mod].adr_mod + 52;
				(long)exp_mod[nb_exp_mod].mod_param = (long)exp_mod[nb_exp_mod].adr_mod + 56;
				(long)exp_mod[nb_exp_mod].mod_param_def = (long)exp_mod[nb_exp_mod].adr_mod + 60;

				strcpy(exp_mod[nb_exp_mod].nom_fic,fic_dta->d_fname);
				strncpy(exp_mod[nb_exp_mod].description,(char *)exp_mod[nb_exp_mod].adr_mod,28);
				exp_mod[nb_exp_mod].id = *(long *)((char *)exp_mod[nb_exp_mod].adr_mod + 28);
				exp_mod[nb_exp_mod].ver = *(long *)((char *)exp_mod[nb_exp_mod].adr_mod + 32);
				exp_mod[nb_exp_mod].size_buf_perm = *(long *)((char *)exp_mod[nb_exp_mod].adr_mod + 36);
				exp_mod[nb_exp_mod].size_buf_para = *(long *)((char *)exp_mod[nb_exp_mod].adr_mod + 40);
				exp_mod[nb_exp_mod].flags = *(long *)((char *)exp_mod[nb_exp_mod].adr_mod + 44);
				
				nb_exp_mod++;
			}
			else
			{	sprintf(str,T_NO_MEM_TO_LOAD_MOD,fic_dta->d_fname);
				super_alert(1,1,str);
			}
			Fclose(handle);
		}else
		{	sprintf(str,T_IMPOS_TO_OPEN_MOD,fic_dta->d_fname);
			super_alert(1,1,str);
		}
		retour = Fsnext();
	}

}

void unload_modules(void)
{	int i;
	
	for(i=0;i<nb_imp_mod;i++)
		Mfree(imp_mod[i].adr_mod);
	for(i=0;i<nb_exp_mod;i++)
		Mfree(exp_mod[i].adr_mod);
}

int get_mod_info(char *fic, int *x, int *y, int *nb_pict, long *id_mod, char *desc)
{	int handle,mod;
	INIT_PARAM para;
	long taille;
	void *adr_buf = 0;
	
	
	handle = (int)Fopen(fic,FO_READ);
	if(handle < 0)
		return 0;
	
	taille = Fseek(0,handle,2);
	Fseek(0,handle,0);
	
	
	
	for(mod=0;mod<nb_imp_mod;mod++)
	{	if(imp_mod[mod].size_buf_para)	/* on va pas se faire avoir par des taille 0 */
		{	adr_buf = Malloc(imp_mod[mod].size_buf_para);		/* le buffer permanent */
			if(!adr_buf)
				return 0;
		}
		
		para.size_fic = taille;
		para.handle = handle;
		para.work_adr = adr_buf;
		
		/* L'appel au module lui meme */
		(imp_mod[mod].init_imp)(&para);
		
		/* on libere le buffer, c'etait juste pour avoir des infos */
		if(adr_buf)
			Mfree(adr_buf);
		
		if(para.retour == 0)
		{	*id_mod = imp_mod[mod].id;		/* pour passer l'id du module */
			*x = para.x;
			*y = para.y;
			*nb_pict = (int)para.nb_img;
			strcpy(desc,para.desc);
			Fclose(handle);
			return 1;
		}
	}
	
	Fclose(handle);
	return 0;
}

int find_module(long id)
{	int mod;

	for(mod=0;mod<nb_imp_mod;mod++)
	{	if(imp_mod[mod].id == id)
			return mod;
	}
	
	return -1;
}
	
void form_suit(int event)
{	WINDFORM_VAR *ptr_var = &suit_var;
	int choix,bout,dum1,dum2;
	char pth[200], nom[14]="";
		
	if (event == OPEN_DIAL)
	{	set_text(ptr_var,SUIT_FIC,T_SELECTOR);
		set_text(ptr_var,SUIT_NB_IMG,"");
		set_text(ptr_var,SUITE_DESC,"");
		set_text(&suit_var,SUIT_MOD,"");
		updt_mod();
		set_popup(ptr_var->adr_form,SUIT_MODE_CHM,mode_chm,POP_RCH);
		set_popup(ptr_var->adr_form,SUIT_TRI,mode_tri,POP_TRI);
		wf_change(ptr_var,SUIT_OK,DISABLED,0);
		open_dialog(ptr_var, T_IMPRT_SUIT_IMG, SUIT_NUM,1);
	}
	
	else 
	{	choix = windform_do(ptr_var, event);
		
		zeros = suit_var.adr_form[SUIT_ZEROS].ob_spec.tedinfo->te_ptext[0] - '0';
		
		if(choix == CLOSE_DIAL)
			choix = SUIT_ANN;
		if(choix > 0)
		{	switch(choix)
			{	case SUIT_MODE_CHM :
					bout = pop_up(ptr_var,SUIT_MODE_CHM,POP_RCH);
					if(bout != -1 && bout != mode_chm)
					{	mode_chm = bout;
						updt_mod();
					}
					break;

				case SUIT_TRI :
					bout = pop_up(ptr_var,SUIT_TRI,POP_TRI);
					if(bout != -1)
						mode_tri = bout;
					break;

				case SUIT_FIC :
					pth[0] = 0;
					nom[0] = 0;
					if(mode_chm == 1)
						Fselecteur(&src,pth,nom,&bout,T_REP_PLUS_MASK);
					else
						Fselecteur(&src,pth,nom,&bout,T_PREM_IMAGE);
					if(bout)
					{	if(mode_chm == 1 && nom[0] == 0)
						{	strcpy(nom,"*.*");		/* par defaut r‚pertoire complet */
							strcat(src.complet,"*.*"); 
						}
						Dsetdrv(src.chemin[0] - 'A');
						Dsetpath(src.chemin);
						if(read_img(&src,nom) )
						{	
							if(mode_chm == 1)
							{	set_text(ptr_var,SUIT_FIC,fic_in);
								
							}else
							{	strcpy(pth,fic_in);
								if(zeros == 4) strcpy(nom,"????");
								if(zeros == 3) strcpy(nom,"???");
								if(zeros == 2) strcpy(nom,"??");
								if(zeros == 1) strcpy(nom,"?");
								if(zeros == 0) nom[0] = 0;
								strcat(pth,nom);
								strcat(pth,in_ext);
								set_text(ptr_var,SUIT_FIC,pth);
	
								set_text(ptr_var,SUITE_DEBUT,itoa(in_start,nom,10));
								set_text(ptr_var,SUIT_NB,itoa(in_nb,nom,10));
							}
							sprintf(pth,T_LIST_D_FIC,in_nb);
							set_text(ptr_var,SUIT_NB_IMG,pth);

							if(test_1st())
							{	wf_change(ptr_var,SUIT_OK,DRAW3D,1);		/* active l'OK */
								sprintf(pth,"%s, %d*%d",in_ext,in_x,in_y);
							}else
							{	wf_change(ptr_var,SUIT_OK,DISABLED,1);		/* d‚active l'OK */
								super_alert(1,1,T_FIC_NO_RECOGN_BY_MOD);
								pth[0]=0;
							}
							set_text(ptr_var,SUITE_DESC,pth);
						}
						graf_mouse(ARROW,0);
					}
					wf_change(ptr_var,choix,0,1);
					break;

				case SUIT_OK:
					if(mode_chm==2)
					{	dum1 = atoi(ptr_var->adr_form[SUITE_DEBUT].ob_spec.tedinfo->te_ptext);
						dum2 = atoi(ptr_var->adr_form[SUIT_NB].ob_spec.tedinfo->te_ptext);
						in_start = min(dum1,in_nb-1);
						in_nb = min(dum2,in_nb);
					}
					find_name(prem_nom,nom);
					new_objet(OBJ_SUIT, nom, src.chemin, "");

				case SUIT_ANN:
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

int test_1st(void)
{	int handle,mod;
	INIT_PARAM para;
	long taille;
	void *adr_buf = 0;
	char fic[200];
	
	/* TROUVE LE PREMIER FICHIER */
	liste_nom=0;
	find_next(prem_nom,0);
	if(liste_nom)
		Mfree(liste_nom);	
	strcpy(fic,src.chemin);
	strcat(fic,prem_nom);
	
	handle = (int)Fopen(fic,FO_READ);
	if(handle < 0)
		return 0;
	
	taille = Fseek(0,handle,2);
	Fseek(0,handle,0);
	
	for(mod=0;mod<nb_imp_mod;mod++)
	{	if(imp_mod[mod].size_buf_para)	/* on va pas se faire avoir par des taille 0 */
		{	adr_buf = Malloc(imp_mod[mod].size_buf_para);		/* le buffer permanent */
			if(!adr_buf)
				return 0;
		}
		
		para.size_fic = taille;
		para.handle = handle;
		para.work_adr = adr_buf;
		
		/* L'appel au module lui meme */
		(imp_mod[mod].init_imp)(&para);
		
		/* on libere le buffer, c'etait juste pour avoir des infos */
		if(adr_buf)
			Mfree(adr_buf);
		
		if(para.retour == 0)
		{	in_id_mod = imp_mod[mod].id;		/* pour passer l'id du module */
			in_x = para.x;
			in_y = para.y;
			set_text(&suit_var,SUIT_MOD,para.desc);
			Fclose(handle);
			return 1;
		}
	}
	
	Fclose(handle);
	return 0;
}

static void updt_mod(void)
{	
	if(mode_chm == 1)
	{	suit_var.adr_form[SUIT_ZEROS].ob_flags |= HIDETREE;
		suit_var.adr_form[SUITE_DEBUT].ob_flags |= HIDETREE;
		suit_var.adr_form[SUIT_NB].ob_flags |= HIDETREE;
		suit_var.adr_form[SUIT_FLECHES].ob_flags |= HIDETREE;
	
		suit_var.adr_form[SUIT_TRI].ob_flags &= ~HIDETREE;
		suit_var.adr_form[SUIT_TXT_TRI].ob_flags &= ~HIDETREE;
	
	}else
	{	suit_var.adr_form[SUIT_ZEROS].ob_flags &= ~HIDETREE;
		suit_var.adr_form[SUITE_DEBUT].ob_flags &= ~HIDETREE;
		suit_var.adr_form[SUIT_NB].ob_flags &= ~HIDETREE;
		suit_var.adr_form[SUIT_FLECHES].ob_flags = NORMAL;

		suit_var.adr_form[SUIT_TRI].ob_flags |= HIDETREE;
		suit_var.adr_form[SUIT_TXT_TRI].ob_flags |= HIDETREE;

	}
	if(suit_var.w_handle)
	{	wf_draw(&suit_var,SUIT_FON_ZEROS);
		wf_draw(&suit_var,SUIT_FON_TRI);
	}
}

int get_suit_info(int *x, int *y, int *nb_img,  long *id_mod, long *num_img)
{	*x = in_x;
	*y = in_y;
	*nb_img = in_nb;
	*id_mod = in_id_mod;
	*num_img=(long)atoi(suit_var.adr_form[SUIT_NUM].ob_spec.tedinfo->te_ptext);
	return 1;
}

int create_liste(char *adr_tab_nom)
{	int i,num=0;
	char nom[14];
	
	liste_nom=0;
	
	for(i=0;i<in_nb;i++)
	{	if( find_next(nom,i))
		{	strcpy(adr_tab_nom + i * 14,nom);
			num++;
		}
	}

	if(liste_nom)
		Mfree(liste_nom);	
	
	return i;
}

static char read_img(CHEMIN *src, char *nom)
{	size_t len;
	char nom_seul[14],nombre[8];
	char fic[150];
	
	graf_mouse(BUSYBEE,0);

	Fsetdta(&fic_dta);
	
	if(Fsfirst(src->complet,0) != 0)
		return 0;

	len = strlen(nom);
	do
	{	--len;
	} while (nom[len] != '.');		/* in trouve le point */

	strcpy(in_ext,&nom[len]);		/* prend l'ext */
	strcpy(nom_seul,nom);			/* copy tt le nom */
	nom_seul[len] = 0;				/* et vir l'ext */

	if(mode_chm == 2)		/* [ liste num‚rt‚e */
	{
	
	
		if(strlen(nom_seul) < zeros)
		{	zeros = strlen(nom_seul);
			set_text(&suit_var,SUIT_ZEROS,itoa(zeros,fic_in,10));
		}
	
		/* CORRECTION AUTOMATIQUE DU NOMBRE DE 'ZEROS' */
		try_again:;
		if( !(nom_seul[len - zeros] >= '0' &&  nom_seul[len - zeros] <= '9'))	/* verifi que c'est bien un nombre */
		{	if(zeros)
			{	zeros--;
				goto try_again;
			}
			in_start = 0;
		}
		/* au cas il aurait ete modifie */
		set_text(&suit_var,SUIT_ZEROS,itoa(zeros,fic_in,10));
	
		strcpy(nombre,&nom_seul[len - zeros]);		/* on prend le numero */
		in_start = atoi(nombre);
		nom_seul[len - zeros] = 0;						/* on l'enleve */
	
		strcpy(fic_in,src->chemin);			/* form fic_in , le chemin */
		strcat(fic_in,nom_seul);			/* le nom sans rien */
	
		/* comptage des fichier */
	
		in_nb = 1;
																		/*               /- zeros */
		sprintf(mask_ajt,"%s%s%dd%s",nom_seul,"%0",zeros,in_ext);	/* creation de NOMS%04d.EXT */
		if(zeros == 0)
		{	in_nb = 1;
			return 1;
		}
	
		for(;;)
		{	sprintf(fic,mask_ajt,in_nb + in_start);	/* cretion du nom de fichier */
			if(Fsfirst(fic,0) != 0)
				return 1;		/* on est au bout */
			in_nb++;
		}
	}	/* liste num‚rot‚e ] */
	else
	{			/*[  r‚pertoire */
		
		strcpy(fic_in,src->complet);	
	
		/* comptage des fichier */
	
		in_nb = 1;	
		while( Fsnext() == 0) in_nb++;
		return 1;
		
	}	/*  r‚pertoire ]*/
	
}

int find_next(char *str, int nb)			/* retour 0 si err */
{	size_t taille;
	char best_nom[14];
	int index=1,best_index;
	unsigned int best_time,best_date;
	 
	if(nb==0)
	{	if(mode_chm == 1)
		{	taille = sizeof(LISTE) * in_nb;
			liste_nom = Malloc(taille);
			if(!liste_nom)
				return 0;
			
			if(Fsfirst(fic_in,0) != 0)
				return 0;
			
			strcpy(liste_nom[0].nom,fic_dta.d_fname);
			liste_nom[0].date = fic_dta.d_date;
			liste_nom[0].time = fic_dta.d_time;
			liste_nom[0].used = 0;
			
			while(Fsnext() == 0) 
			{	strcpy(liste_nom[index].nom,fic_dta.d_fname);
				liste_nom[index].date = fic_dta.d_date;
				liste_nom[index].time = fic_dta.d_time;
				liste_nom[index++].used = 0;
			}
			
		}
	}
	
	if(nb >= in_nb)		/* on demande un fic hors liste */
		return 0;
	
	if(mode_chm == 1)
	{	
		if(mode_tri == 1)		/* par nom */
		{	strcpy(best_nom,"ZZZZZZZZ.ZZZ"); best_index = 0;
			for(index=0; index<in_nb ; index++)
			{	if(liste_nom[index].used == 0)							/* nom libre */
				{	if(alpha_cmp(best_nom,liste_nom[index].nom )	)		/* tri alphanumeric */
					{	strcpy(best_nom,liste_nom[index].nom);
						best_index = index;
					}
				}
			}
			strcpy(str,liste_nom[best_index].nom);
			liste_nom[best_index].used = 1;
			return 1;
		}else
		if(mode_tri == 2)		/* par date */
		{	best_date = 0xffff;				/* on trouvera bien une date avant h‚ h‚ !*/
			best_time = 0xffff;
			best_index = 0;
			for(index=0; index<in_nb ; index++)
			{	if(liste_nom[index].used == 0)							/* fic libre */
				{	if(date_cmp(best_date,best_time,liste_nom[index].date,liste_nom[index].time)	)		/* tri date + time */
					{	best_date = liste_nom[index].date;
						best_time = liste_nom[index].time;
						best_index = index;
					}
				}
			}
			strcpy(str,liste_nom[best_index].nom);
			liste_nom[best_index].used = 1;
			return 1;
		}else
		if(mode_tri == 3)		/* pas de tri */
		{	strcpy(str,liste_nom[nb].nom);
			liste_nom[nb].used = 1;
			return 1;
		}
		return 0;
	}else
	{	sprintf(str,mask_ajt,nb+in_start);				/* creation du nom de fichier */
		return 1;
	}
}

int alpha_cmp(char *best,char *actual)
{	size_t len,i;

	len = min(strlen(best),strlen(actual));
	
	for(i=0;i<len;i++)
	{	if(actual[i]<best[i])
			return 1;
		else if(actual[i]>best[i])
			return 0;					/* on passe au suivant que si les lettres snt = */
	}
	return 0;
}

int date_cmp(unsigned int b_date,unsigned int b_time,unsigned int a_date,unsigned int a_time)
{	
	if(a_date<b_date)
		return 1;				/* + recent */
	else if(a_date>b_date)
		return 0;				/* - recent */
	else							/* mˆme date */
	{	if(a_time<b_time)
			return 1;				/* + recent */
		else if(a_time>b_time)
			return 0;
	}
	return 2;			/* mˆme date et time */
}

int load_sav_mod(void)
{	int handle;
	char chemin[150];
	char str[100];
	char tampon[100];
	DTA *fic_dta = Fgetdta();
	size_t taille_fic,taille_max,taille_bss, taille_mem;
	void *adr_mod;
	long id, size_code, checksum, checksum2,cle;
	char *pos_text;
	long user_info[50];
	void *debut;
	
	strcpy(chemin, chemin_modules);
	strcat(chemin,"MOUNT_R.DTA");

	Fsfirst(chemin,0);

	handle = (int)Fopen(chemin,FO_READ);
	if(handle>=0)
	{	taille_fic = fic_dta->d_length;
		
		/* RESERVE TOUTE LA MEMOIRE POUR LE MODULE */
		taille_mem = (size_t)Malloc(-1);
		taille_mem -= 128000L;
		adr_mod =  Malloc(taille_mem);
		if(adr_mod)
		{	
			Fread(handle,44+28,tampon);
			Fseek(0,handle,0);
				
			/* PREND LES INFO DE L'ENTETE */
			id = *(long *)(tampon + 28);
			
			if(id != 'MNT1')
			{	Mfree(adr_mod);
				return 0;
			}
			
			taille_bss = *(long *)(tampon + 10);
			taille_max = taille_fic + 256 - 28 + taille_bss;
			if(taille_max > taille_mem)
			{	sprintf(str,T_NO_MEM_TO_LOAD_MOD,fic_dta->d_fname);
				Mfree(adr_mod);
				super_alert(1,1,str);
				return 0;
			}
					
			/* charge le module ou il faut*/
			Fread(handle,taille_fic,(char *)adr_mod + 228);
			
			pos_text = (char *)trouve_text(adr_mod);
			decode_text(pos_text,code_text);
			strcpy(binfo_var.adr_form[BINFO_REG].ob_spec.tedinfo->te_ptext,pos_text);
			
			/* ********* DECRYPTAGE *********** */
			make_fichier(user_info);
			cle=make_cle((char *)user_info);
			
			debut = (void *)trouve_debut(adr_mod);
			size_code = (long)pos_text - (long)debut - 10;
			size_code /= 4;		/* siez_code en LONG */
			size_code -= 2;
			checksum = *(long *)debut;
			uncrypt((char *)debut + 4,cle,size_code);
			checksum2 = checkcode((char *)debut + 4, size_code);
			
			
			create_bss(adr_mod); /* creation de la base page, intit bss et relocation */
					
			Mshrink(0,adr_mod,taille_max); /* REDUIT LA MEMOIRE AU FICHIER + BSS (+ basepage) */
			
			if(checksum == checksum2)
			{	(long)ext_mpl_export = (long)adr_mod + 44 + 220;
				(long)ext_mod_export = (long)adr_mod + 48 + 220;
				mnt_registered=TRUE;
			}else
			{	binfo_var.adr_form[BINFO_REG].ob_spec.tedinfo->te_color |= 0x600;
				mnt_registered=FALSE;
			}
			return 1;
		}
	}
	return 0;
}

static long make_cle(char *fic)
{	long cle=0;
	long *data;
	
	data = (long *)fic;
	
	cle = data[0] << 16;
	cle += data[9];
	cle += data[10] << 10;
	cle += data[11] << 11;
	cle += data[12] << 12;
	cle += data[13] << 13;
	cle += data[14] << 14;
	
	return cle;
}

static void make_fichier(long *fichier)
{	int compt;
	DISKINFO disk;
	int retour;
	char str[]="6_8‚1_..3";
	long cook;

	compt=0;
	retour = Dfree(&disk,3);
	if(retour == 0)		/* partition c existe */
	{	fichier[compt++] = disk.b_total;
		fichier[compt++] = disk.b_secsiz;
		fichier[compt++] = disk.b_clsiz;
	}else
	{	fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
	}
	retour = Dfree(&disk,4);
	if(retour == 0)		/* partition d existe */
	{	fichier[compt++] = disk.b_total;
		fichier[compt++] = disk.b_secsiz;
		fichier[compt++] = disk.b_clsiz;
	}else
	{	fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
	}
	retour = Dfree(&disk,5);
	if(retour == 0)		/* partition e existe */
	{	fichier[compt++] = disk.b_total;
		fichier[compt++] = disk.b_secsiz;
		fichier[compt++] = disk.b_clsiz;
	}else
	{	fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
		fichier[compt++] = *(long *)str;
	}
	
	if(get_cookie('MgMc') == '_NO_')
		cook = boot_serial();			 /* 9 */
	else
		cook = 0x44ac;		/* MMac !! */
	fichier[compt++] = cook;

	cook=get_cookie('_CPU');
	fichier[compt++] = cook;
	cook=get_cookie('_VDO');
	fichier[compt++] = cook;
	cook=get_cookie('_MCH');
	fichier[compt++] = cook;
	cook=get_cookie('_FPU');
	fichier[compt++] = cook;
	cook=get_cookie('_AKP');
	fichier[compt++] = cook;
}