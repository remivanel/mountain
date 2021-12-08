/********************MOUTAIN********************/
/*****   GESTION de la fenˆtre des pistes   ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/


#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "flh_lib.h"
#include "gest_flh.h"
#include "mountain.h"
#include "objets.h"
#include "transit.h"
#include "geffets.h"
#include "pistes.h"
#include "effets1.h"
#include "mouvemnt.h"
#include "visu.h"
#include "anim.h"
#include "projet.h"
#include "preview.h"
#include "some_fnc.h"
#include "transp.h"
#include "txts.h"
#include "time_spl.h"

int  abs( int i );
void wait_mk(void);

int hnd_wpistes, attr_pistes = MOVER|NAME|CLOSER|FULLER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE|SIZER;
int xfp,yfp,wfp,hfp;			/* coord fenetre */
int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
PRJ_OBJ prj_obj[MAX_PRJ_OBJ];

extern PROJET projet;
extern int actu_projet;			/* is there any projet here ? */
extern int selected_prj_obj;
extern int mode_smpte;
extern int Magic_flag,Winix_flag;
extern WINDFORM_VAR supi_var;		/* Suppression de piste */

/* FENETRE */
int deb_img = 0;		/* premiere img affichee */
int deb_piste = 0;	/* premiere piste affichee */
extern int preview_pos;
int ful_p = 0;
int piste_animA = 1;
int piste_animB = 1;
int piste_supA = 0;
int piste_supB = 0;
int piste_tran = 1;
int piste_sup = 0;
int draw_intensitee;
int aff_mode,aff_prop;	/* affichage des piste 0:full 1:debend 2:rien */
int aff_size_buffer;	/* taille du buff */
int *aff_buffer=0, aff_nombre, aff_max_nombre, aff_cur_pos;
BUF_OBJ buf_obj[MAX_BUF_OBJ];
static MFDB img_rsc_time;
static MFDB img_rsc_fl_up;
static MFDB img_rsc_fl_down;

#define img2pix(A) ( (int)(((((long)A-deb_img)*48L)/(long)projet.zoom) + x_base))
#define pix2img(A) ((int)(( ((long)A-x_base)*(long)projet.zoom )/48L + deb_img))

extern int char_height;
extern int int_rel;
extern int use_nvdi;
extern OBJET objet[MAX_OBJET];

/* transitions */
extern int nb_used_transit;	/* nombre de transitions UTILISEE */
extern TRANSITION *transition[MAX_TRAN];
extern PRJ_TRANSIT prj_transit[MAX_PRJ_TRAN];


/* proto */
static void draw_piste_tran(int x_base, int y_base, int x,int y,int w,int h);
static void draw_used_tran(int cur_tran, int x_base, int y_base, int x,int y,int w,int h);
static void draw_courb_transit(int time, int x_base, int y_base, int cur_tran, int modulo);
static int nb_prj_obj_on(int super);


int init_wpistes(int x, int y, int w, int h)
{	OBJECT *adr_rsc_images;

	/* cood max utiles d'un fenetre */
	wind_calc(WC_WORK, attr_pistes, x_desk, y_desk, w_desk, h_desk, &xfpu, &yfpu, &wfpu, &hfpu);

	/* calcule les coord max de notre fenetre */
	wind_calc(WC_BORDER, attr_pistes, xfpu, yfpu, wfpu, hfpu, &xfp, &yfp, &wfp, &hfp);
	/* reserve un espace m‚moire */
	
	if((hnd_wpistes = wind_create(attr_pistes, xfp, yfp, wfp, hfp)) <= 0)
	{	super_alert(1,3,T_NO_MORE_WINDOWS);
		return 0;
	}
	
	/* fixe les infos */
	wind_set(hnd_wpistes, WF_NAME, T_NOM_PISTES );
	wind_set(hnd_wpistes, WF_VSLIDE, 0, 0, 0, 0);
	wind_set(hnd_wpistes, WF_HSLIDE, 0, 0, 0, 0);
	wind_set(hnd_wpistes, WF_VSLSIZE, 1000, 0, 0, 0);
	wind_set(hnd_wpistes, WF_HSLSIZE, 1000, 0, 0, 0);
	wind_set(hnd_wpistes, WF_BEVENT,1,0,0,0);
	
	/* CENTRE SI PAS DE PRESET*/
	if(x == 0 && y == 0)
	{	wfp = 2*w_desk/3;
		xfp = x_desk + (w_desk - wfp)/2;
		hfp = 200;
		yfp = y_desk + (h_desk - hfp)/2;
		hfp = min((y_desk+h_desk)-yfp,hfp);
	}else
	{	if(x+w>x_desk+w_desk)
		{	wfp = 2*w_desk/3;
			xfp = x_desk + (w_desk - wfp)/2;
		}
		else
		{	xfp = x;
			wfp = w;
		}
		if(y+h>h_desk)
		{	hfp = 250;
			yfp = y_desk + (h_desk - hfp)/2;
			hfp = min((y_desk+h_desk)-yfp,hfp);
		}else
		{	yfp = y;
			hfp = h;
		}
	}
	
	rsrc_gaddr(0, IMAGES, &adr_rsc_images);
	img_rsc_time.fd_addr = adr_rsc_images[IMG_TIME].ob_spec.bitblk->bi_pdata;
	img_rsc_time.fd_w = 16;
	img_rsc_time.fd_h = 28;
	img_rsc_time.fd_wdwidth = 1;
	img_rsc_time.fd_stand = 0;
	img_rsc_time.fd_nplanes = 1;
	img_rsc_fl_up.fd_addr = adr_rsc_images[IMG_FL_HT].ob_spec.bitblk->bi_pdata;
	img_rsc_fl_up.fd_w = 16;
	img_rsc_fl_up.fd_h = 16;
	img_rsc_fl_up.fd_wdwidth = 1;
	img_rsc_fl_up.fd_stand = 0;
	img_rsc_fl_up.fd_nplanes = 1;
	img_rsc_fl_down.fd_addr = adr_rsc_images[IMG_FL_BAS].ob_spec.bitblk->bi_pdata;
	img_rsc_fl_down.fd_w = 16;
	img_rsc_fl_down.fd_h = 16;
	img_rsc_fl_down.fd_wdwidth = 1;
	img_rsc_fl_down.fd_stand = 0;
	img_rsc_fl_down.fd_nplanes = 1;
	
	return 1;
}

void form_supi(int event)
{
	WINDFORM_VAR *ptr_var = &supi_var;
	int choix;
	
	if (event == OPEN_DIAL)
	{	open_dialog(ptr_var, T_TITR_SUPP, 0, -1);	
	}
		
	else 
	{	choix = windial_do(ptr_var, event);
		if (choix != 0)
		{
			switch(choix)
			{
				case CLOSE_DIAL:
					close_dialog(ptr_var);
					break;
				case SUPI_A:
					if(projet.nb_sup_A)
					{	if(nb_prj_obj_on(0)==0)
						{	projet.nb_sup_A--;
							redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
						}else
							super_alert(1,3,T_SILL_OBJ);
					}else
						super_alert(1,3,T_NO_SUCH_TRACK);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
				case SUPI_B:
					if(projet.nb_sup_B)
					{	if(nb_prj_obj_on(1)==0)
						{	projet.nb_sup_B--;
							redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
						}else
							super_alert(1,3,T_SILL_OBJ);
					}else
						super_alert(1,3,T_NO_SUCH_TRACK);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
				case SUPI_G:
					if(projet.nb_sup_G)
					{	if(nb_prj_obj_on(2)==0)
						{	projet.nb_sup_G--;
							redraw_pistes(hnd_wpistes,xfpu,yfpu,wfpu,hfpu);
						}else
							super_alert(1,3,T_SILL_OBJ);
					}else
						super_alert(1,3,T_NO_SUCH_TRACK);
					wf_change(ptr_var,choix,0,1);
					close_dialog(ptr_var);
					break;
			}
		}
	}
}

static int nb_prj_obj_on(int super)
{	int i,cnt=0,piste;
	if(super==0)	/* A */
		piste=50+projet.nb_sup_A-1;
	else if(super==1) /* B */
		piste=100+projet.nb_sup_B-1;
	else /* GENERAL */
		piste=150+projet.nb_sup_G-1;
	
	for(i=0;i<projet.nb_obj;i++)
		if(prj_obj[i].piste == piste)
			cnt++;
	return cnt;
}
int make_reduc(int cur_objet, int pos, int *img_dest, int *img, int *canal, int *old_frame)
{	int x_dest, y_dest, h_src;
	int i,pos_buffer, *real_dest;
	
	/* TESTE LE BUFFER SI ON L'A DEJA */
	
	for(i=0; i<aff_nombre; i++)
	{	if(buf_obj[i].objet == cur_objet && buf_obj[i].pos == pos)
		{	/* on l'a dans le buffer */
			copy(aff_buffer + (long)i*2304L, img_dest, 2304L);
			return 1;
		}
	}
	
	
	/* TROUVE UNE PLACE DANS LE BUFFER */
	if(aff_max_nombre)
	{	if(aff_nombre<aff_max_nombre)
		{	pos_buffer = aff_nombre;		/* il reste de la place */
			aff_nombre++;
		}else
		{	pos_buffer = aff_cur_pos++;		/* on efface le plus ancien */
			if(aff_cur_pos == aff_max_nombre)
				aff_cur_pos = 0;
		}
		buf_obj[pos_buffer].objet = cur_objet;
		buf_obj[pos_buffer].pos = pos;
		real_dest = aff_buffer + (long)pos_buffer*2304L;
	}else
	{	pos_buffer = -1;
		real_dest = img_dest;
	}
	
	/* TROUVE LE COORDONEES VOULUE */
	
	x_dest = 48;
	h_src = objet[cur_objet].y;
	if(!aff_prop)
		y_dest = 48;
	else
	{	if(objet[cur_objet].x == objet[cur_objet].y)
			y_dest = 48;
		else if(objet[cur_objet].x > objet[cur_objet].y)
			y_dest = (objet[cur_objet].y*48)/objet[cur_objet].x;
		else /* y > x */
		{	y_dest = 48;
			h_src = objet[cur_objet].x; /* on limite les hauteur */
		}
	}	
	
	/* FAIS LA PREVIEW */
	
	if(*canal == -1)			/* le canal n'a pas encore ete ouvert */
	{	*canal = open_anim(cur_objet);
		if(*canal == -1)
			return 0;
	}
	
	
	if(get_frame( *canal, pos, *old_frame, img, objet[cur_objet].x) == 0)
		return 0;
		
	*old_frame = pos;
	
	fill(real_dest,48,48,48,0xd69a); /* rempli de LGRIS */
	
	zoom(img,real_dest, objet[cur_objet].x, h_src,
								objet[cur_objet].x, x_dest,y_dest,48,1024);
	
	if(pos_buffer != -1)
		copy(real_dest, img_dest, 2304L);

	
	return 1;
}

void aff_pos_preview(int pos)
{	int temp,x,x_base=xfpu+18;

	if(pos == preview_pos)
		return;		/* pour pas repeter tj la meme chose */
	
	set_clip(0,0);
	
	temp=preview_pos;
	if(preview_pos != -1)		/* on efface l'ancien si y a */
	{	preview_pos = -1;
		x=img_to_pix(temp,x_base);
		redraw_pistes(hnd_wpistes,x-6,yfpu+8,12,10);
	}
	
	preview_pos = pos;
	x=img_to_pix(pos,x_base);
	redraw_pistes(hnd_wpistes,x-6,yfpu+8,12,10);
	preview_pos = temp;
}

void draw_bar_prev(int in, int out)
{	int x_base = xfpu + 19;
	int x,w;

	v_hide_c(vdi_handle);
	vswr_mode(vdi_handle,MD_XOR);
	
	out++;
	
	x=img_to_pix(in,x_base);
	if(x<xfpu+18)
		x=xfpu+18;
	else
		ligne(x-1,yfpu,1,hfpu,BLACK);		/* ligne vert gauche */

	w=img_to_pix(out,x_base)-x-1;

	if(x+w>xfpu+wfpu)		/* clip hor */
		w = xfpu+wfpu-x;
	else
		ligne(x+w-1,yfpu,1,hfpu,BLACK);		/* ligne vert droite */

	rect_full(x,yfpu,w,8,BLACK);
	v_show_c(vdi_handle, 1);
	vswr_mode(vdi_handle,MD_REPLACE);
}

void move_bar_prev(int mousex)
{	int mx,my,mk,dum,wich,decal,old,pos,old2;
	int x_base = xfpu + 18;
	int projet_in = projet.preview_in, projet_out= projet.preview_out;
	int larg_img;
	
	set_clip(0,0);
	
	larg_img = (int)(((wfpu-18)*(long)projet.zoom)/48L);

	
	if(mousex<img_to_pix( (projet.preview_in+projet.preview_out)/2,x_base))
	{	wich = 0;		/* on modifie le debut */
		decal = mousex - img_to_pix(projet.preview_in,x_base);
		old = projet.preview_in;
	}else
	{	wich = 1;
		decal = mousex - img_to_pix(projet.preview_out-1,x_base);
		old = projet.preview_out;
	}
	
	draw_bar_prev(projet.preview_in, projet_out);

	pos = old;
	graf_mkstate(&mx,&my,&mk,&dum);
	do
	{	old2=deb_img;
		if(mx>xfpu && mx<xfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img)	/* scrool << */
		{	if(!wich)	draw_bar_prev(old, projet_out);
			else	draw_bar_prev(projet_in,old);

			Vsync();				Vsync();
			deb_img = max(0,deb_img-projet.zoom);

			v_hide_c(vdi_handle);
			scrool_left(old2);
			v_show_c(vdi_handle,1);

			if(!wich)	draw_bar_prev(old, projet_out);
			else	draw_bar_prev(projet_in,old);
			
		}else if(mx>xfpu+wfpu && mx<xfpu+wfpu+16 && my>yfpu && my<yfpu+hfpu && deb_img+larg_img<projet.lenght)	/* scrool >> */
		{	if(!wich)	draw_bar_prev(pos, projet_out);
			else	draw_bar_prev(projet_in,pos);
			deb_img += projet.zoom;
			Vsync();				Vsync();
			if(deb_img != old2)
			{	v_hide_c(vdi_handle);
				scrool_right(old2);
				v_show_c(vdi_handle,1);
			}
			if(!wich)	draw_bar_prev(pos, projet_out);
			else	draw_bar_prev(projet_in,pos);
		}
		mx = mx-decal+1;
	
		pos = pix2img(mx);
		if(!wich)
		{	pos = max(0,pos);
			pos = min(pos,projet_out);
		}else
		{	pos++;
			pos = max(projet_in,pos);
			pos = min(pos,(int)projet.lenght-1);
		}
		if(pos != old)
		{	if(!wich)
			{	projet_in = pos;
				Vsync();
				draw_bar_prev(old, projet_out);
				draw_bar_prev(pos, projet_out);
			}
			else
			{	projet_out = pos;
				Vsync();
				draw_bar_prev(projet_in, old);
				draw_bar_prev(projet_in,pos);
			}
			old=pos;
		}
		graf_mkstate(&mx,&my,&mk,&dum);
	}while(mk);
	if(!wich)	draw_bar_prev(pos, projet_out);
	else	draw_bar_prev(projet_in,pos);

	projet.preview_in = projet_in;
	projet.preview_out = projet_out;
	redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,10);
}

void window_pistes(int event)
{	int old,dum,mk,mx,my;
	int larg,choix;
	int pos,piste,obj, x_base;
	int nb_pistes, fin_x;
	int nb_aff_pistes;
	int xt,yt,wt,ht;
	
	if (event & MU_BUTTON && !modal_flag )
	{	if(wind_find(mousex,mousey) == hnd_wpistes && mousex>xfpu+18 && mousex <xfpu+wfpu && mousey > yfpu && mousey <yfpu+hfpu)
		{	/* redirigement suivant le zone de clique */
			x_base = xfpu + 18;
			if( (mousex-xfpu)>18 && (mousey-yfpu)>8 && (mousey-yfpu)<32)	/* ZONE DE PREVIEW INSTANTANEE */
			{	mx=mousex;
				old = -1;
				do
				{	pos = pix2img(mx+1);
					if(old != pos)
					{	set_curseur(pos);
						aff_pos_preview(pos);
						new_preview(pos);
						form_mem(UPDT_MEM);
						old = pos;
					}
					graf_mkstate(&mx,&my,&mk,&dum);
				}while((mx-xfpu)>18 /*&& (my-yfpu)>8 && (my-yfpu)<32*/ && mk==1);
				end_preview();
				form_mem(UPDT_MEM);
				do{	graf_mkstate(&mx,&my,&mk,&dum);
				}while(mk);
			}else if( (mousex-xfpu)>18 && mousey>yfpu && (mousey-yfpu)<8)	/* ZONE DE BARRE DE PREVIEW  */
			{	move_bar_prev(mousex);
			}else if(find_pos_piste(mousex, mousey, &piste, &pos))	/* ZONE DES PISTES */
			{
				obj = find_anim(pos, piste);
				if(obj != -1)
				{	fin_x = img_to_pix(prj_obj[obj].pos_out+1,x_base);	/* le dernier pixel */
					Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync();
					Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync(); Vsync();
					graf_mkstate(&dum,&dum,&mk,&dum);
					if(nbr_clics == 2)		/* dble click, pop_tools */
					{	if(selected_prj_obj != obj)	/* sel obj */
						{	if(selected_prj_obj != -1)
							{	dum = selected_prj_obj;
								selected_prj_obj = obj;
								draw_obj_prj(dum,1);
							}else
								selected_prj_obj = obj;

							draw_obj_prj(obj,1);
							wait_mk();
						}
						choix=pop_fly(POP_OBJ_PRJ);
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
					}
					else if(mk == 1 && mousex>fin_x-8 && mousex<fin_x)		/* on appui encore et on est ds la zone d'alongement */
						widen_objet(obj);
					else if(mk == 1)		/* on appui encore */
						move_objet(obj);
					else
					if(selected_prj_obj != obj)
					{	if(selected_prj_obj != -1)
						{	dum = selected_prj_obj;
							selected_prj_obj = obj;
							draw_obj_prj(dum,1);
						}else
							selected_prj_obj = obj;

						draw_obj_prj(obj,1);
						wait_mk();
					}
				}else if(piste == 2)		/* les transitions*/
				{	obj = find_tran(pos);
					if(obj != -1)
					{	gere_clic_tran(obj,mousex);
					}else
						wind_set(hnd_wpistes, WF_TOP);
				}else
					wind_set(hnd_wpistes, WF_TOP);
			}else
				wind_set(hnd_wpistes, WF_TOP);
		}
	}
	else if (event & MU_MESAG)
	{
		larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);
		switch(buff[0])
		{
			case WM_REDRAW :
				wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
				redraw_pistes(buff[3], buff[4], buff[5], buff[6], buff[7]);
				break;
			case WM_TOPPED :
				if(!modal_flag)
					wind_set(buff[3], WF_TOP);
				break;
			case WM_CLOSED : 
				if(!modal_flag)
				{	wind_close(hnd_wpistes);
					w_trk_open=FALSE;
				}
				break;
			case WM_MOVED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7]);
				wind_get(hnd_wpistes,WF_CURRXYWH,&xfp, &yfp, &wfp, &hfp);
				wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
				break;
			case WM_SIZED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], max(60,buff[7]));
				wind_get(hnd_wpistes,WF_CURRXYWH,&xfp, &yfp, &wfp, &hfp);
				xt = xfpu; yt = yfpu; wt = wfpu; ht = hfpu;
				wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
				larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);
				if(deb_img > projet.lenght - larg)
					deb_img = max(0, (int)projet.lenght - larg);

				slider_ph();
				slider_pv();
				if(Magic_flag || Winix_flag)
				{	if(wt<wfpu)		/* agrandissement hor */
						redraw_pistes(hnd_wpistes, xt+wt-2, yt, 8, ht);
					if(ht<hfpu)		/* agrandissement vert */
						redraw_pistes(hnd_wpistes, xt, yt+ht-2, wt, 8);
				}
				ful_p = 0;									/* Annuler flag */
				break;
			case WM_ARROWED :
				if(buff[4] == WA_LFLINE || buff[4] == WA_RTLINE || buff[4] == WA_LFPAGE || buff[4] == WA_RTPAGE)
				{	old = deb_img;
					if(buff[4] == WA_LFLINE && deb_img)
					{	v_hide_c(vdi_handle);
						deb_img = max(0,deb_img-projet.zoom);
						
						scrool_left(old);
						
						v_show_c(vdi_handle,1);
					}
					else if(buff[4] == WA_RTLINE && projet.lenght > (larg + deb_img) )
					{	v_hide_c(vdi_handle);
						deb_img += projet.zoom;
						if(deb_img > projet.lenght - larg)
							deb_img = max(0, (int)projet.lenght - larg);

						scrool_right(old);

						v_show_c(vdi_handle,1);
					}
					else if(buff[4] == WA_LFPAGE && deb_img)
					{	deb_img = max(0,deb_img-larg);
						redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
						slider_ph();
					}
					else if(buff[4] == WA_RTPAGE)
					{	deb_img += larg;
						if(deb_img > projet.lenght - larg)
							deb_img = max(0, (int)projet.lenght - larg);
						redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
						slider_ph();
					}
				}else  /* fleches verticales */
				{	nb_pistes = piste_animA + piste_animB + projet.nb_sup_A*piste_supA +
						 		  projet.nb_sup_B*piste_supB + piste_tran + projet.nb_sup_G*piste_sup;
					nb_aff_pistes = calc_nb_aff_pistes();
					
					if(buff[4] == WA_UPLINE && deb_piste)
					{	deb_piste--;
						redraw_pistes(hnd_wpistes,xfpu,yfpu+32,wfpu,hfpu-32);
						slider_pv();
					}else if(buff[4] == WA_DNLINE && (deb_piste+nb_aff_pistes)<nb_pistes )
					{	deb_piste++;
						redraw_pistes(hnd_wpistes,xfpu,yfpu+32,wfpu,hfpu-32);
						slider_pv();
					}else if(buff[4] == WA_UPPAGE && deb_piste)
					{	deb_piste = max(0,deb_piste-nb_aff_pistes);
						redraw_pistes(hnd_wpistes,xfpu,yfpu+32,wfpu,hfpu-32);
						slider_pv();
					}else if(buff[4] == WA_DNPAGE)
					{	deb_piste += nb_aff_pistes;
						if(deb_piste > nb_pistes - nb_aff_pistes)
							deb_piste = max(0, nb_pistes - nb_aff_pistes);
						redraw_pistes(hnd_wpistes,xfpu,yfpu+32,wfpu,hfpu-32);
						slider_pv();
					}
					
				}
				break;
			case WM_VSLID :
				nb_pistes = piste_animA + piste_animB + projet.nb_sup_A*piste_supA +
						 		  projet.nb_sup_B*piste_supB + piste_tran + projet.nb_sup_G*piste_sup;
				nb_aff_pistes = calc_nb_aff_pistes();
				old = deb_piste;
				deb_piste = (int)( (( (long)nb_pistes-nb_aff_pistes) * (long)buff[4])/ 1000L);
				if(old != deb_piste)
				{	redraw_pistes(hnd_wpistes,xfpu,yfpu+32,wfpu,hfpu-32);
					slider_pv();
				}
				break;
			case WM_HSLID :
				old = deb_img;
				deb_img = (int)( (( (long)projet.lenght-larg) * (long)buff[4])/ 1000L);
				if(old != deb_img)
				{	if( abs(old-deb_img + projet.zoom)>larg)
					{	redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
						slider_ph();
					}else		/* opt */
					{	v_hide_c(vdi_handle);
						set_clip(0,0);
						if(old>deb_img)			/* on recule */
							scrool_left(old);
						else 							/* on avance */
							scrool_right(old);

						v_show_c(vdi_handle,1);
					}
				}
 				break;
			case WM_FULLED : 
				if(ful_p)
				{	wind_get (buff[3], WF_PREVXYWH, &xfp, &yfp, &wfp, &hfp); /* Coord. pr‚c‚dentes */
					wind_set(buff[3], WF_CURRXYWH, xfp, yfp, wfp, hfp);
					ful_p = 0;									/* Annuler flag */
				}else
				{	wind_calc(WC_WORK, attr_pistes, x_desk, y_desk, w_desk, h_desk, &xfpu, &yfpu, &wfpu, &hfpu);	/* max */
					hfpu = min(hfpu, calc_hpiste()+4);
					wfpu = min(wfpu, (int)((long)projet.lenght*48L/projet.zoom)+40 );
					xfpu = x_desk + (w_desk - wfpu)/2;
					yfpu = y_desk + (h_desk - hfpu)/2;
					wind_calc(WC_BORDER, attr_pistes, xfpu, yfpu, wfpu, hfpu, &xfp, &yfp, &wfp, &hfp);
					wind_set(buff[3], WF_CURRXYWH, xfp, yfp, wfp, hfp);
					ful_p = 1;									/* positionner flag */
				}	
				wind_get(hnd_wpistes,WF_WORKXYWH,&xfpu, &yfpu, &wfpu, &hfpu);
				slider_pv();
				slider_ph();
 				break;
					
		}
	}
}

void scrool_left(long old)
{	int scrool;

	if(projet.zoom<20)
	{	scrool = (int)( (((long)old - (long)deb_img)*48L)/projet.zoom );
		screen_copy(xfpu+20, yfpu, wfpu-scrool-20, hfpu, xfpu+scrool+20, yfpu);
		redraw_pistes(hnd_wpistes,xfpu+18,yfpu,scrool+40,hfpu);
	}else						
		redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
	slider_ph();
}

void scrool_right(long old)
{	int scrool;
	
	if(projet.zoom<20)
	{	scrool = (int)( (((long)deb_img - (long)old)*48L)/projet.zoom );
		if(scrool+22 < wfpu)
			screen_copy(xfpu+19+scrool, yfpu, wfpu-scrool-22, hfpu, xfpu+19, yfpu);
		redraw_pistes(hnd_wpistes,xfpu+wfpu-(scrool+15),yfpu,scrool+15,hfpu);

/*		redraw_pistes(hnd_wpistes,xfpu+wfpu-50,yfpu,50,hfpu);*/
	}else						
		redraw_pistes(hnd_wpistes,xfpu+18,yfpu,wfpu-18,hfpu);
	slider_ph();
}

int find_pos_piste(int mx,int my,int *piste,int *pos_in)
{	int x_base,y_base,i,deb=0;
	
	if(mx>xfpu && mx<xfpu+wfpu && my>yfpu+32 && my<yfpu+hfpu)
	{	x_base = xfpu+18;
		y_base = yfpu+32;
		/* piste */
		*piste = 200;	/* defaut : aucune piste */
		/* piste A */
		if(piste_animA )
		{	if(deb_piste==0)
			{	if(my>=y_base && my<=y_base+50)
				{	*piste = 0;
					goto ok_piste;
				}
				y_base += 50;
			}
			deb++;
		}
		
		/* piste sup A */
		if(piste_supA)
		{	for(i=0;i<projet.nb_sup_A;i++)
			{	if(deb >= deb_piste)
				{	if(my>=y_base && my<=y_base+66)
					{	*piste = 50+i;
						goto ok_piste;
					}
					y_base += 66;
				}
				deb++;
			}
		}
		
		/* piste tran */
		if(piste_tran)
		{	if(deb >= deb_piste)
			{	if(my>=y_base && my<=y_base+66)
				{	*piste = 2;
					goto ok_piste;
				}
				y_base += 66;
			}
			deb++;
		}
		
		/* piste B */
		if(piste_animB)
		{	if(deb >= deb_piste)
			{	if(my>=y_base && my<=y_base+50)
				{	*piste = 1;
					goto ok_piste;
				}
				y_base += 50;
			}
			deb++;
		}
		
		/* piste sup B */
		if(piste_supB)
		{	for(i=0;i<projet.nb_sup_B;i++)
			{	if(deb >= deb_piste)
				{	if(my>=y_base && my<=y_base+66)
					{	*piste = 100+i;
						goto ok_piste;
					}
					y_base += 66;
				}
				deb++;
			}
		}
		
		/* piste sup Gene */
		if(piste_sup)
		{	for(i=0;i<projet.nb_sup_G;i++)
			{	if(deb >= deb_piste)
				{	if(my>=y_base && my<=y_base+66)
					{	*piste = 150+i;
						goto ok_piste;
					}
					y_base += 66;
				}
				deb++;
			}
		}
		
		if(*piste == 200)		/* probleme */
			return 0;
		
		ok_piste:;
		/* position */
		*pos_in=pix2img(mx);
		*pos_in=max(0,*pos_in);		/* si on est avant le debut */
		
		return 1;
	}else
		return 0;
}
	
int calc_hpiste(void)
{	int h;
	
	h = 32+ piste_animA*50 + piste_animB*50 + projet.nb_sup_A*66*piste_supA +
	 		  projet.nb_sup_B*66*piste_supB + piste_tran*66 + projet.nb_sup_G*66*piste_sup;
	         
	return h;
}

void aff_prev(int piste,int pos_in, int pos_out)
{	int x_base,x,y,w,h,i,deb=0;
	
	
	y = yfpu+33;
		/* piste A */
	if(piste == 0 && deb_piste == 0)
		goto ok_ypiste;
	if(piste_animA)
	{	if(deb_piste==0)
			y += 50;
		deb++;
	}
		
	/* piste sup A */
	if(piste_supA)
	{	for(i=0;i<projet.nb_sup_A;i++)
		{	if(piste == i+50 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
		
	/* piste tran */
	if(piste == 2 && deb >= deb_piste)
		goto ok_ypiste;
	if(piste_tran)
	{	if(deb >= deb_piste)
			y += 66;
		deb++;
	}
		
	/* piste B */
	if(piste == 1 && deb >= deb_piste)
		goto ok_ypiste;
	if(piste_animB) 
	{	if(deb >= deb_piste)
			y += 50;
		deb++;
	}
		
	/* piste sup B */
	if(piste_supB)
	{	for(i=0;i<projet.nb_sup_B;i++)
		{	if(piste == i+100 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
		
	/* piste sup Gene */
	if(piste_sup)
	{	for(i=0;i<projet.nb_sup_G;i++)
		{	if(piste == i+150 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
	ok_ypiste:;
	if(piste == 0 || piste == 1 )	/* piste A ou B */
		h = 50;
	else
		h = 66;
	
	/* Affichage */
	v_hide_c(vdi_handle);
	vswr_mode(vdi_handle,MD_XOR);

	x_base = xfpu+19;
	
	x=img_to_pix(pos_in,x_base);
	if(x<xfpu+18)
		x=xfpu+18;
	else
		ligne(x-1,yfpu,1,hfpu,BLACK);		/* ligne vert gauche */

	w=img_to_pix(pos_out,x_base)-x-1;
	
	if(x+w>xfpu+wfpu)		/* clip hor */
		w = xfpu+wfpu-x;
	else
		ligne(x+w-1,yfpu,1,hfpu,BLACK);		/* ligne vert droite */

	if(y+h>yfpu+hfpu)		/* clip vert*/
		h = yfpu+hfpu-y;
	
	rect_full(x,y,w,h,BLACK);
	v_show_c(vdi_handle, 1);
}

void redraw_une_piste(int piste)
{	int y,h,i,deb=0;
	
	
	y = yfpu+33;
		/* piste A */
	if(piste == 0 && deb_piste == 0)
		goto ok_ypiste;
	if(piste_animA)
	{	if(deb_piste==0)
			y += 50;
		deb++;
	}
		
	/* piste sup A */
	if(piste_supA)
	{	for(i=0;i<projet.nb_sup_A;i++)
		{	if(piste == i+50 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
		
	/* piste tran */
	if(piste == 2 && deb >= deb_piste)
		goto ok_ypiste;
	if(piste_tran)
	{	if(deb >= deb_piste)
			y += 66;
		deb++;
	}
		
	/* piste B */
	if(piste == 1 && deb >= deb_piste)
		goto ok_ypiste;
	if(piste_animB) 
	{	if(deb >= deb_piste)
			y += 50;
		deb++;
	}
		
	/* piste sup B */
	if(piste_supB)
	{	for(i=0;i<projet.nb_sup_B;i++)
		{	if(piste == i+100 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
		
	/* piste sup Gene */
	if(piste_sup)
	{	for(i=0;i<projet.nb_sup_G;i++)
		{	if(piste == i+150 && deb >= deb_piste)
				goto ok_ypiste;
			if(deb >= deb_piste)
				y += 66;
			deb++;
		}
	}
	ok_ypiste:;
	if(piste == 0 || piste == 1 )	/* piste A ou B */
		h = 50;
	else
		h = 66;
	
	redraw_pistes(hnd_wpistes,xfpu+19,y,wfpu-19,h);
}


void slider_ph(void)
{	int larg,perte;
	int x_ass,w_ass;
	
	larg = (int)((((long)wfpu-18)*(long)projet.zoom)/48);
	
	if(larg >= projet.lenght)
	{	if(deb_img)
		{	perte = (int)((long)deb_img *48L / projet.zoom );
			w_ass = (int)(((long)larg * 1000L) / (larg+perte));
			wind_set(hnd_wpistes, WF_HSLSIZE, w_ass, 0, 0, 0);
			wind_set(hnd_wpistes, WF_HSLIDE, 1000, 0, 0, 0);
		}else
		{	wind_set(hnd_wpistes, WF_HSLSIZE, 1000, 0, 0, 0);
			wind_set(hnd_wpistes, WF_HSLIDE, 0, 0, 0, 0);
		}
	}else
	{	w_ass = (int)(((long)larg * 1000L) / projet.lenght);
		x_ass = (int)(((long)deb_img * 1000L) / (projet.lenght-larg));
		wind_set(hnd_wpistes, WF_HSLSIZE, w_ass, 0, 0, 0);
		wind_set(hnd_wpistes, WF_HSLIDE, x_ass, 0, 0, 0);
	}
}

int calc_nb_aff_pistes(void)
{	int h = hfpu - 32;
	int nb = 0,i;
	
	if(piste_animA)
	{  h -= 50;
		if(h <= 0) goto end_nb;
		nb++;	
	}
	/* piste sup A */
	if(piste_supA)
	{	for(i=0;i<projet.nb_sup_A;i++)
		{	h -= 66;
			if(h <= 0) goto end_nb;
			nb++;
		}
	}
	/* piste tran */
	if(piste_tran)
	{	h -= 66;
		if(h <= 0) goto end_nb;
		nb++;
	}
	/* piste B */
	if(piste_animB)
	{  h -= 50;
		if(h <= 0) goto end_nb;
		nb++;
	}
	/* piste sup B */
	if(piste_supB)
	{	for(i=0;i<projet.nb_sup_B;i++)
		{	h -= 66;
			if(h <= 0) goto end_nb;
			nb++;
		}
	}
	/* piste sup Gene */
	if(piste_sup)
	{	for(i=0;i<projet.nb_sup_G;i++)
		{	h -= 66;
			if(h <= 0) goto end_nb;
			nb++;
		}
	}
	end_nb:
	return nb;
}

void slider_pv(void)
{	int y_ass,h_ass;
	int nb_pistes = piste_animA + piste_animB + projet.nb_sup_A*piste_supA +
	 		  projet.nb_sup_B*piste_supB + piste_tran + projet.nb_sup_G*piste_sup;
/*	int nb_aff_pistes = max(0,hfpu - 32 -piste_animA*50 - piste_animB*50)/66 + piste_animA + piste_animB;*/
	int nb_aff_pistes = calc_nb_aff_pistes();
	
	if(deb_piste > nb_pistes - nb_aff_pistes)
		deb_piste = max(0, nb_pistes - nb_aff_pistes);
	
	if(nb_aff_pistes >= nb_pistes)
	{	wind_set(hnd_wpistes, WF_VSLIDE, 0, 0, 0, 0);
		wind_set(hnd_wpistes, WF_VSLSIZE, 1000, 0, 0, 0);
	}else
	{	h_ass = (int)(((long)nb_aff_pistes * 1000L) / nb_pistes);
		y_ass = (int)(((long)deb_piste * 1000L) / (nb_pistes-nb_aff_pistes));
		wind_set(hnd_wpistes, WF_VSLSIZE, h_ass, 0, 0, 0);
		wind_set(hnd_wpistes, WF_VSLIDE, y_ass, 0, 0, 0);
	}
}

void init_buffer(void)
{	int i;
	aff_nombre = 0;
	
	if(aff_buffer)
		Mfree(aff_buffer);

	aff_buffer = Malloc((long)aff_size_buffer*1024L);
	if(aff_buffer)
	{	aff_max_nombre = (int)(((long)aff_size_buffer*1024L)/4608L);
		for(i=0; i<aff_max_nombre; i++)
			buf_obj[i].objet = -1;
		aff_nombre = 0;
		aff_cur_pos = 0;
	}
}

void close_buffer(void)
{	if(aff_buffer)
		Mfree(aff_buffer);
	aff_nombre = 0;
	aff_max_nombre = 0;
	aff_cur_pos = 0;
}

char *img_to_smpte(int img, int speed, char *str)
{	int sec,min;
	
	if(speed == 0)
	{	str[0]=0;
		return str;
	}
	
	sec = img / speed;
	min = sec / 60;
	sec = sec - min * 60;
	img = img - sec * speed;
	
	sprintf(str,"%0d:%0d:%0d",min,sec,img);
	
	return str;
}

void redraw_pistes(int w_handle, int x, int y, int w, int h)
{
	GRECT r, rd, reduc;
	int y_base,x_base, in, out, modulo, modulo2; /* modulo : frequence text, modulo 2 : frequence pin vertical */
	int virtual_deb; /* pour toujour caler les num d'images sur le meme deb */
	int larg_img,i,fonte=1,piste=0;
	char str[14];
	int col_nb[2]={1,0},pxy[8];
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
	
	if(w_handle<1 || actu_projet == FALSE)
	{	goto end_redraw;
	}
	
	v_hide_c(vdi_handle);
	wind_update(BEG_UPDATE);
	
	if(use_nvdi)
		fonte = 5003;
	vst_font( vdi_handle, fonte );	/* swiss */
	vst_height(vdi_handle,char_height,&i,&i,&i,&i);
	
	larg_img = (int)(((wfpu-18)*(long)projet.zoom)/48L);
	
	modulo2=1;
	if(projet.zoom < 3)
		modulo = 2;
	else if(projet.zoom < 6)
		modulo = 10;
	else if(projet.zoom < 20)
		modulo = 20;
	else if(projet.zoom < 30)
	{	modulo = 40; modulo2=4; }
	else if(projet.zoom < 60)
	{	modulo = 60; modulo2=6; }
	else	
	{	modulo = 100; modulo2=10; }
		
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)
	{
		if (rc_intersect(&rd, &r))
		{	
			set_clip(1, &r);
			
			vswr_mode(vdi_handle,MD_REPLACE);
			rect_full(r.g_x,r.g_y,r.g_w,r.g_h,color_3d1*int_rel);		/* un rect_full gris */
			
			y_base = yfpu;
			x_base = xfpu;
			
			/* BARRE DE legende */
			if((r.g_x+r.g_w) >= x_base && r.g_x < x_base + 18)
			{	
				rect_3d(x_base,y_base,18,hfpu);
				rect_3d(x_base+1,y_base+1,16,33);
				reduc.g_x = x_base+1;
				reduc.g_y = y_base+4;
				reduc.g_w = 16;
				reduc.g_h = 28;
				if(rc_intersect(&r,&reduc))
				{	/* source */
					pxy[0] = reduc.g_x - (x_base+1);
					pxy[1] = reduc.g_y - (y_base+4);	
					pxy[2] = pxy[0] + reduc.g_w - 1;	
					pxy[3] = pxy[1] + reduc.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
					pxy[4] = reduc.g_x;							
					pxy[5] = reduc.g_y;							
					pxy[6] = pxy[4] + reduc.g_w - 1;	
					pxy[7] = pxy[5] + reduc.g_h - 1;	
					vrt_cpyfm (vdi_handle, MD_TRANS, pxy, &img_rsc_time, &Screen_MFDB,col_nb);
				}
			}

			x_base += 18;

			/* cliping local */
			reduc.g_x = max(x_base+1,r.g_x);
			reduc.g_w = r.g_x + r.g_w - reduc.g_x;
			reduc.g_y = r.g_y;	reduc.g_h = r.g_h;
			
			/* BARRE DE PREVIEW */
			if((r.g_y+r.g_h) >= y_base && r.g_y < y_base + 8)
			{	
				rect_3d(x_base,y_base,wfpu-18,8);

				set_clip(1,&reduc);
				
				in = img_to_pix(projet.preview_in,x_base);
				out = img_to_pix(projet.preview_out+1,x_base)-1;
				rect_full(in,y_base +1, out-in , 6,YELLOW);

				/* triangle 1 */
				if(in>=x_base && in<x_base+wfpu)
				{	in += projet.preview_in==0;
					ligne(in,y_base+1, 1, 6, RED );
					ligne(in+1,y_base+2, 1, 4, RED );
					ligne(in+2,y_base+3, 1, 2, RED );
				}
				/* triangle fin */
				if(out>x_base && in<x_base+wfpu)
				{	ligne(out,y_base+1, 1, 6, RED );
					ligne(out-1,y_base+2, 1, 4, RED );
					ligne(out-2,y_base+3, 1, 2, RED );
				}
				set_clip(1,&r);
			}
			
			y_base += 8;
			
			/* BARRE DE TEMPS */
			if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 26)
			{	vswr_mode(vdi_handle,MD_TRANS);
				
				rect_3d(x_base,y_base,wfpu-18,26);
				set_clip(1,&reduc);
				virtual_deb = deb_img / modulo2;
				virtual_deb *= modulo2;

				for(i=virtual_deb; i<=deb_img+larg_img+modulo2;i+=modulo2)
				{	in = img_to_pix(i,x_base);
					if(i)
					{	in = img_to_pix(i,x_base);
						if(i%modulo==0)
						{	ligne(in,y_base + 4,1,7,BLACK);
							if(mode_smpte)
								img_to_smpte(i,projet.speed,str);
							else itoa(i,str,10);
							text_center_x(in,y_base+5,str);
						}else
							ligne(in,y_base + 4,1,4,BLACK);
					}
				} 

				if(preview_pos != -1)
				{	in = img_to_pix(preview_pos,x_base);
					if(in>=x_base && in<x_base+wfpu)
					{	ligne(in-5,y_base + 1,11,1,LCYAN);
						ligne(in-4,y_base + 2,9,1,LCYAN);
						ligne(in-3,y_base + 3,7,1,LCYAN);
						ligne(in-2,y_base + 4,5,1,LCYAN);
						ligne(in-1,y_base + 5,3,1,LCYAN);
						ligne(in,y_base + 6,1,1,LCYAN);
					}
				}
				set_clip(1,&r);
			}

			y_base += 26;
			
			/* ANIM A */
			if(piste_animA)
			{	if(deb_piste == 0)
				{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 50)
					{	
						rect_3d(xfpu+1,y_base,16,50);
						text_vert(xfpu+9,y_base+12,"A");
						rect_3d(x_base,y_base,wfpu-18,50);
						set_clip(1,&reduc);
						draw_piste(0,x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
						set_clip(1,&r);
					}
					y_base += 50;
				}
				piste++;
			}
			
			/* SUPERPOSITION A */
			if(piste_supA)
			{	for(i=0;i<projet.nb_sup_A;i++)
				{	if(piste >= deb_piste)
					{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 66)
						{	rect_3d(xfpu+1,y_base,16,66);
							textuate_p(i+50,str);
							text_vert(xfpu+9,y_base+6,str);
							rect_3d(x_base,y_base,wfpu-18,66);
							set_clip(1,&reduc);
							draw_piste(50+i,x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
							set_clip(1,&r);
						}
						y_base += 66;
					}
					piste++;
				}
			}
			
			
			/* TRNSITION */
			if(piste_tran)
			{	if(piste >= deb_piste)
				{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 66)
					{	
						rect_3d(xfpu+1,y_base,16,66);
						text_vert(xfpu+9,y_base+20,"T");
						rect_3d(x_base,y_base,wfpu-18,66);
						set_clip(1,&reduc);
						draw_piste_tran(x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
						set_clip(1,&r);

					}
					y_base += 66;
				}
				piste++;
			}
			
			/* ANIM B */
			if(piste_animB)
			{	if(piste >= deb_piste)
				{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 50)
					{		
						rect_3d(xfpu+1,y_base,16,50);
						text_vert(xfpu+9,y_base+12,"B");
						rect_3d(x_base,y_base,wfpu-18,50);
						set_clip(1,&reduc);
						draw_piste(1,x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
						set_clip(1,&r);
					}
					y_base += 50;
				}
				piste++;
			}
	
			/* SUPERPOSITION B */
			if(piste_supB)
			{	for(i=0;i<projet.nb_sup_B;i++)
				{	if(piste >= deb_piste)
					{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 66)
						{	rect_3d(xfpu+1,y_base,16,66);
							textuate_p(i+100,str);
							text_vert(xfpu+9,y_base+6,str);
							rect_3d(x_base,y_base,wfpu-18,66);
							set_clip(1,&reduc);
							draw_piste(100+i,x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
							set_clip(1,&r);
						}
						y_base += 66;
					}
					piste++;
				}
			}

			/* SUPERPOSITION General */
			if(piste_sup)
			{	for(i=0;i<projet.nb_sup_G;i++)
				{	if(piste >= deb_piste)
					{	if( (r.g_y+r.g_h) >= y_base && r.g_y < y_base + 66)
						{	rect_3d(xfpu+1,y_base,16,66);
							textuate_p(i+150,str);
							text_vert(xfpu+9,y_base+6,str);
							rect_3d(x_base,y_base,wfpu-18,66);
							set_clip(1,&reduc);
							draw_piste(150+i,x_base,y_base+1,reduc.g_x,reduc.g_y,reduc.g_w,reduc.g_h);
							set_clip(1,&r);
						}
						y_base += 66;
					}
					piste++;
				}
			}

			/* OUT OF ANIM */
			if(r.g_x+r.g_w > img_to_pix(projet.lenght,x_base) )
			{	vsf_interior(vdi_handle,FIS_HATCH);
				vsf_style(vdi_handle,1);

				vswr_mode(vdi_handle,MD_XOR);
				rect_full(max(x_base,img_to_pix(projet.lenght,x_base)),yfpu,wfpu,hfpu,BLACK);
				
				vsf_interior(vdi_handle,FIS_SOLID);
				vsf_style(vdi_handle,7);
			}

		}
		wind_get(w_handle, WF_NEXTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	}
	
	vst_font( vdi_handle, 1 );	/* normal */
	wind_update(END_UPDATE);
	set_clip(0,&r);
	v_show_c(vdi_handle, 1);
	end_redraw:;
}

int img_to_pix(long pos,int x_base)
{	long result;
	result = (((pos-deb_img)*48L)/(long)projet.zoom) + x_base;
	if(result > 32000)		/* infinity */
		return 30000;
	else
		return (int)result;
}

void draw_courb_int(int cur_prj_obj, int x_base, int y_base,int modulo)
{	int pos_1_img,pos_last_img,i;
	int nb_img_in_wind;
	int xl,yl,oldx,oldy;
	int time,intens,larg_prj_obj;
	int xy[4];
	TIME_SPLINE *la_spline=0;
	
	vsl_color(vdi_handle,BLACK);
	vsl_type(vdi_handle,1);
	
	larg_prj_obj = prj_obj[cur_prj_obj].pos_out - prj_obj[cur_prj_obj].pos_in + 1;

	
	nb_img_in_wind = (int)(((wfpu-18)*(long)projet.zoom)/48L);
	
	/* MARQUEUR DEBUT EN PIXEL */
	
	if(prj_obj[cur_prj_obj].pos_in < deb_img) /* bien avant le debut de l'affichage */
		pos_1_img = xfpu;
	else if(prj_obj[cur_prj_obj].pos_in > deb_img + nb_img_in_wind)
		return;
	else								/* on est ds la fenetre */
		pos_1_img = img_to_pix(prj_obj[cur_prj_obj].pos_in,x_base);

	/* MARQUEUR FIN EN PIXEL */
	
	if(prj_obj[cur_prj_obj].pos_out+1 > deb_img + ((wfpu-18)*(long)projet.zoom)/48L )
		/* bien apres la fenetre */
	pos_last_img = xfpu+wfpu+10;
	else if(prj_obj[cur_prj_obj].pos_out < deb_img) /* bien avant le debut de l'affichage */
		return;
	else								/* on est ds la fenetre */
		pos_last_img = img_to_pix(prj_obj[cur_prj_obj].pos_out+1,x_base)-1;
	
	if(prj_obj[cur_prj_obj].intens & 0x8000)		/* intense fixe */
	{	intens = ((prj_obj[cur_prj_obj].intens>>12)&0x7)*4;
		if(intens) intens--; 
		xy[0] = pos_1_img;		xy[1] = y_base + 14 -intens;
		xy[2] = pos_last_img;	xy[3] = xy[1];
		v_pline(vdi_handle,2,xy);	
	}else														/* avec time spline */
	{	time = prj_obj[cur_prj_obj].intens & 0x0fff;
		if(time > 0)
		{	la_spline = find_time_spline(time);
			time = -1;
		}
	
		oldy = (int)spline_comput(time,la_spline,pix2img(pos_1_img+1) - prj_obj[cur_prj_obj].pos_in,larg_prj_obj,15);
		oldy = y_base + 14 - oldy;
		oldx = pos_1_img;
	
		for(i=pos_1_img+modulo; i<pos_last_img; i+= modulo)
		{	xl = i;
			yl = (int)spline_comput(time,la_spline,pix2img(i+1) - prj_obj[cur_prj_obj].pos_in,larg_prj_obj,15);
			yl = y_base + 14 -yl;
			xy[0] = oldx;		xy[1] = oldy;
			xy[2] = xl;			xy[3] = yl;
			v_pline(vdi_handle,2,xy);
			oldx=xl; 
			oldy=yl;
		}
		xl = pos_last_img;
		yl = (int)spline_comput(time,la_spline,pix2img(pos_last_img+1) - prj_obj[cur_prj_obj].pos_in,larg_prj_obj,15);
		yl = y_base + 14 -yl;
		xy[0] = oldx;		xy[1] = oldy;
		xy[2] = xl;			xy[3] = yl;
		v_pline(vdi_handle,2,xy);
		oldx=xl;
		oldy=yl;
	}
}

static void draw_courb_transit(int time, int x_base, int y_base, int cur_tran, int modulo)
{	int pos_1_img,pos_last_img,i;
	int nb_img_in_wind;
	int xl,yl,oldx,oldy;
	int larg_tran;
	int xy[4];
	TIME_SPLINE *la_spline=0;
	
	vsl_color(vdi_handle,BLACK);
	vsl_type(vdi_handle,1);
	
	larg_tran = prj_transit[cur_tran].pos_out - prj_transit[cur_tran].pos_in + 1;

	
	nb_img_in_wind = (int)(((wfpu-18)*(long)projet.zoom)/48L);
	
	/* MARQUEUR DEBUT EN PIXEL */
	
	if(prj_transit[cur_tran].pos_in < deb_img) /* bien avant le debut de l'affichage */
		pos_1_img = xfpu;
	else if(prj_transit[cur_tran].pos_in > deb_img + nb_img_in_wind)
		return;
	else								/* on est ds la fenetre */
		pos_1_img = img_to_pix(prj_transit[cur_tran].pos_in,x_base);

	/* MARQUEUR FIN EN PIXEL */
	
	if(prj_transit[cur_tran].pos_out+1 > deb_img + ((wfpu-18)*(long)projet.zoom)/48L )
		/* bien apres la fenetre */
	pos_last_img = xfpu+wfpu+10;
	else if(prj_transit[cur_tran].pos_out < deb_img) /* bien avant le debut de l'affichage */
		return;
	else								/* on est ds la fenetre */
		pos_last_img = img_to_pix(prj_transit[cur_tran].pos_out+1,x_base)-1;
	
	if(time > 0)
	{	la_spline = find_time_spline(time);
		time = -1;
	}
	
	oldy = (int)spline_comput(time,la_spline,pix2img(pos_1_img+1) - prj_transit[cur_tran].pos_in,larg_tran,15);
	oldy = y_base + 14 - oldy;
	oldx = pos_1_img;
	
	for(i=pos_1_img+modulo; i<pos_last_img; i+= modulo)
	{	xl = i;
		yl = (int)spline_comput(time,la_spline,pix2img(i+1) - prj_transit[cur_tran].pos_in,larg_tran,15);
		yl = y_base + 14 -yl;
		xy[0] = oldx;		xy[1] = oldy;
		xy[2] = xl;			xy[3] = yl;
		v_pline(vdi_handle,2,xy);
		oldx=xl; 
		oldy=yl;
	}
	xl = pos_last_img;
	yl = (int)spline_comput(time,la_spline,pix2img(pos_last_img+1) - prj_transit[cur_tran].pos_in,larg_tran,15);
	yl = y_base + 14 -yl;
	xy[0] = oldx;		xy[1] = oldy;
	xy[2] = xl;			xy[3] = yl;
	v_pline(vdi_handle,2,xy);
	oldx=xl;
	oldy=yl;
	
}

void draw_piste(int piste, int x_base, int y_base, int x,int y,int w,int h)
{	int img_in, img_out;
	int i,retour; 
	
	
	img_in = pix2img(max(x_base,x));
	img_out = pix2img(x+w+47);

	for(i=img_in; i<img_out; i++)
	{	retour = find_anim(i,piste);
		if(retour != -1)
		{	draw_prj_obj(retour, x_base, y_base, x, y, w, h);
			if(draw_intensitee && piste != 0 && piste != 1)
				draw_courb_int(retour,x_base,y_base+49,12);
			i = prj_obj[retour].pos_out;
		}
	}
}

static void draw_piste_tran(int x_base, int y_base, int x,int y,int w,int h)
{	int img_in, img_out;
	int i,retour; 
	
	
	img_in = pix2img(max(x_base,x));
	img_out = pix2img(x+w);

	for(i=img_in; i<img_out; i++)
	{	retour = find_tran(i);
		if(retour != -1)
		{	draw_used_tran(retour, x_base, y_base, x, y, w, h);
			/* la courbe */
			if(prj_transit[retour].aff_time != -1)
				draw_courb_transit(prj_transit[retour].aff_time, x_base,  y_base +49,retour,12);
			i = prj_transit[retour].pos_out;
		}
	}
}
	 
int find_tran(int pos)
{	int i;
	for(i=0;i<nb_used_transit; i++)
	{	if(pos >= prj_transit[i].pos_in && pos <= prj_transit[i].pos_out)
			return i;
	}
	
	return -1;
}

int find_anim(int pos, int piste)
{	int i;
	for(i=0;i<projet.nb_obj; i++)
	{	if(prj_obj[i].piste == piste)
		{	if(pos >= prj_obj[i].pos_in && pos <= prj_obj[i].pos_out)
				return i;
		}
	}
	
	return -1;
}

static void draw_used_tran(int cur_tran, int x_base, int y_base, int x,int y,int w,int h)
{	int pos_1_img,pos_last_img;
	int pxy[8],cur_transit,pos_txt_pix;
	int decal_txt,col_nb[2]={1,0};
	GRECT rd, rimg, old;
	
	old.g_x = x;
	old.g_y = y;
	old.g_w = w;
	old.g_h = h;
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
	
	cur_transit = find_transit(prj_transit[cur_tran].id);
	if(cur_transit==-1)	return;
		
	/* MARQUEUR DEBUT EN PIXEL */
	
	pos_1_img = img_to_pix(prj_transit[cur_tran].pos_in,x_base);

	/* MARQUEUR FIN EN PIXEL */
	
	pos_last_img = img_to_pix(prj_transit[cur_tran].pos_out+1,x_base)-1;

	rd.g_w = min(w, pos_last_img-rd.g_x);
	
	/* Le fond gris */
	rd.g_x = max(x, pos_1_img);
	rd.g_w = min(w-rd.g_x+x, pos_last_img-rd.g_x);
	rd.g_y = max(y_base,old.g_y);
	rd.g_h = min(y_base+48,old.g_y+old.g_h) - rd.g_y;
	
	if(rd.g_x+rd.g_w > xfpu+wfpu) rd.g_w = xfpu+wfpu-rd.g_x;
	set_clip(1,&rd);
	rd.g_x = pos_1_img;
	rd.g_w = pos_last_img-rd.g_x;

	rect_3d(rd.g_x, y_base , rd.g_w , 48);
		
		/* la preview imagee */
	rimg.g_x = pos_1_img+10;
	rimg.g_y = y_base;
	rimg.g_w = 48;
	rimg.g_h = 48;
	if (rc_intersect(&old, &rimg))
	{	/* Zone source dans l'image en m‚moire : */
		pxy[0] = rimg.g_x - (pos_1_img+10);
		pxy[1] = rimg.g_y - y_base;	
		pxy[2] = pxy[0] + rimg.g_w - 1;	
		pxy[3] = pxy[1] + rimg.g_h - 1;	
		/* Zone destination dans la fenˆtre … l'‚cran */
		pxy[4] = rimg.g_x;							
		pxy[5] = rimg.g_y;							
		pxy[6] = pxy[4] + rimg.g_w - 1;	
		pxy[7] = pxy[5] + rimg.g_h - 1;	
		tnt_cpyfm (pxy, &transition[cur_transit]->preview);
	}

	decal_txt = 0;
	if(!transition[cur_transit]->no_ab_swap)
	{	decal_txt = 22;
		rect_3d(pos_1_img + 63, y_base+13 , 18 , 16);
		rimg.g_x = pos_1_img + 65;
		rimg.g_y = y_base+15;
		rimg.g_w = 16;
		rimg.g_h = 13;
		if(rc_intersect(&old,&rimg))
		{	/* source */
			pxy[0] = rimg.g_x - (pos_1_img + 65);
			pxy[1] = rimg.g_y - (y_base+15);
			pxy[2] = pxy[0] + rimg.g_w - 1;
			pxy[3] = pxy[1] + rimg.g_h - 1;
			/* Zone destination dans la fenˆtre … l'‚cran */
			pxy[4] = rimg.g_x;
			pxy[5] = rimg.g_y;
			pxy[6] = pxy[4] + rimg.g_w - 1;
			pxy[7] = pxy[5] + rimg.g_h - 1;
			if(prj_transit[cur_tran].a_vers_b)
				vrt_cpyfm (vdi_handle, MD_TRANS, pxy, &img_rsc_fl_up, &Screen_MFDB,col_nb);
			else
				vrt_cpyfm (vdi_handle, MD_TRANS, pxy, &img_rsc_fl_down, &Screen_MFDB,col_nb);
		}
	}
	
		/* LE NOM DE La tran */
	pos_txt_pix = pos_1_img + 64 + decal_txt;
	if(pos_txt_pix > 0 && pos_txt_pix<xfpu+wfpu)
		v_gtext(vdi_handle,pos_txt_pix, y_base + 24, transition[cur_transit]->nom);
	set_clip(1,&old);
	
}		
		

void draw_prj_obj(int cur_obj_prj, int x_base, int y_base, int x,int y,int w,int h)
{	int pos_1_img,pos_last_img,i,old_frame = -1;
	int pos,pxy[8],*img,canal=-1,cur_objet,lim_haut=48,centre,pos_txt,pos_txt_pix;
	int nb_img_in_wind, real_pos_1_img, real_pos_last_img;
	GRECT rd, rimg, old;
	MFDB image;
	
	old.g_x = x;
	old.g_y = y;
	old.g_w = w;
	old.g_h = h;
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
	
	cur_objet = prj_obj[cur_obj_prj].objet;

	nb_img_in_wind = (int)(((wfpu-18)*(long)projet.zoom)/48L);
	
	/* MARQUEUR DEBUT EN PIXEL */
	real_pos_1_img = img_to_pix(prj_obj[cur_obj_prj].pos_in,x_base);
	
	if(prj_obj[cur_obj_prj].pos_in < deb_img) /* bien avant le debut de l'affichage */
		pos_1_img = xfpu;
	else if(prj_obj[cur_obj_prj].pos_in > deb_img + nb_img_in_wind)
		return;
	else								/* on est ds la fenetre */
		pos_1_img = real_pos_1_img;

	/* MARQUEUR FIN EN PIXEL */
	real_pos_last_img = img_to_pix(prj_obj[cur_obj_prj].pos_out+1,x_base)-1;

	if(prj_obj[cur_obj_prj].pos_out+1 > deb_img + ((wfpu-18)*(long)projet.zoom)/48L )
		/* bien apres la fenetre */
	pos_last_img = xfpu+wfpu+10;
	else if(prj_obj[cur_obj_prj].pos_out < deb_img) /* bien avant le debut de l'affichage */
		return;
	else								/* on est ds la fenetre */
		pos_last_img = real_pos_last_img;

	rd.g_w = min(w, pos_last_img-rd.g_x);
	
	/* Le fond jaune */
	if(aff_mode == 2 || aff_mode == 1)
	{	rd.g_x = max(x, pos_1_img);
		rd.g_w = min(w-rd.g_x+x, pos_last_img-rd.g_x);
		rd.g_y = max(y_base,old.g_y);
		rd.g_h = min(y_base+48,old.g_y+old.g_h) - rd.g_y;

		set_clip(1,&rd);
	
		rd.g_x = pos_1_img;
		rd.g_w = pos_last_img-rd.g_x;
		rect_full(rd.g_x,y_base, rd.g_w , 48,YELLOW);
		rect(rd.g_x, y_base , rd.g_w , 48,BLACK);
		
		/* LE NOM DE L'ANIM */
		pos_txt = (prj_obj[cur_obj_prj].pos_in+prj_obj[cur_obj_prj].pos_out)/2;
		pos_txt_pix = img_to_pix(pos_txt,x_base);
		if(pos_txt_pix > 0 && pos_txt_pix<xfpu+wfpu)
			text_center( pos_txt_pix, y_base + 24, objet[cur_objet].nom);
		set_clip(1,&old);
		
	}	
	
	if(aff_mode == 1 || aff_mode == 0)
	{	/* overture LE CANAL EST OUVERT PAR MAKE_REDUC ##SI NECESSAIRE## */
		
	
		img = Malloc((size_t)objet[cur_objet].x * (size_t)(objet[cur_objet].y+1) * 2L);
		if(img == 0)
		{	super_alert(1,3,T_NO_MEM_PREV);
			goto end_draw;
		}

		image.fd_addr = Malloc(5000L); /* on est sur > 4608 */
		if(image.fd_addr == 0)
		{	super_alert(1,3,T_NO_MEM_PREV);
			Mfree(img);
			goto end_draw;
		}

		form_mem(UPDT_MEM);

		image.fd_w = 48;
		image.fd_h = 48;
		image.fd_wdwidth = 3; 
		image.fd_stand = 0; 
		image.fd_nplanes = 16; 
		
		if(aff_mode == 1)		/* on affiche que 2 images */
		{	/* calcul la hauteur necessaire pour la tansparence */
			if(aff_prop)
			{	if(objet[cur_objet].x == objet[cur_objet].y)
					lim_haut = 48;
				else if(objet[cur_objet].x > objet[cur_objet].y)
					lim_haut = (objet[cur_objet].y*48)/objet[cur_objet].x;
			}
			centre = (48-lim_haut)/2;
			
			/* AFFICHAGE DE LA PREMIERE IMAGE */
			rimg.g_x = real_pos_1_img;
			rimg.g_y = y_base + centre;
			rimg.g_w = 48;
			rimg.g_h = lim_haut;
			if (rc_intersect(&rd, &rimg))
			{	if(make_reduc(cur_objet, objet[cur_objet].def_in, image.fd_addr, img, &canal, &old_frame))
				{	/* Zone source dans l'image en m‚moire : */
					pxy[0] = rimg.g_x - real_pos_1_img;	
					pxy[1] = rimg.g_y - (y_base+centre);	
					pxy[2] = pxy[0] + rimg.g_w - 1;	
					pxy[3] = pxy[1] + rimg.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
					pxy[4] = rimg.g_x;							
					pxy[5] = rimg.g_y;							
					pxy[6] = pxy[4] + rimg.g_w - 1;	
					pxy[7] = pxy[5] + rimg.g_h - 1;	
					tnt_cpyfm (pxy, &image);	
				}else
					goto erreur;
			}
		
			/* AFFICHAGE DE LA DERNIERE IMAGE */
			/* on reduit la zone de clip au reste (hors 1ere img) */
			rd.g_x = max(pos_1_img+48,rd.g_x);
			
			rimg.g_x = real_pos_last_img-48;
			rimg.g_y = y_base + centre;
			rimg.g_w = 48;
			rimg.g_h = lim_haut;
			if (rc_intersect(&rd, &rimg))
			{	if(make_reduc(cur_objet, objet[cur_objet].def_out, image.fd_addr, img, &canal, &old_frame))
				{	/* Zone source dans l'image en m‚moire : */
					pxy[0] = rimg.g_x - (real_pos_last_img-48);	
					pxy[1] = rimg.g_y - (y_base+centre);	
					pxy[2] = pxy[0] + rimg.g_w - 1;	
					pxy[3] = pxy[1] + rimg.g_h - 1;	
					/* Zone destination dans la fenˆtre … l'‚cran */
					pxy[4] = rimg.g_x;							
					pxy[5] = rimg.g_y;							
					pxy[6] = pxy[4] + rimg.g_w - 1;	
					pxy[7] = pxy[5] + rimg.g_h - 1;	
					tnt_cpyfm (pxy, &image);	
				}else
					goto erreur;
			}

		}else if(aff_mode == 0)		/* on affiche toutes les reducs */
		{	for(i=real_pos_1_img; i<pos_last_img;)
			{	if(i<pos_1_img)
				{	i+=48;
					continue;
				}
				rimg.g_x = i;
				rimg.g_y = y_base;
				rimg.g_w = 48;
				rimg.g_h = 48;
				if (rc_intersect(&rd, &rimg))
				{	pos = time_comput(cur_obj_prj,pix2img(i+1) - prj_obj[cur_obj_prj].pos_in);
					if(make_reduc(cur_objet, pos, image.fd_addr, img, &canal, &old_frame))
					{	/* Zone source dans l'image en m‚moire : */
						pxy[0] = rimg.g_x - i;	
						pxy[1] = rimg.g_y - y_base;	
						pxy[2] = pxy[0] + rimg.g_w - 1;	
						pxy[3] = pxy[1] + rimg.g_h - 1;	
						/* Zone destination dans la fenˆtre … l'‚cran */
						pxy[4] = rimg.g_x;							
						pxy[5] = rimg.g_y;							
						pxy[6] = pxy[4] + rimg.g_w - 1;	
						pxy[7] = pxy[5] + rimg.g_h - 1;	
						tnt_cpyfm (pxy, &image);	
					}else
					goto erreur;
				}
			i += 48;
			}
		}
		erreur:;
		if(canal != -1)
			close_anim(canal);
	
		Mfree(img);
		Mfree(image.fd_addr);
	}
	form_mem(UPDT_MEM);
	
	end_draw:;
	
}