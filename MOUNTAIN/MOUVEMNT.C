/*******************MOUNTAIN*********************/
/*****        GESTION DES MOUVEMENTS        *****/
/*****            Par Vanel R‚mi            *****/
/*****         Premier code : 11/2/98       *****/
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "select.h"
#include "mountain.h"
#include "objets.h"
#include "visu.h"
#include "gest_flh.h"
#include "anim.h"
#include "transit.h"
#include "geffets.h"
#include "projet.h"
#include "pistes.h"
#include "preview.h"
#include "some_fnc.h"
#include "mouvemnt.h"
#include "effets1.h"
#include "time_spl.h"
#include "txts.h"


void wait_mk(void);
int pop_fly(int index);

extern PROJET projet;
extern int actu_projet;			/* is there any projet here ? */
extern int ultra_temps_reel;

/* objets */
extern int nb_objets;
extern OBJET objet[MAX_OBJET];

/* prj objet */
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern int deb_projet;
extern int selected_prj_obj;

int actu_mvt;
int cur_edit;
int cur_pt_edit;
int first_pt_mvt;
int were_no_mvt;
int num_obj_img;
int deb_exp_zoom;
int deb_m_pos_exp,deb_p_pos_exp;
int temps_reel=1;
int imposed_time;
int *adr_preview, *adr_use_obj, x_obj, y_obj, num_obj, x_prev, y_prev, x_premod16;
MFDB mfdb_pre;
int *adr_pic_coul;
char titre_wind[30];

MVT mvt[MAX_MVT];
PT_MVT pt_mvt[MAX_PT_MVT];

USERBLK user_mvt;
USERBLK user_pre;
USERBLK user_pre_c;
USERBLK user_pic_c;
USERBLK user_time;

extern WINDFORM_VAR mou_var;		/* mouvement */

/********* FONCTIONS PRIVEES *********/
static void get_color_picked(void);

void init_mvt(void)
{	int i;
	
	for(i=0;i<MAX_PT_MVT;i++)
	{	clear_pt_mvt(i);
	}
	
	for(i=0;i<MAX_MVT;i++)
	{	mvt[i].used=0;
		mvt[i].nb_mvt_used=1;
		mvt[i].img_used=0;
		mvt[i].transparent=1;
		mvt[i].r=0;
		mvt[i].v=0;
		mvt[i].b=0;
	}
	
	cur_edit = 0;

	deb_exp_zoom = (int)( (long)( 2000L*((long)mou_var.adr_form[MOU_FON_X].ob_width - mou_var.adr_form[MOU_ASS_X].ob_width)) / (long)(MAX_ZOOM));
	deb_m_pos_exp = (int)( (long)( (-1000+MAX_POS)*((long)mou_var.adr_form[MOU_FON_PX].ob_width - mou_var.adr_form[MOU_ASS_PX].ob_width)) / (long)(2*MAX_POS));
	deb_p_pos_exp = (int)( (long)( (1000+MAX_POS)*((long)mou_var.adr_form[MOU_FON_PX].ob_width - mou_var.adr_form[MOU_ASS_PX].ob_width)) / (long)(2*MAX_POS));

	user_mvt.ub_code = draw_mvt;
	mou_var.adr_form[MOU_MOU].ob_type = G_USERDEF;
	user_mvt.ub_parm = (long) mou_var.adr_form[MOU_MOU].ob_spec.userblk;
	mou_var.adr_form[MOU_MOU].ob_spec.userblk = &user_mvt;
	
	user_pre.ub_code = draw_preview;
	mou_var.adr_form[MOU_PRE].ob_type = G_USERDEF;
	user_pre.ub_parm = (long) mou_var.adr_form[MOU_PRE].ob_spec.userblk;
	mou_var.adr_form[MOU_PRE].ob_spec.userblk = &user_pre;

	user_pre_c.ub_code = preview_color;
	mou_var.adr_form[MOU_PRE_COUL].ob_type = G_USERDEF;
	user_pre_c.ub_parm = (long) mou_var.adr_form[MOU_PRE_COUL].ob_spec.userblk;
	mou_var.adr_form[MOU_PRE_COUL].ob_spec.userblk = &user_pre_c;

	user_pic_c.ub_code = pic_coul;
	mou_var.adr_form[MOU_PICK].ob_type = G_USERDEF;
	user_pic_c.ub_parm = (long) mou_var.adr_form[MOU_PICK].ob_spec.userblk;
	mou_var.adr_form[MOU_PICK].ob_spec.userblk = &user_pic_c;

	user_time.ub_code = draw_time;
	mou_var.adr_form[MOU_TEMPS].ob_type = G_USERDEF;
	user_time.ub_parm = (long) mou_var.adr_form[MOU_TEMPS].ob_spec.userblk;
	mou_var.adr_form[MOU_TEMPS].ob_spec.userblk = &user_time;
}

void clear_pt_mvt(int i)
{	pt_mvt[i].zoom_x = 1000;
	pt_mvt[i].zoom_y = 1000;
	pt_mvt[i].pos_x = 0;
	pt_mvt[i].pos_y = 0;
	pt_mvt[i].rot = 0;
	pt_mvt[i].time = 0;
}

int duplique_mvt(int src)
{	int dst;
	dst = find_free_mvt();
	if(dst == -1)
		return 0;
	copy_mvt(src, dst);
	mvt[dst].used=1;
	return dst;
}

int new_mvt(void)
{	int i;
	
	were_no_mvt = 0;
	vswr_mode(vdi_handle,MD_REPLACE);
	
	if(prj_obj[selected_prj_obj].mvt == 0)		/* attribution d'un mouvement */
	{	were_no_mvt = 1;	/* on l'indique comme ca, si on annule, on met pas un mvt bidon */
		actu_mvt =find_free_mvt();
			
		if(actu_mvt == -1)	/* plus de place libre */
		{	prj_obj[selected_prj_obj].mvt = 0;
			return -1;
		}
		for(i=actu_mvt*MAX_USE_MVT;i<(actu_mvt+1)*MAX_USE_MVT;i++)
			clear_pt_mvt(i);
		mvt[actu_mvt].nb_mvt_used = 1;
		mvt[actu_mvt].r = 0;
		mvt[actu_mvt].v = 0;
		mvt[actu_mvt].b = 0;
		mvt[actu_mvt].time = 0;
		mvt[actu_mvt].img_used = 0;
		mvt[actu_mvt].transparent = TRUE;
		prj_obj[selected_prj_obj].mvt = actu_mvt;
		mvt[actu_mvt].used = 1;		/* on le garde */
	}

	actu_mvt = prj_obj[selected_prj_obj].mvt;
	copy_mvt(actu_mvt, TEMP_MVT);
		
	if(projet.x>projet.y)
	{	x_prev = MOU_PREV_W;
		y_prev = (int)(((long)MOU_PREV_H*projet.y)/projet.x);
	}else
	{	y_prev = MOU_PREV_H;
		x_prev = (int)(((long)MOU_PREV_W*projet.x)/projet.y);
	}
	
	x_premod16 = (x_prev+15)/16;
	x_premod16 *= 16;

	num_obj = prj_obj[selected_prj_obj].objet;
	num_obj_img = mvt[actu_mvt].img_used;
	x_obj = objet[num_obj].x;
	y_obj = objet[num_obj].y;
	adr_use_obj = Malloc( (long)x_obj * (y_obj+1) * 2L);

	if(adr_use_obj)
	{	adr_preview = Malloc( (long)x_premod16 * (y_prev+1) * 2L);
		if(!adr_preview)
		{	Mfree(adr_use_obj);
			adr_use_obj = 0;
		}else		/* c'et ok */
		{	get_one_frame(num_obj,num_obj_img,adr_use_obj,x_obj);
			/* on construit la MFDB */
			mfdb_pre.fd_addr = adr_preview;
			mfdb_pre.fd_w = x_premod16;
			mfdb_pre.fd_h = y_prev;
			mfdb_pre.fd_wdwidth = x_premod16/16;
			mfdb_pre.fd_stand = 0;
			mfdb_pre.fd_nplanes = 16;
		}
	}
	
	mou_put_coul();
	
	adr_pic_coul = Malloc( (long)MOU_PIC_W * MOU_PIC_H * 2L);
	if(adr_pic_coul)
		zoom(adr_use_obj,adr_pic_coul,x_obj,y_obj,x_obj,MOU_PIC_W,MOU_PIC_H,MOU_PIC_W,1024);
	
	
	mou_var.adr_form[MOU_TREL].ob_state = SELECTED*(temps_reel);
	itoa(num_obj_img,mou_var.adr_form[MOU_NB_IMG].ob_spec.tedinfo->te_ptext,10);
	
	mou_var.adr_form[MOU_TRAN].ob_state = mvt[TEMP_MVT].transparent*SELECTED;
	mou_var.adr_form[MOU_COUL].ob_state = (!mvt[TEMP_MVT].transparent)*SELECTED;
	aff_time_pt(0);
	
	if(mvt[TEMP_MVT].time)	/* y'a une time spline */
		copy_spline_name(mvt[TEMP_MVT].time, mou_var.adr_form[MOU_TSPLINE].ob_spec.tedinfo->te_ptext);
	else
		strcpy(mou_var.adr_form[MOU_TSPLINE].ob_spec.tedinfo->te_ptext,T_CHOOSE_SPLINE);
	
	cur_edit=1;
	imposed_time=-1;
	cur_pt_edit = 0;
	first_pt_mvt = TEMP_MVT*MAX_USE_MVT;
	form_mou(OPEN_DIAL);
	set_pt_mvt(first_pt_mvt);
	updt_num_pt();
	return 0;	
}

void set_pt_mvt(int src)
{	mou_set_ass_pos(MOU_ASS_PX,MOU_FON_PX,pt_mvt[src].pos_x);
	mou_set_ass_pos(MOU_ASS_PY,MOU_FON_PY,pt_mvt[src].pos_y);
	mou_set_ass_zoom(MOU_ASS_X,MOU_FON_X,pt_mvt[src].zoom_x);
	mou_set_ass_zoom(MOU_ASS_Y,MOU_FON_Y,pt_mvt[src].zoom_y);
	mou_set_ass_rot(MOU_ASS_R,MOU_FON_R,pt_mvt[src].rot);
}

void mou_set_ass_rot(int objc, int ofon, int val)
{	int pos;
	char str[8];
	
	pos = (int)( (long)( (val)*((long)mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width)) / (long)(360*4));
	mou_var.adr_form[objc].ob_x = pos;
	
	itoa(val,str,10);
	strcat(str," \xf8");
	strcpy(mou_var.adr_form[objc].ob_spec.tedinfo->te_ptext,str);
	
	Vsync();
	objc_draw(mou_var.adr_form,ofon,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	
}

void mou_set_ass_zoom(int objc, int ofon, int val)
{	int pos;
	char str[8];
	
	if(val<=2000)
		pos = (int)( (long)( (val)*((long)mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width)) / (long)(MAX_ZOOM));
	else	/* mode expo */
		pos = deb_exp_zoom + (int)( (long)( (val-2000L)*((long)mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width - deb_exp_zoom)) / 10000L);
	mou_var.adr_form[objc].ob_x = pos;
	
	itoa(val/10,str,10);
	strcat(str," %");
	strcpy(mou_var.adr_form[objc].ob_spec.tedinfo->te_ptext,str);
	
	Vsync();
	objc_draw(mou_var.adr_form,ofon,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	
}

void mou_set_ass_pos(int objc, int ofon, int val)
{	int pos;
	char str[8];
		
	if(val<-1000)
		pos = (int)( (((long)val+9000L)*deb_m_pos_exp) / 8000L);
	else if(val<=1000)
		pos = deb_m_pos_exp + (int)( (long)( (val+1000L)*((long)deb_p_pos_exp-deb_m_pos_exp)) / 2000L);
	else
		pos = deb_p_pos_exp + (int)( (long)( (val-1000L)*((long)mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width - deb_p_pos_exp)) / 8000L);
	
	mou_var.adr_form[objc].ob_x = pos;
	
	itoa(val/10,str,10);
	strcat(str," %");
	strcpy(mou_var.adr_form[objc].ob_spec.tedinfo->te_ptext,str);
	
	Vsync();
	objc_draw(mou_var.adr_form,ofon,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	
}

void valid_mvt(void)
{	
	mou_scan_coul();
	mvt[TEMP_MVT].img_used = num_obj_img;
	copy_mvt(TEMP_MVT,actu_mvt);
	cur_edit=0;
}

void copy_mvt(int src, int dst)
{	int j,nb1,nb2;

	mvt[dst].nb_mvt_used=mvt[src].nb_mvt_used;
	mvt[dst].img_used = mvt[src].img_used;
	mvt[dst].r = mvt[src].r;
	mvt[dst].v = mvt[src].v;
	mvt[dst].b = mvt[src].b;
	mvt[dst].time = mvt[src].time;
	mvt[dst].transparent = mvt[src].transparent;
	
	for(j=0;j<mvt[src].nb_mvt_used;j++)
	{	nb1 = src*MAX_USE_MVT+j;
		nb2 = dst*MAX_USE_MVT+j;
		pt_mvt[nb2].zoom_x = pt_mvt[nb1].zoom_x;
		pt_mvt[nb2].zoom_y = pt_mvt[nb1].zoom_y;
		pt_mvt[nb2].rot = pt_mvt[nb1].rot;
		pt_mvt[nb2].pos_x = pt_mvt[nb1].pos_x;
		pt_mvt[nb2].pos_y = pt_mvt[nb1].pos_y;
		pt_mvt[nb2].time = pt_mvt[nb1].time;
	}
}

int find_free_mvt(void)
{	int i;
	
	for(i=1;i<MAX_MVT;i++)
		if(mvt[i].used == 0)
			return i;
	
	return -1;
}

void mou_put_coul(void)
{	itoa(mvt[TEMP_MVT].r,mou_var.adr_form[MOU_R].ob_spec.tedinfo->te_ptext,10);
	itoa(mvt[TEMP_MVT].v,mou_var.adr_form[MOU_V].ob_spec.tedinfo->te_ptext,10);
	itoa(mvt[TEMP_MVT].b,mou_var.adr_form[MOU_B].ob_spec.tedinfo->te_ptext,10);
}

void mou_scan_coul(void)
{	mvt[TEMP_MVT].r = min(255,atoi(mou_var.adr_form[MOU_R].ob_spec.tedinfo->te_ptext));
	mvt[TEMP_MVT].v = min(255,atoi(mou_var.adr_form[MOU_V].ob_spec.tedinfo->te_ptext));
	mvt[TEMP_MVT].b = min(255,atoi(mou_var.adr_form[MOU_B].ob_spec.tedinfo->te_ptext));
}

void get_color_picked(void)
{	int mx,my,dum,col;
	int rvb[3];
	
	graf_mkstate(&mx, &my, &dum,&dum);
	
	mx -= mou_var.adr_form[0].ob_x + mou_var.adr_form[MOU_FOND_COL].ob_x + mou_var.adr_form[MOU_PICK].ob_x;
	my -= mou_var.adr_form[0].ob_y + mou_var.adr_form[MOU_FOND_COL].ob_y + mou_var.adr_form[MOU_PICK].ob_y;
	mx=min(MOU_PIC_W-1,mx);
	my=min(MOU_PIC_H-1,my);
	
	col = *(adr_pic_coul + mx + (long)my*MOU_PIC_W);
	
	conv2_col(col,rvb);
	
	mvt[TEMP_MVT].r = *rvb;
	mvt[TEMP_MVT].v = rvb[1];
	mvt[TEMP_MVT].b = rvb[2];

	mou_put_coul();
	objc_draw(mou_var.adr_form,MOU_PRE_COUL,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	objc_draw(mou_var.adr_form,MOU_R,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	objc_draw(mou_var.adr_form,MOU_V,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	objc_draw(mou_var.adr_form,MOU_B,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	if(temps_reel)
		objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
}
	
void form_mou(int event)
{
	WINDFORM_VAR *ptr_var = &mou_var;
	int choix,i;
	
	if (event == OPEN_DIAL && cur_edit)
	{	sprintf(titre_wind,T_MVT_OF,objet[prj_obj[selected_prj_obj].objet].nom);
		open_dialog(ptr_var, titre_wind, MOU_NB_IMG, -1);
	}

	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{	if(choix == CLOSE_DIAL)	choix = MOU_ANN;
			switch(choix)
			{	
				case MOU_TSPLINE:
					/* init des points d'annotations */
					etim_nb_anno = min(mvt[TEMP_MVT].nb_mvt_used,3);
					for(i=0;i<min(mvt[TEMP_MVT].nb_mvt_used,3);i++)
					{	etim_pos_anno[i]=(int)(((long)pt_mvt[first_pt_mvt+i].time*255L)/1000);
						sprintf(etim_text_anno[i],"Pt. %d",i);
					}
					
					retour_index = &mvt[TEMP_MVT].time;
					retour_wform = ptr_var;
					retour_objet = MOU_TSPLINE;
					form_ctim(OPEN_DIAL);
					wf_change(ptr_var,choix,0,1);
					break;

				case MOU_PX_M:
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_x=max(pt_mvt[first_pt_mvt+cur_pt_edit].pos_x-10-100*(nbr_clics==2),-10000);
					mou_set_ass_pos(MOU_ASS_PX,MOU_FON_PX,pt_mvt[first_pt_mvt+cur_pt_edit].pos_x);
					update_mou();
					break;
				case MOU_PX_P:
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_x=min(pt_mvt[first_pt_mvt+cur_pt_edit].pos_x+10+100*(nbr_clics==2),10000);
					mou_set_ass_pos(MOU_ASS_PX,MOU_FON_PX,pt_mvt[first_pt_mvt+cur_pt_edit].pos_x);
					update_mou();
					break;
				case MOU_ASS_PX:
					move_ass_pos(MOU_ASS_PX,MOU_FON_PX,&pt_mvt[first_pt_mvt+cur_pt_edit].pos_x);
					update_mou();
					break;

	
				case MOU_PY_M:
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_y=max(pt_mvt[first_pt_mvt+cur_pt_edit].pos_y-10-100*(nbr_clics==2),-10000);
					mou_set_ass_pos(MOU_ASS_PY,MOU_FON_PY,pt_mvt[first_pt_mvt+cur_pt_edit].pos_y);
					update_mou();
					break;
				case MOU_PY_P:
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_y=min(pt_mvt[first_pt_mvt+cur_pt_edit].pos_y+10+100*(nbr_clics==2),10000);
					mou_set_ass_pos(MOU_ASS_PY,MOU_FON_PY,pt_mvt[first_pt_mvt+cur_pt_edit].pos_y);
					update_mou();
					break;
				case MOU_ASS_PY:
					move_ass_pos(MOU_ASS_PY,MOU_FON_PY,&pt_mvt[first_pt_mvt+cur_pt_edit].pos_y);
					update_mou();
					break;
		
				case MOU_Y_M:
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y=max(pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y-10-100*(nbr_clics==2),0);
					mou_set_ass_zoom(MOU_ASS_Y,MOU_FON_Y,pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y);
					update_mou();
					break;
				case MOU_Y_P:
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y=min(pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y+10+100*(nbr_clics==2),12000);
					mou_set_ass_zoom(MOU_ASS_Y,MOU_FON_Y,pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y);
					update_mou();
					break;
				case MOU_ASS_Y:
					move_ass_zoom(MOU_ASS_Y,MOU_FON_Y,&pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y);
					update_mou();
					break;
				case MOU_X_M:
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x=max(pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x-10-100*(nbr_clics==2),0);
					mou_set_ass_zoom(MOU_ASS_X,MOU_FON_X,pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x);
					update_mou();
					break;
				case MOU_X_P:
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x=min(pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x+10+100*(nbr_clics==2),12000);
					mou_set_ass_zoom(MOU_ASS_X,MOU_FON_X,pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x);
					update_mou();
					break;
				case MOU_ASS_X:
					move_ass_zoom(MOU_ASS_X,MOU_FON_X,&pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x);
					update_mou();
					break;
				case MOU_PRE:
					objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
					break;
				case MOU_CENTRE:
					pt_mvt[first_pt_mvt+cur_pt_edit].rot = 0;
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_x = (1000 - pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x)/2;
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_y = (1000 - pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y)/2;
					set_pt_mvt(first_pt_mvt+cur_pt_edit);
					update_mou();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_FILL:
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x = 1000;
					pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y = 1000;
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_x = 0;
					pt_mvt[first_pt_mvt+cur_pt_edit].pos_y = 0;
					pt_mvt[first_pt_mvt+cur_pt_edit].rot = 0;
					set_pt_mvt(first_pt_mvt+cur_pt_edit);
					update_mou();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_ANIM:
					anim_prev();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_TREL:
					temps_reel = 1 - temps_reel;
					break;
				case MOU_MAJ:
					if( (num_obj_img != atoi(mou_var.adr_form[MOU_NB_IMG].ob_spec.tedinfo->te_ptext)) && adr_use_obj)
					{	num_obj_img = min(atoi(mou_var.adr_form[MOU_NB_IMG].ob_spec.tedinfo->te_ptext),objet[num_obj].nb_img);
						get_one_frame(num_obj,num_obj_img,adr_use_obj,x_obj);
						objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
						if(adr_pic_coul)
						{	zoom(adr_use_obj,adr_pic_coul,x_obj,y_obj,x_obj,MOU_PIC_W,MOU_PIC_H,MOU_PIC_W,1024);
							objc_draw(mou_var.adr_form,MOU_PICK,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
						}
					}
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_PRE_COUL:
					mou_scan_coul();
					objc_draw(mou_var.adr_form,MOU_PRE_COUL,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
					break;
				case MOU_TRAN:
				case MOU_COUL:
					mvt[TEMP_MVT].transparent = mou_var.adr_form[MOU_TRAN].ob_state & SELECTED;
					objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
					break;
				case MOU_TEMPS:
					click_temps();
					break;
				case MOU_ENLEVE:
					kill_pt(first_pt_mvt+cur_pt_edit);
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_NEW_PT:
					new_pt();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_PICK:
					get_color_picked();
					break;
				case MOU_SAVE:
					save_mvt();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_LOAD:
					load_mvt();
					wf_change(ptr_var,choix,0,1);
					break;
				case MOU_OK:
					valid_mvt();
				case MOU_SUP_MVT:
				case MOU_ANN:
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					if(choix == MOU_SUP_MVT || (choix == MOU_ANN && were_no_mvt))
					{	mvt[actu_mvt].used = 0; prj_obj[selected_prj_obj].mvt = 0; }
					cur_edit = 0;
					if(adr_use_obj)	
					{	Mfree(adr_use_obj); Mfree(adr_preview); }
					if(adr_pic_coul)	
						Mfree(adr_pic_coul);
					break;
			}
		}
	}
}

void load_mvt(void)
{	int handle;
	int bout;
	long head;
	char nom[14]="",pth[250]="\\*.MVT";
	CHEMIN fic;
	
	Fselecteur(&fic,pth,nom,&bout,T_LOAD_THE_MVT);
	if(bout && nom[0])
	{	Dsetdrv(fic.chemin[0] - 'A');
		Dsetpath(fic.chemin);
		handle = (int)Fopen(fic.complet,0);
		if(handle>=0)
		{	Fread(handle,4,&head);
			if(head == 'MMVT')
			{	Fread(handle,sizeof(MVT),&mvt[TEMP_MVT]);
				Fread(handle,sizeof(PT_MVT) * mvt[TEMP_MVT].nb_mvt_used, &pt_mvt[first_pt_mvt]);
				mvt[TEMP_MVT].img_used=0;
				/* OK, c'est charge */
				cur_pt_edit = 0;
				objc_draw(mou_var.adr_form,MOU_TEMPS,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
				set_pt_mvt(first_pt_mvt+cur_pt_edit);
				update_mou();
				updt_num_pt();
				aff_time_pt(pt_mvt[first_pt_mvt].time);
				mou_var.adr_form[MOU_TRAN].ob_state = mvt[TEMP_MVT].transparent*SELECTED;
				mou_var.adr_form[MOU_COUL].ob_state = (!mvt[TEMP_MVT].transparent)*SELECTED;
				mou_put_coul();
				objc_draw(mou_var.adr_form,MOU_FOND_COL,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
			}else
				super_alert(1,3,T_BAD_FILE_FORMAT);
			Fclose(handle);
		}else
			super_alert(1,3,T_CANTPEN_FILE);
	}
}

void save_mvt(void)
{	int handle;
	int bout;
	long head = 'MMVT';
	char nom[14]="",pth[250]="\\*.MVT";
	CHEMIN fic;
	
	Fselecteur(&fic,pth,nom,&bout,T_SAV_THE_MVT);
	if(bout && nom[0])
	{	Dsetdrv(fic.chemin[0] - 'A');
		Dsetpath(fic.chemin);

		find_name(fic.complet,nom);
		strcpy(pth, fic.chemin);
		strcat(pth, nom);
		strcat(pth, ".MVT");		/* on impose l'extention MVT */

		handle = (int)Fcreate(pth,0);
		if(handle>=0)
		{	Fwrite(handle,4,&head);
			Fwrite(handle,sizeof(MVT),&mvt[TEMP_MVT]);
			Fwrite(handle,sizeof(PT_MVT) * mvt[TEMP_MVT].nb_mvt_used, &pt_mvt[first_pt_mvt]);
			Fclose(handle);
		}else
			super_alert(1,3,T_CANT_CREAT_FILE);
	}
	
}

void anim_prev(void)
{	int i,mk,dum,key;
	
	if(mvt[TEMP_MVT].nb_mvt_used != 1)
	{	for(i=0;i<50;i++)
		{	imposed_time = (i*100)/5;
			Vsync();
			objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
			graf_mkstate(&dum, &dum, &mk,&key);
			if(mk == 2 || key)
				goto fin;
		}
		fin:;
		imposed_time = -1;
		objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	}
}	

void kill_pt(int pt)
{	int i;

	if(mvt[TEMP_MVT].nb_mvt_used != 1)
	{	if(pt != mvt[TEMP_MVT].nb_mvt_used-1)	/* car si c'est le derniere, on a juste a reduire le nb */
		{	for(i=pt;i<mvt[TEMP_MVT].nb_mvt_used-1;i++)
			{	pt_mvt[i].time = pt_mvt[i+1].time;
				pt_mvt[i].zoom_x = pt_mvt[i+1].zoom_x;
				pt_mvt[i].zoom_y = pt_mvt[i+1].zoom_y;
				pt_mvt[i].pos_x = pt_mvt[i+1].pos_x;
				pt_mvt[i].pos_y = pt_mvt[i+1].pos_y;
				pt_mvt[i].rot = pt_mvt[i+1].rot;
			}
		}else
		{	/* le dernier est effacer, mais il en faut touours un sauf si y'en a qu'un */
			if(mvt[TEMP_MVT].nb_mvt_used != 2)
				pt_mvt[pt-1].time = 1000;
			
			cur_pt_edit = pt-1;
		}
		if(pt == 0)	/* le premiere est efface */
			pt_mvt[0].time = 0;
		mvt[TEMP_MVT].nb_mvt_used--;
		objc_draw(mou_var.adr_form,MOU_TEMPS,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		set_pt_mvt(first_pt_mvt+cur_pt_edit);
		update_mou();	
		updt_num_pt();	
	}
}

void new_pt(void)
{	int i;
	if(mvt[TEMP_MVT].nb_mvt_used < MAX_USE_MVT)
	{	if(mvt[TEMP_MVT].nb_mvt_used == 1)
		{	pt_mvt[first_pt_mvt+1].time = 1000;
			pt_mvt[first_pt_mvt+1].zoom_x = pt_mvt[first_pt_mvt].zoom_x;
			pt_mvt[first_pt_mvt+1].zoom_y = pt_mvt[first_pt_mvt].zoom_y;
			pt_mvt[first_pt_mvt+1].pos_x = pt_mvt[first_pt_mvt].pos_x;
			pt_mvt[first_pt_mvt+1].pos_y = pt_mvt[first_pt_mvt].pos_y;
			pt_mvt[first_pt_mvt+1].rot = pt_mvt[first_pt_mvt].rot;
			cur_pt_edit = 1;
		}else
		{	if(cur_pt_edit == 0)	cur_pt_edit=1;
			for(i=first_pt_mvt + mvt[TEMP_MVT].nb_mvt_used-1;i>=cur_pt_edit;i--)
			{	pt_mvt[i+1].time = pt_mvt[i].time;
				pt_mvt[i+1].rot = pt_mvt[i].rot;
				pt_mvt[i+1].zoom_x = pt_mvt[i].zoom_x;
				pt_mvt[i+1].zoom_y = pt_mvt[i].zoom_y;
				pt_mvt[i+1].pos_x = pt_mvt[i].pos_x;
				pt_mvt[i+1].pos_y = pt_mvt[i].pos_y;
			}
			pt_mvt[cur_pt_edit].time = (pt_mvt[cur_pt_edit+1].time + pt_mvt[cur_pt_edit-1].time)/2;
			pt_mvt[cur_pt_edit].rot = (pt_mvt[cur_pt_edit+1].rot + pt_mvt[cur_pt_edit-1].rot)/2;
			pt_mvt[cur_pt_edit].zoom_x = (pt_mvt[cur_pt_edit+1].zoom_x + pt_mvt[cur_pt_edit-1].zoom_x)/2;
			pt_mvt[cur_pt_edit].zoom_y = (pt_mvt[cur_pt_edit+1].zoom_y + pt_mvt[cur_pt_edit-1].zoom_y)/2;
			pt_mvt[cur_pt_edit].pos_x = (pt_mvt[cur_pt_edit+1].pos_x + pt_mvt[cur_pt_edit-1].pos_x)/2;
			pt_mvt[cur_pt_edit].pos_y = (pt_mvt[cur_pt_edit+1].pos_y + pt_mvt[cur_pt_edit-1].pos_y)/2;
		}
		mvt[TEMP_MVT].nb_mvt_used++;
		objc_draw(mou_var.adr_form,MOU_TEMPS,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		set_pt_mvt(first_pt_mvt+cur_pt_edit);
		update_mou();	
		updt_num_pt();
		
	}
	aff_time_pt(pt_mvt[cur_pt_edit].time);
}

void click_temps(void)
{	int mx,mk,dum,choix,old,dx,base_x,base_y,min_t,max_t;
	
	Vsync();	Vsync();	Vsync();	Vsync();	Vsync();	Vsync();	Vsync();	Vsync();
	
	graf_mkstate(&mx, &dum, &mk,&dum);
	
	choix = trouve_pt(mx);
	
	if(mk==0)		/* juste un choix de timer */
	{	cur_pt_edit = choix;
		objc_draw(mou_var.adr_form,MOU_TEMPS,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		set_pt_mvt(first_pt_mvt+cur_pt_edit);
		update_mou();
		updt_num_pt();
		aff_time_pt(pt_mvt[first_pt_mvt+choix].time);
	}else		/* un deplacement */
	{	if(choix != 0 && choix !=mvt[TEMP_MVT].nb_mvt_used-1)
		{	base_x = mou_var.adr_form[MOU_TEMPS].ob_x + mou_var.adr_form[MOU_FON_TEMPS].ob_x + mou_var.adr_form[0].ob_x + 4;
			base_y = mou_var.adr_form[MOU_TEMPS].ob_y + mou_var.adr_form[MOU_FON_TEMPS].ob_y + mou_var.adr_form[0].ob_y + 6;
			
			dx = mx - (int)( ((long)pt_mvt[first_pt_mvt+choix].time * 300L)/1000L);
			dx -= base_x;
			old = pt_mvt[first_pt_mvt+choix].time;
			min_t = pt_mvt[first_pt_mvt+choix-1].time+6;
			max_t = pt_mvt[first_pt_mvt+choix+1].time-6;
			vswr_mode(vdi_handle,MD_XOR); 
			v_hide_c(vdi_handle);
			draw_marqueur((old*3)/10+base_x,base_y,0);
			do
			{	graf_mkstate(&mx, &dum, &mk,&dum);
				
				mx -= dx + base_x;
				
				dum = (int)((mx * 1000L)/300L);
				dum = min(dum,max_t);
				dum = max(dum,min_t);
				
				if(dum != old)
				{	Vsync();
					draw_marqueur((old*3)/10+base_x,base_y,0);
					old = dum;
					draw_marqueur((dum*3)/10+base_x,base_y,0);
					aff_time_pt(dum);
				}
			}while(mk==1);
			pt_mvt[first_pt_mvt+choix].time = dum;
			v_show_c(vdi_handle,1);
			vswr_mode(vdi_handle,MD_REPLACE); 
			objc_draw(mou_var.adr_form,MOU_TEMPS,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		}
	}
}

/* affiche le texte de pourcentage du temps du pt */
void aff_time_pt(int val)
{	val /=10;
	sprintf(mou_var.adr_form[MOU_PT_PCENT].ob_spec.tedinfo->te_ptext,T_A_D_PCENT,val);
	objc_draw(mou_var.adr_form,MOU_PT_PCENT,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
}

int trouve_pt(int x)
{	int i,x_min,x_max,time;
	
	x -=  mou_var.adr_form[MOU_TEMPS].ob_x + mou_var.adr_form[MOU_FON_TEMPS].ob_x + mou_var.adr_form[0].ob_x + 4;
	
	if(mvt[TEMP_MVT].nb_mvt_used == 1)
		return 0;
	
	for(i=0;i<mvt[TEMP_MVT].nb_mvt_used;i++)
	{	if(i==0)	x_min = -4;
		else
		{	time = (pt_mvt[first_pt_mvt+i-1].time + pt_mvt[first_pt_mvt+i].time)/2;
			x_min = (int)( ((long)time * 300L)/1000L);
		}
		if(i==mvt[TEMP_MVT].nb_mvt_used-1)	x_max = 304;
		else
		{	time = (pt_mvt[first_pt_mvt+i+1].time + pt_mvt[first_pt_mvt+i].time)/2;
			x_max = (int)( ((long)time * 300L)/1000L);
		}
		if(x>=x_min && x<x_max)
			return i;
	}
	return 0;
}

void updt_num_pt(void)
{	sprintf(mou_var.adr_form[MOU_NUM_PT].ob_spec.tedinfo->te_ptext,"Point %d/%d",cur_pt_edit+1,mvt[TEMP_MVT].nb_mvt_used);
	objc_draw(mou_var.adr_form,MOU_NUM_PT,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
}

void update_mou(void)
{	
	objc_draw(mou_var.adr_form,MOU_MOU,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	if(temps_reel)
		objc_draw(mou_var.adr_form,MOU_PRE,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
}
		
void move_ass_pos(int objc, int ofon, int *val)
{	int mousex,bouton,dummy,old,ax,nb,max,decal,updt=0;
	
	ax = mou_var.adr_form[0].ob_x + mou_var.adr_form[ofon].ob_x;
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	decal= mousex - (ax+mou_var.adr_form[objc].ob_x);
	max = mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width;
	
	graf_mouse (FLAT_HAND,0);
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	old = mousex - ax - decal;
	if(old<deb_m_pos_exp)
		old = (int)( ((long)old * 8000L ) / deb_m_pos_exp - 9000);
	else if(old <= deb_p_pos_exp)
		old = (int)( ((long)((old-deb_m_pos_exp) * 2000L )) / (deb_p_pos_exp-deb_m_pos_exp) - 1000);
	else
		old = (int)( ((long)((old-deb_p_pos_exp) * 8000L )) / (max-deb_p_pos_exp) + 1000);

	graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	do  
	{	/* prise des coordon‚es de la souris : */
		nb = mousex - ax - decal;

		if (nb <  0 )
			nb = 0;
		if (nb > max  )
			nb = max; 
			
		if(nb<deb_m_pos_exp)
			nb = (int)( ((long)nb * 8000L ) / deb_m_pos_exp - 9000);
		else if(nb <= deb_p_pos_exp)
			nb = (int)( ((long)((nb-deb_m_pos_exp) * 2000L )) / (deb_p_pos_exp-deb_m_pos_exp) - 1000);
		else
			nb = (int)( ((long)((nb-deb_p_pos_exp) * 8000L )) / (max-deb_p_pos_exp) + 1000);
		
		if (old != nb ) 
		{	old = nb;
			mou_set_ass_pos(objc, ofon, nb);
			updt = 0;
			if(ultra_temps_reel)
			{	*val = nb;
				update_mou();
			}
		}else
		{	
			if(updt == 3)
			{	*val = nb;
				update_mou();
				updt++;
			}else if(updt<3)
			{	updt++;
				Vsync();
			}
		}
		graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	} while (bouton != 0);
	graf_mouse (ARROW, 0);
	*val = nb;
}

void move_ass_zoom(int objc, int ofon, int *val)
{	int mousex,bouton,dummy,old,ax,nb,max,decal,updt=0;
	
	ax = mou_var.adr_form[0].ob_x + mou_var.adr_form[ofon].ob_x;
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	decal= mousex - (ax+mou_var.adr_form[objc].ob_x);
	max = mou_var.adr_form[ofon].ob_width - mou_var.adr_form[objc].ob_width;
	
	graf_mouse (FLAT_HAND,0);
	graf_mkstate(&mousex, &dummy, &bouton,&dummy);
	old = (int)( ( (long)(mousex - ax - decal) * MAX_ZOOM) / max);

	graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	do  
	{	/* prise des coordon‚es de la souris (+ loin): */
		nb = mousex - ax - decal;

		if (nb <  0 )
			nb = 0;
		if (nb > max  )
			nb = max;
			
		if(nb<=deb_exp_zoom)
			nb = (int)( ((long)nb * MAX_ZOOM ) / max );
		else
			nb = 2000 + (int)( ((long)((nb-deb_exp_zoom) * 10000L ) / (max-deb_exp_zoom)) );
		
		if (old != nb ) 
		{	old = nb;
			mou_set_ass_zoom(objc, ofon, nb);
			updt = 0;
			if(ultra_temps_reel)
			{	*val = nb;
				update_mou();
			}
		}else
		{	if(updt == 3)
			{	*val = nb;
				update_mou();
				updt++;
			}else if(updt<3)
			{	updt++;
				Vsync();
			}
		}
		graf_mkstate(&mousex, &dummy, &bouton,&dummy); 
	} while (bouton != 0);
	graf_mouse (ARROW, 0);
	*val = nb;
}

int cdecl draw_mvt(PARMBLK *parmblock)
{
	GRECT area,real;
	int base_x,base_y,x,y,w,h,i,xy[2];
	
	base_x = mou_var.adr_form[MOU_MOU].ob_x + mou_var.adr_form[0].ob_x;
	base_y = mou_var.adr_form[MOU_MOU].ob_y + mou_var.adr_form[0].ob_y;
	
	real.g_x = base_x;
	real.g_y = base_y;
	real.g_w = MOU_W;
	real.g_h = MOU_H;
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;

	rc_intersect(&real,&area);
	set_clip(1, &area);
	
	vswr_mode(vdi_handle,MD_REPLACE);
	Vsync();
	/* le tour */
	rect_full(base_x,base_y,MOU_W,MOU_H, WHITE);
	rect(base_x,base_y,MOU_W,MOU_H,BLACK);
	
	/* le cadre projet */
	rect(base_x+MOU_W/4,base_y+MOU_H/4,MOU_W/2+1,MOU_H/2+1,BLACK);
	
	vsm_type(vdi_handle,4);
	vsm_height(vdi_handle,6);
	vsm_color(vdi_handle,LBLUE);
	
	/* le chemin */
	if(mvt[TEMP_MVT].nb_mvt_used>1)
	{	for(i=0;i<mvt[TEMP_MVT].nb_mvt_used;i++)
		{	x = (int)(( (long)pt_mvt[first_pt_mvt+i].pos_x*(MOU_W/2))/1000L);
			y = (int)(( (long)pt_mvt[first_pt_mvt+i].pos_y*(MOU_H/2))/1000L);
			w = (int)(( (long)pt_mvt[first_pt_mvt+i+1].pos_x*(MOU_W/2))/1000L)+1 - x;
			h = (int)(( (long)pt_mvt[first_pt_mvt+i+1].pos_y*(MOU_H/2))/1000L)+1 - y;
			x += base_x+MOU_W/4;
			y += base_y+MOU_H/4;
			if(i!=mvt[TEMP_MVT].nb_mvt_used-1)
				ligne(x,y,w,h,LCYAN);
			xy[0]=x;			xy[1]=y;
			v_pmarker(vdi_handle,1,xy);
			w = (int)(((long)pt_mvt[first_pt_mvt+i].zoom_x*(MOU_W/2))/1000L);
			h = (int)(((long)pt_mvt[first_pt_mvt+i].zoom_y*(MOU_H/2))/1000L);
			w++; h++;
			rect(x,y,w,h,LWHITE);
		}
	}
	
	/* norte cadre actuel */
	x = base_x+MOU_W/4 + (int)(( (long)pt_mvt[first_pt_mvt+cur_pt_edit].pos_x*(MOU_W/2))/1000L);
	y = base_y+MOU_H/4 + (int)(( (long)pt_mvt[first_pt_mvt+cur_pt_edit].pos_y*(MOU_H/2))/1000L);
	w = (int)(((long)pt_mvt[first_pt_mvt+cur_pt_edit].zoom_x*(MOU_W/2))/1000L);
	h = (int)(((long)pt_mvt[first_pt_mvt+cur_pt_edit].zoom_y*(MOU_H/2))/1000L);
	w++; h++;
/*	rect_full(x,y,w,h,LWHITE);*/
	rect(x,y,w,h,BLUE);
	
	set_clip(0, &area);
	return 0;
}

int cdecl draw_preview(PARMBLK *parmblock)
{	GRECT area,real;
	int base_x,base_y,time;
	int pxy[8],dum;
	
	base_x = mou_var.adr_form[MOU_PRE].ob_x + mou_var.adr_form[0].ob_x;
	base_y = mou_var.adr_form[MOU_PRE].ob_y + mou_var.adr_form[0].ob_y;
	
	real.g_x = base_x;
	real.g_y = base_y;
	real.g_w = x_prev;
	real.g_h = y_prev;
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;

	rc_intersect(&real,&area);
	set_clip(1, &area);
	
	if(adr_use_obj)
	{	fill(adr_preview,x_prev,y_prev,x_premod16,0xffff);
		
		if(imposed_time == -1)	/* pas de temps impose (anim) on prend le pt actuel */
		{	time = pt_mvt[first_pt_mvt+cur_pt_edit].time;
			/* il faut deactiver la time spline sinon on n'y pige plus rien */
			dum = mvt[TEMP_MVT].time;
			mvt[TEMP_MVT].time = 0;
		}else
			time = imposed_time;

		cacl_n_put_mvt(adr_use_obj,adr_preview,x_obj,y_obj,x_obj,
							x_prev,y_prev,x_premod16,TEMP_MVT,time,-1,0,0,0,1024); 
		
		if(imposed_time == -1)
			mvt[TEMP_MVT].time = dum;
		
		/* Zone source dans l'image en m‚moire : */
		pxy[0] = area.g_x - base_x;	
		pxy[1] = area.g_y - base_y;	
		pxy[2] = pxy[0] + area.g_w - 1;	
		pxy[3] = pxy[1] + area.g_h - 1;	
		/* Zone destination dans la fenˆtre … l'‚cran */
		pxy[4] = area.g_x;							
		pxy[5] = area.g_y;							
		pxy[6] = pxy[4] + area.g_w - 1;	
		pxy[7] = pxy[5] + area.g_h - 1;	
		tnt_cpyfm(pxy, &mfdb_pre);
		
	}else
		rect_full(base_x,base_y,MOU_PREV_W,MOU_PREV_H, LWHITE);
		
	set_clip(0, &area);
	return 0;
	
}

void cacl_n_put_mvt(int *src, int *dst, int src_x, int src_y, int src_tx, int dst_x, int dst_y, int dst_tx, int num_mvt, int time, int transp, int tr,int tv,int tb, int intens)
{	int src_deb_x,src_deb_y,src_fin_w,src_fin_h;
	int dst_deb_x,dst_deb_y,dst_fin_w,dst_fin_h;
	int *real_src,*real_dst;
	int cur_zx,cur_zy,cur_px,cur_py;
	int cur_zx2,cur_zy2,cur_px2,cur_py2;
	int num_pt,i,len;
	
	/* utilisation de la time spline */
	if(mvt[num_mvt].time)
		time = (int)spline_comput(mvt[num_mvt].time,0L,time,1000L,1000L);

	if(mvt[num_mvt].nb_mvt_used == 1)	/* on a pas le choix */
	{	cur_px = pt_mvt[num_mvt*MAX_USE_MVT].pos_x;
		cur_py = pt_mvt[num_mvt*MAX_USE_MVT].pos_y;
		cur_zx = pt_mvt[num_mvt*MAX_USE_MVT].zoom_x;
		cur_zy = pt_mvt[num_mvt*MAX_USE_MVT].zoom_y;
	}else
	{	for(i=1;i<mvt[num_mvt].nb_mvt_used;i++)
		{	if(time <= pt_mvt[num_mvt*MAX_USE_MVT+i].time)
			{	num_pt = num_mvt*MAX_USE_MVT+i;
				len = pt_mvt[num_pt].time - pt_mvt[num_pt-1].time;
				time -= pt_mvt[num_pt-1].time;
				cur_px = pt_mvt[num_pt-1].pos_x;
				cur_py = pt_mvt[num_pt-1].pos_y;
				cur_zx = pt_mvt[num_pt-1].zoom_x;
				cur_zy = pt_mvt[num_pt-1].zoom_y;
				cur_px2 = pt_mvt[num_pt].pos_x;
				cur_py2 = pt_mvt[num_pt].pos_y;
				cur_zx2 = pt_mvt[num_pt].zoom_x;
				cur_zy2 = pt_mvt[num_pt].zoom_y;
				/* le calcul en lui meme */
				cur_px = (int)(((long)cur_px2 * time)/len + ((long)cur_px * (len-time))/len);
				cur_py = (int)(((long)cur_py2 * time)/len + ((long)cur_py * (len-time))/len);
				cur_zx = (int)(((long)cur_zx2 * time)/len + ((long)cur_zx * (len-time))/len);
				cur_zy = (int)(((long)cur_zy2 * time)/len + ((long)cur_zy * (len-time))/len);
				goto suite;
			}
		}
	}
	suite:;
	
	if(mvt[num_mvt].transparent == 0)
		fill(dst, dst_x, dst_y, dst_tx, conv_col(mvt[num_mvt].r,mvt[num_mvt].v,mvt[num_mvt].b));
	
	if(cur_px>=1000 || cur_py>=1000 || cur_zx == 0 || cur_zy == 0)
		return;		/* on va pas s'embeter alors que l'objet n'apparait meme pas */

	
	if(cur_px>=0)
	{	src_deb_x = 0;
		dst_deb_x = (int)(( (long)cur_px*dst_x)/1000L);	/* petit decalage */
		if(cur_px+cur_zx<1000)	/* 1 */
		{	src_fin_w = src_x;		/* tout rentre */
			dst_fin_w = (int)(((long)cur_zx*dst_x)/1000L);
		}else 						/* 2 */
		{	dst_fin_w = dst_x-dst_deb_x;		/* une petite partie de l'obj ressort */
			src_fin_w = (int)( ((1000L-cur_px)*src_x)/(long)cur_zx);
		}
	}else		/* ici px est negatif, l'obj sort forcement par la gauche */
	{	if(cur_zx <= -cur_px)	/* 3 */
			return;			/* et vi, ca passe pas */
		else if(cur_zx <= 1000-cur_px)	/* 4 */
		{	dst_deb_x = 0;
			dst_fin_w = (int)(( (long)(cur_zx+cur_px)*dst_x)/1000L);	/* ce ki resort ds le projet */
			src_deb_x = (int)(( (long)(-cur_px)*src_x)/cur_zx);
			src_fin_w = src_x - src_deb_x;
		}else  /* 5 */ /* l'objet depasse des 2 cotes */
		{	dst_deb_x = 0;
			dst_fin_w = dst_x;
			src_deb_x = (int)(( (long)(-cur_px)*src_x)/cur_zx);
			src_fin_w = (int)(( (long)(1000L-cur_px)*src_x)/cur_zx) - src_deb_x;
		}
	}
	
	if(cur_py>=0)
	{	src_deb_y = 0;
		dst_deb_y = (int)(( (long)cur_py*dst_y)/1000L);	/* petit decalage */
		if(cur_py+cur_zy<1000)	/* 1 */
		{	src_fin_h = src_y;		/* tout rentre */
			dst_fin_h = (int)(((long)cur_zy*dst_y)/1000L);
		}else 						/* 2 */
		{	dst_fin_h = dst_y-dst_deb_y;		/* une petite partie de l'obj ressort */
			if(cur_zy==0) src_fin_h	= 1;		/* ca ne devrait pas arriver */
			else
				src_fin_h = (int)( ((1000L-cur_py)*src_y)/(long)cur_zy);
		}
	}else		/* ici px est negatif, l'obj sort forcement par le haut */
	{	if(cur_zy <= -cur_py)	/* 3 */
			return;			/* et vi, ca passe pas */
		else if(cur_zy <= 1000-cur_py)	/* 4 */
		{	dst_deb_y = 0;
			dst_fin_h = (int)(( (long)(cur_zy+cur_py)*dst_y)/1000L);	/* ce ki resort ds le projet */
			src_deb_y = (int)(( (long)(-cur_py)*src_y)/cur_zy);
			src_fin_h = src_y - src_deb_y;
		}else  /* 5 */ /* l'objet depasse des 2 cotes */
		{	dst_deb_y = 0;
			dst_fin_h = dst_y;
			src_deb_y = (int)(( (long)(-cur_py)*src_y)/cur_zy);
			src_fin_h = (int)(( (long)(1000L-cur_py)*src_y)/cur_zy)- src_deb_y;
		}
	}

	
	real_src = src + (long)src_deb_x + (long)src_deb_y*src_tx;
	real_dst = dst + (long)dst_deb_x + (long)dst_deb_y*dst_tx;
	if(transp == -1)
		zoom(real_src,real_dst,src_fin_w,src_fin_h,src_tx,dst_fin_w,dst_fin_h,dst_tx,intens);
	else
		zoom_tr(real_src,real_dst,src_fin_w,src_fin_h,src_tx,dst_fin_w,dst_fin_h,dst_tx
					,conv_col(tr,tv,tb),transp,intens);
}

int cdecl preview_color(PARMBLK *parmblock)
{
	int x, y, w, h;
	int *img, couleur;
	int pxy[8],w_mod16;
	MFDB src;
	GRECT area;
		
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;
	set_clip(TRUE, &area);
	
	x = parmblock->pb_x;
	y = parmblock->pb_y;
	w = parmblock->pb_w;
	h = parmblock->pb_h;

	w--; h--; /* Conformit‚ avec les Boutons GEM Standards. */

	
	rect_3d(x,y,w,h);
	
	x++; y++; w-=2; h-=2;
	
	w_mod16 = (w+15)/16;
	w_mod16 *= 16;

	
	img = Malloc((long)w_mod16 * (h+1) * 2L);
	
	
	if(img != 0)
	{	couleur = conv_col(mvt[TEMP_MVT].r,mvt[TEMP_MVT].v,mvt[TEMP_MVT].b);
		
		fill(img,w,h,w_mod16,couleur);
		
		src.fd_addr = img;
		src.fd_w = w_mod16;
		src.fd_h = h;
		src.fd_wdwidth = w_mod16 / 16;
		src.fd_stand = 0;
		src.fd_nplanes = 16;

		/* Zone source dans l'image en m‚moire : */
		pxy[0] = 0;	
		pxy[1] = 0;	
		pxy[2] = w - 1;	
		pxy[3] = h - 1;	
		/* Zone destination dans la fenˆtre … l'‚cran */
		pxy[4] = x;							
		pxy[5] = y;							
		pxy[6] = pxy[4] + w - 1;	
		pxy[7] = pxy[5] + h - 1;	
		
		
		tnt_cpyfm(pxy, &src);
		
		Mfree(img);
	}
	
	return 0;
}

int cdecl pic_coul(PARMBLK *parmblock)
{
	int x, y;
	int pxy[8];
	MFDB src;
	GRECT area;
		
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;
	set_clip(TRUE, &area);
	
	x = parmblock->pb_x;
	y = parmblock->pb_y;

	
	rect_3d(x-1,y-1,MOU_PIC_W+2,MOU_PIC_H+2);
	
	src.fd_addr = adr_pic_coul;
	src.fd_w = MOU_PIC_W;
	src.fd_h = MOU_PIC_H;
	src.fd_wdwidth = MOU_PIC_W / 16;
	src.fd_stand = 0;
	src.fd_nplanes = 16;

	/* Zone source dans l'image en m‚moire : */
	pxy[0] = 0;	
	pxy[1] = 0;	
	pxy[2] = MOU_PIC_W - 1;	
	pxy[3] = MOU_PIC_H - 1;	
	/* Zone destination dans la fenˆtre … l'‚cran */
	pxy[4] = x;							
	pxy[5] = y;							
	pxy[6] = pxy[4] + MOU_PIC_W - 1;	
	pxy[7] = pxy[5] + MOU_PIC_H - 1;	
		
		
	tnt_cpyfm(pxy, &src);
	
	set_clip(FALSE, &area);
	return 0;
}

int cdecl draw_time(PARMBLK *parmblock)
{
	GRECT area;
	int base_x,base_y,i,x;
	
	base_x = mou_var.adr_form[MOU_TEMPS].ob_x + mou_var.adr_form[MOU_FON_TEMPS].ob_x + mou_var.adr_form[0].ob_x + 4;
	base_y = mou_var.adr_form[MOU_TEMPS].ob_y + mou_var.adr_form[MOU_FON_TEMPS].ob_y + mou_var.adr_form[0].ob_y + 6;
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;

	set_clip(1, &area);
	
	Vsync();
	
	rect_full(base_x-4,base_y,310,20, color_3d1);
	
	ligne(base_x,base_y+10,300,1,WHITE);
	ligne(base_x+1,base_y+11,300,1,LBLACK);
	
	for(i=0; i<mvt[TEMP_MVT].nb_mvt_used;i++)
	{	x = (int)( ((long)pt_mvt[first_pt_mvt+i].time * 300L)/1000L);
		draw_marqueur(base_x + x ,base_y, i == cur_pt_edit );
	}
		
	set_clip(0, &area);
	return 0;
}

void draw_marqueur(int x, int y, char fill)
{	x -= 5;

	ligne(x,y,11,1,BLACK);
	ligne(x,y,5,5,BLACK);
	ligne(x+5,y+5,6,-4,BLACK);
	ligne(x+5,y+5,1,12,BLACK);
	if(fill)
	{	ligne(x+2,y+1,7,1,BLUE);
		ligne(x+3,y+2,5,1,BLUE);
		ligne(x+4,y+3,3,1,BLUE); 
		ligne(x+5,y+4,1,1,BLUE);
	}
	
}