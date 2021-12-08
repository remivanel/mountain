/*******************MOUNTAIN********************/
/*****           SOURCE PRINCIPAL           ****/
/*****            Par Vanel R‚mi            ****/
/*****         Premier code : 1/5/97        ****/
/***** PremiŠre commercialisation: 25/12/98 ****/
/***********************************************/

#define _debug_
#define _CHEMIN_APP_ "C:\\APPS\\coding\\MNT\\"
#define VERSION_MNT "0.91"

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "select.h"
#include "mountain.h"
#include "gest_flh.h"
#include "gest_fli.h"
#include "transit.h"
#include "geffets.h"
#include "pistes.h"
#include "flh_lib.h"
#include "objets.h"
#include "visu.h"
#include "projet.h"
#include "anim.h"
#include "some_fnc.h"
#include "modules.h"
#include "preview.h"
#include "mouvemnt.h"
#include "transp.h"
#include "export.h"
#include "time_spl.h"
#include "txts.h"


void form_new(int event);
void form_optc(int event);
void form_optp(int event);
void form_optg(int event);
void form_optv(int event);
void form_binfo(int event);
void form_iflh(int event);
void form_ifli(int event);
void form_opto(int event);
void form_ifon(int event);
void form_carte(int event);
void form_col(int event);
void form_infi(int event);
void form_zinf(int event);
void init_prog(void);
void fin_prog(void);
void main(void);
void find_name(char *str, char *dest);
void find_name_ext(char *str, char *dest);
void find_path(char *str);
void init_fin(void);
void set_clip(int clip_flag, GRECT *area);
void sauve_prefs(void);
void set_ass(int obj, int index);
void ass_c(int oass, int fon);
void scan_coul(int rgb[3]);
void vs_color255(int rgb[3]);
void imp_module(void);
void update_mdi(void);

char *trim_cpy(char *dest, char *src);
int get_mem(void);
int get_popup(OBJECT *adr_form, int button, int index);
void set_popup(OBJECT *adr_form, int button, int option, int index);
long get_tt_ram(void);

#define SIZE_CHM 40
#define ASS_C 136

/* Variables globales */
WINDFORM_VAR binfo_var;
WINDFORM_VAR iflh_var;
WINDFORM_VAR new_var;
WINDFORM_VAR opto_var;		/* opt objet */
WINDFORM_VAR optc_var;		/* opt chemin */
WINDFORM_VAR optv_var;		/* opt visu */
WINDFORM_VAR optg_var;		/* opt general */
WINDFORM_VAR optp_var;		/* opt pites */
WINDFORM_VAR ifon_var;		/* image de fond */
WINDFORM_VAR mou_var;		/* mouvement */
WINDFORM_VAR carte_var;		/* carte graphique */
WINDFORM_VAR col_var;		/* selecteur de couleur */
WINDFORM_VAR infi_var;		/* info mod import */
WINDFORM_VAR mem_var;		/* barre mem */
WINDFORM_VAR tran_var;		/* transparence */
WINDFORM_VAR opta_var;		/* opt/info sur un prj obj */
WINDFORM_VAR exp_var;		/* boite d'export */
WINDFORM_VAR mpl_var;		/* boite d'export M Player*/
WINDFORM_VAR int_var;		/* intensite */
WINDFORM_VAR zinf_var;		/* boite info zoom */
WINDFORM_VAR alr_var;		/* boite d'alerte */
WINDFORM_VAR suit_var;		/* import_suite img */
WINDFORM_VAR opre_var;		/* opt preview */
WINDFORM_VAR ctim_var;		/* Choix d'une time spline */
WINDFORM_VAR etim_var;		/* Edition d'une time spline */
WINDFORM_VAR itra_var;		/* info sur transition */
WINDFORM_VAR ptra_var;		/* param des transitions */
WINDFORM_VAR ceff_var;		/* Choix des effets */
WINDFORM_VAR supi_var;		/* Suppression de piste */

int use_nvdi;
int use_n_alert;
int show_mem;
int show_zinf;
int ultra_temps_reel;
long tot_mem;
int int_rel;
int carte;			/* carte graphique */
int force_vdi_use;	/* passe par un buffer intermediaire */
int op_fobj,op_fprj,op_fpist,op_ftran;
int cur_mdi=0;	/* module info import */
int make_new_prj;
int tab_zoom[10] = {1,2,5,8,15,25,50,100};
int old_id_bout,old_id_txt;
int nb_fonts,n_bout,n_txt;
int tmp_n_bout,tmp_n_txt;
int Magic_flag,Winix_flag;
int menu_id;
int coul_caller;
int mode_smpte;
int gemdos_version;
int xb,yb,wb,hb;
#define boot_tot 12

typedef struct
{	char	name[33];
	int	id;
} NOM_FONT;
NOM_FONT *nom_font=0;

/* Variables d'interface et GEM */
OBJECT *adr_menu;			/* Adresse du menu */
OBJECT *adr_key;			/* Adresse du boite imp flh */
OBJECT *adr_ctrl;			/* Adresse de la barre  de control visualisation*/
int char_height,prev_rgb[3];
extern int speedo_flag;			/* Speedo GDOS pr‚sent ? */
extern int taille_def;
extern int id_texte;
extern int id_boutton;
extern int draw_round;

/* Variables animations */
extern ANIM_PAR anim_par[MAX_ANIM_PAR];

extern int nb_imp_mod;
extern int nb_exp_mod;
extern IMP_MOD imp_mod[10];
extern EXP_MOD exp_mod[10];
int mode_ie;

/* Variables fenetre preview */
extern int hnd_wprev;
extern int cur_prev;				/* preview active */
extern int cur_aprev;

/* Variables fenetre objet */
extern int nb_objets;
extern OBJET objet[MAX_OBJET];
extern int info_objet;
extern int hnd_wobjet;
extern int xfo,yfo,wfo,hfo;			/* coord fenetre */
extern int xfou,yfou,wfou,hfou;   /* coord utiles */
extern int w_obj_open;

/* Variables fenetre visu */
extern int cur_visu;				/* visu active */
extern int visu_objet;
extern int hnd_wvisu;
extern int visu_pos,visu_boucle;
extern int visu_fast;
extern int visu_frame;
extern int visu_info;
extern int visu_playing;
extern int def_x_pos_visu,def_y_pos_visu;

/* Variables fenetre prev */
extern int def_x_pos_prev,def_y_pos_prev;

/* Variables import FLH */
CHEMIN flh,ckey;

/* Variables fenetre projet */
extern int hnd_wprojet;
extern int xfr,yfr,wfr,hfr;			/* coord fenetre */
extern int xfru,yfru,wfru,hfru;   	/* coord utiles */

/* Variables fenetre transition */
extern int hnd_wtransit;
extern int xft,yft,wft,hft;			/* coord fenetre */
extern int xftu,yftu,wftu,hftu;   	/* coord utiles */
extern int effet_parametrisation;

/* Variables fenetre pistes */
extern int deb_img;		/* premiere img affichee */
extern int piste_animA;
extern int piste_animB;
extern int piste_supA;
extern int piste_supB;
extern int piste_tran;
extern int piste_sup;
extern int aff_mode,aff_size_buffer,aff_prop;
extern int *aff_buffer, aff_nombre, aff_max_nombre, aff_cur_pos;
extern int hnd_wpistes;
extern int xfp,yfp,wfp,hfp;			/* coord fenetre */
extern int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern int draw_intensitee;

/* Variables img de fond */
int temp_obj_fon;
int temp_r,temp_v,temp_b;
int color[3];

/* Variables Projet */
extern int actu_projet;			/* is there any projet here ? */
extern PROJET projet;
extern char chemin_projet[150];
extern int selected_prj_obj;

/* Variables options */
char chemin_key[150] = "C:\\KEYFRAME\\";
char chemin_anim_externe[150] = "E:\\IMAGES\\M_PLAYER\\M_PLAYER.PRG";
char chemin_img_externe[150] = "E:\\IMAGES\\VIEWER\\SHOWER.TTP";
char chemin_modules[150] = "D:\\PROJETS\\MOUNTAIN\\";
extern char chemin_mplayer[200];
char tmp_chemin_key[150], tmp_chemin_anim_externe[150], tmp_chemin_img_externe[150], tmp_chemin_modules[150],tmp_chemin_mplayer[200];

/* Preview */
extern int aprev_mode;
extern int aprev_preemptif;
extern int aprev_curseur;

/* variables tache de fond */
extern int current_tache;

/* ldg */
extern int ldg_install_ok;
extern int ldg_screen_ok;
int ldg_install;
int ldg_inst_if_sup_8;

/* Vriables iflh et ifli*/
char import_newflh = FALSE;
char import_fli = FALSE;
int  fli_flc;
int  prem_frame;
long pos_prem_frame;
char iflh_flhok = FALSE;
char nom_key[150];

char import_new_suit = FALSE;

void main (void)
{	int event;
	int flags;

	init_prog();
	
	menu_bar(adr_menu, 1);
	
	graf_mouse(ARROW,0);
	
	if(show_zinf)
	{	form_zinf(OPEN_DIAL);
		menu_icheck(adr_menu, MN_ZINF, TRUE );	
	}
	if(show_mem)
	{	form_mem(OPEN_DIAL);
		menu_icheck(adr_menu, MN_MEM, TRUE );	
	}
	if(mode_smpte)
		menu_icheck(adr_menu, MN_SMPTE, TRUE );	
	
	form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */

	for (;;)		/* BOUCLE PRINCIPALE DU PROGRAMME */
	{	
		if(current_tache)
			flags = MU_MESAG|MU_BUTTON|MU_KEYBD|MU_TIMER;
		else
			flags = MU_MESAG|MU_BUTTON|MU_KEYBD;		/* pour aes 4.1 */
		
		event = get_evnt(flags, 0L, 15);

		menu_keyshort(adr_menu, event, 0); /* Raccourcis Clavier des Menus. */
		
		if(mousek == 2)
			nbr_clics = 2;
		
		/* *** Aiguillage en fonction du Handle : *** */
		
		if(event & MU_TIMER)
			launch_tache();
		
		if(event == MU_KEYBD)  /* Gestion a la main de quelques raccourcis clavier */
		{	if( (key >> 8) == 0x61)	/* touche undo */
			{	if(key_s &(K_RSHIFT|K_LSHIFT)) /* mode precalc temporaire */
				{	aprev_mode=1-aprev_mode;
					aprev_start();
					aprev_mode=1-aprev_mode;
				}else
					aprev_start();
			}
			if(wtop == hnd_wvisu && cur_visu)
			{	if((key >> 8) == 0x72)	/* touche enter */
					visu_play_button();
				else if((key >> 8) == 0x70)	/* touche 0 du p.n. */
					visu_stop_button();
				else if((key >> 8) == 0x64)	/* touche ) du p.n. */
				{	adr_ctrl[V_FORW].ob_state = 1;
					redraw_visu_obj(V_FORW);
					if(key_s &(K_RSHIFT|K_LSHIFT)) /* av de 10 img */
					{	if(visu_frame < objet[visu_objet].nb_img-10)
						{	visu_frame+=10;
							aff_frame();
						}else {visu_frame = objet[visu_objet].nb_img-1; aff_frame();}
					}else
					{	if(visu_frame < objet[visu_objet].nb_img-1)
						{	visu_frame++;
							aff_frame();
						}
					}
					adr_ctrl[V_FORW].ob_state = 0;
					redraw_visu_obj(V_FORW);
				}else if((key >> 8) == 0x63)	/* touche ( du p.n. */
				{	adr_ctrl[V_BACK].ob_state = 1;
					redraw_visu_obj(V_BACK);
					if(key_s &(K_RSHIFT|K_LSHIFT)) /* recul de 10 img */
					{	if(visu_frame > 9)
						{	visu_frame-=10;
							aff_frame();
						}else {visu_frame = 0; aff_frame();}
					}else
					{	if(visu_frame > 0)
						{	visu_frame--;
							aff_frame();
						}
					}
					adr_ctrl[V_BACK].ob_state = 0;
					redraw_visu_obj(V_BACK);
				}


			}
		}
		
		if ((event & MU_MESAG) && buff[0] == MN_SELECTED)
		{
			switch (buff[4])
			{
			case MN_INFO :
				form_binfo(OPEN_DIAL);
				break;
			case MN_QUIT :
				fin_prog();
				break;

			case MN_NEW :
				if(et_ke_kon_fe_du_prj())
				{	make_new_prj=TRUE;
					form_new(OPEN_DIAL);
				}
				break;

			case MN_LOAD :
				if(et_ke_kon_fe_du_prj())
					actu_projet = load_projet();
				break;
				
			case MN_SAV :
			case MN_SAV_COM :
				save_projet(buff[4]==MN_SAV_COM);
				break;
				
			case MN_IMP_RIM :
				imp_module();
				break;

			case MN_IMP_SUIT :
				import_new_suit = TRUE;
				form_suit(OPEN_DIAL);
				break;
			
			case MN_IMP_FLI :
				import_newflh = TRUE;
				import_fli = TRUE;
				form_ifli(OPEN_DIAL);
				break;
				
			case MN_IMP_FLH :
				import_newflh = TRUE;
				import_fli = FALSE;
				form_iflh(OPEN_DIAL);
				break;

			case MN_EXP_MOD :
				if(actu_projet)
					form_exp(OPEN_DIAL);
				break;

			case MN_EXP_MPL :
				if(actu_projet)
					form_mpl(OPEN_DIAL);
				break;

			case MN_INFO_MDI :
					form_infi(OPEN_DIAL);
				break;
				
			case MN_OBJ :
				if(!w_obj_open)
				{	wind_open(hnd_wobjet,xfo,yfo,wfo,hfo);
					w_obj_open = TRUE;
					wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
					slider_o();
				}
				break;

			case MN_PISTES :
				if(actu_projet)
				{	if(!w_trk_open)
					{	wind_open(hnd_wpistes,xfp,yfp,wfp,hfp);
						wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
						slider_ph();
						slider_pv();
						w_trk_open=TRUE;
					}
				}
				break;
			
			case MN_PROJET :
				if(actu_projet)
				{	if(!w_prj_open)
					{	wind_open(hnd_wprojet,xfr,yfr,wfr,hfr);
						wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
						slider_r();
						w_prj_open=TRUE;
					}
				}
				break;

			case MN_TRAN :
				if(!w_trsit_open)
				{	wind_open(hnd_wtransit,xft,yft,wft,hft);
					wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
					slider_t();
					w_trsit_open=TRUE;
				}
				break;

			case MN_ZINF :
				if(show_zinf)
				{	close_dialog(&zinf_var);
					show_zinf = 0;
				}else
				{	show_zinf = 1;
					form_zinf(OPEN_DIAL);
				}
				menu_icheck(adr_menu, MN_ZINF, show_zinf );	
				break;
				
			case MN_VISU :
				if(actu_projet && selected_prj_obj != -1)
					new_visu(prj_obj[selected_prj_obj].objet);
				break;

			case MN_INFO_OPT :
				if(actu_projet && selected_prj_obj != -1)
					form_opta(OPEN_DIAL);
				break;

			case MN_DEP :
				if(actu_projet && selected_prj_obj != -1)
					new_mvt();
				break;

			case MN_EFF :
				if(actu_projet && selected_prj_obj != -1)
					form_ceff(OPEN_DIAL);
				break;

			case MN_TRANSP :
				if(actu_projet && selected_prj_obj != -1)
					new_tran();
				break;
			case MN_INT :
				if(actu_projet && selected_prj_obj != -1)
					form_int(OPEN_DIAL);
				break;

			case MN_ANI1 :  piste_animA = 1 - piste_animA;
				menu_icheck(adr_menu, MN_ANI1, piste_animA);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;
			case MN_ANI2 :  piste_animB = 1 - piste_animB;
				menu_icheck(adr_menu, MN_ANI2, piste_animB);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;
			case MN_EFF1 :  piste_supA = 1 - piste_supA;
				menu_icheck(adr_menu, MN_EFF1, piste_supA);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;
			case MN_EFF2 :  piste_supB = 1 - piste_supB;
				menu_icheck(adr_menu, MN_EFF2, piste_supB);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;	
			case MN_COUPL :  piste_tran = 1 - piste_tran;
				menu_icheck(adr_menu, MN_COUPL, piste_tran);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;
			case MN_GENE :  piste_sup = 1 - piste_sup;
				menu_icheck(adr_menu, MN_GENE, piste_sup);
				slider_pv();
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				break;
			case MN_SUPA :
				if(actu_projet)
				{	projet.nb_sup_A++;
					slider_pv();
					if(piste_supA)
						redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				}
				break;
			case MN_SUPB :
				if(actu_projet)
				{	projet.nb_sup_B++;
					slider_pv();
					if(piste_supB)
						redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				}
				break;
			case MN_SUPG :
				if(actu_projet)
				{	projet.nb_sup_G++;
					slider_pv();
					if(piste_sup)
						redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				}
				break;

			case MN_SUPPR_PI :
				if(actu_projet)
					form_supi(OPEN_DIAL);
				break;

			case MN_PRJFORMAT :
				if(actu_projet)
				{	make_new_prj=FALSE;
					form_new(OPEN_DIAL);
				}
				break;
				
			case MN_PRJFON :
				if(actu_projet)
					form_ifon(OPEN_DIAL);
				break;

			case MN_CHM :
				form_optc(OPEN_DIAL);
				break;
			case MN_OPTGENE :
				form_optg(OPEN_DIAL);
				break;
			case MN_OPTPISTES :
				form_optp(OPEN_DIAL);
				break;
			case MN_OPTVISU :
				form_optv(OPEN_DIAL);
					break;
			case MN_OPTPREV :
				form_opre(OPEN_DIAL);
				break;
			case MN_CARTE :
				form_carte(OPEN_DIAL);
				break;
			case MN_MEM :
				if(show_mem)
				{	close_dialog(&mem_var);
					show_mem = 0;
				}else
				{	show_mem = 1;
					form_mem(OPEN_DIAL);
				}
				menu_icheck(adr_menu, MN_MEM, show_mem );	
				break;
			case MN_SMPTE :
				mode_smpte = 1-mode_smpte;
					redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				menu_icheck(adr_menu, MN_SMPTE, mode_smpte );	
				break;
			case MN_SAVOPT :
				sauve_prefs();
				break;
			}
			menu_tnormal(adr_menu, buff[3], 1);
			buff[3]=0;
		}
		else
		{

			/* WINIX */
			if( buff[0] == WM_BOTTOMED)
				wind_set(buff[3],WF_BOTTOM,0,0,0,0);		/* mise en arriere plan par Winx */
		
/* *** Aiguillage en fonction du Handle : *** */

			if (buff[3] > 0)	/* Si W_Handle > 0 .... */
			{	/* TOUCHE ENTER */
/*				if((buff[3] == hnd_wpistes || buff[3] == hnd_wobjet
					|| buff[3] == hnd_wprev || buff[3] == hnd_wprojet)
					&& ((key & 0xff) == 0xd) && event == MU_KEYBD)
				{	key = 0;
					aprev_start();
				}
				else */ if (buff[3] == binfo_var.w_handle)
					form_binfo(event);
				else if (buff[3] == iflh_var.w_handle)
				{	if(import_fli)
						form_ifli(event);
					else
						form_iflh(event);
				}	
				else if (buff[3] == opto_var.w_handle)
					form_opto(event);
				else if (buff[3] == optc_var.w_handle)
					form_optc(event);
				else if (buff[3] == new_var.w_handle)
					form_new(event);
				else if (buff[3] == optg_var.w_handle)
					form_optg(event);
				else if (buff[3] == optp_var.w_handle)
					form_optp(event);
				else if (buff[3] == optv_var.w_handle)
					form_optv(event);
				else if (buff[3] == col_var.w_handle)
					form_col(event);
				else if (buff[3] == ifon_var.w_handle)
					form_ifon(event);
				else if (buff[3] == carte_var.w_handle)
					form_carte(event);
				else if (buff[3] == mou_var.w_handle)
					form_mou(event);
				else if (buff[3] == tran_var.w_handle)
					form_tran(event);
				else if (buff[3] == infi_var.w_handle)
					form_infi(event);
				else if (buff[3] == opta_var.w_handle)
					form_opta(event);
				else if (buff[3] == ctim_var.w_handle)
					form_ctim(event);
				else if (buff[3] == exp_var.w_handle)
					form_exp(event);
				else if (buff[3] == mpl_var.w_handle)
					form_mpl(event);
				else if (buff[3] == ceff_var.w_handle)
					form_ceff(event);
				else if (buff[3] == zinf_var.w_handle)
					form_zinf(event);
				else if (buff[3] == mem_var.w_handle)
					form_mem(event);
				else if (buff[3] == suit_var.w_handle)
					form_suit(event);
				else if (buff[3] == int_var.w_handle)
					form_int(event);
				else if (buff[3] == opre_var.w_handle)
					form_opre(event);
				else if (buff[3] == etim_var.w_handle)
					form_etim(event);
				else if (buff[3] == itra_var.w_handle)
					form_itra(event);
				else if (buff[3] == ptra_var.w_handle && effet_parametrisation)
					form_ptra2(event);
				else if (buff[3] == ptra_var.w_handle)
					form_ptra(event);
				else if (buff[3] == supi_var.w_handle)
					form_supi(event);
				else if (buff[3] == hnd_wobjet)
					window_objet(event);
				else if (buff[3] == hnd_wprev)
					window_prev(event);
				else if (buff[3] == hnd_wtransit)
					window_transit(event);
				else if (buff[3] == hnd_wprojet)
					window_projet(event);
				else if (buff[3] == hnd_wpistes)
					window_pistes(event);
				else if (buff[3] == hnd_wvisu)
					window_visu(event);

			}
		}
	}
}

void form_mem(int event)
{
	WINDFORM_VAR *ptr_var = &mem_var;
	int choix;
	long mem;
	
	if (event == OPEN_DIAL || (event == UPDT_MEM && mem_var.w_handle == FALSE))
	{	if(show_mem)
		{	open_dialog(ptr_var, "Mem", 0, 1);	
			event = UPDT_MEM;
		}
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
			{	close_dialog(ptr_var);
				show_mem = 0;
				menu_icheck(adr_menu, MN_MEM, show_mem );	
			}
		}
	}
	
	if(event == UPDT_MEM)
	{	if(show_mem)
		{	mem = tot_mem - mem_free();
			choix = (int)( ((long)mem_var.adr_form[MEM_FON].ob_width * mem)/tot_mem);
			if(mem_var.adr_form[MEM_ASS].ob_width != choix)
			{	mem_var.adr_form[MEM_ASS].ob_width = choix;
				wf_draw(&mem_var,MEM_FON);
			}
		}
	}
}

void form_zinf(int event)
{
	WINDFORM_VAR *ptr_var = &zinf_var;
	int choix,dum,larg_img;
	
	if (event == OPEN_DIAL)
	{	if(show_zinf)
		{	open_dialog(ptr_var, "Info", 0, 1);	
		}
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
			{	close_dialog(ptr_var);
				show_zinf = 0;
				menu_icheck(adr_menu, MN_ZINF, show_zinf );	
			}
			else if (choix == ZIN_POPZ)
			{	dum = pop_up(ptr_var,ZIN_POPZ,POP_ZOOM);
				if(dum>0)
					projet.zoom = tab_zoom[dum-1];
				if(actu_projet)
				{	larg_img = (int)(((wfpu-18)*(long)projet.zoom)/48L);
					deb_img=(int)min(deb_img,projet.lenght-larg_img);
					deb_img=max(0,deb_img);
					slider_ph();
					redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				}
			}	
		}
	}
}

void set_pop_zoom(int zoom)
{	int i;
	
	for(i=0;i<8;i++)
	{	if(zoom == tab_zoom[i])
		{	set_popup(zinf_var.adr_form, ZIN_POPZ, i+1, POP_ZOOM);
			if(show_zinf)
				wf_draw(&zinf_var,ZIN_POPZ);
		}
	}
}

void set_curseur(int pos)
{	int i;
	char *str = zinf_var.adr_form[ZIN_CUR].ob_spec.tedinfo->te_ptext;
	if(show_zinf)
	{	if(mode_smpte)
			img_to_smpte(pos,projet.speed,str);
		else itoa(pos,str,10);
		i=9;
		do
		{	str[i]=' ';
			--i;
		}while(str[i] != 0);
		str[i]=' ';
		str[9]=0;
		wf_draw(&zinf_var,ZIN_CUR);
	}
}

void form_binfo(int event)
{
	WINDFORM_VAR *ptr_var = &binfo_var;
	int choix;
	long mem;
	
	if (event == OPEN_DIAL)
	{	mem = mem_free();
		sprintf(ptr_var->adr_form[INFO_MEM].ob_spec.tedinfo->te_ptext,
		        T_DISPO_MEM,mem/1024);
		if(ptr_var->w_handle != -1)
			wf_draw(ptr_var,0);
		open_dialog(ptr_var, "Mountain Informations", 0, 0);	
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
				close_dialog(ptr_var);
			else if(choix == BINFO_GREETS && (binfo_var.adr_form[BINFO_BGREETS].ob_flags & HIDETREE))
			{	binfo_var.adr_form[BINFO_BGREETS].ob_flags = binfo_var.adr_form[BINFO_BGREETS].ob_flags - HIDETREE;
				binfo_var.adr_form[BINFO_BLAUT].ob_flags |= HIDETREE;
				objc_draw(ptr_var->adr_form,0,MAX_DEPTH,binfo_var.adr_form[0].ob_x+12,binfo_var.adr_form[0].ob_y+112,binfo_var.adr_form[BINFO_BGREETS].ob_width,binfo_var.adr_form[BINFO_BGREETS].ob_height);
			}
			else if(choix == BINFO_LAUT && (binfo_var.adr_form[BINFO_BLAUT].ob_flags & HIDETREE))
			{	binfo_var.adr_form[BINFO_BLAUT].ob_flags = binfo_var.adr_form[BINFO_BLAUT].ob_flags - HIDETREE;
				binfo_var.adr_form[BINFO_BGREETS].ob_flags |= HIDETREE;
				objc_draw(ptr_var->adr_form,0,MAX_DEPTH,binfo_var.adr_form[0].ob_x+12,binfo_var.adr_form[0].ob_y+112,binfo_var.adr_form[BINFO_BGREETS].ob_width,binfo_var.adr_form[BINFO_BGREETS].ob_height);
			}
			
		}
	}
}

void form_infi(int event)
{
	WINDFORM_VAR *ptr_var = &infi_var;
	int choix;
	
	if (event == OPEN_DIAL)
	{	mode_ie=0;
		init_radio(ptr_var,MDI_IMP);
		update_mdi();
		open_dialog(ptr_var, T_TITR_IO_MOD, 0, 1);	
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
				close_dialog(ptr_var);
			else
			{	if(choix == MDI_NEXT)
				{	cur_mdi++;
					update_mdi();
					wf_change(ptr_var,choix,0,1);
				}
				
				if(choix == MDI_IMP)
				{	mode_ie=0;
					cur_mdi=0;
					update_mdi();
					init_radio(ptr_var,MDI_IMP);
					wf_draw(&infi_var,MDI_IMP);
					wf_draw(&infi_var,MDI_EXP);
				}

				if(choix == MDI_EXP)
				{	mode_ie=1;
					cur_mdi=0;
					update_mdi();
					init_radio(ptr_var,MDI_EXP);
					wf_draw(&infi_var,MDI_EXP);
					wf_draw(&infi_var,MDI_IMP);
				}
				
				if(choix == MDI_PREV)
				{	cur_mdi--;
					update_mdi();
					wf_change(ptr_var,choix,0,1);
				}
			}
		}
	}
}

void update_mdi(void)
{	int nb_mod;
	char *id,*ver,*desc,*nfic,tempo[20];
	long size_buf;
	
	if(mode_ie==0)		/* import */
	{	nb_mod = nb_imp_mod;
		id = (char *)&imp_mod[cur_mdi].id;
		ver = (char *)&imp_mod[cur_mdi].ver;
		desc = imp_mod[cur_mdi].description;
		nfic = imp_mod[cur_mdi].nom_fic;
		size_buf = imp_mod[cur_mdi].size_buf_para;
	}else						/* export */
	{	nb_mod = nb_exp_mod;
		id = (char *)&exp_mod[cur_mdi].id;
		ver = (char *)&exp_mod[cur_mdi].ver;
		desc = exp_mod[cur_mdi].description;
		nfic = exp_mod[cur_mdi].nom_fic;
		size_buf = exp_mod[cur_mdi].size_buf_perm;		
	}
	
	itoa(cur_mdi, infi_var.adr_form[MDI_N_MOD].ob_spec.tedinfo->te_ptext, 10);
	wf_draw(&infi_var,MDI_N_MOD);
	
	sprintf(infi_var.adr_form[MDI_NB_MOD].ob_spec.tedinfo->te_ptext,"%d ",nb_mod);
	wf_draw(&infi_var,MDI_NB_MOD);
	
	if(nb_mod == 0)
	{	wf_change(&infi_var,MDI_PREV,DISABLED,1);
		wf_change(&infi_var,MDI_NEXT,DISABLED,1);
		
		strcpy(infi_var.adr_form[MDI_DESC].ob_spec.tedinfo->te_ptext,T_NO_LOADED_MOD);
		*infi_var.adr_form[MDI_FIC_NAME].ob_spec.tedinfo->te_ptext = 0;
		*infi_var.adr_form[MDI_ID].ob_spec.tedinfo->te_ptext = 0;
		*infi_var.adr_form[MDI_VER].ob_spec.tedinfo->te_ptext = 0;
		*infi_var.adr_form[MDI_BUFF].ob_spec.tedinfo->te_ptext = 0;
	}else
	{	if(cur_mdi == 0)
			wf_change(&infi_var,MDI_PREV,DISABLED,1);
		else
			wf_change(&infi_var,MDI_PREV,-1,1);

		if(cur_mdi == nb_mod-1)
			wf_change(&infi_var,MDI_NEXT,DISABLED,1);
		else
			wf_change(&infi_var,MDI_NEXT,-1,1);
	
		strpcpy_force(infi_var.adr_form[MDI_DESC].ob_spec.tedinfo->te_ptext,desc,31,' ');
		strpcpy_force(infi_var.adr_form[MDI_FIC_NAME].ob_spec.tedinfo->te_ptext,nfic,13,' ');

		strncpy(infi_var.adr_form[MDI_ID].ob_spec.tedinfo->te_ptext,id,4);
		strncpy(infi_var.adr_form[MDI_VER].ob_spec.tedinfo->te_ptext,ver,4);
		ltoa(size_buf,tempo,10);
		strpcpy_force(infi_var.adr_form[MDI_BUFF].ob_spec.tedinfo->te_ptext,tempo,7,' ');
	}
	
	wf_draw(&infi_var,MDI_FON);
}

void form_optc(int event)
{
	WINDFORM_VAR *ptr_var = &optc_var;
	int choix,bout;
	char nom[14],pth[150];
	CHEMIN fic;
	
	if (event == OPEN_DIAL)
	{	if(ptr_var->w_handle < 1)
		{	strcpy(tmp_chemin_anim_externe,chemin_anim_externe);
			strcpy(tmp_chemin_img_externe,chemin_img_externe);
			strcpy(tmp_chemin_key,chemin_key);
			strcpy(tmp_chemin_modules,chemin_modules);
			strcpy(tmp_chemin_mplayer,chemin_mplayer);
			strncpy(ptr_var->adr_form[OPT_KEY].ob_spec.tedinfo->te_ptext,chemin_key,40);
			strncpy(ptr_var->adr_form[OPT_VEXT].ob_spec.tedinfo->te_ptext,chemin_anim_externe,40);
			strncpy(ptr_var->adr_form[OPT_VIEXT].ob_spec.tedinfo->te_ptext,chemin_img_externe,40);
			strncpy(ptr_var->adr_form[OPT_IMP].ob_spec.tedinfo->te_ptext,chemin_modules,40);
			strncpy(ptr_var->adr_form[OPT_MPL].ob_spec.tedinfo->te_ptext,chemin_mplayer,40);
		}
		open_dialog(ptr_var, T_TITR_PATH_OPT, 0, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = OPT_ANN;
			wf_change(ptr_var,choix,0,1);
			switch(choix)
			{	
				case OPT_VEXT:
					find_name_ext(tmp_chemin_anim_externe,nom);
					strcpy(pth,tmp_chemin_anim_externe);
					find_path(pth);
					strcat(pth,"*.*");
					Fselecteur(&fic,pth,nom,&bout,T_ANIM_VIEWER);
					if(bout && nom[0] != 0)
					{	strcpy(tmp_chemin_anim_externe,fic.complet);
						strncpy(ptr_var->adr_form[OPT_VEXT].ob_spec.tedinfo->te_ptext,tmp_chemin_anim_externe,40);
						wf_draw(ptr_var,OPT_VEXT);
					}
					break;
				case OPT_VIEXT:
					find_name_ext(tmp_chemin_img_externe,nom);
					strcpy(pth,tmp_chemin_img_externe);
					find_path(pth);
					strcat(pth,"*.*");
					Fselecteur(&fic,pth,nom,&bout,T_PIC_VIEWER);
					if(bout && nom[0] != 0)
					{	strcpy(tmp_chemin_img_externe,fic.complet);
						strncpy(ptr_var->adr_form[OPT_VIEXT].ob_spec.tedinfo->te_ptext,tmp_chemin_img_externe,40);
						wf_draw(ptr_var,OPT_VIEXT);
					}
					break;
				case OPT_MPL:
					find_name_ext(tmp_chemin_mplayer,nom);
					strcpy(pth,tmp_chemin_mplayer);
					find_path(pth);
					strcat(pth,"M_PLAYER.PRG");
					Fselecteur(&fic,pth,nom,&bout,"Soft M-Player");
					if(bout && nom[0] != 0)
					{	strcpy(tmp_chemin_mplayer,fic.complet);
						strncpy(ptr_var->adr_form[OPT_MPL].ob_spec.tedinfo->te_ptext,tmp_chemin_mplayer,40);
						wf_draw(ptr_var,OPT_MPL);
					}
					break;
				case OPT_KEY:
					nom[0]=0;
					strcpy(pth,tmp_chemin_key);
					strcat(pth,"*.*");
					Fselecteur(&fic,pth,nom,&bout,T_KEY_PATH);
					if(bout)
					{	strcpy(tmp_chemin_key,fic.complet);
						strncpy(ptr_var->adr_form[OPT_KEY].ob_spec.tedinfo->te_ptext,tmp_chemin_key,40);
						wf_draw(ptr_var,OPT_KEY);
					}
					break;
				case OPT_IMP:
					nom[0]=0;
					strcpy(pth,tmp_chemin_modules);
					strcat(pth,"*.*");
					Fselecteur(&fic,pth,nom,&bout,T_IO_MOD_PATH);
					if(bout)
					{	strcpy(tmp_chemin_modules,fic.complet);
						strncpy(ptr_var->adr_form[OPT_IMP].ob_spec.tedinfo->te_ptext,tmp_chemin_modules,40);
						wf_draw(ptr_var,OPT_IMP);
					}
					break;
				case OPT_OK:
					strcpy(chemin_anim_externe,tmp_chemin_anim_externe);
					strcpy(chemin_img_externe,tmp_chemin_img_externe);
					strcpy(chemin_key,tmp_chemin_key);
					strcpy(chemin_modules,tmp_chemin_modules);
					strcpy(chemin_mplayer,tmp_chemin_mplayer);
				case OPT_ANN:
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_carte(int event)
{
	WINDFORM_VAR *ptr_var = &carte_var;
	int choix,dum,dum2;
	
	if (event == OPEN_DIAL)
	{	if(ptr_var->w_handle < 1)
		{	
			init_radio(ptr_var,CARTE_FAL+carte);
			ptr_var->adr_form[CARTE_FORCE_VDI].ob_state = force_vdi_use;
		}
		open_dialog(ptr_var, T_TITR_CARTE, 0, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = CARTE_ANN;
			switch(choix)
			{	
				case CARTE_OK:
					dum = get_rbutton(ptr_var->adr_form,CARTE_FAL) - CARTE_FAL;
					dum2 = ptr_var->adr_form[CARTE_FORCE_VDI].ob_state & SELECTED;
					if(dum != carte || dum2 != force_vdi_use)
					{	carte = dum;
						force_vdi_use = dum2;
						form_dial(FMD_FINISH,0,0,0,0,x_desk,y_desk,w_desk,h_desk);
					}	
				case CARTE_ANN:
					close_dialog(ptr_var);
					wf_change(ptr_var,choix,0,1);
					break;
			}
		}
	}
}

void form_optv(int event)
{
	WINDFORM_VAR *ptr_var = &optv_var;
	int choix,dum;
	
	if (event == OPEN_DIAL)
	{	if(ptr_var->w_handle < 1)
		{	init_radio(ptr_var,OV_HG+visu_pos);
			wf_change(ptr_var,OPTV_FAST,visu_fast,0);
			wf_change(ptr_var,OPTV_INFO,visu_info,0);
			wf_change(ptr_var,OPTV_BOUCLE,visu_boucle,0);
		}
		open_dialog(ptr_var, T_TITR_VIEW_OPT, 0, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = OPTV_ANN;
			wf_change(ptr_var,choix,0,1);
			switch(choix)
			{		
				case OPTV_OK:
					visu_pos = get_rbutton(ptr_var->adr_form,OV_HG) - OV_HG;
					dum = ptr_var->adr_form[OPTV_INFO].ob_state & SELECTED;
					if(dum != visu_info)
					{	visu_info = dum;
						close_visu();
						init_wvisu();
					}
					visu_fast = ptr_var->adr_form[OPTV_FAST].ob_state & SELECTED;
					visu_boucle = ptr_var->adr_form[OPTV_BOUCLE].ob_state & SELECTED;
					if(visu_fast & cur_visu)
						mod_wind_visu(1);
				case OPTV_ANN:
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_optg(int event)
{
	WINDFORM_VAR *ptr_var = &optg_var;
	int choix,dum,dum2,dum3;
	
	if (event == OPEN_DIAL)
	{	tmp_n_bout = n_bout;
		tmp_n_txt = n_txt;
		old_id_bout = id_boutton;
		old_id_txt = id_texte;
		wf_change(ptr_var,OPTG_NVDI,use_nvdi,0);
		wf_change(ptr_var,OPTG_REL,int_rel,0);
		wf_change(ptr_var,OPTG_ALRT,use_n_alert,0);
		wf_change(ptr_var,OPTG_ROUND,draw_round,0);
		wf_change(ptr_var,OPTG_FOBJ,op_fobj,0);
		wf_change(ptr_var,OPTG_FPRJ,op_fprj,0);
		wf_change(ptr_var,OPTG_FPIST,op_fpist,0);
		wf_change(ptr_var,OPTG_FTRAN,op_ftran,0);
		wf_change(ptr_var,OPTG_C_LDG,ldg_install,0);
		wf_change(ptr_var,OPTG_LDG8,ldg_inst_if_sup_8,0);
		itoa(taille_def,ptr_var->adr_form[OPTG_TAILLE].ob_spec.tedinfo->te_ptext,10);
		if(nb_fonts)
		{	strcpy(optg_var.adr_form[OPTG_POP_B].ob_spec.free_string,nom_font[n_bout].name);
			strcpy(optg_var.adr_form[OPTG_POP_T].ob_spec.free_string,nom_font[n_txt].name);
		}
		open_dialog(ptr_var, T_TITR_INTERF_OPT, OPTG_TAILLE, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = OPTG_ANN;
			wf_change(ptr_var,choix,0,1);
			switch(choix)
			{		
				case OPTG_POP_B:
					if(nb_fonts)
					{	tmp_n_bout = super_popup(ptr_var, OPTG_POP_B, CHOIX, tmp_n_bout+1, (char *)nom_font, 36,nb_fonts) - 1;
						id_boutton = nom_font[tmp_n_bout].id;
						wf_draw(ptr_var,OPTG_OK);
						wf_draw(ptr_var,OPTG_ANN);
					}
					break;
			
				case OPTG_POP_T:
					if(nb_fonts)
					{	tmp_n_txt = super_popup(ptr_var, OPTG_POP_T, CHOIX, tmp_n_txt+1, (char *)nom_font, 36,nb_fonts) - 1;
						id_texte = nom_font[tmp_n_txt].id;
					}
					break;

				case OPTG_OK:
					op_fobj = ptr_var->adr_form[OPTG_FOBJ].ob_state & SELECTED;
					op_fprj = ptr_var->adr_form[OPTG_FPRJ].ob_state & SELECTED;
					op_fpist = ptr_var->adr_form[OPTG_FPIST].ob_state & SELECTED;
					op_ftran = ptr_var->adr_form[OPTG_FTRAN].ob_state & SELECTED;
					ldg_install = ptr_var->adr_form[OPTG_C_LDG].ob_state & SELECTED;
					ldg_inst_if_sup_8 = ptr_var->adr_form[OPTG_LDG8].ob_state & SELECTED;
					
					dum = ptr_var->adr_form[OPTG_NVDI].ob_state & SELECTED;
					dum2 = ptr_var->adr_form[OPTG_REL].ob_state & SELECTED;
					dum3 = ptr_var->adr_form[OPTG_ROUND].ob_state & SELECTED;
					if(use_nvdi != dum || int_rel != dum2 || draw_round != dum3)
					{	use_nvdi = dum;
						int_rel = dum2;
						draw_round = dum3;
						form_dial(FMD_FINISH,0,0,0,0,x_desk,y_desk,w_desk,h_desk);
					}else
					{	use_nvdi = dum;
						int_rel = dum2;
						draw_round = dum3;
					}
					n_txt = tmp_n_txt;
					n_bout = tmp_n_bout;
					use_n_alert = ptr_var->adr_form[OPTG_ALRT].ob_state & SELECTED;
					sscanf(optg_var.adr_form[OPTG_TAILLE].ob_spec.tedinfo->te_ptext,"%d",&taille_def);
					close_dialog(ptr_var);
					break;
				case OPTG_ANN:
					id_boutton = old_id_bout;
					id_texte = old_id_txt;
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_optp(int event)
{
	WINDFORM_VAR *ptr_var = &optp_var;
	int choix,dum,dum2,dum3;
	
	if (event == OPEN_DIAL)
	{	
		{	if(ptr_var->w_handle < 1)
			wf_change(ptr_var,OPTP_A1,0,0);
			wf_change(ptr_var,OPTP_A2,0,0);
			wf_change(ptr_var,OPTP_A3,0,0);
			if(aff_mode == 0)
				wf_change(ptr_var,OPTP_A1,1,0);
			else if(aff_mode == 1)
				wf_change(ptr_var,OPTP_A2,1,0);
			else
				wf_change(ptr_var,OPTP_A3,1,0);
			itoa(aff_size_buffer, ptr_var->adr_form[OPTP_BUF].ob_spec.tedinfo->te_ptext, 10);
			wf_change(ptr_var,OPTP_PROP,aff_prop,0);
			wf_change(ptr_var,OPTP_AFF_INT,draw_intensitee,0);
		}
		
		open_dialog(ptr_var, T_TITR_PIST_OPT, OPTP_BUF, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = OPTP_ANN;
			wf_change(ptr_var,choix,0,1);
			switch(choix)
			{		
				case OPTP_OK:
					if(ptr_var->adr_form[OPTP_A1].ob_state & SELECTED)
						dum = 0;
					else if(ptr_var->adr_form[OPTP_A2].ob_state & SELECTED)
						dum = 1;
					else if(ptr_var->adr_form[OPTP_A3].ob_state & SELECTED)
						dum = 2;
					
					dum2 = ptr_var->adr_form[OPTP_PROP].ob_state & SELECTED;
					dum3 = ptr_var->adr_form[OPTP_AFF_INT].ob_state & SELECTED;
					
					if(dum != aff_mode || dum2 != aff_prop || dum3 != draw_intensitee)
					{	aff_mode = dum;
						aff_prop = dum2;
						draw_intensitee = dum3;
						aff_nombre = 0;
						redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
					}else
					{	aff_mode = dum;
						aff_prop = dum2;
						draw_intensitee = dum3;
					}
					aff_size_buffer = atoi(ptr_var->adr_form[OPTP_BUF].ob_spec.tedinfo->te_ptext);
				case OPTP_ANN:
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_opto(int event)
{
	WINDFORM_VAR *ptr_var = &opto_var;
	int choix,num;
	char str[5];
	
	if (event == OPEN_DIAL)
	{	
		sprintf(ptr_var->adr_form[OPTO_NOM].ob_spec.tedinfo->te_ptext,T_OBJET_S,objet[info_objet].nom);
		ptr_var->adr_form[OPTO_KEY].ob_spec.tedinfo->te_ptext[0] = 0;
		ptr_var->adr_form[OPTO_FKEY].ob_flags = HIDETREE;
		switch(objet[info_objet].type)
		{	case OBJ_FLH :
					strcpy(ptr_var->adr_form[OPTO_TYPE].ob_spec.tedinfo->te_ptext,T_FLH_ANIM);
					strcpy(ptr_var->adr_form[OPTO_KEY].ob_spec.tedinfo->te_ptext,objet[info_objet].chemin_key);
					sprintf(ptr_var->adr_form[OPTO_DESC].ob_spec.tedinfo->te_ptext,T_D_D_D_IMG, objet[info_objet].x,objet[info_objet].y,objet[info_objet].nb_img);
					ptr_var->adr_form[OPTO_FKEY].ob_flags = FLAGS10;
				break;
			case OBJ_FLI :
					strcpy(ptr_var->adr_form[OPTO_TYPE].ob_spec.tedinfo->te_ptext,T_FLI_ANIM);
					strcpy(ptr_var->adr_form[OPTO_KEY].ob_spec.tedinfo->te_ptext,objet[info_objet].chemin_key);
					sprintf(ptr_var->adr_form[OPTO_DESC].ob_spec.tedinfo->te_ptext,T_D_D_D_IMG, objet[info_objet].x,objet[info_objet].y,objet[info_objet].nb_img);
					ptr_var->adr_form[OPTO_FKEY].ob_flags = FLAGS10;
				break;
			case OBJ_FLC :
					strcpy(ptr_var->adr_form[OPTO_TYPE].ob_spec.tedinfo->te_ptext,T_FLC_ANIM);
					strcpy(ptr_var->adr_form[OPTO_KEY].ob_spec.tedinfo->te_ptext,objet[info_objet].chemin_key);
					sprintf(ptr_var->adr_form[OPTO_DESC].ob_spec.tedinfo->te_ptext,T_D_D_D_IMG, objet[info_objet].x,objet[info_objet].y,objet[info_objet].nb_img);
					ptr_var->adr_form[OPTO_FKEY].ob_flags = FLAGS10;
				break;
			case OBJ_MOD :
					strcpy(ptr_var->adr_form[OPTO_TYPE].ob_spec.tedinfo->te_ptext,objet[info_objet].desc);
					sprintf(ptr_var->adr_form[OPTO_DESC].ob_spec.tedinfo->te_ptext,T_D_D_D_IMG, objet[info_objet].x,objet[info_objet].y,objet[info_objet].nb_img);
				break;
			case OBJ_SUIT :
					strcpy(ptr_var->adr_form[OPTO_TYPE].ob_spec.tedinfo->te_ptext,T_SUIT_DIMG);
					sprintf(ptr_var->adr_form[OPTO_DESC].ob_spec.tedinfo->te_ptext,T_D_D_D_IMG, objet[info_objet].x,objet[info_objet].y,objet[info_objet].nb_img);
				break;
		}
		strcpy(ptr_var->adr_form[OPTO_CHM].ob_spec.tedinfo->te_ptext,objet[info_objet].chemin);

		sprintf(str,"%d",objet[info_objet].def_in);
		set_editable(ptr_var,OPTO_IN,str,0);
		sprintf(str,"%d",objet[info_objet].def_out);
		set_editable(ptr_var,OPTO_OUT,str,0);

		sprintf(str,"%d",objet[info_objet].img_preview);
		set_editable(ptr_var,OPTO_IPRE,str,0);
		
		open_dialog(ptr_var, T_TITR_INFO_OBJ, OPTO_IN, -1);	
	}
		
	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{
			if(choix == CLOSE_DIAL)
				choix = OPTO_ANN;
			switch(choix)
			{
				case OPTO_ANN :
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
				case OPTO_OK :
					num=atoi(ptr_var->adr_form[OPTO_IPRE].ob_spec.tedinfo->te_ptext);
					if(num != objet[info_objet].img_preview && num<objet[info_objet].nb_img)
					{	objet[info_objet].img_preview = num;
						if(make_preview(info_objet) == 0)
							efface_objet(info_objet);
						redraw_objet(hnd_wobjet, xfo, yfo, wfo, hfo);
					}
					num=atoi(ptr_var->adr_form[OPTO_IN].ob_spec.tedinfo->te_ptext);
					num = min(num,objet[info_objet].nb_img -1);
					num = max(num,0);
					objet[info_objet].def_in = num;

					num=atoi(ptr_var->adr_form[OPTO_OUT].ob_spec.tedinfo->te_ptext);
					num = min(num,objet[info_objet].nb_img -1);
					num = max(num,objet[info_objet].def_in);
					objet[info_objet].def_out = num;
					
					redraw_1objet(info_objet);

					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_ifon(int event)
{
	WINDFORM_VAR *ptr_var = &ifon_var;
	int choix,i;
	char txt_field[MAX_OBJET][25];
	
	if (event == OPEN_DIAL)
	{	if(projet.mode_fond == 0)	/* couleur */
			init_radio(ptr_var,FON_COUL);
		else
			init_radio(ptr_var,FON_OBJ);

		if(projet.mode_obj == 0)	/* motif */
			init_radio(ptr_var,FON_MOT);
		else
			init_radio(ptr_var,FON_ZOM);

		sprintf(ptr_var->adr_form[FON_SET_COUL].ob_spec.free_string,
					T_RVB,projet.r,projet.v,projet.b);
		temp_r = projet.r;
		temp_v = projet.v;
		temp_b = projet.b;
		
		ptr_var->adr_form[FON_NOM_OBJ].ob_spec.free_string[0]=0;
		if(projet.obj_fond != -1)
			strcpy(ptr_var->adr_form[FON_NOM_OBJ].ob_spec.free_string,objet[projet.obj_fond].nom);

		temp_obj_fon=projet.obj_fond;
		
		open_dialog(ptr_var, T_TITR_FON_PRJ, 0, -1);	
	}
		
	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{
			switch(choix)
			{
				case FON_SET_COUL :
					coul_caller=0;
					form_col(OPEN_DIAL);
					wf_change(ptr_var,choix,0,1);
					break;

				case FON_NOM_OBJ :
					if(nb_objets)
					{	for(i=0;i<nb_objets;i++)
							strcpy(txt_field[i],objet[i].nom);
						temp_obj_fon = super_popup(ptr_var, FON_NOM_OBJ, CHOIX,temp_obj_fon+1,(char*)txt_field,25,nb_objets) - 1;
					}
					wf_change(ptr_var,choix,0,1);
					break;
				
				case CLOSE_DIAL :
				case FON_ANN :
					wf_change(ptr_var,FON_ANN,0,1);
					close_dialog(ptr_var);
				case FON_OK :
					if(ptr_var->adr_form[FON_COUL].ob_state & SELECTED || nb_objets == 0 || temp_obj_fon == -1)
						projet.mode_fond = 0;		/* couleur */
					else projet.mode_fond = 1;		/* objet */
					
					if(ptr_var->adr_form[FON_MOT].ob_state & SELECTED)
						projet.mode_obj = 0;			/* motif */
					else projet.mode_obj = 1;		/* zoom‚ */
					
					projet.r = (unsigned char)temp_r;
					projet.v = (unsigned char)temp_v;
					projet.b = (unsigned char)temp_b;
					
					projet.obj_fond = temp_obj_fon;
					
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

void form_col(int event)
{
	WINDFORM_VAR *ptr_var = &col_var;
	int choix, rgb[3];
		
	if (event == OPEN_DIAL)
	{	set_ass(COUL_ASSR,temp_r);
		set_ass(COUL_ASSV,temp_v);
		set_ass(COUL_ASSB,temp_b);
		
		rgb[0] = temp_r;
		rgb[1] = temp_v;
		rgb[2] = temp_b;
		vs_color255(rgb);
		open_dialog(ptr_var, T_TITR_COLOR, COUL_TR,-1);
	}
	
	else 
	{	choix = windial_do(ptr_var, event);
		if(choix == CLOSE_DIAL)
			choix = COUL_OK;
		if(choix > 0)
		{	wf_change(ptr_var,choix,0,1);
			if(choix == COUL_OK)
			{	scan_coul(rgb);
				temp_r = min(rgb[0],255);
				temp_v = min(rgb[1],255);
				temp_b = min(rgb[2],255);
				close_dialog(ptr_var);
				vs_color(vdi_handle,15,prev_rgb);
				if(coul_caller == 0)  	/* info fond */
				{	sprintf(ifon_var.adr_form[FON_SET_COUL].ob_spec.free_string,
									T_RVB,temp_r,temp_v,temp_b);
					wf_draw(&ifon_var,FON_SET_COUL);
				}else if(coul_caller == 1 || coul_caller==2)  	/* Transit */
					tran_set_colors(coul_caller);
				else if(coul_caller == 3 || coul_caller==4)  	/* Effets */
					eff_set_colors(coul_caller-2);
			}
			
			if(choix == COUL_UPR)
			{	scan_coul(rgb);
				if(rgb[0] < 255)
				{	rgb[0]++;
					set_ass(COUL_ASSR,rgb[0]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_DWR)
			{	scan_coul(rgb);
				if(rgb[0])
				{	rgb[0]--;
					set_ass(COUL_ASSR,rgb[0]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_UPV)
			{	scan_coul(rgb);
				if(rgb[1] < 255)
				{	rgb[1]++;
					set_ass(COUL_ASSV,rgb[1]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_DWV)
			{	scan_coul(rgb);
				if(rgb[1])
				{	rgb[1]--;
					set_ass(COUL_ASSV,rgb[1]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_UPB)
			{	scan_coul(rgb);
				if(rgb[2] < 255)
				{	rgb[2]++;
					set_ass(COUL_ASSB,rgb[2]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_DWB)
			{	scan_coul(rgb);
				if(rgb[2])
				{	rgb[2]--;
					set_ass(COUL_ASSB,rgb[2]);
					vs_color255(rgb);
					if(nb_planes>8)	/* mode tc */
						wf_draw(ptr_var,COUL_TST);
				}
			}
			
			if(choix == COUL_ASSR)
				ass_c(COUL_ASSR,COUL_FONR);
			if(choix == COUL_ASSV)
				ass_c(COUL_ASSV,COUL_FONV);
			if(choix == COUL_ASSB)
				ass_c(COUL_ASSB,COUL_FONB);
			
			if(choix == COUL_TSTB)
			{	scan_coul(rgb);
				set_ass(COUL_ASSR,rgb[0]);
				set_ass(COUL_ASSV,rgb[1]);
				set_ass(COUL_ASSB,rgb[2]);
				vs_color255(rgb);
				if(nb_planes>8)	/* mode tc */
					wf_draw(ptr_var,COUL_TST);
			}

			if(choix == COUL_ANN || choix == CLOSE_DIAL)
			{	vs_color(vdi_handle,15,prev_rgb);
				close_dialog(ptr_var);
			}
		}	
	}
}

void vs_color255(int rgb[3])
{	int rgb2[3];
	
	color[0] = rgb[0];
	color[1] = rgb[1];
	color[2] = rgb[2];
	
	rgb2[0] = (int)(((long)rgb[0] * 1000L)/255L);
	rgb2[1] = (int)(((long)rgb[1] * 1000L)/255L);
	rgb2[2] = (int)(((long)rgb[2] * 1000L)/255L);
	vs_color(vdi_handle,15,rgb2);
}

void scan_coul(int rgb[3])
{	sscanf(col_var.adr_form[COUL_TR].ob_spec.tedinfo->te_ptext,"%d",&rgb[0]);
	rgb[0] = min(rgb[0],255);
	sscanf(col_var.adr_form[COUL_TV].ob_spec.tedinfo->te_ptext,"%d",&rgb[1]);
	rgb[1] = min(rgb[1],255);
	sscanf(col_var.adr_form[COUL_TB].ob_spec.tedinfo->te_ptext,"%d",&rgb[2]);
	rgb[2] = min(rgb[2],255);
}

void ass_c(int oass, int fon)
{	int mousey,bouton,dummy,old,ay,nb,index,rgb[3];
	
	ay = col_var.adr_form[0].ob_y + col_var.adr_form[fon].ob_y;
	
	graf_mouse (FLAT_HAND,0);
	graf_mkstate(&mousex, &mousey, &bouton,&dummy);
	old = mousey - ay - 6;
	do  
	{	/* prise des coordon‚es de la souris : */
		graf_mkstate(&dummy, &mousey, &bouton,&dummy); 
		nb = mousey - ay - 6;

		if (nb <  0 )
			nb = 0;
		if (nb > ASS_C  )
			nb = ASS_C;
			
		nb = ASS_C - nb;		/* paske ds les ass de couleur l'origine est en bas */
		
		index = (int)( ( (long)nb * 255L) / ASS_C);
		
		if (old != nb ) 
		{	old = nb;
			set_ass(oass,index);
			scan_coul(rgb);
			vs_color255(rgb);
			if(nb_planes>8)	/* mode tc */
				wf_draw(&col_var,COUL_TST);
		}
	} while (bouton != 0);
	graf_mouse (ARROW, 0);
}

void set_ass(int obj, int index)
{	int haut;
	
	itoa(index,col_var.adr_form[obj].ob_spec.tedinfo->te_ptext,10);

	if(obj == COUL_ASSR)
	{	itoa(index,col_var.adr_form[COUL_TR].ob_spec.tedinfo->te_ptext,10);
		wf_draw(&col_var,COUL_TR);
	}
	if(obj == COUL_ASSV)
	{	itoa(index,col_var.adr_form[COUL_TV].ob_spec.tedinfo->te_ptext,10);
		wf_draw(&col_var,COUL_TV);
	}
	if(obj == COUL_ASSB)
	{	itoa(index,col_var.adr_form[COUL_TB].ob_spec.tedinfo->te_ptext,10);
		wf_draw(&col_var,COUL_TB);
	}
	haut = (int)( ( (long)index * ASS_C) / 255L);
	col_var.adr_form[obj].ob_y = ASS_C - haut;

	wf_draw(&col_var,obj - 1);		/* dessine le fond */
}

void form_new(int event)
{
	WINDFORM_VAR *ptr_var = &new_var;
	int choix,modify;
	long dum;
	
	if (event == OPEN_DIAL)
	{	if(make_new_prj)
		{	actu_projet = FALSE;
			strcpy(new_var.adr_form[NEW_TITRE].ob_spec.tedinfo->te_ptext,T_CREAT_NEW_PRJ);
			new_var.adr_form[NEW_RED].ob_flags = HIDETREE;
			set_popup(new_var.adr_form, NEW_POP_PRE, 3, POP_SIZ_PRE);
			open_dialog(ptr_var, T_NEW_PRJ, NEW_NAME, -1);	
		}else
		{	strcpy(new_var.adr_form[NEW_TITRE].ob_spec.tedinfo->te_ptext,T_INFO_ON_PRJ);
			new_var.adr_form[NEW_RED].ob_flags = SELECTABLE|EXIT|FLAGS9|FLAGS10;
			itoa(projet.x, new_var.adr_form[NEW_X].ob_spec.tedinfo->te_ptext, 10);
			itoa(projet.y, new_var.adr_form[NEW_Y].ob_spec.tedinfo->te_ptext, 10);
			ltoa(projet.lenght, new_var.adr_form[NEW_LONG].ob_spec.tedinfo->te_ptext, 10);
			if(projet.speed<10)
				sprintf(new_var.adr_form[NEW_SPEED].ob_spec.free_string,"0%d",projet.speed);
			else
				itoa(projet.speed, new_var.adr_form[NEW_SPEED].ob_spec.free_string, 10);
			set_popup(new_var.adr_form, NEW_POP_PRE, projet.taille_prev+1, POP_SIZ_PRE);
			open_dialog(ptr_var, T_LE_PRJ, NEW_NAME, -1);	
		}
		
	}
		
	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{	if(choix == CLOSE_DIAL)
				choix = NEW_ANN;

			switch(choix)
			{
				case NEW_POP_PRE :
					pop_up(ptr_var,NEW_POP_PRE,POP_SIZ_PRE);
					break;
				case NEW_SPEED :
					pop_up(ptr_var,NEW_SPEED,POP_SPEED);
					break;
				case NEW_RED :
					dum = real_lenght();
					ltoa(dum, new_var.adr_form[NEW_LONG].ob_spec.tedinfo->te_ptext, 10);
					wf_draw(ptr_var,NEW_LONG);
					wf_change(ptr_var,choix,0,1);
					break;
				case NEW_ANN :
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
				case NEW_OK :
					modify = FALSE;
					if(projet.x != atoi(new_var.adr_form[NEW_X].ob_spec.tedinfo->te_ptext))
					{	projet.x = atoi(new_var.adr_form[NEW_X].ob_spec.tedinfo->te_ptext);	
						modify=TRUE;	}
					if(projet.y != atoi(new_var.adr_form[NEW_Y].ob_spec.tedinfo->te_ptext))
					{	projet.y = atoi(new_var.adr_form[NEW_Y].ob_spec.tedinfo->te_ptext);
						modify=TRUE;	}
					if(projet.lenght != atoi(new_var.adr_form[NEW_LONG].ob_spec.tedinfo->te_ptext))
					{	projet.lenght = max(atoi(new_var.adr_form[NEW_LONG].ob_spec.tedinfo->te_ptext),1);
						deb_img=(int)min(deb_img,projet.lenght-1);
						slider_ph();
						projet.preview_out = min(projet.preview_out,(int)projet.lenght-1);
						redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
					}
					projet.speed = atoi(new_var.adr_form[NEW_SPEED].ob_spec.free_string);
					if(projet.taille_prev != get_popup(new_var.adr_form,NEW_POP_PRE,POP_SIZ_PRE)-1)
					{	projet.taille_prev = get_popup(new_var.adr_form,NEW_POP_PRE,POP_SIZ_PRE)-1;
						modify=TRUE;	}
					if(modify && !make_new_prj)
						close_prev();
					if(projet.x == 0 || projet.y == 0 || projet.lenght == 0 || new_var.adr_form[NEW_NAME].ob_spec.tedinfo->te_ptext[0]==0)
					{	super_alert(1,3,T_NO_NULL_VAL);
						wf_change(ptr_var,choix,0,1);
						goto end_new;
					}
					strcpy(projet.name,new_var.adr_form[NEW_NAME].ob_spec.tedinfo->te_ptext);
					
					if(make_new_prj)
					{	/* plein de valeurs par defaut */
						chemin_projet[0]=0;
						projet.zoom = 5;
						projet.nb_obj = 0;
						projet.preview_in = 0;
						projet.preview_out = (int)projet.lenght-1;
						projet.mode_fond = 0;	/* couleur noire */
						projet.r = projet.v = projet.b = 0;
						projet.obj_fond = -1;
						projet.mode_obj = 1;
						projet.nb_sup_A = projet.nb_sup_B = projet.nb_sup_G = 0;
						projet.id_exp_mod = 0;
						projet.size_param = 0;
						projet.adr_param = 0;
						projet.chemin[0] = 0;
						actu_projet = TRUE;

						set_pop_zoom(projet.zoom);
						/* OUVERTURE DES FENETRE */
						if(op_fobj)
						{	if(!w_obj_open)
							{	wind_open(hnd_wobjet,xfo,yfo,wfo,hfo);
								w_obj_open = TRUE;
								wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
								slider_o();
							}
						}
						if(op_fpist)
						{	if(!w_trk_open)
							{	wind_open(hnd_wpistes,xfp,yfp,wfp,hfp);
								wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
								slider_ph();
								slider_pv();
								w_trk_open=TRUE;
							}
						}
						if(op_fprj)
						{	if(!w_prj_open)
							{	wind_open(hnd_wprojet,xfr,yfr,wfr,hfr);
								wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
								slider_r();
								w_prj_open=TRUE;
							}
						}
						if(op_ftran)
						{	if(!w_trsit_open)
							{	wind_open(hnd_wtransit,xft,yft,wft,hft);
								wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
								w_trsit_open= TRUE;
								slider_t();
							}
						}
					}
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
	end_new:;
}

void imp_module(void)
{	int bout;
	char nom[14]="",pth[150]="";
	CHEMIN fic;
	
	Fselecteur(&fic,pth,nom,&bout,T_FILE_TO_IMP);
	
	find_name(nom,pth); /* pth contiend le nom sans ext */
	
	if(bout)
	{	new_objet(OBJ_MOD, pth, fic.complet,"");
		Dsetdrv(fic.chemin[0] - 'A');
		Dsetpath(fic.chemin);
	}
		
}

void form_iflh(int event)
{
	WINDFORM_VAR *ptr_var = &iflh_var;
	int choix,bout,freq;
	int xb,yb,wb,hb,retour;
	char nom[14],pth[150];
	static int x,y,nb_img;
	
	if (event != OPEN_DIAL)
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{	if(choix == CLOSE_DIAL)
				choix = IFLH_ANN;
			switch(choix)
			{	
				case IFLH_CHM :
					import_newflh = 1;
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_KEY :
					if(iflh_flhok)
					{	find_name(flh.complet,nom);
						strcat(nom,".KEY");
						strcpy(pth,"\\*.KEY");
						Fselecteur(&ckey,pth,nom,&bout,T_KEYFRAME_PATH);
						if(bout && ckey.chemin[0] != 0 )
						{	strcpy(nom_key,ckey.chemin);
							find_name(flh.complet,nom);
							strcat(nom_key,nom);
							strcat(nom_key,".KEY");
							strncpy(ptr_var->adr_form[IFLH_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
							wf_draw(ptr_var,IFLH_KEY);
							if(check_key(nom_key,nom,x,y,nb_img))
							{	new_objet(OBJ_FLH,nom,flh.complet,nom_key);
								wf_change(ptr_var,choix,0,0);
								close_dialog(ptr_var);
							}
						}
					}
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_FREQ :
					pop_up(ptr_var,IFLH_FREQ,POP_KEY);
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_OK :
					if(iflh_flhok)
					{	trim_cpy(nom,ptr_var->adr_form[IFLH_FREQ].ob_spec.free_string);
						freq = atoi(nom);
						
						form_center (adr_key , &xb ,&yb, &wb ,&hb);					/* centre formulaire fin */
						form_dial (FMD_START, 0, 0, 0, 0, xb, yb, wb, hb);		/* prepare l'affichage */
						strncpy(adr_key[KEY_ANI_IMP].ob_spec.tedinfo->te_ptext,flh.complet,SIZE_CHM);
						strncpy(adr_key[KEY_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
						strcpy(adr_key[KEY_DESC].ob_spec.tedinfo->te_ptext,ptr_var->adr_form[IFLH_DESC].ob_spec.tedinfo->te_ptext);
						strcpy(adr_key[KEY_IMG].ob_spec.tedinfo->te_ptext,T_CREAT_TABLE_OFFS);
						itoa(freq,adr_key[KEY_FREQ].ob_spec.tedinfo->te_ptext,10);
						
						objc_draw (adr_key, 0, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
						
						retour = do_key_frame(flh.complet, nom_key, freq, adr_key);
						form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */
						find_name(flh.complet,nom);
						if(retour)
							new_objet(OBJ_FLH,nom,flh.complet,nom_key);
					}
					wf_change(ptr_var,choix,0,1);
				case IFLH_ANN :	/* Case de fermeture	*/
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
	
	if(import_newflh)
	{	import_newflh = FALSE;
		nom[0] = 0;
		strcpy(pth,"\\*.FLH");
		Fselecteur(&flh,pth,nom,&bout,T_FLH_FILE);
		iflh_flhok = FALSE;
		if(bout && nom[0] != 0)
		{	if(get_flh_info(flh.complet,ptr_var->adr_form[IFLH_DESC].ob_spec.tedinfo->te_ptext,&x,&y,&nb_img))
			{	strncpy(ptr_var->adr_form[IFLH_CHM].ob_spec.tedinfo->te_ptext,flh.complet,SIZE_CHM);
				Dsetdrv(flh.chemin[0] - 'A');
				Dsetpath(flh.chemin);
				iflh_flhok = TRUE;
								
				find_name(flh.complet,nom);
				
				/* ESSAYE DE TROUVE UN EVENTUEL FICHIER KEY DS LE M CHMIN QUE LE FLH */
				strcpy(nom_key,flh.chemin);
				strcat(nom_key,nom);
				strcat(nom_key,".KEY");
				if(check_key(nom_key,nom,x,y,nb_img))
				{	new_objet(OBJ_FLH,nom,flh.complet,nom_key);
					close_dialog(ptr_var);
					goto end_iflh;
				}
				
				/* ESSAYE DS LE REPERTOIRE DES KEY */
				strcpy(nom_key,chemin_key);
				strcat(nom_key,nom);
				strcat(nom_key,".KEY");
				if(check_key(nom_key,nom,x,y,nb_img))
				{	new_objet(OBJ_FLH,nom,flh.complet,nom_key);
					close_dialog(ptr_var);
					goto end_iflh;
				}
				strncpy(ptr_var->adr_form[IFLH_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
				
				if(ptr_var->w_handle)
				{	wf_change(ptr_var,IFLH_CHM,-1,1);
					wf_draw(ptr_var,IFLH_DESC);
					wf_draw(ptr_var,IFLH_KEY);
				}

				if(event == OPEN_DIAL)
					open_dialog(ptr_var, T_TITR_FLH_IMP, 0, -1);
					
			}
		}
	}
	end_iflh:;
}

void form_ifli(int event)
{
	WINDFORM_VAR *ptr_var = &iflh_var;
	int choix,bout,freq;
	int xb,yb,wb,hb,type_obj,retour;
	char nom[14],pth[150];
	static int x,y,nb_img;
	
	if (event != OPEN_DIAL)
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{	if(choix == CLOSE_DIAL)
				choix = IFLH_ANN;
			switch(choix)
			{
				case IFLH_CHM :
					import_newflh = 1;
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_KEY :
					if(iflh_flhok)
					{	find_name(flh.complet,nom);
						strcat(nom,".KEY");
						strcpy(pth,"\\*.KEY");
						Fselecteur(&ckey,pth,nom,&bout,T_KEYFRAME_PATH);
						if(bout && ckey.chemin[0] != 0 )
						{	strcpy(nom_key,ckey.chemin);
							find_name(flh.complet,nom);
							strcat(nom_key,nom);
							strcat(nom_key,".KEY");
							strncpy(ptr_var->adr_form[IFLH_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
							wf_draw(ptr_var,IFLH_KEY);
							if(check_key(nom_key,nom,x,y,nb_img))
							{	new_objet(OBJ_FLH,nom,flh.complet,nom_key);
								wf_change(ptr_var,choix,0,0);
								close_dialog(ptr_var);
							}
						}
					}
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_FREQ :
					pop_up(ptr_var,IFLH_FREQ,POP_KEY);
					wf_change(ptr_var,choix,0,1);
					break;
				case IFLH_OK :
					if(iflh_flhok)
					{	trim_cpy(nom,ptr_var->adr_form[IFLH_FREQ].ob_spec.free_string);
						freq = atoi(nom);
						
						form_center (adr_key , &xb ,&yb, &wb ,&hb);					/* centre formulaire fin */
						form_dial (FMD_START, 0, 0, 0, 0, xb, yb, wb, hb);		/* prepare l'affichage */
						strncpy(adr_key[KEY_ANI_IMP].ob_spec.tedinfo->te_ptext,flh.complet,SIZE_CHM);
						strncpy(adr_key[KEY_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
						strcpy(adr_key[KEY_DESC].ob_spec.tedinfo->te_ptext,ptr_var->adr_form[IFLH_DESC].ob_spec.tedinfo->te_ptext);
						strcpy(adr_key[KEY_IMG].ob_spec.tedinfo->te_ptext,T_CREAT_FLI_TABLE_OFFS);
						itoa(freq,adr_key[KEY_FREQ].ob_spec.tedinfo->te_ptext,10);
						
						objc_draw (adr_key, 0, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
						
						retour = do_key_i_frame(flh.complet, nom_key, freq, adr_key, prem_frame, pos_prem_frame);
						form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */
						find_name(flh.complet,nom);
						if(fli_flc)
							type_obj = OBJ_FLI;
						else 	type_obj = OBJ_FLC;
						if(retour)
							new_objet(type_obj,nom,flh.complet,nom_key);
					}
					wf_change(ptr_var,choix,0,1);
				case IFLH_ANN :	/* Case de fermeture	*/
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
	
	if(import_newflh)
	{	import_newflh = FALSE;
		nom[0] = 0;
		strcpy(pth,"\\*.FL?");
		Fselecteur(&flh,pth,nom,&bout,T_FLIC_FILE);
		iflh_flhok = FALSE;
		if(bout && nom[0] != 0)
		{	if(get_fli_info(flh.complet,ptr_var->adr_form[IFLH_DESC].ob_spec.tedinfo->te_ptext,&x,&y,&nb_img,&fli_flc,&prem_frame,&pos_prem_frame))
			{	strncpy(ptr_var->adr_form[IFLH_CHM].ob_spec.tedinfo->te_ptext,flh.complet,SIZE_CHM);
				Dsetdrv(flh.chemin[0] - 'A');
				Dsetpath(flh.chemin);
				iflh_flhok = TRUE;
								
				find_name(flh.complet,nom);
				
				if(fli_flc)
					type_obj = OBJ_FLI;
				else 	type_obj = OBJ_FLC;
				
				/* ESSAYE DE TROUVE UN EVENTUEL FICHIER KEY DS LE M CHMIN QUE LE FLIC */
				strcpy(nom_key,flh.chemin);
				strcat(nom_key,nom);
				strcat(nom_key,".KEY");
				if(check_key(nom_key,nom,x,y,nb_img))
				{	new_objet(type_obj, nom,flh.complet,nom_key);
					close_dialog(ptr_var);
					goto end_ifli;
				}
				
				/* ESSAYE DS LE REPERTOIRE DES KEY */
				strcpy(nom_key,chemin_key);
				strcat(nom_key,nom);
				strcat(nom_key,".KEY");
				if(check_key(nom_key,nom,x,y,nb_img))
				{	new_objet(type_obj,nom,flh.complet,nom_key);
					close_dialog(ptr_var);
					goto end_ifli;
				}
				strncpy(ptr_var->adr_form[IFLH_KEY].ob_spec.tedinfo->te_ptext,nom_key,SIZE_CHM);
				
				if(ptr_var->w_handle)
				{	wf_change(ptr_var,IFLH_CHM,-1,1);
					wf_draw(ptr_var,IFLH_DESC);
					wf_draw(ptr_var,IFLH_KEY);
				}

				if(event == OPEN_DIAL)
					open_dialog(ptr_var, T_TITR_FLIC_IMP, 0, -1);
					
			}
		}
	}
	end_ifli:;
}

void sauve_prefs(void)
{
	graf_mouse(BUSYBEE, 0);
	open_inf("MOUNTAIN.INF", "w");

	write_inf_line("#MOUNTAIN","%d", 101);
	write_inf_line("#use_nvdi","%d", use_nvdi);
	write_inf_line("#spd_fnt","%d %d %d", taille_def, id_texte, id_boutton);
	save_windform(&binfo_var);
	save_windform(&iflh_var);
	save_windform(&opto_var);
	save_windform(&optc_var);
	save_windform(&optv_var);
	save_windform(&optg_var);
	save_windform(&optp_var);
	save_windform(&new_var);
	save_windform(&ifon_var);
	save_windform(&carte_var);
	save_windform(&col_var);
	save_windform(&mou_var);
	save_windform(&infi_var);
	save_windform(&tran_var);
	save_windform(&opta_var);
	save_windform(&exp_var);
	save_windform(&mpl_var);
	save_windform(&int_var);
	save_windform(&alr_var);
	save_windform(&suit_var);
	save_windform(&opre_var);
	save_windform(&ctim_var);
	save_windform(&etim_var);
	save_windform(&itra_var);
	save_windform(&ptra_var);
	save_windform(&ceff_var);
	save_windform(&supi_var);
	
	write_inf_line("#wind_objet","%d %d %d %d", xfo, yfo, wfo, hfo);
	write_inf_line("#wind_projet","%d %d %d %d", xfr, yfr, wfr, hfr);
	write_inf_line("#wind_tran","%d %d %d %d", xft, yft, wft, hft);
	write_inf_line("#wind_visu","%d %d %d %d %d %d", visu_pos, def_x_pos_visu, def_y_pos_visu, visu_info, visu_fast,visu_boucle);
	write_inf_line("#wind_piste","%d %d %d %d %d %d %d %d %d %d %d %d %d %d", aff_size_buffer,
					 aff_mode ,aff_prop, piste_animA ,piste_animB ,piste_supA ,piste_supB ,
					 piste_tran ,piste_sup, xfp, yfp, wfp, hfp,draw_intensitee);
	write_inf_line("#wind_preview","%d %d", def_x_pos_prev, def_y_pos_prev);
	
	write_inf_line("#chemin_vext","%s", chemin_anim_externe);
	write_inf_line("#chemin_viext","%s", chemin_img_externe);
	write_inf_line("#chemin_key","%s", chemin_key);
	write_inf_line("#chemin_mod","%s", chemin_modules);
	write_inf_line("#chemin_mpl","%s", chemin_mplayer);
	write_inf_line("#relief","%d", int_rel);
	write_inf_line("#z_info","%d %d %d", show_zinf, zinf_var.w_x, zinf_var.w_y );
	write_inf_line("#free_mem","%d %d %d", show_mem, mem_var.w_x, mem_var.w_y );
	write_inf_line("#ultra_temps_reel","%d", ultra_temps_reel );	
	write_inf_line("#open_prj","%d %d %d %d", op_fobj, op_fprj, op_fpist, op_ftran);
	write_inf_line("#ldg_inst","%d %d", ldg_install, ldg_inst_if_sup_8);
	write_inf_line("#carte","%d %d", carte, force_vdi_use);
	write_inf_line("#n_alrt","%d", use_n_alert);
	write_inf_line("#draw_round","%d", draw_round);
	write_inf_line("#a_prev","%d %d %d", aprev_mode, aprev_preemptif, aprev_curseur);
	write_inf_line("#m_smpte","%d", mode_smpte);

	close_inf();
	graf_mouse(ARROW, 0);
}

	
void fin_prog(void)
{	int i;
	
	if(et_ke_kon_fe_du_prj())	/* si l'utilisateur est ok */
	{	close_visu();	
		for(i=nb_objets-1;i>=0;i--)
			efface_objet(i);
		close_dialog(&binfo_var);
		close_dialog(&optc_var);
		close_dialog(&optv_var);
		close_dialog(&optg_var);
		close_dialog(&optp_var);
		close_dialog(&carte_var);
		close_dialog(&infi_var);
		close_dialog(&exp_var);
		close_dialog(&mem_var);
		close_dialog(&zinf_var);
		close_dialog(&mpl_var);
		close_dialog(&suit_var);
		close_dialog(&opre_var);
		close_dialog(&ctim_var);
		close_prev();
		if(w_obj_open)		wind_close(hnd_wobjet);
		if(w_trsit_open)	wind_close(hnd_wtransit);
		wind_delete(hnd_wobjet);
		wind_delete(hnd_wtransit);
		wind_delete(hnd_wprojet);	/* projet et pistes sont fermees dans kill_projet() */
		wind_delete(hnd_wpistes);
		if(nom_font)
			Mfree(nom_font);
		kill_used_transit();
		unload_modules();
		unload_transitions();
		unload_effects();
		close_buffer();
		ldg_uninstall();
		rsrc_free();
		vst_unload_fonts(vdi_handle,0);
		v_clsvwk (vdi_handle);				/* Fermer la station de travail */
		appl_exit ();							/* Zou ! on quitte ! */
		exit (0);
	}
}

void init_prog(void)
{	int ver, attributs[10],i,x,y,dummy,j;
	AESPB *l_aes;
	OBJECT *adr_obj,*adr_boot;
	
	init_gem(); /* Nouveau pour Version 3.0 */

	vqt_attributes(vdi_handle, attributs);
	char_height = attributs[7];
		
	get_prg_path("MOUNTAIN.PRG");  /* Recherche r‚pertoire du Programme. */
	
	#ifdef _debug_
	strcpy(app_path,_CHEMIN_APP_);
	#endif
	
	if (load_rsc("mountain.rsc",NOWIND,ICONE) == 0) 
		init_fin();
	
	l_aes=(AESPB *)&AES_pb;
	
	/* preparation de la boite de boot */
	rsrc_gaddr(0,BOOTING,&adr_boot);
	
	form_center(adr_boot , &xb ,&yb, &wb ,&hb);		/* centre formulaire */
	form_dial (FMD_START, 0, 0, 0, 0, xb, yb, wb, hb);		/* prepare l'affichage */
	adr_boot[BOOT_ASS].ob_width=1;
	objc_draw (adr_boot, 0, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);

	barre(1,boot_tot,adr_boot,T_INIT_1);
	
	/* adresse des objets */
	rsrc_gaddr(0, LEMENU, &adr_menu);
	/* on informe la lib */
	le_menu = adr_menu;
	
	rsrc_gaddr(0, KEY_FRAME, &adr_key);
	if (l_aes->global[0] < 0x340 || nb_planes<4)	 /* test si l'AES >= a 3.40 ou pas en 16 couleurs*/
		rsrc_gaddr(0, CONTROL1, &adr_ctrl);
	else
		rsrc_gaddr(0, CONTROL, &adr_ctrl);

	if(l_aes->global[0] >= 0x400) /* test si l'AES >= a 4.0 */
		menu_id = menu_register(app_id,"  Mountain " VERSION_MNT);
	
	Winix_flag = FALSE;
	if(get_cookie('WINX') != '_NO_')
		Winix_flag = TRUE;
	
	if(h_char == 8)
	{	adr_ctrl[0].ob_height *= 2;
		adr_ctrl[V_STOP].ob_height *= 2;
		adr_ctrl[V_PLAY].ob_height *= 2;
		adr_ctrl[V_FORW].ob_height *= 2;
		adr_ctrl[V_BACK].ob_height *= 2;
		adr_ctrl[V_IN].ob_height *= 2;
		adr_ctrl[V_OUT].ob_height *= 2;
		adr_ctrl[V_FON].ob_y = adr_ctrl[V_STOP].ob_height+4;
	}
	
	
	barre(2,boot_tot,adr_boot,T_INIT_2);

	open_inf("MOUNTAIN.INF","r"); /* Ouverture des INF en Lecture.	*/

	read_inf_line("#MOUNTAIN","%d",&ver);
	if(ver > 199)
		close_inf();

	use_nvdi	= 1;
	read_inf_line("#use_nvdi","%d", &use_nvdi);

	/* par defaut */
	speedo_flag = use_nvdi;
	taille_def = 12;
	id_texte = 5011;
	id_boutton = 5003;
	read_inf_line("#spd_fnt","%d %d %d", &taille_def, &id_texte, &id_boutton);
	
	barre(3,boot_tot,adr_boot,T_INIT_3);

	init_windform(&binfo_var, BINFO, 0, 0);
	init_windform(&iflh_var, IMPORT_FLH, 0, 0);
	init_windform(&opto_var, OPT_OBJET, 0, 0);
	init_windform(&optc_var, OPT_CHM, 0, 0);
	init_windform(&optv_var, OPT_VISU, 0, 0);
	init_windform(&optg_var, OPT_GENE, 0, 0);
	init_windform(&optp_var, OPT_PISTE, 0, 0);
	init_windform(&new_var, NEW_PRJ, 0, 0);
	init_windform(&ifon_var, IMG_FON, 0, 0);
	init_windform(&carte_var, CARTE, 0, 0);
	if(nb_planes<16)			/* on est en mode NON true color, pas de userdef couleur */
	{	rsrc_gaddr(0, COULEUR, &adr_obj);
		adr_obj[COUL_TST].ob_type = G_BOX;		/* enleve le 20 detecte par userdef.c */
	}
	init_windform(&col_var, COULEUR, 0, 0);
	init_windform(&mou_var, MOUVEMENT, 0, 0);
	init_windform(&infi_var, MOD_IMP, 0, 0);
	init_windform(&tran_var, TRANSPARENCE, 0, 0);
	init_windform(&opta_var, OPT_ANIM, 0, 0);
	init_windform(&exp_var, CALC_FIN, 0, 0);
	init_windform(&mpl_var, CALC_MPLAYER, 0, 0);
	init_windform(&int_var, INTENSITE, 0, 0);
	init_windform(&alr_var, ALERT, 0, 0);
	init_windform(&suit_var, SUITE_IMG, 0, 0);
	init_windform(&opre_var, OPT_APRV, 0, 0);
	init_windform(&ctim_var, CHX_TIME, 0, 0);
	init_windform(&etim_var, TIME_EDIT, 0, 0);
	init_windform(&itra_var, TRAN_INFO, 0, 0);
	init_windform(&ptra_var, TRAN_PARA_DIAL, 0, 0);
	init_windform(&ceff_var, CHX_EFFET, 0, 0);
	init_windform(&supi_var, SUP_PIS, 0, 0);
	
	
	if(nb_planes==1) /* mode mono, on change la coul des ass */
	{	mou_var.adr_form[MOU_ASS_R].ob_spec.tedinfo->te_color &= 0xfff8;
		mou_var.adr_form[MOU_ASS_X].ob_spec.tedinfo->te_color &= 0xfff8;
		mou_var.adr_form[MOU_ASS_Y].ob_spec.tedinfo->te_color &= 0xfff8;
		mou_var.adr_form[MOU_ASS_PX].ob_spec.tedinfo->te_color &= 0xfff8;
		mou_var.adr_form[MOU_ASS_PY].ob_spec.tedinfo->te_color &= 0xfff8;
		tran_var.adr_form[TRAN_ASS].ob_spec.tedinfo->te_color &= 0xfff8;
		ptra_var.adr_form[TRSP_ASS1].ob_spec.tedinfo->te_color &= 0xfff8;
		ptra_var.adr_form[TRSP_ASS1+10].ob_spec.tedinfo->te_color &= 0xfff8;
		ptra_var.adr_form[TRSP_ASS1+20].ob_spec.tedinfo->te_color &= 0xfff8;
		ptra_var.adr_form[TRSP_PREV_ASS].ob_spec.tedinfo->te_color &= 0xfff8;
	}
	
	Magic_flag = FALSE;
	if(get_cookie('MagX') != '_NO_')
	{	Magic_flag = TRUE;
		adr_ctrl[V_FON].ob_spec.obspec.framesize++;		/* elagissement */
		mou_var.adr_form[MOU_FON_R].ob_spec.obspec.framesize++;
		mou_var.adr_form[MOU_FON_X].ob_spec.obspec.framesize++;
		mou_var.adr_form[MOU_FON_Y].ob_spec.obspec.framesize++;
		mou_var.adr_form[MOU_FON_PX].ob_spec.obspec.framesize++;
		mou_var.adr_form[MOU_FON_PY].ob_spec.obspec.framesize++;
		tran_var.adr_form[TRAN_FON].ob_spec.obspec.framesize++;
		ptra_var.adr_form[TRSP_FON1].ob_spec.obspec.framesize++;
		ptra_var.adr_form[TRSP_FON1+10].ob_spec.obspec.framesize++;
		ptra_var.adr_form[TRSP_FON1+20].ob_spec.obspec.framesize++;
/*		zinf_var.adr_form[0].ob_spec.obspec.framesize++;
*/	}

	strcpy(new_var.adr_form[NEW_NAME].ob_spec.tedinfo->te_ptext,T_NEW_FILE_NAME);
	
	xfo = yfo = 0;
	
	read_inf_line("#wind_objet","%d %d %d %d", &xfo, &yfo, &wfo, &hfo);
	if(!init_wobjet(xfo,yfo,wfo,hfo))
		init_fin();
	
	xfr = yfr = 0;
	
	read_inf_line("#wind_projet","%d %d %d %d", &xfr, &yfr, &wfr, &hfr);
	if(!init_wprojet(xfr,yfr,wfr,hfr))
		init_fin();
	
	xft = yft = 0;
	
	read_inf_line("#wind_tran","%d %d %d %d", &xft, &yft, &wft, &hft);
	if(!init_wtransit(xft,yft,wft,hft))
		init_fin();
	
	/* par defaut */
	visu_pos = V_LAST;
	def_x_pos_visu = 0;
	def_y_pos_visu = 0;
	visu_info = 1;
	visu_fast = 0;
	visu_boucle = 0;
	read_inf_line("#wind_visu","%d %d %d %d %d %d", &visu_pos, &def_x_pos_visu, &def_y_pos_visu, &visu_info, &visu_fast, &visu_boucle);
	
		/* valeurs par defaut pistes */
	aff_size_buffer = 100;
	aff_mode = 0;
	aff_prop = 1;
 	piste_animA = 1;
	piste_animB = 1;
	piste_supA = 1;
	piste_supB = 1;
	piste_tran = 1;
	piste_sup = 1;
	xfp=yfp=wfp=hfp=0;
	draw_intensitee = 1;
	read_inf_line("#wind_piste","%d %d %d %d %d %d %d %d %d %d %d %d %d %d", &aff_size_buffer,
					 &aff_mode ,&aff_prop, &piste_animA ,&piste_animB ,&piste_supA ,&piste_supB ,
					 &piste_tran ,&piste_sup, &xfp, &yfp, &wfp, &hfp, &draw_intensitee);
	if(!init_wpistes(xfp, yfp, wfp, hfp))
		init_fin();

	read_inf_line("#wind_preview","%d %d", &def_x_pos_prev, &def_y_pos_prev);
	init_wprev();

	/*  chemins par defaut */
	#ifndef _debug_
	ver = Dgetdrv();
	chemin_key[0] = ver + 'A';
	chemin_key[1] = ':';
	Dgetpath(&chemin_key[2],0);
	strcat(chemin_key,"\\");
	chemin_anim_externe[0] = 0;
	chemin_img_externe[0] = 0;
	strcpy(chemin_modules,chemin_key);
	strcat(chemin_modules,"modules\\");
	#endif
	
	read_inf_line("#chemin_vext","%s", chemin_anim_externe);
	read_inf_line("#chemin_viext","%s", chemin_img_externe);
	read_inf_line("#chemin_key","%s", chemin_key);
	read_inf_line("#chemin_mod","%s", chemin_modules);
	read_inf_line("#chemin_mpl","%s", chemin_mplayer);

	int_rel = 1;
	read_inf_line("#relief","%d", &int_rel);

	show_zinf	= 1;
	x=1;	y=1000;		/* en bas a gauche par defaut */
	read_inf_line("#z_info","%d %d %d", &show_zinf, &x, &y );
	init_windform(&zinf_var, ZINFO, x, y);

	show_mem	= 1;
	x=zinf_var.w_x+zinf_var.w_w+4;	y=1000;		/* en bas a gauche par defaut a dte de zinfo */
	read_inf_line("#free_mem","%d %d %d", &show_mem, &x, &y );
	init_windform(&mem_var, MEMORY, x, y);
	
	ultra_temps_reel=0;
	read_inf_line("#ultra_temps_reel","%d", &ultra_temps_reel );	
	
	op_fobj = 1;
	op_fprj = 1;
	op_fpist = 1;
	op_ftran = 1;
	read_inf_line("#open_prj","%d %d %d %d", &op_fobj, &op_fprj, &op_fpist, &op_ftran);
	

	ldg_install = 1;
	ldg_inst_if_sup_8 = 1;
	read_inf_line("#ldg_inst","%d %d", &ldg_install, &ldg_inst_if_sup_8);

	barre(4,boot_tot,adr_boot,T_INIT_4);
	/* INITIALISTION LDG */
	if(ldg_install)
	{	if((nb_planes >= 8 && ldg_inst_if_sup_8) || !ldg_inst_if_sup_8)
			ldg_mnt_init();
	}
	if(!ldg_screen_ok)
		carte_var.adr_form[CARTE_LDG].ob_state |= DISABLED;

	carte = 8;		/* Mono Bayer */
	force_vdi_use = 0;
	read_inf_line("#carte","%d %d", &carte, &force_vdi_use);
	if(carte == 9 && !ldg_screen_ok)		/* LDG screen */
		carte = 8;			/* Mono Bayer */
	
	use_n_alert = 1;
	read_inf_line("#n_alrt","%d", &use_n_alert);
	read_inf_line("#draw_round","%d", &draw_round);

	read_inf_line("#a_prev","%d %d %d", &aprev_mode, &aprev_preemptif, &aprev_curseur);
	mode_smpte = 0;
	read_inf_line("#m_smpte","%d", &mode_smpte);
	
			
	close_inf();
	
	if(!init_wvisu())
		init_fin();

	
	if(nb_planes <15)
		visu_fast = 0;
	
	barre(5,boot_tot,adr_boot,T_INIT_5);
	init_buffer();
	barre(6,boot_tot,adr_boot,T_INIT_6);
	init_mvt();
	barre(7,boot_tot,adr_boot,T_INIT_7);
	init_tran();
	barre(8,boot_tot,adr_boot,T_INIT_8);
	init_chx_time();
	barre(9,boot_tot,adr_boot,T_INIT_9);
	init_effets();		
	
	/* vide les canaux animation */
	for(i=0;i<MAX_ANIM_PAR;i++)
		anim_par[i].used = 0;
	
	/* intialisation des check pistes */
	menu_icheck(adr_menu, MN_ANI1, piste_animA);
	menu_icheck(adr_menu, MN_ANI2, piste_animB);
	menu_icheck(adr_menu, MN_EFF1, piste_supA);
	menu_icheck(adr_menu, MN_EFF2, piste_supB);
	menu_icheck(adr_menu, MN_COUPL, piste_tran);
	menu_icheck(adr_menu, MN_GENE, piste_sup);
	
	/* new prj par defaut */
	strcpy(new_var.adr_form[NEW_X].ob_spec.tedinfo->te_ptext,"160");
	strcpy(new_var.adr_form[NEW_Y].ob_spec.tedinfo->te_ptext,"100");
	strcpy(new_var.adr_form[NEW_LONG].ob_spec.tedinfo->te_ptext,"40");
	strcpy(new_var.adr_form[NEW_SPEED].ob_spec.free_string,"16");

	vq_color(vdi_handle,15,0,prev_rgb);	/* on sauve la couleur d'exemple */		
	
	barre(10,boot_tot,adr_boot,T_INIT_10);
	load_modules();
	barre(11,boot_tot,adr_boot,T_INIT_11);
	load_sav_mod();	
	
	tot_mem = (long)get_mem()*1024L*1024L + 1;
	gemdos_version = Sversion();
	if(gemdos_version >= 0x1900) 	/* Gemdos TT ou plus */
		tot_mem += get_tt_ram();
	
	sprintf(mem_var.adr_form[MEM_TXT].ob_spec.tedinfo->te_ptext,"TOTAL : %d Mo", (int)( (tot_mem/1024)/1024) );	
	
	strcpy(binfo_var.adr_form[INFO_VER].ob_spec.free_string,VERSION_MNT);
	
	binfo_var.adr_form[BINFO_BGREETS].ob_y = 111;
	binfo_var.adr_form[BINFO_BGREETS].ob_flags |= HIDETREE;
	
	barre(12,boot_tot,adr_boot,T_INIT_12);

	nom_font = 0;
	nb_fonts = 0;
	optg_var.adr_form[OPTG_POP_B].ob_spec.free_string[0]=0;
	optg_var.adr_form[OPTG_POP_T].ob_spec.free_string[0]=0;

	if (vq_gdos())		/* il y a speedo */
	{		/* chargement des fontes */
		nb_fonts = vst_load_fonts(vdi_handle,0);
			/* init des noms */
		nom_font=Malloc( (nb_fonts+2)*sizeof(NOM_FONT));
	
		if (!nom_font)
		{	nb_fonts = 0;
			use_nvdi = 0;
			goto no_fontes;
		}

		i = 0;
		for(dummy = 2;dummy < nb_fonts+2;dummy++)
		{	nom_font[i].id = vqt_name(vdi_handle,dummy,nom_font[i].name);
			if(nom_font[i].name[32] == 1) i++;
		}
		nb_fonts = i;
	
			/* Attribution d'une fonte au boutons */
		j = 0;
		for(i = 0;i < nb_fonts;i++)
		{	if(nom_font[i].id == id_boutton)
			{	j = 1;
				goto suite_idb;
			}
		}
 		suite_idb:;

		if(j == 0)
		{	id_boutton = nom_font[0].id;
			n_bout  = 0;
		}else n_bout = i;
	
		/* Attribution d'une fonte au textes */
		j = 0;
		for(i = 0;i < nb_fonts;i++)
		{	if(nom_font[i].id == id_texte)
			{	j = 1;
				goto suite_idt;
			}
		}
		suite_idt:;
		if(j == 0)
		{	id_texte = nom_font[0].id;
			n_txt  = 0;
		}else n_txt = i;
	
	}
	no_fontes:;
}

void init_fin(void)
{	form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */
	appl_exit ();
	v_clsvwk (vdi_handle);
	exit (0);
}