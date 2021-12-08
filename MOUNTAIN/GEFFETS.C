/********************MOUNTAIN********************/
/*****          GESTION DES EFFETS          *****/
/*****            Par Vanel R‚mi            *****/
/*****         Premier code : 10/31/98      *****/
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "transit.h"
#include "time_spl.h"
#include "some_fnc.h"
#include "objets.h"
#include "geffets.h"
#include "projet.h"
#include "gest_flh.h"
#include "anim.h"
#include "pistes.h"
#include "txts.h"
#include "preview.h"
#include "flh_lib.h"
#include "effets1.H"
#include "select.H"
#include "mountain.H"
#include "desc_eff.H"

extern WINDFORM_VAR ceff_var;		/* Choix des effets */

void horz_mir(int *src, int *dst, int x, int y);
void vert_mir(int *src, int *dst, int x, int y, int larg);
void noir_et_blanc(int *src, int *dst, int x, int y);
void invers_video(int *src, int *dst, int x, int y);
void strict_netb(int *src, int *dst, int x, int y, int seuil,int col1, int col2);
void asm_rvbc(int *src, int *dst, int x, int y, int r,int v, int b);
void asm_zoom(int *scr, int *dst, int x_src, int x_dest);
void diffusion(int *dst, int x, int y, long rnd);

/* prj objet */
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern int selected_prj_obj;
extern OBJET objet[MAX_OBJET];
extern int ultra_temps_reel;

extern WINDFORM_VAR ptra_var;		/* para des transitions */
extern int temp_r,temp_v,temp_b;
extern int coul_caller;
extern TOOL_BOX tool_box;
void form_col(int event);
void set_ptra_prev_ass(int val, int draw);

int nb_effets;				/* nombre d'effets */
EFFET *effet[MAX_EFFETS];
int eff_deb_v=0;
int effet_av_selected=-1,effet_cur_selected=-1;
static STOQUAG_EFF tempo_sto,memo_sto;
static int were_no_effets;
int effet_parametrisation;
static int effet_cur_param;
static TRAN_DESC_PARAM *selected_desc_param;
static int ptra_min_ass[3],ptra_max_ass[3];
static TRAN_REAL_PARAM tempo_param;
static int prev_treel;
static int ef_prev_val=50;
extern int x_trev,y_trev,x_tpremod16;
static int x_obj,y_obj;
static int *adr_ef_obj=0;
MFDB 	mfdb_efpre;
int 	*adr_ef_prev=0;		/* image de preview */
static EFFET_PARA effet_para;
static int selected_effet,old_calc_frame;

/* Protos */
static void add_effet(int type, int mode_para, const char *nom, long id, char need_preced_img,
								int (*calc_effet)(EFFET_PARA *adr_para), TRAN_DESC_PARAM *effet_desc_param);
static void draw_liste_effets(int draw);
static void draw_cur_effets(int draw);
static void anim_ef_prev(void);
static void ass_prev(void);
static int effet_FMVT(EFFET_PARA *effet_para);
static int effet_NETB(EFFET_PARA *effet_para);
static int effet_SNTB(EFFET_PARA *effet_para);
static int effet_INVV(EFFET_PARA *effet_para);
static int effet_MIRH(EFFET_PARA *effet_para);
static int effet_MIRV(EFFET_PARA *effet_para);
static int effet_DIFF(EFFET_PARA *effet_para);
static int effet_RVBC(EFFET_PARA *effet_para);
/* DANS EFFETS1.C : */
int effet_WOBV(EFFET_PARA *effet_para);
int effet_ROTS(EFFET_PARA *effet_para);
int effet_SPIR(EFFET_PARA *effet_para);
static void add_cur_effet(int selected);
static void get_anno(long id, int num_param);
static void move_ceff_ass(void);
static void efface_cur_effet(STOQUAG_EFF *sto_liste,int num);
static void move_cur_effet(int num);
static void move_effet(int num_src, int num_dest);
static void move_add_effet(int num);
static void set_ptra_ass(int num, long val, int draw);
static void ptra_ass_vari(int num);

void init_effets(void)
{	int i;
	memo_sto.nb_effets=0;
	for(i=0;i<16;i++)
		strcpy(ceff_var.adr_form[CEFF_CU1+i].ob_spec.tedinfo->te_ptext,"                         ");

	nb_effets=0;
	add_effet(0,EFF_STD_PARAM,T_TITR_FMVT,'FMVT',0,effet_FMVT,&desc_eparam_fmvt);
	add_effet(0,EFF_NO_PARAM,T_TITR_NETB,'NETB',0,effet_NETB,0);
	add_effet(0,EFF_STD_PARAM,T_TITR_SNTB,'N&BS',0,effet_SNTB,&desc_eparam_sntb);
	add_effet(0,EFF_NO_PARAM,T_TITR_INVV,'INVV',0,effet_INVV,0);
	add_effet(0,EFF_NO_PARAM,T_TITR_MIRH,'MIRH',0,effet_MIRH,0);
	add_effet(0,EFF_NO_PARAM,T_TITR_MIRV,'MIRV',0,effet_MIRV,0);
	add_effet(0,EFF_STD_PARAM,T_TITR_WOBV,'WOBV',0,effet_WOBV,&desc_eparam_wobv);
/*	add_effet(0,EFF_NO_PARAM,T_TITR_DIFF,'DIFF',0,effet_DIFF,0); */
	add_effet(0,EFF_STD_PARAM,T_TITR_RVBC,'RVBC',0,effet_RVBC,&desc_eparam_rvbc);
	add_effet(0,EFF_STD_PARAM,T_TITR_ROTS,'ROTS',0,effet_ROTS,&desc_eparam_rots);
	add_effet(0,EFF_STD_PARAM,T_TITR_SPIR,'SPIR',0,effet_SPIR,&desc_eparam_spir);
/*	add_effet(0,EFF_NO_PARAM,"Flou...",'FLOU',0,effet_NETB,0);
	add_effet(0,EFF_NO_PARAM,"Vagues...",'VAGU',0,effet_NETB,0);
	add_effet(0,EFF_NO_PARAM,"Pixelisation...",'PXEL',0,effet_NETB,0);
	add_effet(0,EFF_NO_PARAM,"Luminosit‚...",'LUMI',0,effet_NETB,0);
	add_effet(0,EFF_NO_PARAM,"Contrast...",'CNTR',0,effet_NETB,0);
	add_effet(0,EFF_NO_PARAM,"Echange de couleur...",'CCOL',0,effet_NETB,0);
*/}

static int effet_FMVT(EFFET_PARA *effet_para)
{	if(effet_para->preced_img_ok)
		zoom(effet_para->img_src, effet_para->img_dst, effet_para->x, effet_para->y, effet_para->larg_img, effet_para->x, effet_para->y,effet_para->larg_img,effet_para->effet_real_param->vari[0]);
	else copy_tc2(effet_para->img_src, effet_para->img_dst, effet_para->y, effet_para->x, effet_para->larg_img-effet_para->x,effet_para->larg_img-effet_para->x);
	return 1;
}

static int effet_MIRV(EFFET_PARA *effet_para)
{	vert_mir(effet_para->img_src, effet_para->img_dst, effet_para->x, effet_para->y, effet_para->larg_img);
	return 1;
}static int effet_MIRH(EFFET_PARA *effet_para)
{	horz_mir(effet_para->img_src, effet_para->img_dst, effet_para->larg_img, effet_para->y);
	return 1;
}
static int effet_SNTB(EFFET_PARA *effet_para)
{	int col1,col2;
	col1=conv_col(effet_para->effet_real_param->color[0][0],effet_para->effet_real_param->color[0][1],effet_para->effet_real_param->color[0][2]);
	col2=conv_col(effet_para->effet_real_param->color[1][0],effet_para->effet_real_param->color[1][1],effet_para->effet_real_param->color[1][2]);
	strict_netb(effet_para->img_src, effet_para->img_dst, effet_para->larg_img, effet_para->y,
					effet_para->effet_real_param->vari[0],col1,col2);
	return 1;
}
static int effet_NETB(EFFET_PARA *effet_para)
{	noir_et_blanc(effet_para->img_src, effet_para->img_dst, effet_para->larg_img, effet_para->y);
	return 1;
}
static int effet_INVV(EFFET_PARA *effet_para)
{	invers_video(effet_para->img_src, effet_para->img_dst, effet_para->larg_img, effet_para->y);
	return 1;
}
static int effet_DIFF(EFFET_PARA *effet_para)
{	long rnd;
	rnd = random(52) | ((long)random(50)<<16);
	if(!effet_para->preced_img_ok)
		copy_tc2(effet_para->img_src, effet_para->img_dst, effet_para->y, effet_para->x, effet_para->larg_img-effet_para->x,effet_para->larg_img-effet_para->x);
	diffusion(effet_para->img_dst, effet_para->larg_img, effet_para->y,rnd);
	
	return 1;
}
static int effet_RVBC(EFFET_PARA *effet_para)
{	asm_rvbc(effet_para->img_src, effet_para->img_dst, effet_para->larg_img, effet_para->y,
					effet_para->effet_real_param->vari[0],effet_para->effet_real_param->vari[1],effet_para->effet_real_param->vari[2]);
	return 1;
}

void unload_effects(void)
{	int i;
	for (i=0;i<nb_effets;i++)
		free(effet[i]);
	nb_effets=0;
}

void form_ceff(int event)
{
	WINDFORM_VAR *ptr_var = &ceff_var;
	int choix,dum,handle,bout,mk;
	long head='MEFF';
	char pth[160],nom[16];
	CHEMIN fic;
	
	if (event == OPEN_DIAL)
	{	/* INITIALISATIONS */
		were_no_effets = 1;
		tempo_sto.nb_effets=0;
		if(prj_obj[selected_prj_obj].stoquag_eff)
		{	were_no_effets = 0;
			copy_sto_eff(prj_obj[selected_prj_obj].stoquag_eff,&tempo_sto);
		}
		effet_cur_selected=-1;
		
		draw_liste_effets(0);
		draw_cur_effets(0);
		open_dialog(ptr_var, T_TITR_CHX_EFFET, 0, -1);
	}

	else 
	{	choix = windial_do(ptr_var, event);
	 	if(choix == CLOSE_DIAL)
	 		choix = CEFF_ANN;
	 	if(choix != 0)
		{	
			if (choix >= CEFF_CU1 && choix < CEFF_CU1+MAX_CUR_EFFETS)
			{	Vsync();	Vsync();	Vsync();
				graf_mkstate(&dum, &dum, &mk,&dum);
				dum = choix - CEFF_CU1;
				if(dum<tempo_sto.nb_effets)
				{	if(nbr_clics == 2 || mousek==2) 	/* dbleclick = parametrage */
					{	effet_cur_param = dum;
						dum=find_effet(tempo_sto.id[effet_cur_param]);
						if(dum != -1)
						{	if(effet[dum]->mode_para == EFF_STD_PARAM)
								form_ptra2(OPEN_DIAL);
						}
					}else 			/* deplacement ou selection */
					{	effet_cur_selected = dum;
						if(mk==1) /* on appuis encore */
							move_cur_effet(dum);
						draw_cur_effets(1);
					}
				}
			}
			if (choix >= CEFF_AV1 && choix < CEFF_AV1+16)
			{	Vsync();	Vsync();	Vsync();
				graf_mkstate(&dum, &dum, &mk,&dum);
				dum = choix - CEFF_AV1 + eff_deb_v;
				if(dum<nb_effets)
				{	if(mk==1)
					{	if(tempo_sto.nb_effets<MAX_CUR_EFFETS-2)		/* il faut un esp de + pour le depalcemet */
							move_add_effet(dum);
						else super_alert(1,3,T_IMPOS_ADD_CEFF);
					}else
					{	effet_av_selected = dum;
						draw_liste_effets(1);
					}
				}
			}
			if(choix == CEFF_PARAM)
			{	if(effet_cur_selected != -1)
				{	effet_cur_param = effet_cur_selected;
					dum=find_effet(tempo_sto.id[effet_cur_param]);
					if(dum != -1)
					{	if(effet[dum]->mode_para == EFF_STD_PARAM)
							form_ptra2(OPEN_DIAL);
					}
				}
				wf_change(ptr_var,choix,0,1);
			}
			if (choix == CEFF_UP && eff_deb_v)
			{	eff_deb_v--;
				draw_liste_effets(2);
			}
			if (choix == CEFF_DOWN && eff_deb_v<nb_effets-16)
			{	eff_deb_v++;
				draw_liste_effets(2);
			}
			if (choix == CEFF_ASS && nb_effets+1>16)
				move_ceff_ass();
				
			if (choix == CEFF_APP)
			{	if(effet_av_selected != -1)
				{	if(tempo_sto.nb_effets<MAX_CUR_EFFETS-2)		/* il faut un esp de + pour le depalcemet */
					{	add_cur_effet(effet_av_selected);				/* installe l'effet */
						effet_cur_selected = tempo_sto.nb_effets-1;	/* le selectionne */
						draw_cur_effets(1);
					}else
						super_alert(1,3,T_IMPOS_ADD_CEFF);
				}
				wf_change(ptr_var,choix,0,1);
			}
			
			if (choix == CEFF_SAVE)
			{	strcpy(pth,"\\*.EFF");
				Fselecteur(&fic,pth,nom,&bout,T_SAVE_EFF);
				if(bout && nom[0])
				{	Dsetdrv(fic.chemin[0] - 'A');
					Dsetpath(fic.chemin);
					find_name(fic.complet,nom);
					strcpy(pth, fic.chemin);
					strcat(pth, nom);
					strcat(pth, ".EFF");		/* on impose l'extention EFF */
					handle = (int)Fcreate(pth,FA_ARCHIVE);
					if(handle>0)
					{	Fwrite(handle,4,&head);
						Fwrite(handle,2,&tempo_sto.nb_effets);
						Fwrite(handle,tempo_sto.nb_effets*4,tempo_sto.id);
						Fwrite(handle,tempo_sto.nb_effets*sizeof(TRAN_REAL_PARAM),tempo_sto.effet_param);
						Fclose(handle);
					}
				}
				wf_change(ptr_var,choix,0,1);
			}
			if (choix == CEFF_LOAD)
			{	strcpy(pth,"\\*.EFF");
				Fselecteur(&fic,pth,nom,&bout,T_LOAD_EFF);
				if(bout && nom[0])
				{	Dsetdrv(fic.chemin[0] - 'A');
					Dsetpath(fic.chemin);
					handle = (int)Fopen(fic.complet,FO_READ);
					if(handle>0)
					{	Fread(handle,4,&head);
						if(head == 'MEFF')
						{	Fread(handle,2,&tempo_sto.nb_effets);
							Fread(handle,tempo_sto.nb_effets*4,tempo_sto.id);
							Fread(handle,tempo_sto.nb_effets*sizeof(TRAN_REAL_PARAM),tempo_sto.effet_param);
							Fclose(handle);
							effet_cur_selected=-1;
							draw_cur_effets(1);
						}
					}
				}
				wf_change(ptr_var,choix,0,1);
			}
			
			if (choix == CEFF_MEM)
			{	copy_sto_eff(&tempo_sto,&memo_sto);
				wf_change(ptr_var,choix,0,1);
			}if (choix == CEFF_RAP)
			{	copy_sto_eff(&memo_sto,&tempo_sto);
				effet_cur_selected=-1;
				draw_cur_effets(1);
				wf_change(ptr_var,choix,0,1);
			}
			
			if (choix == CEFF_EFF)
			{	if(effet_cur_selected != -1)
				{	efface_cur_effet(&tempo_sto,effet_cur_selected);
					effet_cur_selected=-1;
					draw_cur_effets(1);
				}
				wf_change(ptr_var,choix,0,1);
			}
			
			if (choix == CEFF_OK) 
			{	if(tempo_sto.nb_effets)
				{	if(were_no_effets)
					{	/* IL FAUT RESEVER UN ESPACE */
						prj_obj[selected_prj_obj].stoquag_eff = malloc(sizeof(STOQUAG_EFF));
						if(prj_obj[selected_prj_obj].stoquag_eff)
							copy_sto_eff(&tempo_sto,prj_obj[selected_prj_obj].stoquag_eff);
						else
							super_alert(1,1,T_NO_MEMORY);
					}else
						copy_sto_eff(&tempo_sto,prj_obj[selected_prj_obj].stoquag_eff);
				}else /* pas d'effets, on vire */
				{	if(!were_no_effets)
					{	free(prj_obj[selected_prj_obj].stoquag_eff);
						prj_obj[selected_prj_obj].stoquag_eff=0;
					}
				}
				wf_change(ptr_var,choix,0,1);
				close_dialog(ptr_var);
			}
			
			if (choix == CEFF_ANN) 
			{	wf_change(ptr_var,choix,0,1);
				close_dialog(ptr_var);
			}
		}
	}
}

static void move_add_effet(int num)
{	int mx,my,dum,old_x,old_y,pos=num,old_pos=-1,mk,ax,ay,decal_x,decal_y;
	int w,h;
	
	graf_mouse (FLAT_HAND, 0);
	ax = ceff_var.adr_form[0].ob_x + ceff_var.adr_form[CEFF_CURFON].ob_x;
	ay = ceff_var.adr_form[0].ob_y + ceff_var.adr_form[CEFF_CURFON].ob_y;
	w=ceff_var.adr_form[CEFF_AV1].ob_width;
	h=ceff_var.adr_form[CEFF_AV1].ob_height;
	old_x = ceff_var.adr_form[0].ob_x + ceff_var.adr_form[CEFF_AVFON].ob_x+ceff_var.adr_form[CEFF_AV1+num-eff_deb_v].ob_x;
	old_y = ceff_var.adr_form[0].ob_y + ceff_var.adr_form[CEFF_AVFON].ob_y+ceff_var.adr_form[CEFF_AV1+num-eff_deb_v].ob_y;
	graf_mkstate(&mx, &my, &mk,&dum); 
	decal_x= mx - old_x;
	decal_y= my - old_y;
	vswr_mode(vdi_handle,MD_XOR);
	v_hide_c(vdi_handle);
	rect(old_x,old_y,w,h,BLACK);
	v_show_c(vdi_handle, 1);
	
	do  
	{	/* prise des coordon‚es de la souris : */
		graf_mkstate(&mx, &my, &mk,&dum);
		pos = objc_find(ceff_var.adr_form, CEFF_CURFON, 4, mx,my);
		if(pos >= CEFF_CU1 && pos <= CEFF_CU1 + tempo_sto.nb_effets)
			pos -= CEFF_CU1;
		else if(pos > CEFF_CU1 + tempo_sto.nb_effets && pos < CEFF_CU1+16)
			pos = tempo_sto.nb_effets;
		else
			pos = -1;
		if(pos != old_pos)
		{	/* efface rect old */
			Vsync();
			v_hide_c(vdi_handle);
			if(old_pos != -1)
			{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-1,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
				ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
			}
			if(pos != -1)
			{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-1,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-2,1,4,BLACK);
				ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-2,1,4,BLACK);
			}
			old_pos=pos;
			v_show_c(vdi_handle, 1);
		}
		mx -= decal_x;
		my -= decal_y;
		if(mx != old_x || my != old_y)
		{	/* efface rect old */
			Vsync();
			v_hide_c(vdi_handle);
			rect(old_x,old_y,w,h,BLACK);
			rect(mx,my,w,h,BLACK);
			v_show_c(vdi_handle, 1);
			old_x = mx;
			old_y = my;
		}
	}while (mk !=0);
	v_hide_c(vdi_handle);
	if(old_pos != -1)
	{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-1,w,1,BLACK);
		ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y,w,1,BLACK);
		ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
		ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
		add_cur_effet(num);
		move_effet(tempo_sto.nb_effets-1, old_pos);
		effet_cur_selected = old_pos;
		if(effet_cur_selected == tempo_sto.nb_effets) effet_cur_selected--;
	}
	rect(old_x,old_y,w,h,BLACK);
	v_show_c(vdi_handle, 1);
	graf_mouse (ARROW, 0);
	vswr_mode(vdi_handle,MD_REPLACE);
	draw_cur_effets(1);
}

static void move_cur_effet(int num)
{	int mx,my,dum,old_x,old_y,pos=num,old_pos=-1,mk,ax,ay,decal_x,decal_y;
	int w,h;
	
	graf_mouse (FLAT_HAND, 0);
	ax = ceff_var.adr_form[0].ob_x + ceff_var.adr_form[CEFF_CURFON].ob_x;
	ay = ceff_var.adr_form[0].ob_y + ceff_var.adr_form[CEFF_CURFON].ob_y;
	w=ceff_var.adr_form[CEFF_CU1+num].ob_width;
	h=ceff_var.adr_form[CEFF_CU1+num].ob_height;
	old_x = ax+ceff_var.adr_form[CEFF_CU1+num].ob_x;
	old_y = ay+ceff_var.adr_form[CEFF_CU1+num].ob_y;
	graf_mkstate(&mx, &my, &mk,&dum); 
	decal_x= mx - old_x;
	decal_y= my - old_y;
	vswr_mode(vdi_handle,MD_XOR);
	v_hide_c(vdi_handle);
	rect(old_x,old_y,w,h,BLACK);
	v_show_c(vdi_handle, 1);
	
	do  
	{	/* prise des coordon‚es de la souris : */
		graf_mkstate(&mx, &my, &mk,&dum);
		pos = objc_find(ceff_var.adr_form, CEFF_CURFON, 4, mx,my);
		if(pos >= CEFF_CU1 && pos <= CEFF_CU1 + tempo_sto.nb_effets)
			pos -= CEFF_CU1;
		else if(pos > CEFF_CU1 + tempo_sto.nb_effets && pos < CEFF_CU1+16)
			pos = tempo_sto.nb_effets;
		else
			pos = -1;
		if(pos != old_pos)
		{	/* efface rect old */
			Vsync();
			v_hide_c(vdi_handle);
			if(old_pos != -1)
			{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-1,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
				ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
			}
			if(pos != -1)
			{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-1,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y,w,1,BLACK);
				ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-2,1,4,BLACK);
				ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+pos].ob_y-2,1,4,BLACK);
			}
			old_pos=pos;
			v_show_c(vdi_handle, 1);
		}
		mx -= decal_x;
		my -= decal_y;
		if(mx != old_x || my != old_y)
		{	/* efface rect old */
			Vsync();
			v_hide_c(vdi_handle);
			rect(old_x,old_y,w,h,BLACK);
			rect(mx,my,w,h,BLACK);
			v_show_c(vdi_handle, 1);
			old_x = mx;
			old_y = my;
		}
	}while (mk !=0);
	v_hide_c(vdi_handle);
	if(old_pos != -1)
	{	ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-1,w,1,BLACK);
		ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y,w,1,BLACK);
		ligne(ax,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
		ligne(ax+w-1,ay+ceff_var.adr_form[CEFF_CU1+old_pos].ob_y-2,1,4,BLACK);
		move_effet(num, old_pos);
		effet_cur_selected = old_pos;
		if(effet_cur_selected > num) effet_cur_selected--;
		if(effet_cur_selected == tempo_sto.nb_effets) effet_cur_selected--;
	}
	rect(old_x,old_y,w,h,BLACK);
	v_show_c(vdi_handle, 1);
	graf_mouse (ARROW, 0);
	vswr_mode(vdi_handle,MD_REPLACE);
}

static void move_effet(int num_src, int num_dest)
{	int restant,alt_src;
	if(num_src == num_dest)
		return;

	restant = tempo_sto.nb_effets - num_dest;
	if(num_src > num_dest)
		alt_src=num_src+1;
	else	alt_src=num_src;
	
	/* decalage, on cree un espace vide */
	back_memcpy(&tempo_sto.effet_param[num_dest+1], &tempo_sto.effet_param[num_dest], (int)(restant * sizeof(TRAN_REAL_PARAM)));
	back_memcpy(&tempo_sto.id[num_dest+1], &tempo_sto.id[num_dest], restant * 4);
	/* on y copie l'effet */
	memcpy(&tempo_sto.effet_param[num_dest], &tempo_sto.effet_param[alt_src], sizeof(TRAN_REAL_PARAM));
	tempo_sto.id[num_dest] = tempo_sto.id[alt_src];
	/* on efface l'ancien */
	tempo_sto.nb_effets++;
	efface_cur_effet(&tempo_sto,alt_src);

}
static void efface_cur_effet(STOQUAG_EFF *sto_liste,int num)
{	
	if(num < sto_liste->nb_effets-1) /* c'est pas le dernier */
	{	memcpy(&sto_liste->id[num], &sto_liste->id[num+1], (sto_liste->nb_effets - num - 1) * 4);
		memcpy(&sto_liste->effet_param[num], &sto_liste->effet_param[num+1], (sto_liste->nb_effets - num - 1) * sizeof(TRAN_REAL_PARAM));
	}
	sto_liste->nb_effets--;

}

static void move_ceff_ass(void)
{	int my,dum,old,nb,mk,ay,decal;

	old = eff_deb_v;
	graf_mouse (FLAT_HAND, 0);
	ay = ceff_var.adr_form[0].ob_y + ceff_var.adr_form[CEFF_FON].ob_y;
	graf_mkstate(&dum, &my, &mk,&dum); 
	decal= my - (ay+ceff_var.adr_form[CEFF_ASS].ob_y);

	do  
	{	/* prise des coordon‚es de la souris : */
		graf_mkstate(&dum, &my, &mk,&dum); 
		nb = my - ay - decal;
		if (nb <  0 )
			nb = 0;
		if (nb > ceff_var.adr_form[CEFF_FON].ob_height - ceff_var.adr_form[CEFF_ASS].ob_height)
			nb = ceff_var.adr_form[CEFF_FON].ob_height - ceff_var.adr_form[CEFF_ASS].ob_height;
					
		eff_deb_v = ((nb_effets-16)*nb) / (ceff_var.adr_form[CEFF_FON].ob_height - ceff_var.adr_form[CEFF_ASS].ob_height);

		if(eff_deb_v != old)
		{	old = eff_deb_v;
			/* redraw */
			draw_liste_effets(2);
		}
	}while (mk !=0);
	graf_mouse (ARROW, 0);
}	

static void add_cur_effet(int selected)
{	TRAN_REAL_PARAM *tran_real_param;
	TRAN_DESC_PARAM *tran_desc_param;
	
	tempo_sto.id[tempo_sto.nb_effets] = effet[selected]->id;
	
	if(effet[selected]->mode_para == EFF_STD_PARAM)
	{	tran_real_param = &tempo_sto.effet_param[tempo_sto.nb_effets];
		tran_desc_param = effet[selected]->effet_desc_param;
		inst_def_para(tran_real_param,tran_desc_param);
	}
	tempo_sto.nb_effets++;
}

static void draw_cur_effets(int draw)
{	int i;
	int cur_effet;
	
	for(i=0;i<MAX_CUR_EFFETS;i++)
	{	if(i<tempo_sto.nb_effets)
		{	cur_effet = find_effet(tempo_sto.id[i]);
			if(cur_effet != -1)
				strpcpy_force(ceff_var.adr_form[CEFF_CU1+i].ob_spec.tedinfo->te_ptext+1,effet[cur_effet]->nom,24,' ');
		}
		else
			strcpy(ceff_var.adr_form[CEFF_CU1+i].ob_spec.tedinfo->te_ptext,"                         ");
		if(i == effet_cur_selected)
			ceff_var.adr_form[CEFF_CU1+i].ob_state = SELECTED;
		else	ceff_var.adr_form[CEFF_CU1+i].ob_state = NORMAL;
		if(draw)
			objc_draw(ceff_var.adr_form, CEFF_CU1+i, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
	}
}

static void draw_liste_effets(int draw)
{	int i;
	
	for(i=0;i<16;i++)
	{	if(i+eff_deb_v<nb_effets)
			strpcpy_force(ceff_var.adr_form[CEFF_AV1+i].ob_spec.tedinfo->te_ptext+1,effet[i+eff_deb_v]->nom,24,' ');
		else
			strcpy(ceff_var.adr_form[CEFF_AV1+i].ob_spec.tedinfo->te_ptext,"                         ");
		if(i+eff_deb_v == effet_av_selected)
			ceff_var.adr_form[CEFF_AV1+i].ob_state = SELECTED;
		else	ceff_var.adr_form[CEFF_AV1+i].ob_state = NORMAL;
		if(draw)
			objc_draw(ceff_var.adr_form, CEFF_AV1+i, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
	}
	
	if(nb_effets+1<=16)
	{	ceff_var.adr_form[CEFF_ASS].ob_y=0;
		ceff_var.adr_form[CEFF_ASS].ob_height=ceff_var.adr_form[CEFF_FON].ob_height;
	}else
	{	ceff_var.adr_form[CEFF_ASS].ob_height=(ceff_var.adr_form[CEFF_FON].ob_height*16)/nb_effets;
		ceff_var.adr_form[CEFF_ASS].ob_y= (eff_deb_v*(ceff_var.adr_form[CEFF_FON].ob_height-ceff_var.adr_form[CEFF_ASS].ob_height))/(nb_effets-16);
	}
	
	if(draw == 2)
		wf_draw(&ceff_var,CEFF_FON);
}


static void add_effet(int type, int mode_para, const char *nom, long id, char need_preced_img,
								int (*calc_effet)(EFFET_PARA *adr_para), TRAN_DESC_PARAM *effet_desc_param)
{	
	if(nb_effets>= MAX_EFFETS)
	{	super_alert(1,2,"[ No memory to| add effect.|Please ctc the author.][ OK ]");
		return;
	}
	
	effet[nb_effets] = malloc(sizeof(EFFET));	/* pour y mettre la l'effet */
	if(!effet[nb_effets])	return;
	
	effet[nb_effets]->type = type;
	effet[nb_effets]->mode_para = mode_para;
	effet[nb_effets]->need_preced_img = need_preced_img;
	effet[nb_effets]->id = id;
	strncpy(effet[nb_effets]->nom,nom,23);
	effet[nb_effets]->calc_effet = calc_effet;
	effet[nb_effets]->effet_desc_param = effet_desc_param;
	nb_effets++;
}

void copy_sto_eff(STOQUAG_EFF *src, STOQUAG_EFF *dst)
{	int i;

	dst->nb_effets = src->nb_effets;
	for(i=0;i<src->nb_effets;i++)
		dst->id[i] = src->id[i];
	for(i=0;i<src->nb_effets;i++)
		copy_tran_param(&src->effet_param[i],&dst->effet_param[i]);
}

int find_effet(long id)
{	int i;

	for(i=0;i<nb_effets;i++)
	{	if(id == effet[i]->id)
			return i;
	}
	return -1;
}

int para_1eff_coul(void)
{	return conv_col(tempo_param.color[0][0],tempo_param.color[0][1],tempo_param.color[0][2]);
}

int para_2eff_coul(void)
{	return conv_col(tempo_param.color[1][0],tempo_param.color[1][1],tempo_param.color[1][2]);
}

void scan_eff_param(void)
{	int i;
	for(i=0;i<3;i++)
	{	if(ptra_var.adr_form[TRSP_PF1+i*10].ob_state == SELECTED)
			tempo_param.mode_vari[i] = 0;
		else if(ptra_var.adr_form[TRSP_PTS1+i*10].ob_state == SELECTED)
			tempo_param.mode_vari[i] = 1;
		else
			tempo_param.mode_vari[i] = 2;
	}
	tempo_param.switchs = 0;
	for(i=0;i<selected_desc_param->nb_switchs;i++)
	{	if(ptra_var.adr_form[TRSP_SW1+i].ob_state & SELECTED)
			tempo_param.switchs |= 1<<i;
	}
}

void calc_eff_prev(int draw)
{	int j,time;

	if(adr_ef_prev)
	{	if(old_calc_frame==-1 || ef_prev_val >= old_calc_frame)
			effet_para.preced_img_ok = 0;
		else	effet_para.preced_img_ok = 1;
						
										/* INIT DES VARIABLES */
		if(effet[selected_effet]->mode_para == EFF_STD_PARAM)
		{
			for(j=0;j<selected_desc_param->nb_variables;j++)
			{	time = tempo_param.time[j];
				if(tempo_param.mode_vari[j] == 1)		/* mode time spline */
				{	if(TRUE)		/* evite division par 0 */
						tempo_param.vari[j] = selected_desc_param->min_vari[j]+(int)spline_comput(time,0, ef_prev_val,
													   100, selected_desc_param->max_vari[j]-selected_desc_param->min_vari[j]);
				}else if(tempo_param.mode_vari[j] == 0) /* fixe */
					tempo_param.vari[j] = tempo_param.vari_def[j];
			}
			effet_para.effet_real_param = &tempo_param;
		}

			/* GOOOOOOOOOO ! */
		(effet[selected_effet]->calc_effet)(&effet_para);
		old_calc_frame = ef_prev_val;
		
		if(draw)
		{ 	Vsync();	
			objc_draw(ptra_var.adr_form,TRSP_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		}
	}
}

void eff_set_colors(int set)
{	tempo_param.color[set-1][0]=temp_r;
	tempo_param.color[set-1][1]=temp_v;
	tempo_param.color[set-1][2]=temp_b;
	wf_draw(&ptra_var,TRSP_C1+(set-1)*2);
}

static void set_ptra_ass(int num, long val, int draw)
{	ptra_var.adr_form[TRSP_ASS1+num*10].ob_x = (int)( 
		((val-ptra_min_ass[num])*(ptra_var.adr_form[TRSP_FON1+num*10].ob_width - 
		ptra_var.adr_form[TRSP_ASS1+num*10].ob_width))/(ptra_max_ass[num]-ptra_min_ass[num]));
	itoa((int)val,ptra_var.adr_form[TRSP_ASS1+num*10].ob_spec.tedinfo->te_ptext,10);
	if(draw)
	{ Vsync();	wf_draw(&ptra_var,TRSP_FON1+num*10); }
}

static void ptra_ass_vari(int num)
{	int mousex,bouton,dummy,old,ax,nb,max,decal,updt=0;
	
	ax = ptra_var.adr_form[0].ob_x + ptra_var.adr_form[TRSP_FON1+num*10].ob_x;
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	decal= mousex - (ax+ptra_var.adr_form[TRSP_ASS1+num*10].ob_x);
	max = ptra_var.adr_form[TRSP_FON1+num*10].ob_width - ptra_var.adr_form[TRSP_ASS1+num*10].ob_width;
	
	graf_mouse (FLAT_HAND,0);
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	old = mousex - ax - decal;
	old = ptra_min_ass[num] + (int)( ((long)old * (ptra_max_ass[num]-ptra_min_ass[num]) ) / max);

	scan_eff_param(); /* pour la preview */

	graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	do  
	{	/* prise des coordon‚es de la souris : */
		nb = mousex - ax - decal;

		if (nb <  0 )
			nb = 0;
		if (nb > max  )
			nb = max; 
			
		nb = ptra_min_ass[num] + (int)( ((long)nb * (ptra_max_ass[num]-ptra_min_ass[num]) ) / max);
		
		if (old != nb ) 
		{	old = nb;
			set_ptra_ass(num,nb,TRUE);
			updt = 0;
			if(ultra_temps_reel && prev_treel)
			{	tempo_param.vari_def[num] = nb;
				calc_eff_prev(1);
			}
		}else
		{	if(updt == 3)
			{	if(prev_treel)
				{	tempo_param.vari_def[num] = nb;
					calc_eff_prev(1);
				}
				updt++;
			}else if(updt<3)
			{	updt++;
				Vsync();
			}
		}
		graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	} while (bouton != 0);
	graf_mouse (ARROW, 0);
	tempo_param.vari_def[num] = nb;
}

static void ass_prev(void)
{	int mousex,bouton,dummy,old,ax,nb,max,decal,updt=0;
	
	ax = ptra_var.adr_form[0].ob_x + ptra_var.adr_form[TRSP_PREV_FON].ob_x;
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	decal= mousex - (ax+ptra_var.adr_form[TRSP_PREV_ASS].ob_x);
	max = ptra_var.adr_form[TRSP_PREV_FON].ob_width - ptra_var.adr_form[TRSP_PREV_ASS].ob_width;
	
	graf_mouse (FLAT_HAND,0);
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	old = mousex - ax - decal;
	old = (old * 100) / max;

	scan_eff_param(); /* pour la preview */

	graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	do  
	{	/* prise des coordon‚es de la souris : */
		nb = mousex - ax - decal;

		if (nb < 0)
			nb = 0;
		if (nb > max)
			nb = max; 
			
		nb = (nb * 100) / max;
		
		if (old != nb ) 
		{	old = nb;
			set_ptra_prev_ass(nb,TRUE);
			updt = 0;
		}else
		{	if(updt == 3)
			{	if(prev_treel)
				{	ef_prev_val=nb;
					calc_eff_prev(1);
				}
				updt++;
			}else if(updt<3)
			{	updt++;
				Vsync();
			}
		}
		graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	} while (bouton != 0);
	graf_mouse (ARROW, 0);
	
	ef_prev_val=nb;
}

static void anim_ef_prev(void)
{	int frame,larg_eff,old=ef_prev_val;
	int dum,mk,key;
	scan_eff_param();
	larg_eff = prj_obj[selected_prj_obj].pos_out - prj_obj[selected_prj_obj].pos_in;
	for(frame = 0; frame<larg_eff;frame++)
	{	calc_eff_prev(1);
		ef_prev_val = (frame * 100)/larg_eff;
		set_ptra_prev_ass(ef_prev_val,1);
		graf_mkstate(&dum, &dum, &mk,&key);
		if(mk == 2 || key)
			goto fin;
	}
	fin:;
	ef_prev_val=old;
	set_ptra_prev_ass(ef_prev_val,1);
	calc_eff_prev(1);
}

void form_ptra2(int event)
{
	WINDFORM_VAR *ptr_var = &ptra_var;
	int choix,i,j;
	char str[8];
	int *adr_obj;
	
	if (event == OPEN_DIAL)
	{	effet_parametrisation = TRUE;
		graf_mouse(BUSYBEE,0);
		
		selected_effet = find_effet(tempo_sto.id[effet_cur_param]);
		if(selected_effet==-1)
			return;
		
		selected_desc_param = effet[selected_effet]->effet_desc_param;
		copy_tran_param(&tempo_sto.effet_param[effet_cur_param],&tempo_param);

		set_ptra_prev_ass(ef_prev_val,0);
		old_calc_frame=-1;
		
		x_obj = objet[prj_obj[selected_prj_obj].objet].x;
		y_obj = objet[prj_obj[selected_prj_obj].objet].y;
		
		if(x_obj>y_obj)
		{	x_trev = TR_PREV_W;
			y_trev = (int)(((long)TR_PREV_H*y_obj)/x_obj);
		}else
		{	y_trev = TR_PREV_H;
			x_trev = (int)(((long)TR_PREV_W*x_obj)/y_obj);
		}
		x_tpremod16 = (x_trev+15)/16;
		x_tpremod16 *= 16;

		
		adr_obj = Malloc( (long)x_obj * (y_obj+1) * 2L);
		adr_ef_obj = Malloc( (long)x_tpremod16 * (y_trev+1) * 2L);
		if(adr_obj==0)
		{	super_alert(1,3,T_NO_MEMORY);
			if(adr_ef_obj) {	Mfree(adr_ef_obj); adr_ef_obj=0; }
		}else
		{	if(get_one_frame(prj_obj[selected_prj_obj].objet,prj_obj[selected_prj_obj].obj_in,adr_obj,x_obj) == 0)
			{	Mfree(adr_obj);
				if(adr_ef_obj) {	Mfree(adr_ef_obj); adr_ef_obj=0; }
			}else
			{	if(adr_ef_obj)
				{	zoom(adr_obj,adr_ef_obj,x_obj,y_obj,x_obj,x_trev,y_trev,x_tpremod16,1024);
					Mfree(adr_obj);
					adr_ef_prev = Malloc( (long)x_tpremod16 * (y_trev+1) * 2L);
					if(adr_ef_prev==0)
					{	Mfree(adr_ef_obj); adr_ef_obj=0; }
				}
			}	
		}

		/* on construit la MFDB */
		mfdb_efpre.fd_addr = adr_ef_prev;
		mfdb_efpre.fd_w = x_tpremod16;
		mfdb_efpre.fd_h = y_trev;
		mfdb_efpre.fd_wdwidth = x_tpremod16/16;
		mfdb_efpre.fd_stand = 0;
		mfdb_efpre.fd_nplanes = 16;
		/* init param */
		effet_para.img_src = adr_ef_obj;
		effet_para.img_dst = adr_ef_prev;
		effet_para.larg_img = x_tpremod16;
		effet_para.x = x_trev;
		effet_para.y = y_trev;
		effet_para.adr_tool_box = &tool_box;
		
		
		if(adr_ef_prev)
			calc_eff_prev(0);
				
		/* EFFACEMENT DES VARIABLES */
		for(i=TRSP_PT1;i<TRSP_PT1+3*10;i++)
			ptra_var.adr_form[i].ob_flags |= HIDETREE;
		/* EFFACEMENT DES COULEURS */
		ptra_var.adr_form[TRSP_CT1].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_CT2].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_C1].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_C2].ob_flags |= HIDETREE;
		/* EFFACEMENT DES SWITCHS */
		ptra_var.adr_form[TRSP_SW1].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_SW2].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_SW3].ob_flags |= HIDETREE;
		ptra_var.adr_form[TRSP_SW4].ob_flags |= HIDETREE;
		/* EFFACEMENT DES COORDS */
		for(i=0;i<2;i++)
		{	ptra_var.adr_form[TRSP_CDT1+i*4].ob_flags |= HIDETREE;
			ptra_var.adr_form[TRSP_CDX1+i*4].ob_flags |= HIDETREE;
			ptra_var.adr_form[TRSP_CDY1+i*4].ob_flags |= HIDETREE;
			ptra_var.adr_form[TRSP_CDC1+i*4].ob_flags |= HIDETREE;
		}
		
		/* CREATION DES VARIABLES */
		for(i=0;i<selected_desc_param->nb_variables;i++)
		{	for(j=0;j<8;j++)
				ptra_var.adr_form[TRSP_PT1+i*10+j].ob_flags &= ~HIDETREE;
			if(selected_desc_param->pict_compliant & (1<<i))
			{	ptra_var.adr_form[TRSP_SIMG1+i*10].ob_flags &= ~HIDETREE;
				ptra_var.adr_form[TRSP_IMG1+i*10].ob_flags &= ~HIDETREE;
				textuate_p(tempo_param.piste[i],str);
				sprintf(ptra_var.adr_form[TRSP_IMG1+i*10].ob_spec.tedinfo->te_ptext,T_PISTE,str);
			}
			strcpy(ptra_var.adr_form[TRSP_PT1+i*10].ob_spec.tedinfo->te_ptext,selected_desc_param->text_vari[i]);
			ptra_min_ass[i] = selected_desc_param->min_vari[i];
			ptra_max_ass[i] = selected_desc_param->max_vari[i];
			set_ptra_ass(i,tempo_param.vari_def[i],FALSE);
			ptra_var.adr_form[TRSP_PF1+i*10].ob_state = NORMAL;
			ptra_var.adr_form[TRSP_PTS1+i*10].ob_state = NORMAL;
			ptra_var.adr_form[TRSP_SIMG1+i*10].ob_state = NORMAL;
			if(tempo_param.mode_vari[i] == 0)
				ptra_var.adr_form[TRSP_PF1+i*10].ob_state = SELECTED;
			else if(tempo_param.mode_vari[i] == 1)
				ptra_var.adr_form[TRSP_PTS1+i*10].ob_state = SELECTED;
			else ptra_var.adr_form[TRSP_SIMG1+i*10].ob_state = SELECTED;
			copy_spline_name(tempo_param.time[i], ptra_var.adr_form[TRSP_TSPL1+i*10].ob_spec.tedinfo->te_ptext);
		}
		/* LES SWITCHS */
		for(i=0;i<selected_desc_param->nb_switchs;i++)
		{	ptra_var.adr_form[TRSP_SW1+i].ob_flags &= ~HIDETREE;
			strcpy((char *)ptra_var.adr_form[TRSP_SW1+i].ob_spec.userblk->ub_parm,selected_desc_param->text_switchs[i]);
			ptra_var.adr_form[TRSP_SW1+i].ob_state = NORMAL;
			if((tempo_param.switchs>>i)&1)
			ptra_var.adr_form[TRSP_SW1+i].ob_state = SELECTED;
		}
		for(i=0;i<selected_desc_param->nb_color;i++)
		{	ptra_var.adr_form[TRSP_CT1+i*2].ob_flags &= ~HIDETREE;
			ptra_var.adr_form[TRSP_C1+i*2].ob_flags &= ~HIDETREE;
			strcpy(ptra_var.adr_form[TRSP_CT1+i*2].ob_spec.tedinfo->te_ptext,selected_desc_param->text_color[i]);
		}
		for(i=0;i<selected_desc_param->nb_coords;i++)
		{	ptra_var.adr_form[TRSP_CDT1+i*4].ob_flags &= ~HIDETREE;
			ptra_var.adr_form[TRSP_CDX1+i*4].ob_flags &= ~HIDETREE;
			ptra_var.adr_form[TRSP_CDY1+i*4].ob_flags &= ~HIDETREE;
			ptra_var.adr_form[TRSP_CDC1+i*4].ob_flags &= ~HIDETREE;
			strcpy(ptra_var.adr_form[TRSP_CDT1+i*4].ob_spec.tedinfo->te_ptext,selected_desc_param->text_coords[i]);
			sprintf(ptra_var.adr_form[TRSP_CDX1+i*4].ob_spec.tedinfo->te_ptext,"X: %d %s",tempo_param.coords[i][0]/10,"%");
			sprintf(ptra_var.adr_form[TRSP_CDY1+i*4].ob_spec.tedinfo->te_ptext,"Y: %d %s",tempo_param.coords[i][1]/10,"%");
		}
		graf_mouse(ARROW,0);
		open_dialog(ptr_var, effet[selected_effet]->nom, 0, -1);
	}

	else 
	{	prev_treel = ptra_var.adr_form[TRSP_TREEL].ob_state == SELECTED;
		choix = windial_do(ptr_var, event);
	 	if(choix == CLOSE_DIAL)
	 		choix = TRSP_ANN;
		if (choix != 0) 
		{	switch(choix)
			{	
				case TRSP_SW1:
				case TRSP_SW2:
				case TRSP_SW3:
				case TRSP_SW4:
					if(prev_treel)
					{	scan_eff_param();
						calc_eff_prev(1);
					}
					break;
				case TRSP_ASS1:
				case TRSP_ASS1+10:
				case TRSP_ASS1+20:
					i = (choix-TRSP_MN1)/10;
					ptra_ass_vari(i);
					break;
				case TRSP_PL1:
				case TRSP_PL1+10:
				case TRSP_PL1+20:
					i = (choix-TRSP_MN1)/10;
					if(tempo_param.vari_def[i] < ptra_max_ass[i])
						tempo_param.vari_def[i]++;
					set_ptra_ass(i,tempo_param.vari_def[i],TRUE);
						break;
				case TRSP_MN1:
				case TRSP_MN1+10:
				case TRSP_MN1+20:
					i = (choix-TRSP_MN1)/10;
					if(tempo_param.vari_def[i] > ptra_min_ass[i])
						tempo_param.vari_def[i]--;
					set_ptra_ass(i,tempo_param.vari_def[i],TRUE);
						break;
				case TRSP_PF1:
				case TRSP_PF1+10:
				case TRSP_PF1+20:
					i = (choix-TRSP_PF1)/10;
					wf_change(ptr_var,TRSP_PF1+i*10, SELECTED, 1);
					wf_change(ptr_var,TRSP_PTS1+i*10, NORMAL, 1);
					wf_change(ptr_var,TRSP_SIMG1+i*10, NORMAL, 1);
					break;
				case TRSP_PTS1:
				case TRSP_PTS1+10:
				case TRSP_PTS1+20:
					i = (choix-TRSP_PF1)/10;
					wf_change(ptr_var,TRSP_PF1+i*10, NORMAL, 1);
					wf_change(ptr_var,TRSP_PTS1+i*10, SELECTED, 1);
					wf_change(ptr_var,TRSP_SIMG1+i*10, NORMAL, 1);
					break;
				case TRSP_SIMG1:
				case TRSP_SIMG1+10:
				case TRSP_SIMG1+20:
					i = (choix-TRSP_PF1)/10;
					wf_change(ptr_var,TRSP_PF1+i*10, NORMAL, 1);
					wf_change(ptr_var,TRSP_PTS1+i*10, NORMAL, 1);
					wf_change(ptr_var,TRSP_SIMG1+i*10, SELECTED, 1);
					break;
				case TRSP_TSPL1:
				case TRSP_TSPL1+10:
				case TRSP_TSPL1+20:
					i = (choix-TRSP_TSPL1)/10;
					/* initialisation des annotation par defaut */
					etim_nb_anno=0;
					get_anno(effet[selected_effet]->id,i);
					
					retour_index = &tempo_param.time[i];
					retour_wform = ptr_var;
					retour_objet = choix;
					form_ctim(OPEN_DIAL);
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSP_IMG1:
				case TRSP_IMG1+10:
				case TRSP_IMG1+20:
					i = (choix-TRSP_IMG1)/10;
					choose_para_img(i,&tempo_param);
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSP_CDC1:
				case TRSP_CDC1+4:
					i = (TRSP_CDC1+4==choix);	/* 0>la prem 1>second */
					get_coords(i,&tempo_param);
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSP_C1:
					coul_caller=3;
					temp_r=tempo_param.color[0][0]; temp_v=tempo_param.color[0][1]; temp_b=tempo_param.color[0][2];
					form_col(OPEN_DIAL);
					break;
				case TRSP_C2:
					coul_caller=4;
					temp_r=tempo_param.color[1][0]; temp_v=tempo_param.color[1][1]; temp_b=tempo_param.color[1][2];
					form_col(OPEN_DIAL);
					break;
				case TRSP_ANIM:
					anim_ef_prev();
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSP_PREV_ASS:
					ass_prev();
					break;
				case TRSP_PREV:
					graf_mouse(BUSYBEE,0);
					scan_eff_param();
					calc_eff_prev(1);
					graf_mouse(ARROW,0);
					break;
				case TRSP_OK:
					if(adr_ef_prev) {	Mfree(adr_ef_prev); adr_ef_prev=0; }
					if(adr_ef_obj)  {	Mfree(adr_ef_obj);  adr_ef_obj=0;  }
					scan_eff_param();
					copy_tran_param(&tempo_param, &tempo_sto.effet_param[effet_cur_param]);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
				case TRSP_ANN:
					if(adr_ef_prev) {	Mfree(adr_ef_prev); adr_ef_prev=0; }
					if(adr_ef_obj)  {	Mfree(adr_ef_obj);  adr_ef_obj=0;  }
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;

			}
		}
	}
}

static void get_anno(long id, int num_param)
{	
	etim_nb_anno=0;
   if(id=='ROTS')
   {	if(num_param == 0)
   	{	etim_nb_anno=3;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=128;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"-360");
   		strcpy(etim_text_anno[1],"0");
   		strcpy(etim_text_anno[2],"360");
   	}else
   	{	etim_nb_anno=3;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=51;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0%");
   		strcpy(etim_text_anno[1],"100%");
   		strcpy(etim_text_anno[2],"500%");
		}
	}else if(id=='SPIR')
   {	etim_nb_anno=3;
   	etim_pos_anno[0]=0;
   	etim_pos_anno[1]=128;
   	etim_pos_anno[2]=255;
   	strcpy(etim_text_anno[0],"-360");
   	strcpy(etim_text_anno[1],"0");
   	strcpy(etim_text_anno[2],"360");
   }else if(id=='RVBC')
   {	etim_nb_anno=3;
   	etim_pos_anno[0]=0;
   	etim_pos_anno[1]=128;
   	etim_pos_anno[2]=255;
   	strcpy(etim_text_anno[0],"0");
   	strcpy(etim_text_anno[1],"512");
   	strcpy(etim_text_anno[2],"1024");
   }else if(id=='FMVT')
   {	etim_nb_anno=3;
   	etim_pos_anno[0]=0;
   	etim_pos_anno[1]=128;
   	etim_pos_anno[2]=255;
   	strcpy(etim_text_anno[0],"0");
   	strcpy(etim_text_anno[1],"512");
   	strcpy(etim_text_anno[2],"1024");
   }else if(id=='SNTB')
   {	etim_nb_anno=3;
   	etim_pos_anno[0]=0;
   	etim_pos_anno[1]=128;
   	etim_pos_anno[2]=255;
 		strcpy(etim_text_anno[0],"0%");
  		strcpy(etim_text_anno[1],"100%");
  		strcpy(etim_text_anno[2],"500%");
   }
}
