/********************MOUTAIN********************/
/*****          GESTION DE L'EXPORT         ****/
/*****             Version 0.9              ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "select.h"
#include "some_fnc.h"
#include "modules.h"
#include "export.h"
#include "mountain.h"
#include "preview.h"
#include "projet.h"
#include "transit.h"
#include "geffets.h"
#include "pistes.h"
#include "effets1.h"
#include "txts.h"
#include "sav_mod\sav_mod.h"


int cdecl draw_calc_preview(PARMBLK *parmblock);
void set_bar(long val, long tot, WINDFORM_VAR *ptr_var, int fon, int bar);
void calc_for_mpl(void);
void MPL_calc(void);
void Save_stack(void);
long Put_stack(void);
int get_popup(OBJECT *adr_form, int button, int index);
static void updt_param(void);
void export_2_mplayer(void);

int intel(int mot);

extern EXP_MOD exp_mod[10];
extern int nb_exp_mod;
extern int actu_projet;			/* is there any projet here ? */
extern PROJET projet;
extern int preview_pos;			/* frame de preview */
extern WINDFORM_VAR exp_var;		/* boite d'export */
extern WINDFORM_VAR mpl_var;		/* boite d'export */
extern char chemin_key[150];
extern char chemin_img_externe[150];

EXP_INIT_PARAM exp_init_param;
EXP_IMG_PARAM exp_img_param;
USERBLK user_calc_pre;
int *calc_pre;
int calc_pre_x;
int calc_pre_x_mod16;
int calc_pre_y;
MFDB calc_mfdb;

CHEMIN fic_exp={"","",""};
CHEMIN fic_son={"","",""};
char nom_anim[14]="";
int num_mod=0,mode_end,no_draw;
long start,end;
int key_choose=3;
int key_tab[10]={0,2,4,8,16,25};
char chemin_mplayer[200]="E:\\IMAGES\\M_PLAYER\\M_PLAYER.PRG";
char actu_img;
int compt_bar=0, *img[2];
long nb_img;
int mod_mpl;
long mountain_stack,mpl_stack;			/* la pile */

void (*ext_mpl_export)(EXT_MPL_EXP *ext_mpl_exp)=0;
void (*ext_mod_export)(EXT_EXP_MOD *ext_exp_mod)=0;


MPL_PARA mpl_para;

void form_exp(int event)
{
	WINDFORM_VAR *ptr_var = &exp_var;
	int choix,bout;
	char pth[150]="";
	
	if (event == OPEN_DIAL)
	{	if(!nb_exp_mod)
		{	super_alert(1,3,T_ALR_NO_LOADED_MOD);
			return;
		}
		mod_mpl=FALSE;
		mode_end=FALSE;
		
		strcpy(fic_exp.complet,projet.chemin);
		if(projet.chemin[0])
			find_name(projet.chemin,nom_anim);
		
		strcpy(ptr_var->adr_form[CALC_GO].ob_spec.free_string,T_LANC_CALC);
		/* Preparation de la preview */
		
		user_calc_pre.ub_code = draw_calc_preview;
		exp_var.adr_form[CALC_PREV].ob_type = G_USERDEF;
		user_calc_pre.ub_parm = (long) exp_var.adr_form[CALC_PREV].ob_spec.userblk;
		exp_var.adr_form[CALC_PREV].ob_spec.userblk = &user_calc_pre;

		
		if(projet.x>projet.y)
		{	calc_pre_x = EXP_PRE_W;
			calc_pre_y = (int)(((long)EXP_PRE_H*projet.y)/projet.x);
		}else
		{	calc_pre_y = EXP_PRE_H;
			calc_pre_x = (int)(((long)EXP_PRE_H*projet.x)/projet.y);
		}
		calc_pre_x_mod16	= (calc_pre_x+15)/16;
		calc_pre_x_mod16 *= 16;
	
		calc_pre = Malloc( (long)calc_pre_x_mod16 * (calc_pre_y+1) * 2L);
	
		/* on construit la MFDB */
		calc_mfdb.fd_addr = calc_pre;
		calc_mfdb.fd_w = calc_pre_x_mod16;
		calc_mfdb.fd_h = calc_pre_y;
		calc_mfdb.fd_wdwidth = calc_pre_x_mod16/16;
		calc_mfdb.fd_stand = 0;
		calc_mfdb.fd_nplanes = 16;

		if(calc_pre)
			fill(calc_pre,calc_pre_x,calc_pre_y,calc_pre_x_mod16,0xd69a);

		no_draw=TRUE;		/* pour n'afficher la prev k pdt le calc */
		
		exp_var.adr_form[CALC_BAR].ob_width = 1;
		itoa(projet.preview_in,ptr_var->adr_form[CALC_DE].ob_spec.tedinfo->te_ptext,10);
		itoa(projet.preview_out,ptr_var->adr_form[CALC_A].ob_spec.tedinfo->te_ptext,10);
		sprintf(ptr_var->adr_form[CALC_NB].ob_spec.free_string,T_LD_IMG,projet.lenght);
		strcpy(ptr_var->adr_form[CALC_FIC].ob_spec.tedinfo->te_ptext, fic_exp.complet);
		
		num_mod = find_exp_mod(projet.id_exp_mod);
		if(num_mod == -1)
		{	num_mod=0;
			if(exp_mod[num_mod].flags & ME_PARAM)		/* prepare les prametres */
			{	if(exp_mod[num_mod].size_buf_para <=4)
					(exp_mod[num_mod].mod_param_def)(&(long)projet.adr_param);
				else
				{	projet.adr_param = Malloc(exp_mod[num_mod].size_buf_para);
					(exp_mod[num_mod].mod_param_def)(projet.adr_param);
				}
				projet.size_param = exp_mod[num_mod].size_buf_para;
				projet.id_exp_mod = exp_mod[num_mod].id;
			}
		}
			
		
		strcpy(ptr_var->adr_form[CALC_EXP].ob_spec.free_string, exp_mod[num_mod].description);
		if(exp_mod[num_mod].flags & ME_PARAM)
			wf_change(ptr_var,CALC_CONF,-1,1);
		else
			wf_change(ptr_var,CALC_CONF,DISABLED,1);
		open_dialog(ptr_var, T_TITRE_EXP_ANI, CALC_DE, 1);	
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
			{	close_dialog(ptr_var);
				if(calc_pre)
					Mfree(calc_pre);
				calc_pre=0;
			}

			switch(choix)
			{	
				case CALC_FIC:
					Fselecteur(&fic_exp,pth,nom_anim,&bout,T_NOM_SANS_EXT);
					if(bout)
					{	if(nom_anim[0]==0)
							ptr_var->adr_form[CALC_FIC].ob_spec.tedinfo->te_ptext[0]=0;
						else
						{	vire_ext(fic_exp.complet);
							vire_ext(nom_anim);
							strcpy(ptr_var->adr_form[CALC_FIC].ob_spec.tedinfo->te_ptext, fic_exp.complet);
							strcpy(projet.chemin, fic_exp.complet);
						}
						Dsetdrv(fic_exp.chemin[0] - 'A');
						Dsetpath(fic_exp.chemin);
					}
					wf_change(ptr_var,choix,0,1);					
					break;

				case CALC_CONF:
					updt_param();
					if(projet.size_param > 4)
						(exp_mod[num_mod].mod_param)(projet.adr_param);
					else
						(exp_mod[num_mod].mod_param)(&(long)projet.adr_param);
					wf_change(ptr_var,choix,0,1);					
					break;

				case CALC_EXP:
					num_mod = super_popup(ptr_var, CALC_EXP, CHOIX,num_mod+1,(char*)(exp_mod)+56,(int)sizeof(EXP_MOD),nb_exp_mod) - 1;
					if(exp_mod[num_mod].flags & ME_PARAM)
						wf_change(ptr_var,CALC_CONF,-1,1);
					else
						wf_change(ptr_var,CALC_CONF,DISABLED,1);
						
					wf_change(ptr_var,choix,0,1);					
					break;
				case CALC_GO:
					if(mode_end)
					{	close_dialog(ptr_var);
						if(calc_pre)
							Mfree(calc_pre);
						calc_pre=0;
					}else
					{	updt_param();
						if(ptr_var->adr_form[CALC_TOUT].ob_state & SELECTED)
						{	start = 0;
							end = projet.lenght-1;
						}else
						{	start = max(atoi(ptr_var->adr_form[CALC_DE].ob_spec.tedinfo->te_ptext),0);
							end = min(atoi(ptr_var->adr_form[CALC_A].ob_spec.tedinfo->te_ptext),projet.lenght-1);
						}
						if(ext_mod_export)
							export_anim();
						else
							super_alert(1,3,T_REGISTER_ONLY);
						
						strcpy(ptr_var->adr_form[CALC_GO].ob_spec.free_string,T_SORTIR);
						mode_end=TRUE;
						Cconout(7);	/* pong ! */
					}
					wf_change(ptr_var,choix,0,1);					
					break;
			}
		}
	}
}

static void updt_param(void)
{	/* gestion des paramtres */
	if(projet.id_exp_mod != exp_mod[num_mod].id)
	{	if(projet.size_param >4)
			Mfree(projet.adr_param);
		if(exp_mod[num_mod].size_buf_para <=4)
			(exp_mod[num_mod].mod_param_def)(&(long)projet.adr_param);
		else
		{	projet.adr_param = Malloc(exp_mod[num_mod].size_buf_para);
			(exp_mod[num_mod].mod_param_def)(projet.adr_param);
		}
		projet.size_param = exp_mod[num_mod].size_buf_para;
		projet.id_exp_mod = exp_mod[num_mod].id;
	}
}

int find_exp_mod(long id)
{	int i;
	for(i=0;i<nb_exp_mod;i++)
	{	if(exp_mod[i].id == id)
			return i;
	}
	return -1;
}

void form_mpl(int event)
{
	WINDFORM_VAR *ptr_var = &mpl_var;
	int choix,bout;
	char pth[150]="",name[14];
	
	if (event == OPEN_DIAL)
	{	mode_end=FALSE;
		mod_mpl=TRUE;

		/* ? */
		fic_son.complet[0]=0;
		
		strcpy(ptr_var->adr_form[MPL_GO].ob_spec.free_string,T_LANC_CALC);
		if(fic_son.complet[0])
			find_name_ext(fic_son.complet,name);
		else name[0]=0;
		
		strcpy(ptr_var->adr_form[MPL_SON].ob_spec.tedinfo->te_ptext,name);
		/* Preparation de la preview */
		
		user_calc_pre.ub_code = draw_calc_preview;
		mpl_var.adr_form[MPL_PREV].ob_type = G_USERDEF;
		user_calc_pre.ub_parm = (long) mpl_var.adr_form[MPL_PREV].ob_spec.userblk;
		mpl_var.adr_form[MPL_PREV].ob_spec.userblk = &user_calc_pre;

		
		if(projet.x>projet.y)
		{	calc_pre_x = EXP_PRE_W;
			calc_pre_y = (int)(((long)EXP_PRE_H*projet.y)/projet.x);
		}else
		{	calc_pre_y = EXP_PRE_H;
			calc_pre_x = (int)(((long)EXP_PRE_H*projet.x)/projet.y);
		}
		calc_pre_x_mod16	= (calc_pre_x+15)/16;
		calc_pre_x_mod16 *= 16;
	
		calc_pre = Malloc( (long)calc_pre_x_mod16 * (calc_pre_y+1) * 2L);
	
		/* on construit la MFDB */
		calc_mfdb.fd_addr = calc_pre;
		calc_mfdb.fd_w = calc_pre_x_mod16;
		calc_mfdb.fd_h = calc_pre_y;
		calc_mfdb.fd_wdwidth = calc_pre_x_mod16/16;
		calc_mfdb.fd_stand = 0;
		calc_mfdb.fd_nplanes = 16;

		if(calc_pre)
			fill(calc_pre,calc_pre_x,calc_pre_y,calc_pre_x_mod16,0xd69a);

		no_draw=TRUE;		/* pour n'afficher la prev k pdt le calc */
		
		mpl_var.adr_form[MPL_BAR].ob_width = 1;
		itoa(projet.preview_in,ptr_var->adr_form[MPL_DE].ob_spec.tedinfo->te_ptext,10);
		itoa(projet.preview_out,ptr_var->adr_form[MPL_A].ob_spec.tedinfo->te_ptext,10);
		sprintf(ptr_var->adr_form[MPL_NB].ob_spec.free_string,T_LD_IMG,projet.lenght);
		strcpy(ptr_var->adr_form[MPL_FIC].ob_spec.tedinfo->te_ptext, fic_exp.complet);
		strncpy(ptr_var->adr_form[MPL_MPL].ob_spec.tedinfo->te_ptext, chemin_mplayer,40);
		open_dialog(ptr_var, T_TITR_EXP_MPL, MPL_DE, 1);	
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if (choix == CLOSE_DIAL)
			{	close_dialog(ptr_var);
				if(calc_pre)
					Mfree(calc_pre);
				calc_pre=0;
			}

			switch(choix)
			{	
				case MPL_FIC:
					Fselecteur(&fic_exp,pth,nom_anim,&bout,"Nom sans extension");
					if(bout)
					{	if(nom_anim[0]==0)
							ptr_var->adr_form[MPL_FIC].ob_spec.tedinfo->te_ptext[0]=0;
						else
						{	vire_ext(fic_exp.complet);
							vire_ext(nom_anim);
							strcpy(ptr_var->adr_form[MPL_FIC].ob_spec.tedinfo->te_ptext, fic_exp.complet);
							if(get_popup(mpl_var.adr_form,MPL_FORMAT,POP_FORMAT) == 3)
								strcat(ptr_var->adr_form[MPL_FIC].ob_spec.tedinfo->te_ptext, ".AVI");
							else
								strcat(ptr_var->adr_form[MPL_FIC].ob_spec.tedinfo->te_ptext, ".MOV");
						}
						Dsetdrv(fic_exp.chemin[0] - 'A');
						Dsetpath(fic_exp.chemin);
					}
					wf_change(ptr_var,choix,0,1);					
					break;
				case MPL_SON:
					pth[0]=0;
					Fselecteur(&fic_son,pth,name,&bout,T_SONO_FILE);
					if(bout)
					{	if(name[0]==0)
							ptr_var->adr_form[MPL_SON].ob_spec.tedinfo->te_ptext[0]=0;
						else
						{	strcpy(ptr_var->adr_form[MPL_SON].ob_spec.tedinfo->te_ptext, name);
						}
						Dsetdrv(fic_exp.chemin[0] - 'A');
						Dsetpath(fic_exp.chemin);
					}
					wf_change(ptr_var,choix,0,1);					
					break;
				case MPL_FORMAT:
					pop_up(ptr_var,MPL_FORMAT,POP_FORMAT);
					break;
				case MPL_POP_Q:
					pop_up(ptr_var,MPL_POP_Q,POP_QUALITE);
					break;
				case MPL_KEY:
					key_choose = pop_up(ptr_var,MPL_KEY,POP_MPL_KEY)-1;
					break;
				case MPL_GO:
					if(mode_end)
					{	close_dialog(ptr_var);
						if(calc_pre)
							Mfree(calc_pre);
						calc_pre=0;
					}else
					{	if(ptr_var->adr_form[MPL_TOUT].ob_state & SELECTED)
						{	start = 0;
							end = projet.lenght-1;
						}else
						{	start = max(atoi(ptr_var->adr_form[MPL_DE].ob_spec.tedinfo->te_ptext),0);
							end = min(atoi(ptr_var->adr_form[MPL_A].ob_spec.tedinfo->te_ptext),projet.lenght-1);
						}
						if(ext_mpl_export)
							export_2_mplayer();
						else
							super_alert(1,3,T_REGISTER_ONLY);
						strcpy(ptr_var->adr_form[MPL_GO].ob_spec.free_string,T_SORTIR);
						mode_end=TRUE;
						Cconout(7);	/* pong ! */
					}
					wf_change(ptr_var,choix,0,1);					
					break;
			}
		}
	}
}

void export_2_mplayer(void)
{	EXT_MPL_EXP ext_mpl_exp;
	int format,qualite;
	/* inits plus simples a mettre ici */
	actu_img=0;
	compt_bar=0;
	nb_img=end-start+1;
	mpl_para.rout_mpl = MPL_calc;

	qualite = get_popup(mpl_var.adr_form,MPL_POP_Q,POP_QUALITE);
	format = get_popup(mpl_var.adr_form,MPL_FORMAT,POP_FORMAT);

	img[0]=Malloc((long)projet.x*projet.y*2L+20);
	if(img[0]==0)
		return ;
	img[1]=Malloc((long)projet.x*projet.y*2L+20);	/* 0x2b */
	if(img[1]==0)
	{	Mfree(img[0]);
		return ;
	}

	no_draw=FALSE;		/* pour n'afficher la prev k pdt le calc */

	ext_mpl_exp.nom_fic=fic_exp.complet;
	ext_mpl_exp.freq_key=key_tab[key_choose];
	ext_mpl_exp.nb_img=nb_img;
	ext_mpl_exp.qualite=qualite;
	ext_mpl_exp.format=format;
	ext_mpl_exp.img[0]=img[0];
	ext_mpl_exp.img[1]=img[1];
	ext_mpl_exp.mpl_para=&mpl_para;
	ext_mpl_exp.projet=&projet;
	ext_mpl_exp.chemin_key=chemin_key;
	ext_mpl_exp.chemin_mplayer=chemin_mplayer;
	ext_mpl_exp.Save_stack = Save_stack;
	ext_mpl_exp.super_alert = super_alert;
	ext_mpl_exp.find_path = find_path;
	if(mpl_var.adr_form[MPL_SON].ob_spec.tedinfo->te_ptext[0])
		ext_mpl_exp.fic_son = fic_son.complet;
	else
		ext_mpl_exp.fic_son = 0;
		
	(ext_mpl_export)(&ext_mpl_exp);
	Mfree(img[0]);	Mfree(img[1]);
	end_preview();
}
	


void calc_for_mpl(void)
{	int draw_preview = mpl_var.adr_form[MPL_AFF].ob_state & SELECTED;
	int x = projet.x;
	int y = projet.y;


	mpl_para.ptr_img = img[actu_img];

	if(calc_frame((int)mpl_para.num_frame, img[actu_img], x, y, x) == -1)
	{	Cconout(7);		Cconout(7);		Cconout(7);		Cconout(7);		Cconout(7);		Cconout(7);	
		super_alert(1,3,T_ERR_HAPEN);
		goto err_sav_m;
	}

	actu_img = 1-actu_img;
	
	if(draw_preview)
	{	zoom(mpl_para.ptr_img,calc_pre,x,y,x,calc_pre_x,calc_pre_y,calc_pre_x_mod16,1024);
		objc_draw(mpl_var.adr_form,MPL_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	}
	set_bar(++compt_bar,nb_img,&mpl_var,MPL_FON,MPL_BAR);
	err_sav_m:;
}
	
void set_bar(long val, long tot, WINDFORM_VAR *ptr_var, int fon, int bar)
{	int x;
	
	x = (int)( (val*(ptr_var->adr_form[fon].ob_width-2)) / tot);
	ptr_var->adr_form[bar].ob_width = x;
	wf_draw(ptr_var,bar);
}
	
int export_anim(void)
{	int x,y, *img[2],dummy,key;
	char chemin[200];
	long nb_img=end-start+1,frame;
	void *work_adr,*buf,*adr_param;
	int draw_preview = exp_var.adr_form[CALC_AFF].ob_state & SELECTED;
	EXT_EXP_MOD ext_exp_mod;
	
	if(!actu_projet || nb_exp_mod==0)
		return 0;

	x=projet.x;
	y=projet.y;
	
	img[0]=Malloc((long)x*y*2L);
	if(img[0]==0)
		return 0;
	img[1]=Malloc((long)x*y*2L);
	if(img[1]==0)
	{	Mfree(img[0]);
		return 0;
	}

	if(exp_mod[num_mod].size_buf_perm)
	{	work_adr=Malloc(exp_mod[num_mod].size_buf_perm);
		if(work_adr==0)
		{	Mfree(img[0]);	Mfree(img[1]);
			return 0;
		}
	}

	if(projet.size_param <= 4)
		adr_param = &(long)projet.adr_param;
	else
		adr_param = projet.adr_param;

	ext_exp_mod.nb_img=nb_img;
	ext_exp_mod.img[0]=img[0];
	ext_exp_mod.img[1]=img[1];
	ext_exp_mod.projet=&projet;
	ext_exp_mod.chemin=chemin;
	ext_exp_mod.find_path = find_path;
	ext_exp_mod.nom_anim = nom_anim;
	ext_exp_mod.adr_param = adr_param;
	ext_exp_mod.work_adr = work_adr;
	ext_exp_mod.exp_init_param = &exp_init_param;

	(ext_mod_export)(&ext_exp_mod);
	
	
	actu_img=0;
	compt_bar=0;
	
	no_draw=FALSE;		/* pour n'afficher la prev k pdt le calc */


	/* INITIALISATION */
	if(nom_anim[0])
	{	(exp_mod[num_mod].init_exp)(&exp_init_param);	
	
		if(exp_init_param.retour == -1)
		{	Mfree(img[0]);	Mfree(img[1]);
			Mfree(work_adr);
			return 0;
		}
	}

	/* init ok */
	if(exp_init_param.size_buf)
	{	buf=Malloc(exp_init_param.size_buf);
		if(buf==0)
		{	Mfree(img[0]);	Mfree(img[1]);
			Mfree(work_adr);
			return 0;
		}
	}
	
	form_mem(UPDT_MEM);
	
	exp_img_param.buf=buf;
	exp_img_param.work_adr=work_adr;
	exp_img_param.param_adr=adr_param;
	
	for(frame=start;frame<=end;frame++)
	{	set_bar(++compt_bar,nb_img*2, &exp_var, CALC_FON, CALC_BAR);
		
		exp_img_param.img=img[actu_img];
		exp_img_param.old_img=img[1-actu_img];
	
		exp_img_param.frame=frame-start;
	
		if(calc_frame((int)frame, img[actu_img], x, y, x) == -1)
		{	super_alert(1,3,T_ERR_HAPEN);
			goto err_sav;
		}

		/* Preview */
		if(draw_preview)
		{	zoom(img[actu_img],calc_pre,x,y,x,calc_pre_x,calc_pre_y,calc_pre_x_mod16,1024);
			objc_draw(exp_var.adr_form,CALC_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		}
		set_bar(++compt_bar,nb_img*2,&exp_var, CALC_FON, CALC_BAR);
	
		if(nom_anim[0])
		{	(exp_mod[num_mod].put_frame)(&exp_img_param);
			if(exp_img_param.retour == -1)
			{	super_alert(1,3,T_ERR_SAV);
				goto err_sav;
			}
		}
		actu_img = 1-actu_img;
		
		graf_mkstate(&dummy, &dummy, &dummy,&key);
		if(key == (K_LSHIFT|K_RSHIFT))
			goto err_sav;
		
	}


	err_sav:;
	Mfree(img[0]);	Mfree(img[1]);
	Mfree(work_adr);
	Mfree(buf);

	end_preview();
	
	return 1;
}

int cdecl draw_calc_preview(PARMBLK *parmblock)
{	GRECT area,real,img;
	int base_x,base_y;
	int pxy[8];

	vswr_mode(vdi_handle,MD_REPLACE);
	
	if(mod_mpl)
	{	base_x = mpl_var.adr_form[MPL_PREV].ob_x + mpl_var.adr_form[0].ob_x;
		base_y = mpl_var.adr_form[MPL_PREV].ob_y + mpl_var.adr_form[0].ob_y;
	}else
	{	base_x = exp_var.adr_form[CALC_PREV].ob_x + exp_var.adr_form[0].ob_x;
		base_y = exp_var.adr_form[CALC_PREV].ob_y + exp_var.adr_form[0].ob_y;
	}
	
	real.g_x = base_x-1;
	real.g_y = base_y-1;
	real.g_w = calc_pre_x+2;
	real.g_h = calc_pre_y+2;
	img.g_x = base_x;
	img.g_y = base_y;
	img.g_w = calc_pre_x;
	img.g_h = calc_pre_y;
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;

	rc_intersect(&real,&area);
	set_clip(1, &area);

	rect_3d(base_x-1,base_y-1,calc_pre_x+2,calc_pre_y+2);
		
	if(calc_pre && !no_draw)
	{	
		/* Zone source dans l'image en m‚moire : */
		pxy[0] = img.g_x - base_x;	
		pxy[1] = img.g_y - base_y;	
		pxy[2] = pxy[0] + img.g_w - 1;	
		pxy[3] = pxy[1] + img.g_h - 1;	
		/* Zone destination dans la fenˆtre … l'‚cran */
		pxy[4] = img.g_x;							
		pxy[5] = img.g_y;							
		pxy[6] = pxy[4] + img.g_w - 1;	
		pxy[7] = pxy[5] + img.g_h - 1;	
		tnt_cpyfm(pxy, &calc_mfdb);
		
	}else
		rect_full(base_x,base_y,calc_pre_x,calc_pre_y, LWHITE);
		
	set_clip(0, &area);
	return 0;
	
}