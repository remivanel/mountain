/*******************MOUNTAIN*********************/
/*****          GESTION DU PROJET           *****/
/*****            Par Vanel R‚mi            *****/
/*****         Premier code : 1/5/97        *****/
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
#include "transp.h"
#include "time_spl.h"
#include "txts.h"

#define TXT_DECAL 3		/*decalage du texte vers e haut */

#define img2pix(A) ( (int)(((((long)A-deb_img)*48L)/(long)projet.zoom) + x_base))
#define pix2img(A) ((int)(( ((long)A-x_base)*(long)projet.zoom )/48L + deb_img))

void wait_mk(void);
int pop_fly(int index);
void form_zinf(int event);

PROJET projet;
int actu_projet=FALSE;			/* is there any projet here ? */
char chemin_projet[150];
extern char chemin_key[150];
/* objets */
extern int nb_objets;
extern OBJET objet[MAX_OBJET];
extern int hnd_wobjet;
extern int xfo,yfo,wfo,hfo;			/* coord fenetre */
extern int xfou,yfou,wfou,hfou;   /* coord utiles */
extern int xfp,yfp,wfp,hfp;			/* coord fenetre */
extern int w_obj_open;
extern int Magic_flag,Winix_flag;
extern int show_mem;
extern int show_zinf;
extern WINDFORM_VAR zinf_var;		/* boite info zoom */
extern WINDFORM_VAR mem_var;		/* barre mem */

extern int int_rel;
extern int use_nvdi;
extern int char_height;
extern int op_fobj,op_fprj,op_fpist,op_ftran;

/* prj objet */
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern int hnd_wpistes;
extern int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
extern int ful_p;
extern int deb_img;		/* premiere img affiche */

/* mvt */
extern MVT mvt[MAX_MVT];
extern PT_MVT pt_mvt[MAX_PT_MVT];

/* transitions */
extern int nb_used_transit;	/* nombre de transitions UTILISEE */
extern PRJ_TRANSIT prj_transit[MAX_PRJ_TRAN];

/* Variables fenetre transition */
extern int hnd_wtransit;
extern int xft,yft,wft,hft;			/* coord fenetre */
extern int xftu,yftu,wftu,hftu;   	/* coord utiles */


int hnd_wprojet, attr_projet = MOVER|NAME|CLOSER|UPARROW|DNARROW|VSLIDE|SIZER;
int xfr,yfr,wfr,hfr;			/* coord fenetre */
int xfru,yfru,wfru,hfru;   /* coord utiles */
int max_wfr;		/* largeur max */
int deb_projet=0;
int selected_prj_obj=-1;
int old_replay;
int w_prj_open=FALSE;
int w_trk_open=FALSE;
static int tmp_wind_closed=FALSE,tmp_show_mem;

/* time spline */
extern int nb_time_spline;	
extern TIME_SPLINE *first_time_spline;


/* boite opt anim */
extern WINDFORM_VAR opta_var;		/* opt/info sur un prj obj */
char titre_wind_opta[30];

/* boite intens */
char titre_wind_int[40];
int temp_int;
int temp_time;
extern WINDFORM_VAR int_var;		/* intensite */

int init_wprojet(int x, int y, int w, int h)
{	
	/* cood max utiles d'un fenetre */
	wind_calc(WC_WORK, attr_projet, x_desk, y_desk, w_desk, h_desk, &xfru, &yfru, &wfru, &hfru);

	/* on reduit a notre max */
	wfru = 304; 
	
	/* calcule les coord max de notre fenetre */
	wind_calc(WC_BORDER, attr_projet, xfru, yfru, wfru, hfru, &xfr, &yfr, &wfr, &hfr);
	
	max_wfr = wfr;
	
	/* reserve un espace m‚moire */
	if((hnd_wprojet = wind_create(attr_projet, xfr, yfr, wfr, hfr)) <= 0)
	{	super_alert(1,3,T_NO_MORE_WINDOWS);
		return 0;
	}
	
	/* fixe les infos */
	wind_set(hnd_wprojet, WF_NAME, T_PROJET);
	wind_set(hnd_wprojet, WF_VSLIDE, 0, 0, 0, 0);
	wind_set(hnd_wprojet, WF_VSLSIZE, 1000, 0, 0, 0);
	wind_set(hnd_wprojet, WF_BEVENT,1,0,0,0);
	
	/* CENTRE SI PAS DE PRESET*/
	if(x == 0 && y == 0)
	{	xfr = x_desk + (w_desk - wfr)/2;
		hfr = 250;
		yfr = y_desk + (h_desk - hfr)/2;
		hfr = min((y_desk+h_desk)-yfr,hfr);
	}else
	{	wfr = w;
		if(x+wfr>x_desk+w_desk)
			xfr = x_desk + (w_desk - wfr)/2;
		else
			xfr = x;
		if(y+h>y_desk+h_desk)
		{	hfr = 250;
			yfr = y_desk + (h_desk - hfr)/2;
			hfr = min((y_desk+h_desk)-yfr,hfr);
		}else
		{	yfr = y;
			hfr = h;
		}
	}
	
	return 1;
}

void draw_obj_prj(int obj, char red)	/* red = 1 -> pas redraw l'icone */
{	if(obj >= deb_projet && obj < deb_projet+(hfru/OBJ_H+1) )
		redraw_projet(hnd_wprojet, xfru +red*55, yfru +(obj - deb_projet)*OBJ_H, wfru-red*55, OBJ_H);
}

void form_opta(int event)
{
	WINDFORM_VAR *ptr_var = &opta_var;
	int choix,val;
	char dum[6],dum2[6],modif=0;
	
	if (event == OPEN_DIAL)
	{	sprintf(titre_wind_opta,T_INFO_SUR, objet[prj_obj[selected_prj_obj].objet].nom);
		itoa(prj_obj[selected_prj_obj].obj_in, opta_var.adr_form[OPTA_POSO].ob_spec.tedinfo->te_ptext ,10);
		itoa(prj_obj[selected_prj_obj].obj_out, opta_var.adr_form[OPTA_POSO_OUT].ob_spec.tedinfo->te_ptext ,10);
		sprintf(opta_var.adr_form[OPTA_POSA].ob_spec.free_string,
					T_OPTA_POSP, prj_obj[selected_prj_obj].pos_in, prj_obj[selected_prj_obj].pos_out);
		sprintf(opta_var.adr_form[OPTA_PISTE].ob_spec.free_string,
					T_PISTE, textuate_p(prj_obj[selected_prj_obj].piste, dum));
		
		temp_time = prj_obj[selected_prj_obj].time;
		copy_spline_name(temp_time,opta_var.adr_form[OPTA_TEX_SPLINE].ob_spec.tedinfo->te_ptext);
		strcpy(dum,T_OUI);
		strcpy(dum2,T_OUI);
		if(prj_obj[selected_prj_obj].trans == -1)
			strcpy(dum,T_NON);
		if(prj_obj[selected_prj_obj].mvt == 0)
			strcpy(dum2,T_NON);
		sprintf(opta_var.adr_form[OPTA_TRAN].ob_spec.free_string,
					T_OPTA_TRAN,dum,dum2);
		old_replay = prj_obj[selected_prj_obj].mode_replay;
		init_radio(ptr_var,old_replay + OPTA_ONCE);
		
		open_dialog(ptr_var, titre_wind_opta, OPTA_POSO, -1);
	}

	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0) 
		{	switch(choix)
			{		case OPTA_TIMESPLINE:
						retour_index = &temp_time;
						retour_wform = ptr_var;
						retour_objet = OPTA_TEX_SPLINE;
						form_ctim(OPEN_DIAL);
						wf_change(ptr_var,choix,0,1);
						break;
					
					case OPTA_OK:
						if(prj_obj[selected_prj_obj].time != temp_time)
						{	prj_obj[selected_prj_obj].time = temp_time;
							modif = 1;
						}
						prj_obj[selected_prj_obj].mode_replay = get_rbutton(opta_var.adr_form,OPTA_ONCE) - OPTA_ONCE;
						if(prj_obj[selected_prj_obj].mode_replay != old_replay)
							modif =1;
						val = atoi(opta_var.adr_form[OPTA_POSO].ob_spec.tedinfo->te_ptext);
						val = max(0,val);
						val = min(val,objet[prj_obj[selected_prj_obj].objet].nb_img-1);
						if(val != prj_obj[selected_prj_obj].obj_in)
						{	prj_obj[selected_prj_obj].obj_in = val;
							modif = 1;
						}
					
						val = atoi(opta_var.adr_form[OPTA_POSO_OUT].ob_spec.tedinfo->te_ptext);
						val = max(prj_obj[selected_prj_obj].obj_in,val);
						val = min(val,objet[prj_obj[selected_prj_obj].objet].nb_img-1);
						if(val != prj_obj[selected_prj_obj].obj_out)
						{	prj_obj[selected_prj_obj].obj_out = val;
							modif = 1;
						}
			
						if(modif)
						{	redraw_une_piste(prj_obj[selected_prj_obj].piste);
							draw_obj_prj(selected_prj_obj,0);
						}
					case OPTA_AN:
						wf_change(ptr_var,choix,0,1);
						close_dialog(ptr_var);
					break;
			}
		}
	}
}



void window_projet(int event)
{	int old,choix,obj;
	int dum;
	int xt,yt,wt,ht;
			
	if (event & MU_BUTTON && !modal_flag)
	{	if(wind_find(mousex,mousey) == hnd_wprojet && mousex>xfru && mousex <xfru+wfru && mousey > yfru && mousey <yfru+hfru)
		{	obj = (mousey-yfru)/OBJ_H + deb_projet;
			if(obj >= 0 && obj < projet.nb_obj)
			{	
				if(nbr_clics == 2)
					new_visu(prj_obj[obj].objet);
				else
				{	if(selected_prj_obj == obj)
					{	choix=pop_fly(POP_OBJ_PRJ);
						if(choix == PPRJ_EFF)
						{	selected_prj_obj = -1;
							efface_prj_obj(obj);
						}else	if(choix == PPRJ_MOU)
							new_mvt();
						else	if(choix == PPRJ_VIS)
							new_visu(prj_obj[obj].objet);
						else	if(choix == PPRJ_INT)
							form_int(OPEN_DIAL);
						else	if(choix == PPRJ_TRAN)
							new_tran();
						else	if(choix == PPRJ_FX)
							form_ceff(OPEN_DIAL);
						else	if(choix == PPRJ_OPT)
							form_opta(OPEN_DIAL);
						
							
					}else
					{	if(selected_prj_obj != -1)
						{	dum = selected_prj_obj;
							selected_prj_obj = obj;
							draw_obj_prj(dum,1);
						}else
							selected_prj_obj = obj;
	
						draw_obj_prj(obj,1);
						wait_mk();
					}
				}
			}
		}
	}
	else if (event & MU_MESAG)
	{
		switch(buff[0])
		{
			case WM_REDRAW :
				wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
				redraw_projet(buff[3], buff[4], buff[5], buff[6], buff[7]);
				break;
			case WM_TOPPED :
				if(!modal_flag)
					wind_set(buff[3], WF_TOP);
				break;
			case WM_CLOSED : 
				if(!modal_flag)
				{	wind_close(hnd_wprojet);
					w_prj_open=FALSE;
				}
				break;
			case WM_MOVED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7]);
				wind_get(hnd_wprojet,WF_CURRXYWH,&xfr, &yfr, &wfr, &hfr);
				wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
				break;
			case WM_SIZED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], min(max_wfr,buff[6]), max(60,buff[7]));
				wind_get(hnd_wprojet,WF_CURRXYWH,&xfr, &yfr, &wfr, &hfr);
				xt = xfru; yt = yfru; wt = wfru; ht = hfru; 
				wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
				if(deb_projet > projet.nb_obj - hfru/OBJ_H)
					deb_projet = max(0, projet.nb_obj - hfru/OBJ_H);
				if(Magic_flag || Winix_flag)
				{	if(wt<wfru)		/* agrandissement hor */
						redraw_projet(hnd_wprojet, xt+wt-2, yt, 8, ht);
					if(ht<hfru)		/* agrandissement vert */
						redraw_projet(hnd_wprojet, xt, yt+ht-2, wt, 8);
				}
				slider_r();
				break;
			case WM_ARROWED :
				wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
				old = deb_projet;
				if(buff[4] == WA_UPLINE && deb_projet)
					deb_projet--;
				else if(buff[4] == WA_DNLINE && projet.nb_obj > (hfru/OBJ_H + deb_projet) )
					deb_projet++;
				else if(buff[4] == WA_UPPAGE)
					deb_projet = max(0,deb_projet - hfru/OBJ_H);
				else if(buff[4] == WA_DNPAGE)
				{	deb_projet = deb_projet + hfru/OBJ_H;
					if(deb_projet > projet.nb_obj - hfru/OBJ_H)
						deb_projet = max(0, projet.nb_obj - hfru/OBJ_H);
				}
				
				if(old != deb_projet)
				{	slider_r();
					redraw_projet(hnd_wprojet, xfr, yfr, wfr, hfr);
				}
				break;
			case WM_VSLID :
				old = deb_projet;
				deb_projet = ((projet.nb_obj -(hfru/OBJ_H)) * buff[4])/ 1000;
				if(old != deb_projet)
				{	wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
					slider_r();
					redraw_projet(hnd_wprojet, xfr, yfr, wfr, hfr);
				}
 				break;
			
		}
	}
}

void slider_r(void)
{	int y_ass,h_ass;
	
	if(hfru/OBJ_H >= projet.nb_obj)
	{	wind_set(hnd_wprojet, WF_VSLIDE, 0, 0, 0, 0);
		wind_set(hnd_wprojet, WF_VSLSIZE, 1000, 0, 0, 0);
	}else
	{	h_ass = ((hfru/OBJ_H) * 1000) / projet.nb_obj;
		y_ass = (deb_projet * 1000) / (projet.nb_obj-(hfru/OBJ_H));
		wind_set(hnd_wprojet, WF_VSLSIZE, h_ass, 0, 0, 0);
		wind_set(hnd_wprojet, WF_VSLIDE, y_ass, 0, 0, 0);
	}
}	

char *textuate_p(int piste, char *str)
{	
	if(piste == 0)
	{	*str = 'A'; str[1]=0; }
	else if(piste == 1)
	{	*str = 'B'; str[1]=0; }
	else if(piste >= 50 && piste < 100)
		sprintf(str,"SA%d",piste-49);
	else if(piste >= 100 && piste < 150)
		sprintf(str,"SB%d",piste-99);
	else if(piste >= 150)
		sprintf(str,"SG%d",piste-149);
	
	return str;
}

void redraw_projet(int w_handle, int x, int y, int w, int h)
{
	GRECT r, rd,rimg,rt;
	int i,y_base,pxy[8], nb_objet,dum,fonte=1;
	char chaine[80],txt[5];
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
			
	rt.g_x = xfru+1;
	rt.g_y = yfru+1;
	rt.g_w = wfru-2;
	rt.g_h = hfru-2;
	
	if(w_handle<1)
	{	goto end_redraw;
	}
	
	v_hide_c(vdi_handle);
	wind_update(BEG_UPDATE);
	if(use_nvdi)
		fonte = 5003;
	vst_font( vdi_handle, fonte );	/* swiss */
	
	nb_objet = hfru/OBJ_H + 1;
	
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)
	{
		if (rc_intersect(&rd, &r))
		{	
			set_clip(1, &r);

			vswr_mode(vdi_handle,MD_REPLACE);
			rect_full(r.g_x,r.g_y,r.g_w,r.g_h,color_3d1*int_rel);		/* un rect_full gris */
			
			for(i=0;i<nb_objet;i++)
			{	y_base = i*OBJ_H + yfru;
				rect_3d(xfru, y_base, wfru, OBJ_H); 
			}
			
			if (rc_intersect(&rt, &r))
			{	
				set_clip(1, &r);

				vswr_mode(vdi_handle,MD_TRANS);
				for(i=0;i<projet.nb_obj-deb_projet;i++)
				{	y_base = i*OBJ_H + yfru;
				
					vst_height(vdi_handle,char_height+2,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,1);		/* le nom en gras */
					sprintf(chaine,"%s (%s)",objet[prj_obj[i+deb_projet].objet].nom, textuate_p(prj_obj[i+deb_projet].piste,txt));
					v_gtext(vdi_handle,xfru + 62,y_base + 20 - TXT_DECAL, chaine);

					vst_height(vdi_handle,char_height,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,0);		/* le reste */
					
					sprintf(chaine,T_PROJET_POS, prj_obj[i+deb_projet].pos_in,
							 prj_obj[i+deb_projet].pos_out - prj_obj[i+deb_projet].pos_in+1);
					v_gtext(vdi_handle,xfru + 62,y_base + 36 - TXT_DECAL,chaine);
					
					sprintf(chaine,T_OBJET_POS, prj_obj[i+deb_projet].obj_in,
							 prj_obj[i+deb_projet].obj_out - prj_obj[i+deb_projet].obj_in+1);
					v_gtext(vdi_handle,xfru + 62,y_base + 52 - TXT_DECAL,chaine);
					
					/* selection */
					if(selected_prj_obj == i+deb_projet)
					{	vswr_mode(vdi_handle,MD_XOR);
						rect_full(xfru+57,y_base+1,wfru-58,OBJ_H-2,BLACK);
						vswr_mode(vdi_handle,MD_TRANS);
					}
					
					/* la petite preview */
					rimg.g_x = xfru + 4;
					rimg.g_y = y_base + 4;
					rimg.g_w = 48;
					rimg.g_h = 48;
				
					if(rc_intersect(&r, &rimg))
					{	
					/* Zone source dans l'image en m‚moire : */
						pxy[0] = rimg.g_x - (xfru+4);	
						pxy[1] = rimg.g_y - (y_base+4);	
						pxy[2] = pxy[0] + rimg.g_w - 1;	
						pxy[3] = pxy[1] + rimg.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
						pxy[4] = rimg.g_x;							
						pxy[5] = rimg.g_y;							
						pxy[6] = pxy[4] + rimg.g_w - 1;	
						pxy[7] = pxy[5] + rimg.g_h - 1;	
						tnt_cpyfm (pxy, &objet[prj_obj[i+deb_projet].objet].preview);
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

int save_projet(char comme)
{	char pth[150],nom[14];
	int handle,bout,nb_mvt,i,j;
	long head = 'MPRJ';
	CHEMIN prj;
	TIME_SPLINE *cur_spline;
	
	if(!actu_projet)
		goto ann_sauve;
	
	if(comme || chemin_projet[0] == 0)
	{	strcpy(pth,"\\*.MPJ");
		strcpy(nom,projet.name);
		strcat(nom,".MPJ");
		Fselecteur(&prj,pth,nom,&bout,T_SAVPRJ);
		if(bout == 0 || nom[0] == 0)
			goto ann_sauve;

		find_name(prj.complet,nom);
		strcpy(chemin_projet, prj.chemin);
		strcat(chemin_projet, nom);
		strcat(chemin_projet, ".MPJ");		/* on impose l'extention MPJ */
		
	}
	
	handle = (int)Fcreate(chemin_projet,0);
	if(handle < 0)
	{	super_alert(1,3,T_DISK_ERROR);
		goto ann_sauve;
	}
	
	/* ID */
	
	Fwrite(handle,4,&head);
	
	/* PROJET */
	
	Fwrite(handle,SIZE_PROJET,&projet);

	Fwrite(handle,2,&nb_objets);					/* les objets */
	if(nb_objets)
		Fwrite(handle,SIZE_OBJ*nb_objets,objet);
	
	/* LES PRJ OBJ */
	
	if(projet.nb_obj)
		Fwrite(handle,SIZE_PRJ_OBJ*projet.nb_obj,prj_obj);	/* les objets inclus leurs nombre est dans projet */
	
	/* LES MOUVEMENTs */
	
	nb_mvt=0;
	for(i=1;i<MAX_MVT;i++)
	{	if(mvt[i].used)
			nb_mvt++;
	}

	Fwrite(handle,2,&nb_mvt);
	
	for(i=1;i<MAX_MVT;i++)
	{	if(mvt[i].used)
		{	Fwrite(handle,sizeof(MVT),&mvt[i]);		/* on sauve le mvt */
			j=	find_related_prj_ralated(i);
			Fwrite(handle,2,&j);					/* et le prj obj corresondant */
		}
	}
	
	for(i=1;i<MAX_MVT;i++)
	{	if(mvt[i].used)
		{	Fwrite(handle,mvt[i].nb_mvt_used*sizeof(PT_MVT),&pt_mvt[i*MAX_USE_MVT]);
		}
	}
	
	/* LES LISTES DE FICHIERS POUR LES SUITES D'IMG */
	
	for(i=0;i<nb_objets;i++)
	{	if(objet[i].type == OBJ_SUIT)
			Fwrite(handle,  objet[i].nb_img*14L, objet[i].tab_noms);
	}
	
	/* LES PRAMETRES MODULE D'EXPORT */
	if(projet.size_param > 4)
		Fwrite(handle, projet.size_param , projet.adr_param);
	
	/* LES TIMES SPLINES */
	Fwrite(handle,2,&nb_time_spline);	/* le nombre */
	if(nb_time_spline>1)
	{	cur_spline = first_time_spline;
		do
		{	Fwrite(handle,sizeof(TIME_SPLINE),cur_spline);	/* la spline */
			cur_spline = cur_spline->next_time_spline;
		}while(cur_spline);
	}

	
	/* LES USED TRANSITIONS */
	
	Fwrite(handle,2,&nb_used_transit);
	if(nb_used_transit)
	{	Fwrite(handle,sizeof(PRJ_TRANSIT)*nb_used_transit,prj_transit);	
	}
	
	/* LES USED EFFETS */
	for(i=0;i<projet.nb_obj;i++)
	{	if(prj_obj[i].stoquag_eff)
			Fwrite(handle,sizeof(STOQUAG_EFF),prj_obj[i].stoquag_eff);
	}
			
	/* ON FERME */
	Fclose(handle);
	
	return 1;
	
	ann_sauve:;
	return 0;
}

int find_related_prj_ralated(int i)
{	int obj;
	
	for(obj=0;obj<projet.nb_obj;obj++)
	{	if(prj_obj[obj].mvt == i)
			return obj;
	}
	return obj;
}

int load_projet(void)
{	char pth[200],nom[14]="";
	char chemin_obj[200],str[200];
	int handle,bout,i,j,nb_mvt;
	long head;
	CHEMIN prj;
	DTA ficdta;
	char *adr_tab_nom;
	TIME_SPLINE *cur_spline,*preced_spline=first_time_spline; /* inutile mais evite un warning */
	
	strcpy(pth,"\\*.MPJ");
	Fselecteur(&prj,pth,nom,&bout,T_LOAD_PRJ);
	if(bout == 0 || nom[0] == 0)
		goto ann_charge;
		
	handle = (int)Fopen(prj.complet, FO_READ);
	if(handle < 0)
	{	super_alert(1,3,T_DISK_ERROR);
		goto ann_charge;
	}
	
	Fread(handle,4,&head);
	if(head != 'MPRJ')
	{	super_alert(1,3,T_NOT_A_MNT);
		Fclose(handle);
		goto ann_charge;
	}
	
	Fread(handle,SIZE_PROJET,&projet);

	Fread(handle,2,&nb_objets);					/* les objets */
	if(nb_objets)
		Fread(handle,SIZE_OBJ*nb_objets,objet);
	
	if(projet.nb_obj)
		Fread(handle,SIZE_PRJ_OBJ*projet.nb_obj,prj_obj);	/* les objets inclus leurs nombre est dans projet */
	
	/* LES MOUVEMENTs */
	Fread(handle,2,&nb_mvt);
	
	for(i=1;i<nb_mvt+1;i++)
	{	Fread(handle,sizeof(MVT),&mvt[i]);		/* on charge le mvt */
		Fread(handle,2,&j);					/* et le prj obj corresondant */
		prj_obj[j].mvt=i;
	}
	
	for(i=1;i<nb_mvt+1;i++)
	{	Fread(handle,mvt[i].nb_mvt_used*sizeof(PT_MVT),&pt_mvt[i*MAX_USE_MVT]);
	}

	
	/* LES LISTES DE FICHIERS POUR LES SUITES D'IMG */
	
	for(i=0;i<nb_objets;i++)
	{	if(objet[i].type == OBJ_SUIT)
		{	adr_tab_nom = Malloc(objet[i].nb_img*14L);
			if(adr_tab_nom)
			{	objet[i].tab_noms = adr_tab_nom;
				Fread(handle, objet[i].nb_img*14L, objet[i].tab_noms);
			}else
			{	super_alert(1,1,T_NO_MEMORY);
				goto ann_charge;
			}
		}
	}
	
	/* LES PRAMETRES MODULE D'EXPORT */
	if(projet.size_param > 4)
	{	projet.adr_param = Malloc (projet.size_param);
		Fread(handle, projet.size_param , projet.adr_param);
	}


	/* LES TIMES SPLINES */
	Fread(handle,2,&nb_time_spline);	/* le nombre */
	if(nb_time_spline>1)
	{	
		for(i=1;i<nb_time_spline;i++)
		{	cur_spline = malloc(sizeof(TIME_SPLINE));
			if(cur_spline)
			{	Fread(handle,sizeof(TIME_SPLINE),cur_spline);	/* la spline */
				if(i==1)
					first_time_spline = cur_spline;
				else
					preced_spline->next_time_spline = cur_spline;
				preced_spline = cur_spline;
			}
		}
		cur_spline->next_time_spline=0;
	}
	
		
	/* LES USED TRANSITIONS */
	
	Fread(handle,2,&nb_used_transit);
	if(nb_used_transit)
	{	Fread(handle,sizeof(PRJ_TRANSIT)*nb_used_transit,prj_transit);	
	}
	
	/* LES USED EFFETS */
	for(i=0;i<projet.nb_obj;i++)
	{	if(prj_obj[i].stoquag_eff)	/* s'il y avait un effet */
		{	prj_obj[i].stoquag_eff = malloc(sizeof(STOQUAG_EFF));
			if(prj_obj[i].stoquag_eff)
				Fread(handle,sizeof(STOQUAG_EFF),prj_obj[i].stoquag_eff);
		}
	}
	
	/* ON FERME */
	Fclose(handle);

	strcpy(chemin_projet, prj.complet);
	
	/* Verification de la presence de tous les objets */
	strcpy(chemin_obj,prj.chemin);
	Fsetdta(&ficdta);
	for(i=0;i<nb_objets;i++)
	{	if(objet[i].type != OBJ_SUIT)		/* on laisse tomber les suites */
		{	if(Fsfirst(objet[i].chemin,0) != 0)
			{	find_name_ext(objet[i].chemin,nom);
	
				/* essaye dans le repertoire courant */
				strcpy(pth,chemin_obj);
				strcat(pth,nom);
				if(Fsfirst(pth,0) != 0)
				{	sprintf(str,T_OBJ_NOT_HERE,nom);
					super_alert(1,3,str);

					sprintf(str,T_OBJET_S,nom);
					pth[0]=0;
					Fselecteur(&prj,pth,nom,&bout,str);
					if(bout == 0)
						goto ann_charge;
				
					/* verification */
					find_name_ext(objet[i].chemin,nom);
					strcpy(pth,prj.chemin);
					strcpy(chemin_obj,prj.chemin);
					strcat(pth,nom);
					if(Fsfirst(pth,0) != 0)	/* impossible */
						goto ann_charge;
					/* c'est ok on garde le chemin */
					strcpy(objet[i].chemin,pth);
				}else
					strcpy(objet[i].chemin,pth);
			}
			/* verification de la presence du fichier de keyframe */
			if(Fsfirst(objet[i].chemin_key,0) != 0 && (objet[i].type == OBJ_FLH || objet[i].type == OBJ_FLI || objet[i].type == OBJ_FLC))
			{	find_name_ext(objet[i].chemin_key,nom);
	
				/* essaye dans le repertoire courant */
				strcpy(pth,chemin_obj);
				strcat(pth,nom);
				if(Fsfirst(pth,0) != 0)
				{	/* essaye dans le repertoire des key frame */
					strcpy(pth,chemin_key);
					strcat(pth,nom);
					if(Fsfirst(pth,0) != 0)
					{	sprintf(str,T_KEYF_S,objet[i].nom,nom);
						super_alert(1,3,str);

						sprintf(str,T_FICH_S,nom);
						pth[0]=0;
						Fselecteur(&prj,pth,nom,&bout,str);
						if(bout == 0)
							goto ann_charge;
				
						/* verification */
						find_name_ext(objet[i].chemin_key,nom);
						strcpy(pth,prj.chemin);
						strcat(pth,nom);
						if(Fsfirst(pth,0) != 0)	/* impossible */
							goto ann_charge;
					/* c'est ok on garde le chemin */
						strcpy(objet[i].chemin_key,pth);
					
					}else
						strcpy(objet[i].chemin_key,pth);
				}else
					strcpy(objet[i].chemin_key,pth);
			}
		}
	}
				
	
	/* il faut faire les preview de tous les objets */
	for(i=0;i<nb_objets;i++)
	{	/* m‚moire pour la preview */
		objet[i].preview.fd_addr = Malloc(4608);  /* 48*48*2 */
		if(objet[i].preview.fd_addr == 0)
		{	super_alert(1,1,T_NO_MEMORY);
			objet[i].preview.fd_addr = 0;
			nb_objets=0;
			goto ann_charge;
		}else
		if(make_preview(i) == 0)
		{	super_alert(1,3,T_PREV_UNPOS);
			Mfree(objet[i].preview.fd_addr);
			nb_objets=0;
			goto ann_charge;
		}
	}
	
	close_visu();
	
	if(op_fobj)
	{	if(!w_obj_open)
		{	wind_open(hnd_wobjet,xfo,yfo,wfo,hfo);
			w_obj_open = TRUE;
			wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
			slider_o();
		}
	}

	if(op_fprj)
	{	if(!w_prj_open)
		{	wind_open(hnd_wprojet,xfr,yfr,wfr,hfr);
			wind_get(hnd_wprojet,WF_WORKXYWH,&xfru, &yfru, &wfru, &hfru);
			w_prj_open=TRUE;
			slider_r();
		}
	}
	
	if(op_ftran)
	{	if(!w_trsit_open)
		{	wind_open(hnd_wtransit,xft,yft,wft,hft);
			wind_get(hnd_wtransit,WF_WORKXYWH,&xftu, &yftu, &wftu, &hftu);
			slider_t();
			w_trsit_open = TRUE;
		}
	}
	
	deb_img=0;		/* ca aide */
	
	if(op_fpist)
	{	if(!w_trk_open)
		{	wind_open(hnd_wpistes,xfp,yfp,wfp,hfp);
			wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
			slider_ph();
			slider_pv();
			w_trk_open=TRUE;
		}
	}
	
	set_pop_zoom(projet.zoom);
	form_dial(FMD_FINISH,0,0,0,0,x_desk,y_desk,w_desk,h_desk);

	return 1;
		
	ann_charge:;
	return 0;
}

int et_ke_kon_fe_du_prj(void)
{	int retour;

	if(actu_projet)
	{	retour = super_alert(2,2,T_KE_KON_FE);
		if(retour == 3)
			return 0;				/* annule */
		if(retour == 1)
		{	if(save_projet(0) == 0)
				return 0;			/* pas sauve, annule */
		}
		kill_projet();	
	}
	return 1;				/* on quitte */
}

void kill_projet(void)
{	int i;
	TIME_SPLINE *cur_spline = first_time_spline,*next_spline;
	
	if(actu_projet)
	{	/* efface les effets du projet */
		for(i=0;i<projet.nb_obj;i++)
		{	if(prj_obj[i].stoquag_eff)
				free(prj_obj[i].stoquag_eff);
		}
		projet.nb_obj=0;
		for(i=nb_objets-1;i>=0;i--)
			efface_objet(i);
		nb_objets=0;
		actu_projet = 0;
		selected_prj_obj=-1;
		close_prev();
		if(w_trk_open)
		{	wind_close(hnd_wpistes);
			w_trk_open=FALSE;
		}
		if(w_prj_open)
		{	wind_close(hnd_wprojet);
			w_prj_open=FALSE;
		}
		kill_used_transit();
		if(projet.size_param > 4)
			Mfree(projet.adr_param);
		if(nb_time_spline>1)
		{	do
			{	next_spline = cur_spline->next_time_spline;
				free(cur_spline);
				cur_spline = next_spline;
			}while(cur_spline);
			nb_time_spline=1;
		}
	}	
}

int list_related_prj_obj(int num_objet, int *liste)
{	int i,num=0;

	for(i=0;i<projet.nb_obj;i++)
	{	if(prj_obj[i].objet == num_objet)
			liste[num++] = i;
	}

	return num;
}

void efface_prj_obj(int num)
{	int larg,redraw_piste;
	int mini,maxi,piste;
	
	/* VOIR AUSSI kill_projet */
	
	if(num < projet.nb_obj)
	{	piste = prj_obj[num].piste;
		
		/* efface les effets s'il faut */
		if(prj_obj[num].stoquag_eff)
			free(prj_obj[num].stoquag_eff);
		
		/* teste s'il aparait ds la fenetre de piste */
		larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);
		mini = max(prj_obj[num].pos_in, deb_img);
		maxi = min(prj_obj[num].pos_out , deb_img+larg);
		redraw_piste = (mini < maxi);
		
		if(num < projet.nb_obj-1) /* c'est pas le dernier */
			memcpy(&prj_obj[num], &prj_obj[num+1], (projet.nb_obj - num - 1) * SIZE_PRJ_OBJ);
		projet.nb_obj--;
		
		/* effacement ds la fenetre projet */
		redraw_projet(hnd_wprojet, xfr, yfr, wfr, hfr);
		slider_r(); 
		
		/* effacemnt ds la fenetre piste */
		if(redraw_piste)
			redraw_une_piste(piste);
	}
}

void new_prj_obj(int cur_objet, int pos_in, int piste)
{	int obj = projet.nb_obj;
	
	if(can_put_here(cur_objet, pos_in, piste, -1))
	{	prj_obj[obj].objet = cur_objet;
		prj_obj[obj].piste = piste;
		prj_obj[obj].pos_in = pos_in;
		prj_obj[obj].pos_out = pos_in + objet[cur_objet].def_out - objet[cur_objet].def_in;
		prj_obj[obj].obj_in = objet[cur_objet].def_in;
		prj_obj[obj].obj_out = objet[cur_objet].def_out;
		prj_obj[obj].mode_replay = 0;
		prj_obj[obj].time = 0;
		prj_obj[obj].mvt = 0;
		prj_obj[obj].trans = -1;
		prj_obj[obj].intens = 0xc000; /* 100 % fixe */
		prj_obj[obj].r = 0;
		prj_obj[obj].v = 0;
		prj_obj[obj].b = 0;
		prj_obj[obj].stoquag_eff = 0;
	
		if(prj_obj[obj].pos_out+1>projet.lenght)
		{	projet.lenght = prj_obj[obj].pos_out+1;
			ful_p = 0;									/* Annuler flag fuller */
		}
		projet.nb_obj++;
		slider_ph();
		
		draw_obj_prj(obj,0);
		slider_r(); 
		
	}
}

void copy_prj_obj(int cur_objet, int pos_in, int piste)
{	int obj = projet.nb_obj, len;
	
	len = prj_obj[cur_objet].pos_out - prj_obj[cur_objet].pos_in;
	
	if(can_put_op_here(pos_in, pos_in + len, piste, -1))
	{	prj_obj[obj].objet = prj_obj[cur_objet].objet;
		prj_obj[obj].piste = piste;
		prj_obj[obj].pos_in = pos_in;
		prj_obj[obj].pos_out = pos_in + len;
		prj_obj[obj].obj_in = prj_obj[cur_objet].obj_in;
		prj_obj[obj].obj_out = prj_obj[cur_objet].obj_out;
		prj_obj[obj].time = prj_obj[cur_objet].time;
		prj_obj[obj].mode_replay = prj_obj[cur_objet].mode_replay;
		prj_obj[obj].intens = prj_obj[cur_objet].intens;
		/* le mvt doit etre copie */
		prj_obj[obj].mvt = duplique_mvt(prj_obj[cur_objet].mvt);
		prj_obj[obj].trans = prj_obj[cur_objet].trans;
		prj_obj[obj].r = prj_obj[cur_objet].r;
		prj_obj[obj].v = prj_obj[cur_objet].v;
		prj_obj[obj].b = prj_obj[cur_objet].b;
		
		/* on copie les effets */
		if(prj_obj[cur_objet].stoquag_eff)		/* s'il y a des effets */
		{	prj_obj[obj].stoquag_eff = malloc(sizeof(STOQUAG_EFF));
			if(prj_obj[obj].stoquag_eff)
				copy_sto_eff(prj_obj[cur_objet].stoquag_eff,prj_obj[obj].stoquag_eff);
			else
				super_alert(1,1,T_NO_MEMORY);

		}
	
		if(prj_obj[obj].pos_out+1>projet.lenght)
		{	projet.lenght = prj_obj[obj].pos_out+1;
			ful_p = 0;									/* Annuler flag fuller */
		}
		projet.nb_obj++;
		slider_ph();
		
		draw_obj_prj(obj,0);
		slider_r(); 
		
	}
}

long real_lenght(void)
{	long len;
	
	len = projet.lenght -1;
	
	for(;len>1;len--)
	{	if(find_anim_whole(len) != -1)
			return len+1;
	}
	return 1;
}
	
long find_anim_whole(long pos)
{	int i;
	for(i=0;i<projet.nb_obj; i++)
	{	if(pos >= prj_obj[i].pos_in && pos <= prj_obj[i].pos_out)
			return i;
	}
	
	return -1;
}


void form_int(int event)
{
	WINDFORM_VAR *ptr_var = &int_var;
	int choix;
	
	if (event == OPEN_DIAL)
	{	sprintf(titre_wind_int,T_INTEN_S,objet[prj_obj[selected_prj_obj].objet].nom);
		temp_int = prj_obj[selected_prj_obj].intens;
		if(temp_int & 0x8000)
		{	int_var.adr_form[INT_CTE].ob_state = SELECTED;
			int_var.adr_form[INT_VARI].ob_state = 0;
		}
		else
		{	int_var.adr_form[INT_CTE].ob_state = 0;
			int_var.adr_form[INT_VARI].ob_state = SELECTED;
		}
		
		temp_time = temp_int & 0x0fff;
		copy_spline_name(temp_time, int_var.adr_form[INT_T_TEXT].ob_spec.tedinfo->te_ptext);
		
		int_var.adr_form[INT_0P].ob_state = int_var.adr_form[INT_25P].ob_state = int_var.adr_form[INT_50P].ob_state 
		= int_var.adr_form[INT_75P].ob_state = int_var.adr_form[INT_100P].ob_state = 0;
		int_var.adr_form[INT_0P + ((temp_int>>12)&0x7)].ob_state = SELECTED;

		open_dialog(ptr_var, titre_wind_int, 0, -1);
	}

	else 
	{	choix = windial_do(ptr_var, event);
	 	if(choix == CLOSE_DIAL)
	 		choix = INT_ANN;
		if (choix != 0) 
		{	switch(choix)
			{	
					
				case INT_TIME:
					etim_nb_anno = 3;
					etim_pos_anno[0]=0;
					etim_pos_anno[1]=128;
					etim_pos_anno[2]=256;
					strcpy(etim_text_anno[0],"0 %");
					strcpy(etim_text_anno[1],"50 %");
					strcpy(etim_text_anno[2],"100 %");

					retour_index = &temp_time;
					retour_wform = ptr_var;
					retour_objet = INT_T_TEXT;
					form_ctim(OPEN_DIAL);
					wf_change(ptr_var,choix,0,1);
					break;
				case INT_OK:
					temp_int = ((get_rbutton(int_var.adr_form,INT_0P)-INT_0P)<<12)&0x7000;
					temp_int |= temp_time & 0x0fff;
					temp_int |= ( (int_var.adr_form[INT_CTE].ob_state&SELECTED)<<15) & 0x8000;
					prj_obj[selected_prj_obj].intens = temp_int;
					redraw_une_piste(prj_obj[selected_prj_obj].piste);
				case INT_ANN:
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;

			}
		}
	}
}


int can_put_here(int cur_objet, int pos_in, int piste, int nonobstant)
{	int i,pos_out,found;
	
	pos_out = pos_in + objet[cur_objet].def_out - objet[cur_objet].def_in+1;
	for(i=pos_in;i<pos_out;i++)
	{	found = find_anim(i,piste);
		if( found != -1 && found != nonobstant)
			return 0;
	}
	return 1;
}

int can_put_op_here(int pos_in, int pos_out, int piste, int nonobstant)
{	int i,found;
	
	for(i=pos_in;i<pos_out;i++)
	{	found = find_anim(i,piste);
		if( found != -1 && found != nonobstant)
			return 0;
	}
	return 1;
}

int time_comput(int obj,int pos)
{	int prj_larg, obj_larg;
	int time=0;
	
	prj_larg = prj_obj[obj].pos_out - prj_obj[obj].pos_in;	/* longueur ds le projet */
	obj_larg = prj_obj[obj].obj_out - prj_obj[obj].obj_in;	/* longueur ds l'objet */
	
	if(prj_larg==0)		/* non mais */
		return 0;
	
	if(prj_obj[obj].time == 0)		/* sans timespline - lineaire */
		time = pos;
	else
		time = (int)(spline_comput( prj_obj[obj].time,0,pos,prj_larg+1,prj_larg+1));
	
	
	if(prj_obj[obj].mode_replay == 0)		/* une fois */
	{	return (int)( ((long)time*(1+obj_larg))/(1+prj_larg)) + prj_obj[obj].obj_in;
	}else if(prj_obj[obj].mode_replay == 1)	/* en boucle */
	{	time = time % (obj_larg+1);
		return time + prj_obj[obj].obj_in;
	}else  /* en ping pong */
	{	time = time % (obj_larg*2);
		if(time >= obj_larg)
			time = 2*obj_larg-time;
		return  time + prj_obj[obj].obj_in;
	}
}

void put_objet(int num_objet)
{	int mx,my,mk,dum,wind,larg,old;
	int pos,old_pos=-1,old_pos_out=-1,pos_out;
	int piste,old_piste=-1, preview = 0;	/* preview=1 si il y avait une preview */
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
				{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu)	/* scrool >> */
			{	if(preview)
				{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
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
			{	if(piste != old_piste || pos != old_pos)
				{	pos_out = pos + objet[num_objet].def_out - objet[num_objet].def_in + 1;
				
					if(preview)
					{	Vsync();
						aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
						preview=0;
					}
					if(piste != 2)	/* pas sur la piste transition ! */
					{
						old_piste = piste;
						old_pos = pos;
						old_pos_out = pos_out;
					
						if( can_put_here(num_objet,pos,piste, -1))
						{	aff_prev(piste, pos, pos_out);	/* affichage de la preview */
							preview=1;
						}
					}
				}
			}else if(preview)
			{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	aff_prev(old_piste, old_pos, pos_out);	/* effacement de l'ancienne preview */
		if(pos_out+1>projet.lenght)		/* on redraw tout a cause des hachures */
		{	new_prj_obj(num_objet,pos,piste);
			redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
		}else									/* on redraw que la piste */
		{	new_prj_obj(num_objet,pos,piste);
			redraw_une_piste(piste);
		}
	}

	graf_mouse(ARROW,0);
}

void move_objet(int num_objet)
{	int mx,my,mk,mkey,dum,wind,old,len,decale_pos,x_base = xfpu+18;
	int pos,old_pos=-1,old_pos_out=-1,pos_out,very_old_pos,very_old_piste;
	int piste,old_piste=-1, preview = 0;	/* preview=1 si il y avait une preview */
	GRECT r;
	char move,old_move=99;
	
	graf_mouse(FLAT_HAND,0);
	set_clip(0,&r);
/*	larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);  */
	len = prj_obj[num_objet].pos_out - prj_obj[num_objet].pos_in +1;
	
	graf_mkstate(&mx,&my,&mk,&dum);
	decale_pos = prj_obj[num_objet].pos_in - max(0,pix2img(mx));
	
	very_old_pos = prj_obj[num_objet].pos_in;
	very_old_piste = prj_obj[num_objet].piste;
	
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
				{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu)	/* scrool >> */
			{	if(preview)
				{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
					preview=0;
				}
				deb_img += projet.zoom;
/*				if(deb_img > projet.lenght - larg + projet.zoom)     *//* on ajoute une marge pour voir la fin */
/*					deb_img = max(0, projet.lenght - larg + projet.zoom);  */
				if(deb_img != old)
				{	v_hide_c(vdi_handle);
					scrool_right(old);
					v_show_c(vdi_handle,1);
				}
			}else if(find_pos_piste(mx,my,&piste,&pos))
			{	pos += decale_pos;
				pos = max(0,pos);
				if(piste != old_piste || pos != old_pos || old_move != move)
				{	pos_out = pos + len;
				
					if(preview)
					{	Vsync();
						aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
						preview=0;
					}
					if(piste != 2)	/* pas sur la piste transition ! */
					{
						old_piste = piste;
						old_pos = pos;
						old_pos_out = pos_out;
						old_move = move;
					
						if( can_put_op_here(pos,pos+len,piste, move))
						{	aff_prev(piste, pos, pos_out);	/* affichage de la preview */
							preview=1;
							set_curseur(pos);
						}
					}
				}
			}else if(preview)
			{	aff_prev(old_piste, old_pos, old_pos_out);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	aff_prev(old_piste, old_pos, pos_out);	/* effacement de l'ancienne preview */
		if(move == -1)	/* copy */
		{	copy_prj_obj(num_objet,pos,piste);
			redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
		}
		else
		{	prj_obj[num_objet].piste = piste;
			prj_obj[num_objet].pos_in = pos;
			prj_obj[num_objet].pos_out = pos + len - 1;
			draw_obj_prj(num_objet,1);
			if(prj_obj[num_objet].pos_out+1>projet.lenght)
			{	projet.lenght = prj_obj[num_objet].pos_out+1;
				ful_p = 0;									/* Annuler flag fuller */
				slider_ph();
			}
			if(pos != very_old_pos || piste != very_old_piste)
				redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
		}
	}

	graf_mouse(ARROW,0);
}

void widen_objet(int num_objet)
{	int mx,my,mk,mkey,dum,wind,old,decale_pos,x_base = xfpu+18;
	int pos_in,pos,old_pos=-1,very_old_pos;
	int piste, preview = 0;	/* preview=1 si il y avait une preview */
	GRECT r;
	
	graf_mouse(POINT_HAND,0);
	set_clip(0,&r);
	
	graf_mkstate(&mx,&my,&mk,&dum);
	decale_pos = prj_obj[num_objet].pos_out - max(0,pix2img(mx)) + 1;
	
	pos_in = prj_obj[num_objet].pos_in;
	very_old_pos = prj_obj[num_objet].pos_out;
	piste = prj_obj[num_objet].piste;
	
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
				{	aff_prev(piste, pos_in, old_pos);	/* effacement de l'ancienne preview */
					preview=0;
				}
				Vsync();				Vsync();
				v_hide_c(vdi_handle);
				deb_img = max(0,deb_img-projet.zoom);
				scrool_left(old);
				v_show_c(vdi_handle,1);
				
			}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu)	/* scrool >> */
			{	if(preview)
				{	aff_prev(piste, pos_in, old_pos);	/* effacement de l'ancienne preview */
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
				pos = max(pos_in+1,pos);
				if(pos != old_pos)
				{	
					if(preview)
					{	Vsync();
						aff_prev(piste, pos_in, old_pos);	/* effacement de l'ancienne preview */
						preview=0;
					}

					old_pos = pos;
				
					if( can_put_op_here(pos_in,pos,piste, num_objet))
					{	aff_prev(piste, pos_in, pos);	/* affichage de la preview */
						preview=1;
						set_curseur(pos-pos_in);
					}
				}
			}else if(preview)
			{	aff_prev(piste, pos_in, pos);	/* effacement de l'ancienne preview */
				preview=0;
			}
		}
	}while(mk!=0);
	
	if(preview)
	{	aff_prev(piste, pos_in, pos);	/* effacement de l'ancienne preview */
		prj_obj[num_objet].pos_out = pos-1;
		draw_obj_prj(num_objet,1);
		if(prj_obj[num_objet].pos_out+1>projet.lenght)
		{	projet.lenght = prj_obj[num_objet].pos_out+1;
				ful_p = 0;									/* Annuler flag fuller */
			slider_ph();
		}
		if(pos-1 != very_old_pos)
			redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
	}

	graf_mouse(ARROW,0);
}

/* Ferme temporairement toutes les fenetres en vue d'un Pexec */

void tmp_close_all_wind(void)
{	if(!tmp_wind_closed)
	{	if(w_obj_open)		wind_close(hnd_wobjet);
		if(w_trsit_open)	wind_close(hnd_wtransit);
		if(w_prj_open)		wind_close(hnd_wprojet);
		if(w_trk_open)		wind_close(hnd_wpistes);
		if(show_zinf)		close_dialog(&zinf_var);
		if(show_mem)		close_dialog(&mem_var);
		tmp_show_mem = show_mem;
		show_mem = FALSE;
		close_visu();		/* celles la ne seront pas reouvertes */
		close_prev();
		tmp_wind_closed = TRUE;
	}
}

void tmp_open_all_wind(void)
{	if(tmp_wind_closed)
	{	if(w_obj_open)		wind_open(hnd_wobjet,xfo,yfo,wfo,hfo);
		if(w_trsit_open)	wind_open(hnd_wtransit,xft,yft,wft,hft);
		if(w_prj_open)		wind_open(hnd_wprojet,xfr,yfr,wfr,hfr);
		if(w_trk_open)		wind_open(hnd_wpistes,xfp,yfp,wfp,hfp);
		if(show_zinf)		form_zinf(OPEN_DIAL);
		show_mem = tmp_show_mem;
		if(show_mem)		form_mem(OPEN_DIAL);
		tmp_wind_closed = TRUE;
	}
}
