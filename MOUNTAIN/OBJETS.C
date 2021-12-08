/********************MOUTAIN********************/
/*****   GESTION de la fenˆtre des objets   ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/


#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include <stdio.h>
#include "flh_lib.h"
#include "gest_flh.h"
#include "gest_fli.h"
#include "mountain.h"
#include "objets.h"
#include "effets1.h"
#include "visu.h"
#include "anim.h"
#include "transit.h"
#include "geffets.h"
#include "projet.h"
#include "pistes.h"
#include "some_fnc.h"
#include "modules.h"
#include "txts.h"

int nb_objets = 0;
int deb_objet = 0;
int info_objet = 0;
OBJET objet[MAX_OBJET];
int hnd_wobjet, attr_objet = MOVER|NAME|CLOSER|UPARROW|DNARROW|VSLIDE|SIZER;
int xfo,yfo,wfo,hfo;			/* coord fenetre */
int max_wf;
int xfou,yfou,wfou,hfou;   /* coord utiles */
int w_obj_open=FALSE;
#define TXT_DECAL 3		/*decalage du texte vers e haut */

extern char chemin_anim_externe[150];
extern char chemin_img_externe[150];
extern int char_height;
extern OBJECT *adr_menu;			/* Adresse du menu */
extern int int_rel;
extern int use_nvdi;
extern int hnd_wpistes;
extern int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
extern int cur_visu;				/* visu active */
extern int visu_objet;
extern int aff_nombre;
extern PROJET projet;
extern int actu_projet;			/* is there any projet here ? */
extern int Magic_flag,Winix_flag;
extern IMP_MOD imp_mod[20];

void form_opto(int event);
void find_path(char *str);

int pop_fly(int index);

typedef char inom[14];


void new_objet(char type, char nom[10], char chm[150], char chm_key[150])
{	int x, y, nb_img, dummy;
	long dumy,mod;
	char dum[50];
	char *adr_tab_nom;
/*	inom *adr_tab_nom; */
	
	if(nb_objets == MAX_OBJET)
	{	super_alert(1,3,T_NO_MORE_OBJ_AVAIABLE);
		goto end_new_objet;
	}
	
	if(type == OBJ_FLH)
		get_flh_info(chm,dum,&x,&y,&nb_img);
	else if(type == OBJ_FLI || type == OBJ_FLC)
		get_fli_info(chm,dum,&x,&y,&nb_img,&dummy,&dummy,&dumy);
	else if(type == OBJ_MOD)
	{	if(get_mod_info(chm,&x,&y,&nb_img,&mod,objet[nb_objets].desc))
		{	objet[nb_objets].module_id = mod;
		}else
		{	super_alert(1,1,T_UNRECOGNISED_ANIM);
			goto end_new_objet;
		}
	}
	else if(type == OBJ_SUIT)
	{	if(get_suit_info(&x,&y,&nb_img,&objet[nb_objets].module_id, &objet[nb_objets].num_img ))
		{	adr_tab_nom = Malloc(nb_img*14L);
			if(adr_tab_nom)
			{	nb_img = create_liste(adr_tab_nom);
				objet[nb_objets].tab_noms = adr_tab_nom;
				
			}else
				goto end_new_objet;
		}else
			goto end_new_objet;
	}
	
	objet[nb_objets].type = type;
	objet[nb_objets].x = x;
	objet[nb_objets].y = y;
	objet[nb_objets].nb_img = nb_img;
	objet[nb_objets].def_in = 0;				/* par defaut toutes l'anim */
	objet[nb_objets].def_out = nb_img - 1;
	objet[nb_objets].img_preview = 0;
	strcpy(objet[nb_objets].nom,nom);
	strcpy(objet[nb_objets].chemin,chm);
	strcpy(objet[nb_objets].chemin_key,chm_key);
	
	/* m‚moire pour la preview */
	objet[nb_objets].preview.fd_addr = Malloc(4608);  /* 48*48*2 */
	if(objet[nb_objets].preview.fd_addr == 0)
	{	super_alert(1,3,T_NO_MEMORY);
		goto end_new_objet;
	}
	
	if(make_preview(nb_objets) == 0)
	{	super_alert(1,3,T_PREV_UNPOS);
		goto end_new_objet;
	}
		
	nb_objets++;
	
	slider_o();
	redraw_objet(hnd_wobjet, xfo, yfo, wfo, hfo);
	
	end_new_objet:;
}

int make_preview(int num_objet)
{	int *img, pos_x, pos_y,x_dest, y_dest;
	int x, y;
	
	x = objet[num_objet].x;
	y = objet[num_objet].y;
	
	objet[num_objet].preview.fd_w = 48;
	objet[num_objet].preview.fd_h = 48;
	objet[num_objet].preview.fd_wdwidth = 3; 
	objet[num_objet].preview.fd_stand = 0; 
	objet[num_objet].preview.fd_nplanes = 16; 
	
	fill(objet[num_objet].preview.fd_addr,48,48,48,0xd69a); /* rempli de LGRIS */
	
	/* realise la preview */
	
	img = Malloc((size_t)x * (size_t)y * 2);
	if(img == 0)
	{	super_alert(1,3,T_NO_MEMORY);
		Mfree(objet[num_objet].preview.fd_addr);
		return 0;
	}
		
	
	if(get_one_frame(num_objet,objet[num_objet].img_preview,img,x) == 0)
	{	
		Mfree(objet[num_objet].preview.fd_addr);
		Mfree(img);
		return 0;
	}
	
	if(x == y)
		x_dest = y_dest = 48;
	else if(x>y)
	{	x_dest = 48;
		y_dest = (y*48)/x;
	}else
	{	y_dest = 48;
		x_dest = (x*48)/y;
	}
	
	/* centrage */
	pos_x = (48-x_dest)/2;
	pos_y = (48-y_dest)/2;
	
	zoom(img,(int *)objet[num_objet].preview.fd_addr + pos_x + pos_y*48,x,y,x,x_dest,y_dest,48,1024);

	Mfree(img);
	return 1;
}

void efface_objet(int num_objet)
{	int num_prj_obj,liste[MAX_PRJ_OBJ],i;
	char str[120];
	
	if(num_objet < nb_objets)
	{	aff_nombre = 0;		/* vide le buffer pistes */
		
		num_prj_obj = list_related_prj_obj(num_objet,liste);
		if(num_prj_obj)
		{	sprintf(str,T_OBJ_USED_N_FOIS,num_prj_obj);
			if(super_alert(1,2,str) == 2)
				goto no_eff;
			for(i=0;i<num_prj_obj;i++)
				efface_prj_obj(liste[i]);
			redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
		}
		
		if(cur_visu && visu_objet == num_objet)
			close_visu();

		if(actu_projet && projet.obj_fond == num_objet)
		{	projet.obj_fond = -1;
			projet.mode_fond = 0;
		}
		
		if(objet[num_objet].preview.fd_addr)
			Mfree(objet[num_objet].preview.fd_addr);
		if(num_objet < nb_objets-1) /* c'est pas le dernier */
			memcpy(&objet[num_objet], &objet[num_objet+1], (nb_objets - num_objet - 1) * SIZE_OBJ);
		
		if(objet[num_objet].type == OBJ_SUIT)
			Mfree(objet[num_objet].tab_noms);
		
		nb_objets--;
		if(num_objet < deb_objet+(hfou/OBJ_H+1) && !( (num_objet<deb_objet)&&num_objet==nb_objets) )
			redraw_objet(hnd_wobjet, xfo, max(yfo,yfo+(num_objet-deb_objet)*OBJ_H), wfo, hfo);

		slider_o();
	}
	no_eff:;
}
		

int init_wobjet(int x, int y, int w, int h)
{	
	/* cood max utiles d'un fenetre */
	wind_calc(WC_WORK, attr_objet, x_desk, y_desk, w_desk, h_desk, &xfou, &yfou, &wfou, &hfou);

	/* on reduit a notre max */
	wfou = 284; 
	
	/* calcule les coord max de notre fenetre */
	wind_calc(WC_BORDER, attr_objet, xfou, yfou, wfou, hfou, &xfo, &yfo, &wfo, &hfo);
	
	max_wf = wfo;
	
	/* reserve un espace m‚moire */
	if((hnd_wobjet = wind_create(attr_objet, xfo, yfo, wfo, hfo)) <= 0)
	{	super_alert(1,3,T_NO_MORE_WINDOWS);
		return 0;
	}
	
	/* fixe les infos */
	wind_set(hnd_wobjet, WF_NAME, T_TITR_OBJ_GRAF);
	wind_set(hnd_wobjet, WF_VSLIDE, 0, 0, 0, 0);
	wind_set(hnd_wobjet, WF_VSLSIZE, 1000, 0, 0, 0);
	wind_set(hnd_wobjet, WF_BEVENT,1,0,0,0);
	
	/* CENTRE SI PAS DE PRESET*/
	if(x == 0 && y == 0)
	{	xfo = x_desk + (w_desk - wfo)/2;
		hfo = 250;
		yfo = y_desk + (h_desk - hfo)/2;
		hfo = min((y_desk+h_desk)-yfo,hfo);
	}else
	{	wfo = w;
		if(x+wfo>x_desk+w_desk)
			xfo = x_desk + (w_desk - wfo)/2;
		else
			xfo = x;
		if(y+h>y_desk+h_desk)
		{	hfo = 250;
			yfo = y_desk + (h_desk - hfo)/2;
			hfo = min((y_desk+h_desk)-yfo,hfo);
		}else
		{	yfo = y;
			hfo = h;
		}
	}
	
	return 1;
}

void redraw_1objet(int num_objet)
{	if(num_objet >= deb_objet && num_objet < deb_objet+(hfou/OBJ_H+1) )
		redraw_objet(hnd_wobjet, xfou, yfou +(num_objet - deb_objet)*OBJ_H, wfou, OBJ_H);
}

void window_objet(int event)
{	int old,choix,obj;
	char str[150] = "?";
	int mk,dum;
	int xt,yt,wt,ht;
	
	if (event & MU_BUTTON && !modal_flag)
	{	if(wind_find(mousex,mousey) == hnd_wobjet && mousex>xfou && mousex <xfou+wfou && mousey > yfou && mousey <yfou+hfou)
		{	obj = (mousey-yfou)/OBJ_H + deb_objet;
			if(obj >= 0 && obj < nb_objets)
			{	
				if(nbr_clics == 2)
					new_visu(obj);
				else
				{	Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync();
					Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync();
					graf_mkstate(&dum,&dum,&mk,&dum);
					if(mk == 1)		/* on appui encore */
						put_objet(obj);
					else
					{
						choix=pop_fly(POP_OBJET);
						if(choix == POBJ_EFF)
							efface_objet(obj);

						else if(choix == POBJ_INFO)
						{	info_objet = obj;
							form_opto(OPEN_DIAL);
						}else if(choix == POBJ_VISU )
							new_visu(obj);

						else if(choix == POBJ_VEXT)
						{	if((objet[obj].type == OBJ_FLH || objet[obj].type == OBJ_FLI || objet[obj].type == OBJ_FLC) && chemin_anim_externe[0])
							{	strcpy(str,chemin_anim_externe);
								find_path(str);
								Dsetdrv(str[0] - 'A');
								Dsetpath(str);
								strcpy(str+1,objet[obj].chemin);
								tmp_close_all_wind();
								Pexec(0,chemin_anim_externe,str,NULL);
								tmp_open_all_wind();
							}else if(objet[obj].type == OBJ_MOD)
							{	dum=find_module(objet[obj].module_id);
								if(imp_mod[dum].flags & 0x20) /* c'est une image */
								{	strcpy(str,chemin_img_externe);
									find_path(str);
									Dsetdrv(str[0] - 'A');
									Dsetpath(str);
									strcpy(str+1,objet[obj].chemin);
									tmp_close_all_wind();
									Pexec(0,chemin_img_externe,str,NULL);
									tmp_open_all_wind();
								}else
								{	strcpy(str,chemin_anim_externe);
									find_path(str);
									Dsetdrv(str[0] - 'A');
									Dsetpath(str);
									strcpy(str+1,objet[obj].chemin);
									tmp_close_all_wind();
									Pexec(0,chemin_anim_externe,str,NULL);
									tmp_open_all_wind();
								}
							}
							menu_bar(adr_menu, 1);
							form_dial (FMD_FINISH, 0, 0, 0, 0, x_desk, y_desk, w_desk, h_desk);
							graf_mouse(ARROW,0);
						}
					}
				}
			}else
				wind_set(hnd_wobjet, WF_TOP);
				
		}
	}
	else if (event & MU_MESAG)
	{
		switch(buff[0])
		{
			case WM_REDRAW :
				wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
				redraw_objet(buff[3], buff[4], buff[5], buff[6], buff[7]);
				break;
			case WM_TOPPED :
				if(!modal_flag)
					wind_set(buff[3], WF_TOP);
				break;
			case WM_CLOSED : 
				if(!modal_flag)
				{	wind_close(hnd_wobjet);
					w_obj_open=FALSE;
				}
				break;
			case WM_MOVED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7]);
				wind_get(hnd_wobjet,WF_CURRXYWH,&xfo, &yfo, &wfo, &hfo);
				wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
				break;
			case WM_SIZED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], min(max_wf,buff[6]), max(60,buff[7]));
				wind_get(hnd_wobjet,WF_CURRXYWH,&xfo, &yfo, &wfo, &hfo);
				xt = xfou; yt = yfou; wt = wfou; ht = hfou;
				wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
				if(deb_objet > nb_objets - hfou/OBJ_H)
					deb_objet = max(0, nb_objets - hfou/OBJ_H);
				if(Magic_flag || Winix_flag)
				{	if(wt<wfou)		/* agrandissement hor */
						redraw_objet(hnd_wobjet, xt+wt-2, yt, 8, ht);
					if(ht<hfou)		/* agrandissement vert */
						redraw_objet(hnd_wobjet, xt, yt+ht-2, wt, 8);
				}
				slider_o();
				break;
			case WM_ARROWED :
				wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
				old = deb_objet;
				if(buff[4] == WA_UPLINE && deb_objet)
					deb_objet--;
				else if(buff[4] == WA_DNLINE && nb_objets > (hfou/OBJ_H + deb_objet) )
					deb_objet++;
				else if(buff[4] == WA_UPPAGE)
					deb_objet = max(0,deb_objet - hfou/OBJ_H);
				else if(buff[4] == WA_DNPAGE)
				{	deb_objet = deb_objet + hfou/OBJ_H;
					if(deb_objet > nb_objets - hfou/OBJ_H)
						deb_objet = max(0, nb_objets - hfou/OBJ_H);
				}
				
				if(old != deb_objet)
				{	slider_o();
					redraw_objet(hnd_wobjet, xfo, yfo, wfo, hfo);
				}
				break;
			case WM_VSLID :
				old = deb_objet;
				deb_objet = ((nb_objets-(hfou/OBJ_H)) * buff[4])/ 1000;
				if(old != deb_objet)
				{	wind_get(hnd_wobjet,WF_WORKXYWH,&xfou, &yfou, &wfou, &hfou);
					slider_o();
					redraw_objet(hnd_wobjet, xfo, yfo, wfo, hfo);
				}
 				break;
			
		}
	}
}

void slider_o(void)
{	int y_ass,h_ass;
	
	if(hfou/OBJ_H >= nb_objets)
	{	wind_set(hnd_wobjet, WF_VSLIDE, 0, 0, 0, 0);
		wind_set(hnd_wobjet, WF_VSLSIZE, 1000, 0, 0, 0);
	}else
	{	h_ass = ((hfou/OBJ_H) * 1000) / nb_objets;
		y_ass = (deb_objet * 1000) / (nb_objets-(hfou/OBJ_H));
		wind_set(hnd_wobjet, WF_VSLSIZE, h_ass, 0, 0, 0);
		wind_set(hnd_wobjet, WF_VSLIDE, y_ass, 0, 0, 0);
	}
}	

void redraw_objet(int w_handle, int x, int y, int w, int h)
{
	GRECT r, rd,rimg,rt;
	int i,y_base,pxy[8], nb_objet,dum,fonte=1;
	char chaine[80];
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
			
	rt.g_x = xfou+1;
	rt.g_y = yfou+1;
	rt.g_w = wfou-2;
	rt.g_h = hfou-2;
	
	if(w_handle<1)
	{	goto end_redraw;
	}
	
	v_hide_c(vdi_handle);
	wind_update(BEG_UPDATE);
	if(use_nvdi)
		fonte = 5003;
	vst_font( vdi_handle, fonte );	/* swiss */
	
	nb_objet = hfou/OBJ_H + 1;
	
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)
	{
		if (rc_intersect(&rd, &r))
		{	
			set_clip(1, &r);

			vswr_mode(vdi_handle,MD_REPLACE);
			rect_full(r.g_x,r.g_y,r.g_w,r.g_h,color_3d1*int_rel);		/* un rect_full gris */
			
			for(i=0;i<nb_objet;i++)
			{	y_base = i*OBJ_H + yfou;
				rect_3d(xfou, y_base, wfou, OBJ_H); 
			}
			
			if (rc_intersect(&rt, &r))
			{	
				set_clip(1, &r);

				vswr_mode(vdi_handle,MD_TRANS);
				for(i=0;i<nb_objets-deb_objet;i++)
				{	y_base = i*OBJ_H + yfou;
				
					vst_height(vdi_handle,char_height+2,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,1);		/* le nom en gras */
					v_gtext(vdi_handle,xfou + 62,y_base + 20 - TXT_DECAL,objet[i+deb_objet].nom);

					vst_height(vdi_handle,char_height,&dum,&dum,&dum,&dum);
					vst_effects(vdi_handle,0);		/* le reste */
					switch(objet[i+deb_objet].type)
					{	case OBJ_FLH :
								v_gtext(vdi_handle,xfou + 62,y_base + 36 - TXT_DECAL,T_FLH_ANIM);
								sprintf(chaine,T_DDD_IMG_DD, objet[i+deb_objet].x,objet[i+deb_objet].y,objet[i+deb_objet].nb_img,objet[i+deb_objet].def_in,objet[i+deb_objet].def_out);
								break;
						case OBJ_FLI :
								v_gtext(vdi_handle,xfou + 62,y_base + 36 - TXT_DECAL,T_FLI_ANIM);
								sprintf(chaine,T_DDD_IMG_DD, objet[i+deb_objet].x,objet[i+deb_objet].y,objet[i+deb_objet].nb_img,objet[i+deb_objet].def_in,objet[i+deb_objet].def_out);
								break;
						case OBJ_FLC :
								v_gtext(vdi_handle,xfou + 62,y_base + 36 - TXT_DECAL,T_FLC_ANIM);
								sprintf(chaine,T_DDD_IMG_DD, objet[i+deb_objet].x,objet[i+deb_objet].y,objet[i+deb_objet].nb_img,objet[i+deb_objet].def_in,objet[i+deb_objet].def_out);
								break;
						case OBJ_SUIT :
								v_gtext(vdi_handle,xfou + 62,y_base + 36 - TXT_DECAL,T_SUIT_DIMG);
								sprintf(chaine,T_DDD_IMG_DD, objet[i+deb_objet].x,objet[i+deb_objet].y,objet[i+deb_objet].nb_img,objet[i+deb_objet].def_in,objet[i+deb_objet].def_out);
								break;
						case OBJ_MOD :
								v_gtext(vdi_handle,xfou + 62,y_base + 36 - TXT_DECAL,objet[i+deb_objet].desc);
								sprintf(chaine,T_DDD_IMG_DD, objet[i+deb_objet].x,objet[i+deb_objet].y,objet[i+deb_objet].nb_img,objet[i+deb_objet].def_in,objet[i+deb_objet].def_out);
								break;
					}
					v_gtext(vdi_handle,xfou + 62,y_base + 52 - TXT_DECAL,chaine);
				
					/* la petite preview */
					rimg.g_x = xfou + 4;
					rimg.g_y = y_base + 4;
					rimg.g_w = 48;
					rimg.g_h = 48;
				
					if(rc_intersect(&r, &rimg))
					{	
					/* Zone source dans l'image en m‚moire : */
						pxy[0] = rimg.g_x - (xfou+4);	
						pxy[1] = rimg.g_y - (y_base+4);	
						pxy[2] = pxy[0] + rimg.g_w - 1;	
						pxy[3] = pxy[1] + rimg.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
						pxy[4] = rimg.g_x;							
						pxy[5] = rimg.g_y;							
						pxy[6] = pxy[4] + rimg.g_w - 1;	
						pxy[7] = pxy[5] + rimg.g_h - 1;	
						tnt_cpyfm (pxy, &objet[i+deb_objet].preview);
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