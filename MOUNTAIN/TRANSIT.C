/********************MOUNTAIN********************/
/*****        GESTION DES TRANSITIONS       *****/
/*****            Par Vanel R‚mi            *****/
/*****         Premier code : 2/9/98        *****/
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
#include "pistes.h"
#include "txts.h"
#include "txtran.h"
#include "preview.h"
#include "image_AB.H"
#include "flh_lib.H"
#include "effets1.H"
#include "tran_mod.H"
#include "mountain.H"

#define TXT_DECAL 3		/*decalage du texte vers e haut */
#define pix2img(A) ((int)(( ((long)A-x_base)*(long)projet.zoom )/48L + deb_img))

void wait_mk(void);
int pop_fly(int index);
void form_col(int event);

extern PROJET projet;
extern int actu_projet;			/* is there any projet here ? */
extern int Magic_flag,Winix_flag;

extern int int_rel;
extern int use_nvdi;
extern int char_height;
extern WINDFORM_VAR itra_var;		/* info sur transition */
extern WINDFORM_VAR ptra_var;		/* para des transitions */
extern int temp_r,temp_v,temp_b;
extern int coul_caller;
extern int effet_parametrisation;
extern MFDB mfdb_efpre;
extern int *adr_ef_prev;

/* prj objet */
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern int hnd_wpistes;
extern int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
extern int ful_p;
extern int deb_img;		/* premiere img affiche */


int hnd_wtransit, attr_transit = MOVER|NAME|CLOSER|UPARROW|DNARROW|VSLIDE|SIZER;
int xft,yft,wft,hft;			/* coord fenetre */
int xftu,yftu,wftu,hftu;   /* coord utiles */
int max_wft;		/* largeur max */
int deb_transit=0;
int nb_transit=0;	/* nombre de transitions */
int nb_used_transit=0;	/* nombre de transitions UTILISEE */
int w_trsit_open=FALSE;
TRANSITION *transition[MAX_TRAN];
TOOL_BOX tool_box = {1};
PRJ_TRANSIT prj_transit[MAX_PRJ_TRAN];
static int selected_used_tran;
static int temp_time;
static TRAN_REAL_PARAM tempo_param;
static TRAN_REAL_PARAM saved_param;
static TRANSITION *selected_tran;
static TRAN_DESC_PARAM *selected_desc_param;
static int ptra_min_ass[3],ptra_max_ass[3];
static USERBLK user_set1_c;
static USERBLK user_set2_c;
static USERBLK user_tr_prev;
static int *adr_tr_prev;
static MFDB mfdb_trpre;
static int tr_prev_val=50;
static int prev_treel;
int x_trev,y_trev,x_tpremod16;

/* proto */
static void calc_best_tran_pos(int pos,int *pos_in,int *pos_out, int *a_vers_b);
static void load_transitions(void);
static int can_put_tran_here(int pos_in, int pos_out, int nonobstant);
static void add_transit(int type, int mode_para, int max_para, int def_para, const char *nom,
				const char *desc1, const char *desc2, int *adr_preview, long id, int no_ab_swap,int (*calc_tran)(TRAN_PARA *adr_para), TRAN_DESC_PARAM *tran_desc_param);
static int cdecl set1_color(PARMBLK *parmblock);
static int cdecl set2_color(PARMBLK *parmblock);
static int cdecl prev_color(PARMBLK *parmblock, int coul);
static int cdecl tr_prev(PARMBLK *parmblock);
int tran_PISA(TRAN_PARA *adr_para);
int tran_PISB(TRAN_PARA *adr_para);
int tran_FNDU(TRAN_PARA *adr_para);
static void scan_tr_param(void);
static void calc_n_draw_prev(int draw);
static void anim_tr_prev(void);
static void ass_prev(void);
static void widen_transit(int num_objet);
static void move_transit(int num_objet);
static void copy_transit(int src_obj, int pos);
static void set_ptra_ass(int num, long val, int draw);
static void get_anno(long id, int num_param);

/* asm */
void copy_intens_img(int *src, int *dst,int *para,int x, int y, int x_dst_to_skip);


int init_wtransit(int x, int y, int w, int h)
{	
	/* cood max utiles d'un fenetre */
	wind_calc(WC_WORK, attr_transit, x_desk, y_desk, w_desk, h_desk, &xftu, &yftu, &wftu, &hftu);

	/* on reduit a notre max */
	wftu = 320; 
	
	/* calcule les coord max de notre fenetre */
	wind_calc(WC_BORDER, attr_transit, xftu, yftu, wftu, hftu, &xft, &yft, &wft, &hft);
	
	max_wft = wft;
	
	/* reserve un espace m‚moire */
	if((hnd_wtransit = wind_create(attr_transit, xft, yft, wft, hft)) <= 0)
	{	super_alert(1,3,T_NO_MORE_WINDOWS);
		return 0;
	}
	
	/* fixe les infos */
	wind_set(hnd_wtransit, WF_NAME, T_TITRE_TRANSIT);
	wind_set(hnd_wtransit, WF_VSLIDE, 0, 0, 0, 0);
	wind_set(hnd_wtransit, WF_VSLSIZE, 1000, 0, 0, 0);
	wind_set(hnd_wtransit, WF_BEVENT,1,0,0,0);
	
	/* CENTRE SI PAS DE PRESET*/
	if(x == 0 && y == 0)
	{	xft = x_desk + (w_desk - wft)/2;
		hft = 250;
		yft = y_desk + (h_desk - hft)/2;
		hft = min((y_desk+h_desk)-yft,hft);
	}else
	{	wft = w;
		if(x+wft>x_desk+w_desk)
			xft = x_desk + (w_desk - wft)/2;
		else
			xft = x;
		if(y+h>y_desk+h_desk)
		{	hft = 250;
			yft = y_desk + (h_desk - hft)/2;
			hft = min((y_desk+h_desk)-yft,hft);
		}else
		{	yft = y;
			hft = h;
		}
	}

	user_set1_c.ub_code = set1_color;
	ptra_var.adr_form[TRSP_C1].ob_type = G_USERDEF;
	user_set1_c.ub_parm = (long) ptra_var.adr_form[TRSP_C1].ob_spec.userblk;
	ptra_var.adr_form[TRSP_C1].ob_spec.userblk = &user_set1_c;
	user_set2_c.ub_code = set2_color;
	ptra_var.adr_form[TRSP_C2].ob_type = G_USERDEF;
	user_set2_c.ub_parm = (long) ptra_var.adr_form[TRSP_C2].ob_spec.userblk;
	ptra_var.adr_form[TRSP_C2].ob_spec.userblk = &user_set2_c;

	user_tr_prev.ub_code = tr_prev;
	ptra_var.adr_form[TRSP_PREV].ob_type = G_USERDEF;
	user_tr_prev.ub_parm = (long) ptra_var.adr_form[TRSP_PREV].ob_spec.userblk;
	ptra_var.adr_form[TRSP_PREV].ob_spec.userblk = &user_tr_prev;
	
	load_transitions();
	slider_t();
	return 1;
}

int tran_PISA(TRAN_PARA *adr_para)
{	copy_tc2(adr_para->img_a, adr_para->img_dst, adr_para->y, adr_para->x, adr_para->larg_img_dst-adr_para->x,0);
	return 1;
}

int tran_PISB(TRAN_PARA *adr_para)
{	copy_tc2(adr_para->img_b, adr_para->img_dst, adr_para->y, adr_para->x, adr_para->larg_img_dst-adr_para->x,0);
	return 1;
}

int tran_FNDU(TRAN_PARA *adr_para)
{	copy_tc2(adr_para->img_a, adr_para->img_dst, adr_para->y, adr_para->x, adr_para->larg_img_dst-adr_para->x,0);
	
	if(adr_para->tran_real_param->mode_vari[0] == 2)		/* mode para image */
		copy_intens_img(adr_para->img_b, adr_para->img_dst,adr_para->tran_real_param->adr_img[0],adr_para->x,adr_para->y,adr_para->larg_img_dst-adr_para->x);
	else
		zoom(adr_para->img_b, adr_para->img_dst, adr_para->x,adr_para->y,adr_para->x,
				adr_para->x,adr_para->y,adr_para->larg_img_dst,(int)adr_para->uniq_para);
	return 1;
}

void unload_transitions(void)
{	int i;
	for (i=0;i<nb_transit;i++)
		free(transition[i]);
	nb_transit=0;
}

static void load_transitions(void)
{	nb_transit=0;
	add_transit(TYPE_INTERN,NO_PARAM,0,0,T_PISTE_A,TT_DESC_PA,"",0,'PISA',1,tran_PISA,0);
	add_transit(TYPE_INTERN,NO_PARAM,0,0,T_PISTE_B,TT_DESC_PB,"",0,'PISB',1,tran_PISB,0);
	add_transit(TYPE_INTERN,UNIQ_IMG_PARAM,1024,512,TT_FONDU,TT_DESC_F,"",0,'FNDU',0,tran_FNDU,0);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_ZBOX,TT_DESC_ZBOX1,TT_DESC_ZBOX2,0,'ZBOX',0,tran_ZBOX,&desc_param_zbox);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_ISQR,TT_ISQR_D1,TT_ISQR_D2,0,'ISQR',0,tran_ISQR,&desc_param_isqr);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_SLDE,TT_SLDE_D1,TT_SLDE_D2,0,'SLDE',0,tran_SLDE,&desc_param_slde);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_STRH,TT_STRH_D1,TT_STRH_D2,0,'STRH',0,tran_STRH,&desc_param_strh);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_PUSH,TT_PUSH_D1,TT_PUSH_D2,0,'PUSH',0,tran_PUSH,&desc_param_push);
	add_transit(TYPE_INTERN,STAND_PARAM,0,0,TT_PSHZ,TT_PSHZ_D1,TT_PSHZ_D2,0,'PSHZ',0,tran_PSHZ,&desc_param_pshz);
}

/* mettre 0 dans adr_preview si preview a calculer */
static void add_transit(int type, int mode_para, int max_para, int def_para, const char *nom,
								const char *desc1, const char *desc2, int *adr_preview, long id,
								int no_ab_swap,int 	(*calc_tran)(TRAN_PARA *adr_para), TRAN_DESC_PARAM *tran_desc_param)
{	TRAN_PARA tran_para;
	TRAN_REAL_PARAM tran_real_param;
	
	if(nb_transit>= MAX_TRAN)
	{	super_alert(1,2,"[ No memory to| add transition.|Please ctc the author.][ OK ]");
		return;
	}
		
	transition[nb_transit] = malloc(sizeof(TRANSITION) + 48*48*2 + 64);	/* pour y mettre la transition, la preview, et une marge */
	if(!transition[nb_transit])	return;
	
	transition[nb_transit]->type = type;	
	transition[nb_transit]->mode_para = mode_para;
	transition[nb_transit]->max_para = max_para;
	transition[nb_transit]->def_para = def_para;
	transition[nb_transit]->no_ab_swap = no_ab_swap;
	transition[nb_transit]->id = id;
	strcpy(transition[nb_transit]->nom,nom);
	strcpy(transition[nb_transit]->desc1,desc1);
	strcpy(transition[nb_transit]->desc2,desc2);
	transition[nb_transit]->calc_tran = calc_tran;
	transition[nb_transit]->tran_desc_param = tran_desc_param;
	if(adr_preview)
		copy(adr_preview,(int *)((char *)transition[nb_transit] + sizeof(TRANSITION) + 32), 48*48);
	else
	{	tran_para.img_a = image_A;
		tran_para.img_b = image_B;
		tran_para.img_dst = (int *)((char *)transition[nb_transit] + sizeof(TRANSITION) + 32);
		tran_para.x = 48;
		tran_para.y = 48;
		tran_para.larg_img_dst = 48;
		tran_para.adr_tool_box = &tool_box;
		tran_para.uniq_para = transition[nb_transit]->def_para;
		if(transition[nb_transit]->mode_para == 2)
			inst_def_para(&tran_real_param,transition[nb_transit]->tran_desc_param);
		tran_para.tran_real_param = &tran_real_param;
		
		calc_tran(&tran_para);
	}
	transition[nb_transit]->preview.fd_addr = (char *)transition[nb_transit] + sizeof(TRANSITION) + 32;
	transition[nb_transit]->preview.fd_w = 48;
	transition[nb_transit]->preview.fd_h = 48;
	transition[nb_transit]->preview.fd_wdwidth = 3;
	transition[nb_transit]->preview.fd_stand = 0;
	transition[nb_transit]->preview.fd_nplanes = 16;
	
	nb_transit++;
}

void inst_def_para(TRAN_REAL_PARAM *tran_real_param, TRAN_DESC_PARAM *tran_desc_param)
{	int i;
	for(i=0;i<3;i++)
	{	tran_real_param->time[i] = 0;
		tran_real_param->piste[i] = 0;
		if(tran_desc_param->def_vari[i] == -12345)	/* time spline */
		{	tran_real_param->vari[i] = (tran_desc_param->min_vari[i]+tran_desc_param->max_vari[i])/2;
			tran_real_param->mode_vari[i] = 1;
			tran_real_param->vari_def[i] = tran_real_param->vari[i];
		}else
		{	tran_real_param->vari[i] = tran_desc_param->def_vari[i];
			tran_real_param->vari_def[i] = tran_real_param->vari[i];
			tran_real_param->mode_vari[i] = 0;
		}
	}
	tran_real_param->color[0][0] = tran_desc_param->def_color[0][0];
	tran_real_param->color[0][1] = tran_desc_param->def_color[0][1];
	tran_real_param->color[0][2] = tran_desc_param->def_color[0][2];
	tran_real_param->color[1][0] = tran_desc_param->def_color[1][0];
	tran_real_param->color[1][1] = tran_desc_param->def_color[1][1];
	tran_real_param->color[1][2] = tran_desc_param->def_color[1][2];
	tran_real_param->switchs = tran_desc_param->def_switchs;
	tran_real_param->coords[0][0] = tran_desc_param->def_coords[0][0];
	tran_real_param->coords[0][1] = tran_desc_param->def_coords[0][1];
	tran_real_param->coords[1][0] = tran_desc_param->def_coords[1][0];
	tran_real_param->coords[1][1] = tran_desc_param->def_coords[1][1];
}

void window_transit(int event)
{	int old,obj;
	int xt,yt,wt,ht;
		
	if (event & MU_BUTTON && !modal_flag)
	{	if(wind_find(mousex,mousey) == hnd_wtransit && mousex>xftu && mousex <xftu+wftu && mousey > yftu && mousey <yftu+hftu)
		{	obj = (mousey-yftu)/OBJ_H + deb_transit;
			if(obj >= 0 && obj < nb_transit)
			{	/* on a cliqu‚ */
				put_transit(obj);
			}
		}
	}
	else if (event & MU_MESAG)
	{
		switch(buff[0])
		{
			case WM_REDRAW :
				wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
				redraw_transit(buff[3], buff[4], buff[5], buff[6], buff[7]);
				break;
			case WM_TOPPED :
				if(!modal_flag)
					wind_set(buff[3], WF_TOP);
				break;
			case WM_CLOSED : 
				if(!modal_flag)
				{	wind_close(hnd_wtransit);
					w_trsit_open=FALSE;
				}
				break;
			case WM_MOVED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7]);
				wind_get(hnd_wtransit,WF_CURRXYWH,&xft, &yft, &wft, &hft);
				wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
				break;
			case WM_SIZED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], min(max_wft,buff[6]), max(60,buff[7]));
				wind_get(hnd_wtransit,WF_CURRXYWH,&xft, &yft, &wft, &hft);
				xt = xftu; yt = yftu; wt = wftu; ht = hftu; 
				wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
				if(deb_transit > nb_transit - hftu/OBJ_H)
					deb_transit = max(0, nb_transit - hftu/OBJ_H);
				if(Magic_flag || Winix_flag)
				{	if(wt<wftu)		/* agrandissement hor */
						redraw_transit(hnd_wtransit, xt+wt-2, yt, 8, ht);
					if(ht<hftu)		/* agrandissement vert */
						redraw_transit(hnd_wtransit, xt, yt+ht-2, wt, 8);
				}
				slider_t();
				break;
			case WM_ARROWED :
				wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
				old = deb_transit;
				if(buff[4] == WA_UPLINE && deb_transit)
					deb_transit--;
				else if(buff[4] == WA_DNLINE && nb_transit > (hftu/OBJ_H + deb_transit) )
					deb_transit++;
				else if(buff[4] == WA_UPPAGE)
					deb_transit = max(0,deb_transit - hftu/OBJ_H);
				else if(buff[4] == WA_DNPAGE)
				{	deb_transit = deb_transit + hftu/OBJ_H;
					if(deb_transit > nb_transit - hftu/OBJ_H)
						deb_transit = max(0, nb_transit - hftu/OBJ_H);
				}
				
				if(old != deb_transit)
				{	slider_t();
					redraw_transit(hnd_wtransit, xft, yft, wft, hft);
				}
				break;
			case WM_VSLID :
				old = deb_transit;
				deb_transit = ((nb_transit -(hftu/OBJ_H)) * buff[4])/ 1000;
				if(old != deb_transit)
				{	wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
					slider_t();
					redraw_transit(hnd_wtransit, xft, yft, wft, hft);
				}
 				break;
			
		}
	}
}

void slider_t(void)
{	int y_ass,h_ass;
	
	if(hftu/OBJ_H >= nb_transit)
	{	wind_set(hnd_wtransit, WF_VSLIDE, 0, 0, 0, 0);
		wind_set(hnd_wtransit, WF_VSLSIZE, 1000, 0, 0, 0);
	}else
	{	h_ass = ((hftu/OBJ_H) * 1000) / nb_transit;
		y_ass = (deb_transit * 1000) / (nb_transit-(hftu/OBJ_H));
		wind_set(hnd_wtransit, WF_VSLSIZE, h_ass, 0, 0, 0);
		wind_set(hnd_wtransit, WF_VSLIDE, y_ass, 0, 0, 0);
	}
}	

void gere_clic_tran(int obj, int mousex)
{	int choix,tran,fin_x;

	tran = find_transit(prj_transit[obj].id);
	if(tran==-1)
	{	super_alert(1,3,T_NO_MATCH_TRAN);
		return;
	}

	fin_x = img_to_pix(prj_transit[obj].pos_out+1,xfpu + 18);	/* le dernier pixel */
	if(nbr_clics == 1)
	{	if(mousex>fin_x-8 && mousex<fin_x)
			widen_transit(obj);
		else
			move_transit(obj);
	}if(nbr_clics == 2)		/* dble click, pop_tools */
	{	choix=pop_fly(POP_TRANSIT);
		if(choix == PTRS_EFF)
		{	if(obj < nb_used_transit-1) /* c'est pas le dernier */
				memcpy(&prj_transit[obj], &prj_transit[obj+1], (nb_used_transit - obj - 1) * sizeof(PRJ_TRANSIT));
			nb_used_transit--;
		
		/* effacemnt ds la fenetre piste */
			redraw_une_piste(2);
		}
		
		selected_used_tran = obj;
		if(choix == PTRS_INFO)
			form_itra(OPEN_DIAL);
		else if(choix == PTRS_PARA && !transition[tran]->mode_para == 0)
			form_ptra(OPEN_DIAL);
	}	
}

static void move_transit(int num_objet)
{	int mx,my,mk,mkey,dum,wind,old,len,decale_pos,x_base = xfpu+18;
	int pos,old_pos=-1,old_pos_out=-1,pos_out,very_old_pos,larg;
	int piste, preview = 0;	/* preview=1 si il y avait une preview */
	GRECT r;
	char move,old_move=99;

	graf_mouse(FLAT_HAND,0);
	set_clip(0,&r);
	len = prj_transit[num_objet].pos_out - prj_transit[num_objet].pos_in +1;
	larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);
	
	graf_mkstate(&mx,&my,&mk,&dum);
	decale_pos = prj_transit[num_objet].pos_in - max(0,pix2img(mx));
	
	very_old_pos = prj_transit[num_objet].pos_in;
	
	do
	{	graf_mkstate(&mx,&my,&mk,&mkey);
		if(mkey & K_LSHIFT)		/* copy au lieu de dep */
			move = -1;
		else move = num_objet;
		
		wind = wind_find(mx,my);
		old=deb_img;
		
		if(wind == hnd_wpistes)
		{	if(wtop != hnd_wpistes)
			{	wind_set(hnd_wpistes, WF_TOP);
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				wtop = hnd_wpistes;
			}else if(mx>xfpu && mx<xfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img)	/* scrool << */
			{	if(preview)
				{	aff_prev(2, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img+larg<projet.lenght)	/* scrool >> */
			{	if(preview)
				{	aff_prev(2, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				deb_img += projet.zoom;
				if(deb_img != old)
				{	v_hide_c(vdi_handle);
					scrool_right(old);
					v_show_c(vdi_handle,1);
				}
			}else if(find_pos_piste(mx,my,&piste,&pos))
			{	pos += decale_pos;
				pos = max(0,pos);
				if(pos+len>projet.lenght)
					pos = (int)projet.lenght-len;
				if(pos != old_pos || old_move != move)
				{	pos_out = pos + len;
				
					if(preview)
					{	Vsync();
						aff_prev(2, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
						preview=0;
					}
					if(piste == 2)	/* uniquement sur la piste transition ! */
					{
						old_pos = pos;
						old_pos_out = pos_out;
						old_move = move;
					
						if( can_put_tran_here(pos,pos+len, move))
						{	aff_prev(2, pos, pos_out);	/* affichage de la preview */
							preview=1;
							set_curseur(pos);
						}
					}
				}
			}else if(preview)
			{	aff_prev(2, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	aff_prev(2, old_pos, pos_out);	/* effacement de l'ancienne preview */
		if(move == -1)	/* copy */
		{	copy_transit(num_objet,pos);
			redraw_une_piste(2);
		}
		else
		{	prj_transit[num_objet].pos_in = pos;
			prj_transit[num_objet].pos_out = pos + len - 1;
			if(pos != very_old_pos)
				redraw_une_piste(2);
		}
	}

	graf_mouse(ARROW,0);
}

static void copy_transit(int src_obj, int pos)
{	int obj = nb_used_transit, len;
	
	len = prj_transit[src_obj].pos_out - prj_transit[src_obj].pos_in;
	
	prj_transit[obj].pos_in = pos;
	prj_transit[obj].pos_out = pos + len;
	prj_transit[obj].id = prj_transit[src_obj].id;
	prj_transit[obj].a_vers_b = prj_transit[src_obj].a_vers_b;
	prj_transit[obj].aff_time = prj_transit[src_obj].aff_time;
	prj_transit[obj].tran_real_param.vari[0] = prj_transit[src_obj].tran_real_param.vari[0];
	prj_transit[obj].tran_real_param.vari[1] = prj_transit[src_obj].tran_real_param.vari[1];
	prj_transit[obj].tran_real_param.vari[2] = prj_transit[src_obj].tran_real_param.vari[2];
	prj_transit[obj].tran_real_param.time[0] = prj_transit[src_obj].tran_real_param.time[0];
	prj_transit[obj].tran_real_param.time[1] = prj_transit[src_obj].tran_real_param.time[1];
	prj_transit[obj].tran_real_param.time[2] = prj_transit[src_obj].tran_real_param.time[2];
	prj_transit[obj].tran_real_param.vari_def[0] = prj_transit[src_obj].tran_real_param.vari_def[0];
	prj_transit[obj].tran_real_param.vari_def[1] = prj_transit[src_obj].tran_real_param.vari_def[1];
	prj_transit[obj].tran_real_param.vari_def[2] = prj_transit[src_obj].tran_real_param.vari_def[2];
	prj_transit[obj].tran_real_param.mode_vari[0] = prj_transit[src_obj].tran_real_param.mode_vari[0];
	prj_transit[obj].tran_real_param.mode_vari[1] = prj_transit[src_obj].tran_real_param.mode_vari[1];
	prj_transit[obj].tran_real_param.mode_vari[2] = prj_transit[src_obj].tran_real_param.mode_vari[2];
	prj_transit[obj].tran_real_param.piste[0] = prj_transit[src_obj].tran_real_param.piste[0];
	prj_transit[obj].tran_real_param.piste[1] = prj_transit[src_obj].tran_real_param.piste[1];
	prj_transit[obj].tran_real_param.piste[2] = prj_transit[src_obj].tran_real_param.piste[2];
	prj_transit[obj].tran_real_param.color[0][0] = prj_transit[src_obj].tran_real_param.color[0][0];
	prj_transit[obj].tran_real_param.color[0][1] = prj_transit[src_obj].tran_real_param.color[0][1];
	prj_transit[obj].tran_real_param.color[0][2] = prj_transit[src_obj].tran_real_param.color[0][2];
	prj_transit[obj].tran_real_param.color[1][0] = prj_transit[src_obj].tran_real_param.color[1][0];
	prj_transit[obj].tran_real_param.color[1][1] = prj_transit[src_obj].tran_real_param.color[1][1];
	prj_transit[obj].tran_real_param.color[1][2] = prj_transit[src_obj].tran_real_param.color[1][2];
	prj_transit[obj].tran_real_param.switchs = prj_transit[src_obj].tran_real_param.switchs;
	prj_transit[obj].tran_real_param.coords[0][0] = prj_transit[src_obj].tran_real_param.coords[0][0];
	prj_transit[obj].tran_real_param.coords[0][1] = prj_transit[src_obj].tran_real_param.coords[0][1];
	prj_transit[obj].tran_real_param.coords[1][0] = prj_transit[src_obj].tran_real_param.coords[1][0];
	prj_transit[obj].tran_real_param.coords[1][1] = prj_transit[src_obj].tran_real_param.coords[1][1];
	nb_used_transit++;
}

static void widen_transit(int num_objet)
{	int mx,my,mk,mkey,dum,wind,old,decale_pos,x_base = xfpu+18;
	int pos_in,pos,old_pos=-1,very_old_pos;
	int preview = 0;	/* preview=1 si il y avait une preview */
	GRECT r;
	
	graf_mouse(POINT_HAND,0);
	set_clip(0,&r);
	
	graf_mkstate(&mx,&my,&mk,&dum);
	decale_pos = prj_transit[num_objet].pos_out - max(0,pix2img(mx)) + 1;
	
	pos_in = prj_transit[num_objet].pos_in;
	very_old_pos = prj_transit[num_objet].pos_out;
	
	do
	{	graf_mkstate(&mx,&my,&mk,&mkey);
		
		wind = wind_find(mx,my);
		old=deb_img;
		
		if(wind == hnd_wpistes)
		{	if(wtop != hnd_wpistes)
			{	wind_set(hnd_wpistes, WF_TOP);
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				wtop = hnd_wpistes;
			}else if(mx>xfpu && mx<xfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img)	/* scrool << */
			{	if(preview)
				{	aff_prev(2, pos_in, old_pos);	/* effacement de l'ancienne preview */
					preview=0;
				}
				Vsync();				Vsync();
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu)	/* scrool >> */
			{	if(preview)
				{	aff_prev(2, pos_in, old_pos);	/* effacement de l'ancienne preview */
					preview=0;
				}
				deb_img += projet.zoom;
				Vsync();				Vsync();
				if(deb_img != old)
				{	v_hide_c(vdi_handle);
					scrool_right(old);
					v_show_c(vdi_handle,1);
				}
			}else if(find_pos_piste(mx,my,&dum,&pos))
			{	pos += decale_pos;
				pos = max(pos_in+2,pos);
				pos = min(pos,(int)projet.lenght);
				if(pos != old_pos)
				{	
					if(preview)
					{	Vsync();
						aff_prev(2, pos_in, old_pos);	/* effacement de l'ancienne preview */
						preview=0;
					}

					old_pos = pos;
				
					if( can_put_op_here(pos_in,pos,2, num_objet))
					{	aff_prev(2, pos_in, pos);	/* affichage de la preview */
						preview=1;
						set_curseur(pos-pos_in);
					}
				}
			}else if(preview)
			{	aff_prev(2, pos_in, pos);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	aff_prev(2, pos_in, pos);	/* effacement de l'ancienne preview */
		prj_transit[num_objet].pos_out = pos-1;
		if(pos-1 != very_old_pos)
			redraw_une_piste(2);
	}

	graf_mouse(ARROW,0);
}

int find_transit(long id)
{	int i;

	for(i=0;i<nb_transit;i++)
	{	if(id == transition[i]->id)
			return i;
	}
	return -1;
}

void put_transit(int num_objet)
{	int mx,my,mk,dum,wind,larg,old,a_vers_b;
	int pos,old_pos=-1,old_pos_out=-1,pos_out,pos_in,old_pos_in=-1;
	int piste, old_piste=-1, preview = 0;	/* preview=1 si il y avait une preview */
	GRECT r;
	
	graf_mouse(FLAT_HAND,0);
	set_clip(0,&r);
	larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);

	do
	{	graf_mkstate(&mx,&my,&mk,&dum);
		wind = wind_find(mx,my);
		old=deb_img;
		
		if(wind == hnd_wpistes)
		{	if(wtop != hnd_wpistes)
			{	wind_set(hnd_wpistes, WF_TOP);
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
				wtop = hnd_wpistes;
			}else if(mx>xfpu && mx<xfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img)	/* scrool << */
			{	if(preview)
				{	aff_prev(2, old_pos_in, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img+larg<projet.lenght)	/* scrool >> */
			{	if(preview)
				{	aff_prev(2, old_pos_in, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				deb_img += projet.zoom;
				if(deb_img > (int)projet.lenght - larg + projet.zoom)	/* on ajoute une marge pour voir la fin */
					deb_img = max(0, (int)projet.lenght - larg + projet.zoom);
				if(deb_img != old)
				{	v_hide_c(vdi_handle);
					scrool_right(old);
					v_show_c(vdi_handle,1);
				}
			}else if(find_pos_piste(mx,my,&piste,&pos))
			{	if(pos != old_pos || piste != old_piste)
				{	old_piste = piste;
					old_pos = pos;	
					if(preview)
					{	Vsync();
						aff_prev(2, old_pos_in, old_pos_out);	/* effacement de l'ancienne preview */
						preview=0;
					}
					if(piste == 2 && pos<projet.lenght)	/* uniquement sur la piste transition ! */
					{	
						calc_best_tran_pos(pos,&pos_in,&pos_out, &a_vers_b);
						if(can_put_tran_here(pos_in,pos_out,-1))
						{	old_pos_in = pos_in;
							old_pos_out = pos_out;
					
							aff_prev(2, pos_in, pos_out);		/* affichage de la preview */
							preview=1;
						}
					}
				}
			}else if(preview)
			{	aff_prev(2, old_pos_in, old_pos_out);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	if(nb_used_transit < MAX_PRJ_TRAN)
		{	aff_prev(2, old_pos_in, pos_out);				/* effacement de l'ancienne preview */
			prj_transit[nb_used_transit].id = transition[num_objet]->id;
			prj_transit[nb_used_transit].pos_in = pos_in;
			prj_transit[nb_used_transit].pos_out = pos_out-1;
			prj_transit[nb_used_transit].a_vers_b = a_vers_b;
			switch(transition[num_objet]->mode_para)
			{	case 0 :	prj_transit[nb_used_transit].aff_time = -1; /* pas de para */
						break;
				case 3 :
				case 1 : prj_transit[nb_used_transit].aff_time = 0;
							prj_transit[nb_used_transit].tran_real_param.time[0] = 0;
							prj_transit[nb_used_transit].tran_real_param.mode_vari[0] = 1;
							prj_transit[nb_used_transit].tran_real_param.vari_def[0] = (int)transition[num_objet]->def_para;
							break;
				case 2 : prj_transit[nb_used_transit].aff_time = 0;
							inst_def_para(&prj_transit[nb_used_transit].tran_real_param, transition[num_objet]->tran_desc_param);
							break;
			}
			nb_used_transit++;
			redraw_une_piste(2);
		}else
			super_alert(1,2,"[ No memory to| add transition.|Please ctc the author.][ OK ]");
	}

	graf_mouse(ARROW,0);
}

static int can_put_tran_here(int pos_in, int pos_out, int nonobstant)
{	int i,found;
	
	for(i=pos_in;i<pos_out;i++)
	{	found = find_tran(i);
		if( found != -1 && found != nonobstant)
			return 0;
	}
	return 1;
}

static void calc_best_tran_pos(int pos,int *pos_in,int *pos_out, int *a_vers_b)
{	int i,obj_a,obj_b,temp;
	
	/* trouve la borne debut */
	i=pos;
	obj_a = find_anim(i,0);		/* check piste A */
	obj_b = find_anim(i,1);		/* check piste B */
	do
	{	temp = find_anim(i,0);	/* check piste A */
		if(temp != obj_a)
		{	i++; *a_vers_b=1; break; }
		temp = find_anim(i,1);	/* check piste B */
		if(temp != obj_b)
		{	i++; *a_vers_b=0; break; }
		
		if(find_tran(i)!=-1)
		{	i++;	break; }
		if(i>0) i--; 
		else
		{	if(obj_a!=-1)	*a_vers_b=1;
			break;
		}
	}while(1);
	*pos_in=i;
	
	/* trouve la borne fin */
	i=pos+1;
	obj_a = find_anim(i,0);	/* check piste A */
	obj_b = find_anim(i,1);	/* check piste B */
	do
	{	temp = find_anim(i,0);	/* check piste A */
		if(temp != obj_a)
			break;
		temp = find_anim(i,1);	/* check piste B */
		if(temp != obj_b)
			break;
		i++;
		if(find_tran(i)!=-1)
			break;
		if(i>=projet.lenght)
		{	i = (int)projet.lenght;
			break;
		}
	}while(1);
	*pos_out=i;

}

void form_itra(int event)
{
	WINDFORM_VAR *ptr_var = &itra_var;
	int choix,transit,dum,dum2;
	
	if (event == OPEN_DIAL)
	{	transit=find_transit(prj_transit[selected_used_tran].id);
		strcpy(itra_var.adr_form[TRSI_NOM].ob_spec.tedinfo->te_ptext,transition[transit]->nom);
	
		temp_time=prj_transit[selected_used_tran].aff_time;
		if(temp_time != -1)
		{	copy_spline_name(temp_time, itra_var.adr_form[TRSI_SPLINE].ob_spec.tedinfo->te_ptext);
			itra_var.adr_form[TRSI_AFFSPLINE].ob_state = SELECTED;
		}else
		{	temp_time = 0;
			copy_spline_name(temp_time, itra_var.adr_form[TRSI_SPLINE].ob_spec.tedinfo->te_ptext);
			itra_var.adr_form[TRSI_AFFSPLINE].ob_state = NORMAL;
		}
		itoa(prj_transit[selected_used_tran].pos_in,itra_var.adr_form[TRSI_DEP].ob_spec.tedinfo->te_ptext,10);
		itoa(prj_transit[selected_used_tran].pos_out,itra_var.adr_form[TRSI_FIN].ob_spec.tedinfo->te_ptext,10);

		itra_var.adr_form[TRSI_SENS].ob_state = NORMAL;
		if(prj_transit[selected_used_tran].a_vers_b)
			itra_var.adr_form[TRSI_SENS].ob_state = SELECTED;
		if(transition[transit]->no_ab_swap)
			itra_var.adr_form[TRSI_SENS].ob_state |= DISABLED;
		else
			itra_var.adr_form[TRSI_SENS].ob_state &= ~DISABLED;
		open_dialog(ptr_var, T_TITR_TRAN_INFO, TRSI_DEP, -1);
	}

	else 
	{	choix = windial_do(ptr_var, event);
	 	if(choix == CLOSE_DIAL)
	 		choix = TRSI_ANN;
		if (choix != 0) 
		{	switch(choix)
			{	
					
				case TRSI_SPLINE:
					retour_index = &temp_time;
					retour_wform = ptr_var;
					retour_objet = TRSI_SPLINE;
					form_ctim(OPEN_DIAL);
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSI_OK:
					dum = atoi(itra_var.adr_form[TRSI_DEP].ob_spec.tedinfo->te_ptext);
					dum2 = atoi(itra_var.adr_form[TRSI_FIN].ob_spec.tedinfo->te_ptext);
					if(can_put_tran_here(dum,dum2,selected_used_tran))
					{	prj_transit[selected_used_tran].pos_in = dum;
						prj_transit[selected_used_tran].pos_out = dum2;
					}
					if(itra_var.adr_form[TRSI_AFFSPLINE].ob_state & SELECTED)
						prj_transit[selected_used_tran].aff_time = temp_time;
					else	
						prj_transit[selected_used_tran].aff_time = -1;
					prj_transit[selected_used_tran].a_vers_b = itra_var.adr_form[TRSI_SENS].ob_state & SELECTED;
					redraw_une_piste(2);
				case TRSI_ANN:
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;

			}
		}
	}
}

void kill_used_transit(void)
{	
	nb_used_transit=0;
}

void copy_tran_param(TRAN_REAL_PARAM *scr,TRAN_REAL_PARAM *dst)
{	int i;
	for(i=0;i<3;i++)
	{	dst->vari[i] = scr->vari[i];
		dst->time[i] = scr->time[i];
		dst->vari_def[i] = scr->vari_def[i];
		dst->mode_vari[i] = scr->mode_vari[i];
		dst->piste[i] = scr->piste[i];
	}
	dst->color[0][0] = scr->color[0][0];
	dst->color[0][1] = scr->color[0][1];
	dst->color[0][2] = scr->color[0][2];
	dst->color[1][0] = scr->color[1][0];
	dst->color[1][1] = scr->color[1][1];
	dst->color[1][2] = scr->color[1][2];
	dst->switchs = scr->switchs;
	dst->coords[0][0] = scr->coords[0][0];
	dst->coords[0][1] = scr->coords[0][1];
	dst->coords[1][0] = scr->coords[1][0];
	dst->coords[1][1] = scr->coords[1][1];
}

static void set_ptra_ass(int num, long val, int draw)
{	ptra_var.adr_form[TRSP_ASS1+num*10].ob_x = (int)( 
		((val-ptra_min_ass[num])*(ptra_var.adr_form[TRSP_FON1+num*10].ob_width - 
		ptra_var.adr_form[TRSP_ASS1+num*10].ob_width))/(ptra_max_ass[num]-ptra_min_ass[num]));
	itoa((int)val,ptra_var.adr_form[TRSP_ASS1+num*10].ob_spec.tedinfo->te_ptext,10);
	if(draw)
	{ Vsync();	wf_draw(&ptra_var,TRSP_FON1+num*10); }
}

void set_ptra_prev_ass(int val, int draw)
{	ptra_var.adr_form[TRSP_PREV_ASS].ob_x = 
		(val*(TR_PREV_W-ptra_var.adr_form[TRSP_PREV_ASS].ob_width))/100;
	itoa(val,ptra_var.adr_form[TRSP_PREV_ASS].ob_spec.tedinfo->te_ptext,10);
	if(draw)
	{ 	Vsync();	
		objc_draw(ptra_var.adr_form,TRSP_PREV_FON,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
	}
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

	scan_tr_param(); /* pour la preview */

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
				{	tr_prev_val=nb;
					calc_n_draw_prev(1);
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
	
	tr_prev_val=nb;
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

	scan_tr_param(); /* pour la preview */

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
		}else
		{	if(updt == 3)
			{	if(prev_treel)
				{	tempo_param.vari_def[num] = nb;
					calc_n_draw_prev(1);
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

void choose_para_img(int num, TRAN_REAL_PARAM *tempo_param)
{	char *les_texts,str[8];
	int i,nb=2,retour;
	
	les_texts = malloc(152*20L);
	if(!les_texts)
	{	super_alert(1,3,T_NO_MEMORY);
		return;
	}
	textuate_p(0,str);
	sprintf(les_texts,T_PISTE,str);
	textuate_p(1,str);
	sprintf(les_texts+20,T_PISTE,str);
	for(i=0;i<projet.nb_sup_A;i++)
	{	textuate_p(i+50,str);
		sprintf(les_texts+(nb++)*20,T_PISTE,str);
	}for(i=0;i<projet.nb_sup_B;i++)
	{	textuate_p(i+100,str);
		sprintf(les_texts+(nb++)*20,T_PISTE,str);
	}for(i=0;i<projet.nb_sup_G;i++)
	{	textuate_p(i+150,str);
		sprintf(les_texts+(nb++)*20,T_PISTE,str);
	}
	
	retour=super_popup(&ptra_var,TRSP_IMG1+num*10,CHOIX,0,les_texts,20,nb)-1;
	free(les_texts);
	nb=2;
	if(retour == 0 || retour == 1)
		tempo_param->piste[num]=retour;
	else
	{	for(i=0;i<projet.nb_sup_A;i++)
		{	if(retour == nb)
			{	tempo_param->piste[num]=i+50;
				return;
			}else nb++;
		}
		for(i=0;i<projet.nb_sup_B;i++)
		{	if(retour == nb)
			{	tempo_param->piste[num]=i+100;
				return;
			}else nb++;
		}
		for(i=0;i<projet.nb_sup_G;i++)
		{	if(retour == nb)
			{	tempo_param->piste[num]=i+150;
				return;
			}else nb++;
		}
	}
}

void get_coords(int num, TRAN_REAL_PARAM *tempo_param)
{	int mx,my,mk,dum,cx,cy,oldx=-1,oldy=-1,updt=0;

	wait_mk();
	graf_mkstate(&mx, &my, &mk,&dum); 
	do  
	{	mx-=ptra_var.adr_form[0].ob_x;
		my-=ptra_var.adr_form[0].ob_y;
		if(mx>ptra_var.adr_form[TRSP_PREV].ob_x && mx<ptra_var.adr_form[TRSP_PREV].ob_x+x_trev
			&& my>ptra_var.adr_form[TRSP_PREV].ob_y && my<ptra_var.adr_form[TRSP_PREV].ob_y+y_trev )
		{	mx-=ptra_var.adr_form[TRSP_PREV].ob_x;
			my-=ptra_var.adr_form[TRSP_PREV].ob_y;
			cx = (int)( ((long)mx*1000L)/x_trev);
			cy = (int)( ((long)my*1000L)/y_trev);
			if(cx != oldx || cy != oldy)
			{	sprintf(ptra_var.adr_form[TRSP_CDX1+num*4].ob_spec.tedinfo->te_ptext,"X: %d %s",cx/10,"%");
				sprintf(ptra_var.adr_form[TRSP_CDY1+num*4].ob_spec.tedinfo->te_ptext,"Y: %d %s",cy/10,"%");
				Vsync();
				wf_draw(&ptra_var,TRSP_CDX1+num*4);
				wf_draw(&ptra_var,TRSP_CDY1+num*4);
				oldx=cx;
				oldy=cy;
				updt=0;
			}else
			{	if(updt == 4)
				{	if(prev_treel)
					{	tempo_param->coords[num][0] = cx;
						tempo_param->coords[num][1] = cy;
						if(effet_parametrisation)
							calc_eff_prev(1);
						else
							calc_n_draw_prev(1);
					}
					updt++;
				}else if(updt<4)
				{	updt++;
					Vsync();
				}
			}
		}
		graf_mkstate(&mx, &my, &mk,&dum); 
	}while(mk==0);
	
	mx-=ptra_var.adr_form[0].ob_x;
	my-=ptra_var.adr_form[0].ob_y;
	if(mx>ptra_var.adr_form[TRSP_PREV].ob_x && mx<ptra_var.adr_form[TRSP_PREV].ob_x+x_trev
			&& my>ptra_var.adr_form[TRSP_PREV].ob_y && my<ptra_var.adr_form[TRSP_PREV].ob_y+y_trev )
	{	tempo_param->coords[num][0] = cx;
		tempo_param->coords[num][1] = cy;
	}
	sprintf(ptra_var.adr_form[TRSP_CDX1+num*4].ob_spec.tedinfo->te_ptext,"X: %d %s",tempo_param->coords[num][0]/10,"%");
	sprintf(ptra_var.adr_form[TRSP_CDY1+num*4].ob_spec.tedinfo->te_ptext,"Y: %d %s",tempo_param->coords[num][1]/10,"%");
	wf_draw(&ptra_var,TRSP_CDX1+num*4);
	wf_draw(&ptra_var,TRSP_CDY1+num*4);
	
}

void form_ptra(int event)
{
	WINDFORM_VAR *ptr_var = &ptra_var;
	int choix,i,j;
	char str[8];
	
	if (event == OPEN_DIAL)
	{	effet_parametrisation = FALSE;
		
		i = find_transit(prj_transit[selected_used_tran].id);
		
		selected_tran = transition[i];
		selected_desc_param = selected_tran->tran_desc_param;
		copy_tran_param(&prj_transit[selected_used_tran].tran_real_param,&tempo_param);
		copy_tran_param(&prj_transit[selected_used_tran].tran_real_param,&saved_param);
		
				
		if(projet.x>projet.y)
		{	x_trev = TR_PREV_W;
			y_trev = (int)(((long)TR_PREV_H*projet.y)/projet.x);
		}else
		{	y_trev = TR_PREV_H;
			x_trev = (int)(((long)TR_PREV_W*projet.x)/projet.y);
		}
		x_tpremod16 = (x_trev+15)/16;
		x_tpremod16 *= 16;

		
		adr_tr_prev = Malloc( (long)x_tpremod16 * (y_trev+1) * 2L);
	
		/* on construit la MFDB */
		mfdb_trpre.fd_addr = adr_tr_prev;
		mfdb_trpre.fd_w = x_tpremod16;
		mfdb_trpre.fd_h = y_trev;
		mfdb_trpre.fd_wdwidth = x_tpremod16/16;
		mfdb_trpre.fd_stand = 0;
		mfdb_trpre.fd_nplanes = 16;
		set_ptra_prev_ass(tr_prev_val,0);
		calc_n_draw_prev(0);
		
			
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
		
		if(selected_tran->mode_para == 1 || selected_tran->mode_para == 3)		/* un seul para */
		{	for(j=0;j<8;j++)
				ptra_var.adr_form[TRSP_PT1+j].ob_flags &= ~HIDETREE;
			if(selected_tran->mode_para == 3)
			{	ptra_var.adr_form[TRSP_SIMG1].ob_flags &= ~HIDETREE;
				ptra_var.adr_form[TRSP_IMG1].ob_flags &= ~HIDETREE;
				textuate_p(tempo_param.piste[0],str);
				sprintf(ptra_var.adr_form[TRSP_IMG1].ob_spec.tedinfo->te_ptext,T_PISTE,str);
			}
			strcpy(ptra_var.adr_form[TRSP_PT1].ob_spec.tedinfo->te_ptext,selected_tran->nom);
			strupr(ptra_var.adr_form[TRSP_PT1].ob_spec.tedinfo->te_ptext);
			ptra_min_ass[0] = 0;
			ptra_max_ass[0] = selected_tran->max_para;
			set_ptra_ass(0,tempo_param.vari_def[0],FALSE);
			ptra_var.adr_form[TRSP_PF1].ob_state = NORMAL;
			ptra_var.adr_form[TRSP_PTS1].ob_state = NORMAL;
			ptra_var.adr_form[TRSP_SIMG1].ob_state = NORMAL;
			if(tempo_param.mode_vari[0] == 0)
				ptra_var.adr_form[TRSP_PF1].ob_state = SELECTED;
			else if(tempo_param.mode_vari[0] == 1)
				ptra_var.adr_form[TRSP_PTS1].ob_state = SELECTED;
			else ptra_var.adr_form[TRSP_SIMG1].ob_state = SELECTED;
			copy_spline_name(tempo_param.time[0], ptra_var.adr_form[TRSP_TSPL1].ob_spec.tedinfo->te_ptext);
		}else												/* mode multipara */
		{	/* CREATION DES VARIABLES */
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
		}
		
		open_dialog(ptr_var, selected_tran->nom, 0, -1);
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
					{	scan_tr_param();
						calc_n_draw_prev(1);
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
					get_anno(prj_transit[selected_used_tran].id,i);
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
					coul_caller=1;
					temp_r=tempo_param.color[0][0]; temp_v=tempo_param.color[0][1]; temp_b=tempo_param.color[0][2];
					form_col(OPEN_DIAL);
					break;
				case TRSP_C2:
					coul_caller=2;
					temp_r=tempo_param.color[1][0]; temp_v=tempo_param.color[1][1]; temp_b=tempo_param.color[1][2];
					form_col(OPEN_DIAL);
					break;
				case TRSP_ANIM:
					anim_tr_prev();
					wf_change(ptr_var,choix,0,1);
					break;
				case TRSP_PREV_ASS:
					ass_prev();
					break;
				case TRSP_PREV:
					scan_tr_param();
					calc_n_draw_prev(1);
					break;
				case TRSP_OK:
					scan_tr_param();
					copy_tran_param(&tempo_param, &prj_transit[selected_used_tran].tran_real_param);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					Mfree(adr_tr_prev);
					end_preview();
					redraw_une_piste(2);
					break;
				case TRSP_ANN:
					copy_tran_param(&saved_param,&prj_transit[selected_used_tran].tran_real_param);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					Mfree(adr_tr_prev);
					end_preview();
					break;

			}
		}
	}
}

static void anim_tr_prev(void)
{	int frame,pcent,larg_tran;
	int dum,mk,key;
	scan_tr_param();
	larg_tran = prj_transit[selected_used_tran].pos_out - prj_transit[selected_used_tran].pos_in;
	copy_tran_param(&tempo_param,&prj_transit[selected_used_tran].tran_real_param);
	for(frame = prj_transit[selected_used_tran].pos_in; frame<=prj_transit[selected_used_tran].pos_out;frame++)
	{	if(adr_tr_prev)
			calc_frame(frame, adr_tr_prev,  x_trev, y_trev, x_tpremod16);
		pcent = ((frame-prj_transit[selected_used_tran].pos_in) * 100)/larg_tran;
		set_ptra_prev_ass(pcent,1);
		Vsync();
		objc_draw(ptra_var.adr_form,TRSP_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		graf_mkstate(&dum, &dum, &mk,&key);
		if(mk == 2 || key)
			goto fin;

	}
	fin:;
	set_ptra_prev_ass(tr_prev_val,1);
	calc_n_draw_prev(1);
}

static void calc_n_draw_prev(int draw)
{	int frame,larg_tran;
	larg_tran = prj_transit[selected_used_tran].pos_out - prj_transit[selected_used_tran].pos_in;

	copy_tran_param(&tempo_param,&prj_transit[selected_used_tran].tran_real_param);
	
	frame = prj_transit[selected_used_tran].pos_in + (tr_prev_val*larg_tran)/100;
	
	if(adr_tr_prev)
		calc_frame(frame, adr_tr_prev, x_trev, y_trev, x_tpremod16);
	if(draw)
		objc_draw(ptra_var.adr_form,TRSP_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
}

static void scan_tr_param(void)
{	int i,j;
	for(i=0;i<3;i++)
	{	if(ptra_var.adr_form[TRSP_PF1+i*10].ob_state == SELECTED)
			tempo_param.mode_vari[i] = 0;
		else if(ptra_var.adr_form[TRSP_PTS1+i*10].ob_state == SELECTED)
			tempo_param.mode_vari[i] = 1;
		else
			tempo_param.mode_vari[i] = 2;
		if(selected_tran->mode_para == UNIQ_PARAM || selected_tran->mode_para == UNIQ_IMG_PARAM)		/* un seul para */
			j=0;
		else j=selected_desc_param->important_vari;
						
		if(i == j)		/* la time spline est affichee ? */
		{	prj_transit[selected_used_tran].aff_time = -1;
			if(tempo_param.mode_vari[i] == 1)
					prj_transit[selected_used_tran].aff_time = tempo_param.time[i];
		}
	}
	tempo_param.switchs = 0;
	if(selected_tran->mode_para == STAND_PARAM)
	{	for(i=0;i<selected_desc_param->nb_switchs;i++)
		{	if(ptra_var.adr_form[TRSP_SW1+i].ob_state & SELECTED)
				tempo_param.switchs |= 1<<i;
		}
	}
}

void tran_set_colors(int set)
{	tempo_param.color[set-1][0]=temp_r;
	tempo_param.color[set-1][1]=temp_v;
	tempo_param.color[set-1][2]=temp_b;
	wf_draw(&ptra_var,TRSP_C1+(set-1)*2);
}

void redraw_transit(int w_handle, int x, int y, int w, int h)
{
	GRECT r, rd,rimg,rt;
	int i,y_base,pxy[8], nb_objet,dum,fonte=1;
	char chaine[80];
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
			
	rt.g_x = xftu+1;
	rt.g_y = yftu+1;
	rt.g_w = wftu-2;
	rt.g_h = hftu-2;
	
	if(w_handle<1)
	{	goto end_redraw;
	}
	
	v_hide_c(vdi_handle);
	wind_update(BEG_UPDATE);
	if(use_nvdi)
		fonte = 5003;
	vst_font( vdi_handle, fonte );	/* swiss */
	
	nb_objet = hftu/OBJ_H + 1;
	
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)
	{
		if (rc_intersect(&rd, &r))
		{	
			set_clip(1, &r);

			vswr_mode(vdi_handle,MD_REPLACE);
			rect_full(r.g_x,r.g_y,r.g_w,r.g_h,color_3d1*int_rel);		/* un rect_full gris */
			
			for(i=0;i<nb_objet;i++)
			{	y_base = i*OBJ_H + yftu;
				rect_3d(xftu, y_base, wftu, OBJ_H); 
			}
			
			if (rc_intersect(&rt, &r))
			{	
				set_clip(1, &r);

				vswr_mode(vdi_handle,MD_TRANS);
				for(i=0;i<nb_transit-deb_transit;i++)
				{	y_base = i*OBJ_H + yftu;
				
					vst_height(vdi_handle,char_height+2,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,1);		/* le nom en gras */
					sprintf(chaine,transition[i+deb_transit]->nom);
					v_gtext(vdi_handle,xftu + 62,y_base + 20 - TXT_DECAL, chaine);

					vst_height(vdi_handle,char_height,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,0);		/* le reste */
					
					sprintf(chaine,transition[i+deb_transit]->desc1);
					v_gtext(vdi_handle,xftu + 62,y_base + 36 - TXT_DECAL,chaine);
					
					sprintf(chaine,transition[i+deb_transit]->desc2);
					v_gtext(vdi_handle,xftu + 62,y_base + 52 - TXT_DECAL,chaine);

					/* la petite preview */
					rimg.g_x = xftu + 4;
					rimg.g_y = y_base + 4;
					rimg.g_w = 48;
					rimg.g_h = 48;
				
					if(rc_intersect(&r, &rimg))
					{	
					/* Zone source dans l'image en m‚moire : */
						pxy[0] = rimg.g_x - (xftu+4);	
						pxy[1] = rimg.g_y - (y_base+4);	
						pxy[2] = pxy[0] + rimg.g_w - 1;	
						pxy[3] = pxy[1] + rimg.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
						pxy[4] = rimg.g_x;							
						pxy[5] = rimg.g_y;							
						pxy[6] = pxy[4] + rimg.g_w - 1;	
						pxy[7] = pxy[5] + rimg.g_h - 1;	
						tnt_cpyfm (pxy, &transition[i+deb_transit]->preview);
					}
				}
			}

		}
		wind_get(w_handle, WF_NEXTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	}
	wind_update(END_UPDATE);
	v_show_c(vdi_handle, 1);
	vst_font( vdi_handle, 1 );	/* normal */
	end_redraw:;
}

static int cdecl set1_color(PARMBLK *parmblock)
{	int col;
	if(!effet_parametrisation)
	{	col =  conv_col(tempo_param.color[0][0],tempo_param.color[0][1],tempo_param.color[0][2]);
		prev_color(parmblock,col);
	}else
		prev_color(parmblock,para_1eff_coul());
	return 0;
}
static int cdecl set2_color(PARMBLK *parmblock)
{	int col;
	if(!effet_parametrisation)
	{	col =  conv_col(tempo_param.color[1][0],tempo_param.color[1][1],tempo_param.color[1][2]);
		prev_color(parmblock,col);
	}else
		prev_color(parmblock,para_2eff_coul());
		
	return 0;
}

static int cdecl prev_color(PARMBLK *parmblock, int coul)
{
	int x, y, w, h;
	int *img;
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
	{	
		fill(img,w,h,w_mod16,coul);
		
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

static int cdecl tr_prev(PARMBLK *parmblock)
{	GRECT area,real;
	int base_x,base_y;
	int pxy[8];
	
	base_x = ptra_var.adr_form[TRSP_PREV].ob_x + ptra_var.adr_form[0].ob_x;
	base_y = ptra_var.adr_form[TRSP_PREV].ob_y + ptra_var.adr_form[0].ob_y;
	
	real.g_x = base_x;
	real.g_y = base_y;
	real.g_w = x_trev;
	real.g_h = y_trev;
	
	area.g_x = parmblock->pb_xc;
	area.g_y = parmblock->pb_yc;
	area.g_w = parmblock->pb_wc;
	area.g_h = parmblock->pb_hc;

	rc_intersect(&real,&area);
	set_clip(1, &area);
	
	if(effet_parametrisation)		/* preview effet */
	{	
		if(adr_ef_prev)
		{
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
			tnt_cpyfm(pxy, &mfdb_efpre);
		}else
			rect_full(base_x,base_y,TR_PREV_W,TR_PREV_H, LWHITE);

	}else 								/* previex transition */
	{	if(adr_tr_prev)
		{
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
			tnt_cpyfm(pxy, &mfdb_trpre);
		}else
			rect_full(base_x,base_y,TR_PREV_W,TR_PREV_H, LWHITE);
	}
		
	set_clip(0, &area);
	return 0;
	
}

static void get_anno(long id, int num_param)
{	
	etim_nb_anno=0;
   if(id=='ZBOX')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}else if(id=='ISQR')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}else if(id=='SLDE')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}else if(id=='STRH')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}else if(id=='PUSH')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}else if(id=='PHSZ')
   {	if(num_param == 0)
   	{	etim_nb_anno=2;
   		etim_pos_anno[0]=0;
   		etim_pos_anno[1]=255;
   		etim_pos_anno[2]=255;
   		strcpy(etim_text_anno[0],"0");
   		strcpy(etim_text_anno[1],"10000");
   		strcpy(etim_text_anno[2],"360");
   	}
	}
}