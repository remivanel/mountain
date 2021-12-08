/*******************MOUNTAIN*********************/
/*****   GESTION de la fenˆtre de preview    ****/
/*****            Par Vanel R‚mi             ****/
/************************************************/


#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "flh_lib.h"
#include "gest_flh.h"
#include "mountain.h"
#include "objets.h"
#include "anim.h"
#include "some_fnc.h"
#include "preview.h"
#include "transit.h"
#include "geffets.h"
#include "effets1.h"
#include "projet.h"
#include "pistes.h"
#include "mouvemnt.h"
#include "time_spl.h"
#include "txts.h"

int hnd_wprev=-1, attr_prev = MOVER|NAME|CLOSER;
int xfe,yfe,wfe,hfe;			/* coord fenetre */
int xfeu,yfeu,wfeu,hfeu;   /* coord utiles */
int def_x_pos_prev=0,def_y_pos_prev=0;

int cur_prev;				/* preview active */
int cur_calc_prev;
int *prev_img;				/* adresse */
MFDB mfdb_prev;
int r_x_mod16;

int preview_pos = -1;	/* frame de preview */

int conv_col(int r,int v,int b);
extern int actu_projet;			/* is there any projet here ? */
extern PROJET projet;
extern PRJ_OBJ prj_obj[MAX_PRJ_OBJ];
extern OBJET objet[MAX_OBJET];
extern int hnd_wpistes,deb_img;
extern int xfpu,yfpu,wfpu,hfpu;   /* coord utiles */
extern char chemin_key[150];
extern int mnt_registered;

/* transitions */
extern int nb_used_transit;	/* nombre de transitions UTILISEE */
extern TRANSITION *transition[MAX_TRAN];
extern PRJ_TRANSIT prj_transit[MAX_PRJ_TRAN];
extern TOOL_BOX tool_box;
extern EFFET *effet[MAX_EFFETS];

int opened_anim[256];	/* les case correspondes a chaque pistes, 
							contiennent le num de canal, -1 si pas ouvert
							le 2 est reserve au fond */
int *ptr_anim[256];	/* pointeurs sur l'image acinnennement ouverte */
int *ptr_calc[256];	/* pointeurs sur l'image ouverte affectee de l'effet */
int *ptr_eff_old[256][MAX_CUR_EFFETS];	/* old pour mullti effets */
int eff_obj[256];		/* prj_obj anciennement  affecte de l'effet */
int eff_frame[256];	/* frame anciennement  affectee de l'effet */
int oldf_anim[256];	/* num de l'old frame */
int old_obj_anim[256];	/* num de l'old obj  */
int *ptr_fon;
int tab_red_pre[5]={4,2,1,1};
static int px=100,py=100;
int aprev_mode = 0;			/* 0->temps reel, 1-> precalculee */
int aprev_preemptif = 0;	/* mode preemptif */
int aprev_curseur = 1;		/* curseur qui avance */
static int old_curseur;
int cur_aprev=0;
static int aprev_pos,nb_img_in_wind_pist;
extern WINDFORM_VAR opre_var;		/* opt preview */
#define img2pix(A) ( (int)(((((long)A-deb_img)*48L)/(long)projet.zoom) + x_base))

/* protos */
static void stop_aprev(void);

int init_wprev(void)
{	int i,j;

	cur_prev = 0;
	cur_calc_prev = 0;
	
	/* cood max utiles d'un fenetre */
	wind_calc(WC_WORK, attr_prev, x_desk, y_desk, w_desk, h_desk, &xfeu, &yfeu, &wfeu, &hfeu);

	/* calcule les coord max de notre fenetre */
	wind_calc(WC_BORDER, attr_prev, xfeu, yfeu, wfeu, hfeu, &xfe, &yfe, &wfe, &hfe);
	
	
	for(i=0;i<256;i++)
	{	opened_anim[i] = -1;
		oldf_anim[i] = -1;
		ptr_anim[i] = 0;
		ptr_calc[i] = 0;
		for(j=0;j<MAX_CUR_EFFETS;j++)
			ptr_eff_old[i][j] = 0;
		eff_frame[i]= -2;
		eff_obj[i]  = -1;
	}
	ptr_fon=0;
	
	return 1;
}

int new_preview(int frame)
{	
	if(projet.taille_prev == 3)
	{	px = 2*projet.x;
		py = 2*projet.y;
	}else
	{	px = projet.x / tab_red_pre[projet.taille_prev];
		py = projet.y / tab_red_pre[projet.taille_prev];
	}
	
	
	if(!actu_projet)
		return 0;
	
	if(cur_aprev)
		stop_aprev();
/*		close_aprev();  */
	
	if(!cur_prev)
	{	r_x_mod16 = (px+15)/16;
		r_x_mod16 *= 16;
	
	/* cherche une position pour la fenetre */
		wfeu = px;
		hfeu = py;
		xfeu = x_desk+(w_desk-wfeu)/2;
		yfeu = y_desk+(h_desk-hfeu)/2;
	
		wind_calc(WC_BORDER, attr_prev, xfeu, yfeu, wfeu, hfeu, &xfe, &yfe, &wfe, &hfe);
		if(xfe<x_desk || yfe<y_desk || wfe>w_desk || hfe>h_desk)
		{	super_alert(1,3,T_TOO_SMALL_SCREEN);
			return 0;
		}
		if(def_x_pos_prev != 0 && def_y_pos_prev != 0)
		{	xfe = def_x_pos_prev;
			yfe = def_y_pos_prev;
		}
	
		prev_img = Malloc((size_t)r_x_mod16 * (size_t)(py+2) * 2);
		if(prev_img == 0)
		{	super_alert(1,1,T_NO_BUFF_MEMORY);
			return 0;
		}
	
		mfdb_prev.fd_addr = prev_img;
		mfdb_prev.fd_w = r_x_mod16;
		mfdb_prev.fd_h = py;
		mfdb_prev.fd_wdwidth = r_x_mod16 / 16;
		mfdb_prev.fd_stand = 0;
		mfdb_prev.fd_nplanes = 16;
		
		if(xfe+wfe>x_desk+w_desk)
			xfe = x_desk+w_desk-wfe;
		xfe = max(0,xfe);

		if(yfe+wfe>y_desk+h_desk)
			yfe = y_desk+h_desk-hfe;
		yfe = max(0,yfe);
		
		/* creation de la fenetre */
		
		if(hnd_wprev == -1)
		{	if((hnd_wprev = wind_create(attr_prev, xfe, yfe, wfe, hfe)) <= 0)
			{	super_alert(1,3,T_NO_MORE_WINDOWS);
				hnd_wprev = -1;
				Mfree(prev_img);
				return 0;
			}
		}
	
		/* fixe les infos */
		wind_set(hnd_wprev, WF_NAME, T_PREVIEW);
		
		wind_open(hnd_wprev,xfe,yfe,wfe,hfe);
		wind_get(hnd_wprev,WF_WORKXYWH,&xfeu, &yfeu, &wfeu, &hfeu);
		
		cur_prev = 1;
	}
	preview_pos = frame;
	calc_frame(frame, prev_img, px, py, r_x_mod16);
	redraw_prev(hnd_wprev,x_desk,y_desk,w_desk,h_desk);
	return 1;
}

void end_preview(void)
{	int i,j;
	
	if(cur_calc_prev)
	{	for(i=0;i<256;i++)
		{	if(opened_anim[i] != -1)
			{	close_anim(opened_anim[i]);
				opened_anim[i] = -1;
				if(ptr_anim[i])
					Mfree(ptr_anim[i]);
			}
			oldf_anim[i]=-1;
			ptr_anim[i]=0;
			if(ptr_calc[i])
				free(ptr_calc[i]);
			ptr_calc[i]=0;
			for(j=0;j<MAX_CUR_EFFETS;j++)
			{	if(ptr_eff_old[i][j])
					free(ptr_eff_old[i][j]);
				ptr_eff_old[i][j] = 0;
			}
			eff_frame[i]=-2;
			eff_obj[i]=-1;
		}
		if(ptr_fon)
			Mfree(ptr_fon);
		ptr_fon=0;
		cur_calc_prev=0;
	}
	
}

int calc_frame(int frame,int *prev_img, int px, int py, int r_x_mod16)
{	int col,frame_to_get,i,time;
	int anim_pA,anim_pB,anim_pT;
	int *ptr_calc_fon, *ptr_animA, *ptr_animB;
	int used_tran, transit, *adr_img_param[10]={0,0,0,0,0,0,0,0,0,0},piste,obj;
	TRAN_PARA tran_para;
	TRAN_REAL_PARAM *tran_real_param;
	TRAN_DESC_PARAM *tran_desc_param;
	
	if(projet.mode_fond && !ptr_fon)	/* mode objet de fond */
	{	ptr_fon = Malloc((size_t)objet[projet.obj_fond].x * (size_t)(objet[projet.obj_fond].y+1) * 2L);
		if(ptr_fon)
		{	opened_anim[2]=open_anim(projet.obj_fond);
			if(opened_anim[2]==-1)
			{	Mfree(ptr_fon);	ptr_fon = 0; }
			cur_calc_prev=1;
		}
	}
	
	
	/* REMPLISSAGE DU FOND */
	ptr_calc_fon = Malloc( (long)px * py * 2L);
	if(!ptr_calc_fon)
	{	Mfree(ptr_fon);	ptr_fon = 0;
		end_preview();
		return -1;
	}
	
	if(projet.mode_fond == 0)		/* mode couleur de remplissage */
	{	col = conv_col(projet.r,projet.v,projet.b);
		fill(ptr_calc_fon, px, py, px, col);
	}else							/* mode objet de fond */
	{	
		if(ptr_fon)
		{	frame_to_get = frame%objet[projet.obj_fond].nb_img;
			get_frame(opened_anim[2], frame_to_get, oldf_anim[2], ptr_fon, objet[projet.obj_fond].x);
			oldf_anim[2]= frame_to_get;
			
			if(projet.mode_obj == 1) /* obj zoom‚ */
			{	zoom(ptr_fon,ptr_calc_fon, objet[projet.obj_fond].x, objet[projet.obj_fond].y, objet[projet.obj_fond].x,
			        px, py, px,1024);
			}
			else		/* mode motif */
			{	motif(ptr_fon,ptr_calc_fon, objet[projet.obj_fond].x, objet[projet.obj_fond].y, objet[projet.obj_fond].x,
			        px, py, px);
			}
		}
	}
	

 	/* RESERVATION DES IMG A ET B */
	ptr_animA = Malloc( (long)px * py * 2L);
	if(!ptr_animA)
	{	Mfree(ptr_fon);	ptr_fon = 0;
		Mfree(ptr_calc_fon);
		end_preview();
		return -1;
	}
	ptr_animB = Malloc( (long)px * py * 2L);
	if(!ptr_animB)
	{	Mfree(ptr_fon);	ptr_fon = 0;
		Mfree(ptr_calc_fon);
		Mfree(ptr_animA);
		end_preview();
		return -1;
	}
	
	form_mem(UPDT_MEM);
	
	/* CALCUL DE L'ANIM AAAAA */
	
	/* ON COMMENCE PAR FOUTRE LE FOND */
	copy_tc(ptr_calc_fon, ptr_animA, py, px, 0);

	/* ON MET L'ANIM PRICIPALE */
	anim_pA=find_anim(frame, 0);	/* une anim sur la piste A ? */

	if(anim_pA != -1)		
		put_ani_pis(0,anim_pA,frame, ptr_animA, px, py, px);
	
	/* ON MET LES SUPERPOSITIONS */
	for(i=50;i<100;i++)
	{	anim_pT=find_anim(frame, i);	/* une anim sur la piste de superpos i ? */
		if(anim_pT != -1)		
			put_ani_pis(i,anim_pT,frame, ptr_animA, px, py, px);
	}
	
	/* CALCUL DE L'ANIM BBBBBB */
	
	/* ON COMMENCE PAR FOUTRE LE FOND */
	copy_tc(ptr_calc_fon, ptr_animB, py, px, 0);

	/* ON MET L'ANIM PRICIPALE */
	anim_pB=find_anim(frame, 1);	/* une anim sur la piste B ? */

	if(anim_pB != -1)		
		put_ani_pis(1,anim_pB,frame, ptr_animB, px, py, px);
	
	/* ON MET LES SUPERPOSITIONS */
	for(i=100;i<150;i++)
	{	anim_pT=find_anim(frame, i);	/* une anim sur la piste de superpos i ? */
		if(anim_pT != -1)		
			put_ani_pis(i,anim_pT,frame, ptr_animB, px, py, px);
	}


	/* LA TRANSITION */
	
	used_tran = find_tran(frame);
	
	if(used_tran != -1)
	{	transit = find_transit(prj_transit[used_tran].id);
		if(transit != -1)
		{	tran_real_param=&prj_transit[used_tran].tran_real_param;
			if(!prj_transit[used_tran].a_vers_b || transition[transit]->no_ab_swap)
			{	tran_para.img_a = ptr_animA;
				tran_para.img_b = ptr_animB;
			}else
			{	tran_para.img_a = ptr_animB;
				tran_para.img_b = ptr_animA;
			}
			tran_para.img_dst = prev_img;
			tran_para.x = px;
			tran_para.y = py;
			tran_para.larg_img_dst = r_x_mod16;
			tran_para.adr_tool_box = &tool_box;
			try_again_imgp:;
			if(transition[transit]->mode_para == 1 || transition[transit]->mode_para == 3)
			{	time = tran_real_param->time[0];
				if(tran_real_param->mode_vari[0] == 1)		/* mode time spline */
				{	if(prj_transit[used_tran].pos_out != prj_transit[used_tran].pos_in)		/* evite division par 0 */
						tran_para.uniq_para = (int)spline_comput(time,0, frame-prj_transit[used_tran].pos_in, prj_transit[used_tran].pos_out-prj_transit[used_tran].pos_in, transition[transit]->max_para);
				}else if(tran_real_param->mode_vari[0] == 0) /* fixe */
					tran_para.uniq_para = tran_real_param->vari_def[0];
				else /* img */
				{	if(tran_real_param->piste[0] == 0)	/* piste A tj ouverte */
						adr_img_param[0] = ptr_animA;
					else if(tran_real_param->piste[0] == 1)	/* piste B tj ouverte */
						adr_img_param[0] = ptr_animB;
					else										/* superpos */
					{	adr_img_param[0] = malloc((long)px*(py+1)*2);
						if(!adr_img_param[0])
						{	tran_real_param->mode_vari[0] = 0;		/* vire l'img param mode */
							goto try_again_imgp;
						}
						if(ptr_anim[tran_real_param->piste[0]])
						{	zoom(ptr_anim[tran_real_param->piste[0]],adr_img_param[0], objet[old_obj_anim[tran_real_param->piste[0]]].x, objet[old_obj_anim[tran_real_param->piste[0]]].y, objet[old_obj_anim[tran_real_param->piste[0]]].x,
									px,py,px,1024);
						}else
						{		/* ICI, SOIT IL N'Y A RIEN, SOIT C'EST UN OBJET SUR LA PISTE SG? QUI N'EST PAS ENCORE OUVERT*/
							piste = tran_real_param->piste[0];
							if(piste >= 150) /* piste sg */
							{	anim_pT=find_anim(frame, piste);	/* une anim sur la piste de superpos piste ? */
								if(anim_pT != -1)
								{	/* Y'A UNE ANIM, ON L'OUVRE */
									obj = prj_obj[anim_pT].objet;
									ptr_anim[piste] = Malloc((size_t)objet[obj].x * (size_t)(objet[obj].y+1) * 2L);
									if(ptr_anim[piste])
									{	opened_anim[piste]=open_anim(obj);
										if(opened_anim[piste]!=-1)
										{	cur_calc_prev=1;
											old_obj_anim[piste] = obj;
											/* ON PREND LA BONNE FRAME DE L'OBJ */
											frame_to_get = time_comput(anim_pT,frame - prj_obj[anim_pT].pos_in);
											get_frame(opened_anim[piste],frame_to_get, oldf_anim[piste], ptr_anim[piste], objet[obj].x);
											oldf_anim[piste]=frame_to_get;
											/* ON S'EN SERT POUR LA TRAN */
											zoom(ptr_anim[piste],adr_img_param[0], objet[old_obj_anim[piste]].x, objet[old_obj_anim[piste]].y, objet[old_obj_anim[piste]].x,
												px,py,px,1024);
										}else
										{	Mfree(ptr_anim[piste]);	ptr_anim[piste] = 0; old_obj_anim[piste]=-1; 
											fill(adr_img_param[0],px,py,px,0);
										}
									}else
										fill(adr_img_param[0],px,py,px,0);
								}else
									fill(adr_img_param[0],px,py,px,0);
							}else
								fill(adr_img_param[0],px,py,px,0);
						}
					}
					tran_real_param->adr_img[0]=adr_img_param[0];
				}
			}else if(transition[transit]->mode_para == 2) /* param stand */
			{	tran_desc_param = transition[transit]->tran_desc_param;
				/* INIT DES VARIABLES */
				for(i=0;i<tran_desc_param->nb_variables;i++)
				{	time = tran_real_param->time[i];
					if(tran_real_param->mode_vari[i] == 1)		/* mode time spline */
					{	if(prj_transit[used_tran].pos_out != prj_transit[used_tran].pos_in)		/* evite division par 0 */
							tran_real_param->vari[i] = tran_desc_param->min_vari[i]+(int)spline_comput(time,0, frame-prj_transit[used_tran].pos_in,
															   prj_transit[used_tran].pos_out-prj_transit[used_tran].pos_in, tran_desc_param->max_vari[i]-tran_desc_param->min_vari[i]);
					}else if(tran_real_param->mode_vari[i] == 0) /* fixe */
						tran_real_param->vari[i] = tran_real_param->vari_def[i];
					else /* img */
					{	if(tran_real_param->piste[i] == 0)	/* piste A tj ouverte */
							adr_img_param[i] = ptr_animA;
						else if(tran_real_param->piste[i] == 1)	/* piste B tj ouverte */
							adr_img_param[i] = ptr_animB;
						else										/* superpos */
						{	adr_img_param[i] = malloc((long)px*(py+1)*2);
							if(!adr_img_param[i])
							{	tran_real_param->mode_vari[i] = 0;		/* vire l'img param mode */
								goto try_again_imgp;
							}
							piste = tran_real_param->piste[i];
							if(ptr_anim[piste])
							{	zoom(ptr_anim[piste],adr_img_param[i], objet[old_obj_anim[piste]].x, objet[old_obj_anim[piste]].y, objet[old_obj_anim[piste]].x,
										px,py,px,1024);
							}else
							{		/* ICI, SOIT IL N'Y A RIEN, SOIT C'EST UN OBJET SUR LA PISTE SG? QUI N'EST PAS ENCORE OUVERT*/
								
								if(piste >= 150) /* piste sg */
								{	anim_pT=find_anim(frame, piste);	/* une anim sur la piste de superpos piste ? */
									if(anim_pT != -1)
									{	/* Y'A UNE ANIM, ON L'OUVRE */
										obj = prj_obj[anim_pT].objet;
										ptr_anim[piste] = Malloc((size_t)objet[obj].x * (size_t)(objet[obj].y+1) * 2L);
										if(ptr_anim[piste])
										{	opened_anim[piste]=open_anim(obj);
											if(opened_anim[piste]!=-1)
											{	cur_calc_prev=1;
												old_obj_anim[piste] = obj;
												/* ON PREND LA BONNE FRAME DE L'OBJ */
												frame_to_get = time_comput(anim_pT,frame - prj_obj[anim_pT].pos_in);
												get_frame(opened_anim[piste],frame_to_get, oldf_anim[piste], ptr_anim[piste], objet[obj].x);
												oldf_anim[piste]=frame_to_get;
												/* ON S'EN SERT POUR LA TRAN */
												zoom(ptr_anim[piste],adr_img_param[i], objet[old_obj_anim[piste]].x, objet[old_obj_anim[piste]].y, objet[old_obj_anim[piste]].x,
													px,py,px,1024);
											}else
											{	Mfree(ptr_anim[piste]);	ptr_anim[piste] = 0; old_obj_anim[piste]=-1; 
												fill(adr_img_param[i],px,py,px,0);
											}
										}else
											fill(adr_img_param[i],px,py,px,0);
									}else
										fill(adr_img_param[i],px,py,px,0);
								}else
									fill(adr_img_param[i],px,py,px,0);
							}
						}
					}
					tran_real_param->adr_img[i]=adr_img_param[i];
				}
			}	
			tran_para.tran_real_param = tran_real_param;
		
			(transition[transit]->calc_tran)(&tran_para);
			if(transition[transit]->mode_para == 3 && adr_img_param[0])
				free(adr_img_param[0]);
		}
	}else
	{	if(anim_pA != -1)
			copy_tc(ptr_animA, prev_img, py, px, r_x_mod16 - px);
		else
			copy_tc(ptr_animB, prev_img, py, px, r_x_mod16 - px);
	}

	/* ON MET LES SUPERPOSITIONS GENERALES */
	for(i=150;i<200;i++)
	{	anim_pT=find_anim(frame, i);	/* une anim sur la piste de superpos i ? */
		if(anim_pT != -1)		
			put_ani_pis(i,anim_pT,frame, prev_img, px, py, r_x_mod16);
	}
	
	/* EFFETS GENEREAUX */
	
	
	
	/* On ferme tout ca si y'a plus d'anim sur la piste */
	if(anim_pA == -1 && ptr_anim[0])
	{	Mfree(ptr_anim[0]);
		ptr_anim[0]=0;
		if(opened_anim[0] != -1)
		{	close_anim(opened_anim[0]); opened_anim[0]=-1; oldf_anim[0]=-1; }
	}
	if(anim_pB == -1 && ptr_anim[1])
	{	Mfree(ptr_anim[1]);
		ptr_anim[1]=0;
		if(opened_anim[1] != -1)
		{	close_anim(opened_anim[1]); opened_anim[1]=-1; oldf_anim[1]=-1; }
	}

	Mfree(ptr_calc_fon);
	Mfree(ptr_animA);
	Mfree(ptr_animB);
	return 0;
}

/* routine ki met un prj obj sur un fon */

void put_ani_pis(int piste, int anim_p,int frame, int *ptr_img, int px, int py, int x_mod16)
{	int obj,frame_to_get,time,time_spline,i,j;
	int intens=1024,tmp,*final_adr,num_effet/*,actu_buffer,nb_buffers*/;
/*	int *buffer1,*buffer2;*/
	STOQUAG_EFF *les_effets;
	EFFET_PARA effet_para;
	TRAN_DESC_PARAM *effet_desc_param;
	TRAN_REAL_PARAM *effet_real_param;
	
	obj = prj_obj[anim_p].objet;
		
	if(!ptr_anim[piste])		/* pas d'obj precedement ouvert sur cette piste */
	{	ptr_anim[piste] = Malloc((size_t)objet[obj].x * (size_t)(objet[obj].y+1) * 2L);
		if(ptr_anim[piste])
		{	opened_anim[piste]=open_anim(obj);
			if(opened_anim[piste]==-1)
			{	Mfree(ptr_anim[piste]);	ptr_anim[piste] = 0; }
			cur_calc_prev=1;
			old_obj_anim[piste] = obj;
		}
	}else if(obj != old_obj_anim[piste])	/* le precedent n'est pas le meme */
	{	/* on le ferme */
		Mfree(ptr_anim[piste]);	ptr_anim[piste] = 0;
		if(opened_anim[piste] != -1)
		{	close_anim(opened_anim[piste]); opened_anim[piste]=-1; oldf_anim[piste]=-1; }
		/* et on ouvre le nouveau */
		ptr_anim[piste] = Malloc((size_t)objet[obj].x * (size_t)(objet[obj].y+1) * 2L);
		if(ptr_anim[piste])
		{	opened_anim[piste]=open_anim(obj);
			if(opened_anim[piste]==-1)
			{	Mfree(ptr_anim[piste]);	ptr_anim[piste] = 0; }
			cur_calc_prev=1;
			old_obj_anim[piste] = obj;
		}
	}	
	
	if(ptr_anim[piste])
	{	frame_to_get = time_comput(anim_p,frame - prj_obj[anim_p].pos_in);
		get_frame(opened_anim[piste],frame_to_get, oldf_anim[piste], ptr_anim[piste], objet[obj].x);
		oldf_anim[piste]=frame_to_get;
		
		final_adr = ptr_anim[piste]; /* par defaut */
		/* APPLICATION DES EFFETS */
		les_effets = prj_obj[anim_p].stoquag_eff;
		if(les_effets)
		{	if(les_effets->nb_effets == 1)
			{	/* SI C'EST LE PREMIER CALCUL, ON RESERVE UN BUFFER */
				if(!ptr_calc[piste])
					ptr_calc[piste] = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
					
				if(ptr_calc[piste] && eff_obj[piste] != anim_p)	/* pas le bon buffer */
				{	free(ptr_calc[piste]);
					ptr_calc[piste] = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
				}
				if(ptr_calc[piste])
				{	eff_obj[piste] = anim_p;

					
					num_effet = find_effet(les_effets->id[0]);
					if(num_effet != -1)
					{	effet_para.img_src = ptr_anim[piste];
						effet_para.img_dst = ptr_calc[piste];
						effet_para.larg_img = objet[obj].x;
						effet_para.x = objet[obj].x;
						effet_para.y = objet[obj].y;
						effet_para.adr_tool_box = &tool_box;
						effet_para.preced_img_ok = (eff_frame[piste] == (frame-1));
						
										/* INIT DES VARIABLES */
						if(effet[num_effet]->mode_para == EFF_STD_PARAM)
						{
							effet_desc_param = effet[num_effet]->effet_desc_param;
							effet_real_param = &les_effets->effet_param[0];
							
							for(j=0;j<effet_desc_param->nb_variables;j++)
							{	time = effet_real_param->time[j];
								if(effet_real_param->mode_vari[j] == 1)		/* mode time spline */
								{	if(prj_obj[anim_p].pos_out != prj_obj[anim_p].pos_in)		/* evite division par 0 */
										effet_real_param->vari[j] = effet_desc_param->min_vari[j]+(int)spline_comput(time,0, frame-prj_obj[anim_p].pos_in,
																	   prj_obj[anim_p].pos_out-prj_obj[anim_p].pos_in, effet_desc_param->max_vari[j]-effet_desc_param->min_vari[j]);
								}else if(effet_real_param->mode_vari[j] == 0) /* fixe */
									effet_real_param->vari[j] = effet_real_param->vari_def[j];
							}
							effet_para.effet_real_param = effet_real_param;
						}

						
						/* GOOOOOOOOOO ! */
						(effet[num_effet]->calc_effet)(&effet_para);
						eff_frame[piste] = frame;
						final_adr = ptr_calc[piste];
					}

					/* SI C'EST LE DERNIER CALCUL, ON VIDE LE BUFFER */
					if(frame == prj_obj[anim_p].pos_out)
					{	free(ptr_calc[piste]);
						ptr_calc[piste]=0;
						eff_obj[piste] = -1;
						eff_frame[piste] = -2;
					}
				}else
					goto no_effect;
			}else /* PLUSIEURS EFFETS */
			{	for(i=0;i<les_effets->nb_effets;i++)
				{	/* SI C'EST LE PREMIER CALCUL, ON RESERVE UN BUFFER */
					if(!ptr_eff_old[piste][i])
						ptr_eff_old[piste][i] = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
					
					if(ptr_eff_old[piste][i] && eff_obj[piste] != anim_p)	/* pas le bon buffer */
					{	free(ptr_eff_old[piste][i]);
						ptr_eff_old[piste][i] = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
					}
					if(ptr_eff_old[piste][i])
					{	eff_obj[piste] = anim_p;

					
						num_effet = find_effet(les_effets->id[i]);
						if(num_effet != -1)
						{	effet_para.img_src = final_adr;
							effet_para.img_dst = ptr_eff_old[piste][i];
							effet_para.larg_img = objet[obj].x;
							effet_para.x = objet[obj].x;
							effet_para.y = objet[obj].y;
							effet_para.adr_tool_box = &tool_box;
							effet_para.preced_img_ok = (eff_frame[piste] == (frame-1));
			
													/* INIT DES VARIABLES */
							if(effet[num_effet]->mode_para == EFF_STD_PARAM)
							{
								effet_desc_param = effet[num_effet]->effet_desc_param;
								effet_real_param = &les_effets->effet_param[i];
								
								for(j=0;j<effet_desc_param->nb_variables;j++)
								{	time = effet_real_param->time[j];
									if(effet_real_param->mode_vari[j] == 1)		/* mode time spline */
									{	if(prj_obj[anim_p].pos_out != prj_obj[anim_p].pos_in)		/* evite division par 0 */
											effet_real_param->vari[j] = effet_desc_param->min_vari[j]+(int)spline_comput(time,0, frame-prj_obj[anim_p].pos_in,
																	   prj_obj[anim_p].pos_out-prj_obj[anim_p].pos_in, effet_desc_param->max_vari[j]-effet_desc_param->min_vari[j]);
									}else if(effet_real_param->mode_vari[j] == 0) /* fixe */
										effet_real_param->vari[j] = effet_real_param->vari_def[j];
								}
								effet_para.effet_real_param = effet_real_param;
							}
							/* GOOOOOOOOOO ! */
							(effet[num_effet]->calc_effet)(&effet_para);
							
							final_adr = ptr_eff_old[piste][i];
					}

					/* SI C'EST LE DERNIER CALCUL, ON VIDE LE BUFFER */
					if(frame == prj_obj[anim_p].pos_out)
					{	free(ptr_eff_old[piste][i]);
						ptr_eff_old[piste][i]=0;
						eff_obj[piste] = -1;
						eff_frame[piste] = -2;
					}
				}else
					goto no_effect;

/*					if(nb_buffers == 0)
					{	buffer1 = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
						if(!buffer1)
							goto no_effect;
						actu_buffer=1;
						nb_buffers=1;
					}else if(nb_buffers == 1)
					{	buffer2 = malloc((long)objet[obj].x * (objet[obj].y+2) * 2L);
						if(!buffer2)
						{	free(buffer1);
							goto no_effect;
						}
						actu_buffer=2;
						nb_buffers=2;
					}
	*/					
				}
				eff_frame[piste] = frame;
					/* SI C'EST LE DERNIER CALCUL */
				if(frame == prj_obj[anim_p].pos_out)
				{	eff_obj[piste] = -1;
					eff_frame[piste] = -2;
				}
			}
		}
		
		no_effect:;
		/* CALCUL DE L'INTENSITE */
		tmp = prj_obj[anim_p].intens;
		if(tmp & 0x8000)
			intens = ((tmp>>12)&0x7)*256;
		else
		{	time_spline = prj_obj[anim_p].intens & 0x0fff;
			if(time_spline == 0)	/* lineaire */
			{	if(prj_obj[anim_p].pos_out ==  prj_obj[anim_p].pos_in)
					intens=0;
				else
					intens = (int)( (((long)frame - prj_obj[anim_p].pos_in) * 1024L )/ ( (long)prj_obj[anim_p].pos_out -  prj_obj[anim_p].pos_in) );
			}else
				intens = (int)spline_comput(time_spline,0,frame - prj_obj[anim_p].pos_in,prj_obj[anim_p].pos_out -  prj_obj[anim_p].pos_in+1,1023);
			
		}
		/* PLACEMENT */
		if(intens)  /* sinon ca sert a rien */
		{	if(prj_obj[anim_p].mvt == 0)	/* mode zoome ki rempli tou l'ecran */
			{	if(prj_obj[anim_p].trans == -1)
					zoom(final_adr,ptr_img, objet[obj].x, objet[obj].y, objet[obj].x,
				        px, py, x_mod16,intens);
				else
					zoom_tr(final_adr,ptr_img, objet[obj].x, objet[obj].y, objet[obj].x,
				        px, py, x_mod16,
				        conv_col(prj_obj[anim_p].r,prj_obj[anim_p].v,prj_obj[anim_p].b), prj_obj[anim_p].trans,intens);
			}
			else	 /*il y a un mouvement */
			{	time = (int)( (((long)frame - prj_obj[anim_p].pos_in)*1000L)/( prj_obj[anim_p].pos_out - prj_obj[anim_p].pos_in));
				cacl_n_put_mvt(final_adr,ptr_img,
									 objet[obj].x, objet[obj].y, objet[obj].x,
									 px, py, x_mod16,
								 prj_obj[anim_p].mvt,time,
								 prj_obj[anim_p].trans,prj_obj[anim_p].r,prj_obj[anim_p].v,prj_obj[anim_p].b,intens);
			}
		}	
	}
}

void close_prev(void)
{	if(cur_prev)
	{	cur_prev = 0;
		preview_pos = -1;
		redraw_pistes(hnd_wpistes,xfpu+20,yfpu+8,wfpu,10);
		wind_close(hnd_wprev);
		wind_delete(hnd_wprev);
		hnd_wprev=-1;
		Mfree(prev_img);
	}
}

void close_aprev(void)
{	if(cur_aprev)
	{	cur_aprev = 0;
		wind_close(hnd_wprev);
		wind_delete(hnd_wprev);
		hnd_wprev=-1;
		Mfree(prev_img);
	}
}

void aprev_start(void)
{	int dummy,key,hnd,mem_replay;
	int x_base = xfpu+18;
	int pxy[8];
	char str[160],*buffer;
	long count,taille,cur,tot,taille_tot,speed,timer_c;
	int xb, yb, wb, hb;
	OBJECT *adr_boot;
	
	if(!mnt_registered)
		aprev_mode=0;
	
	nb_img_in_wind_pist = (int)(((wfpu-18)*(long)projet.zoom)/48L);
	
	if(!actu_projet)
		return;
	if(cur_aprev)
	{	stop_aprev();
		return;
	}
	/* on utilise la fenetre de preview, alors on la ferme */
	close_prev();

	old_curseur=-1;
	
	/* c'est parti */
	if(projet.taille_prev == 3)
	{	px = 2*projet.x;
		py = 2*projet.y;
	}else
	{	px = projet.x / tab_red_pre[projet.taille_prev];
		py = projet.y / tab_red_pre[projet.taille_prev];
	}
	
	
	r_x_mod16 = (px+15)/16;
	r_x_mod16 *= 16;
	
	/* cherche une position pour la fenetre */
	wfeu = px;
	hfeu = py;
	xfeu = x_desk+(w_desk-wfeu)/2;
	yfeu = y_desk+(h_desk-hfeu)/2;
	
	wind_calc(WC_BORDER, attr_prev|(INFO*aprev_mode), xfeu, yfeu, wfeu, hfeu, &xfe, &yfe, &wfe, &hfe);
	if(xfe<x_desk || yfe<y_desk || wfe>w_desk || hfe>h_desk)
	{	super_alert(1,3,T_TOO_SMALL_SCREEN);
		return;
	}
	if(def_x_pos_prev != 0 && def_y_pos_prev != 0)
	{	xfe = def_x_pos_prev;
		yfe = def_y_pos_prev;
	}
	
	prev_img = Malloc((size_t)r_x_mod16 * (size_t)py * 2);
	if(prev_img == 0)
	{	super_alert(1,1,T_NO_BUFF_MEMORY);
		return;
	}
	
	mfdb_prev.fd_addr = prev_img;
	mfdb_prev.fd_w = r_x_mod16;
	mfdb_prev.fd_h = py;
	mfdb_prev.fd_wdwidth = r_x_mod16 / 16;
	mfdb_prev.fd_stand = 0;
	mfdb_prev.fd_nplanes = 16;
		
	if(xfe+wfe>x_desk+w_desk)
		xfe = x_desk+w_desk-wfe;
	xfe = max(0,xfe);

	if(yfe+wfe>y_desk+h_desk)
		yfe = y_desk+h_desk-hfe;
	yfe = max(0,yfe);
		
	/* creation de la fenetre */

	if(hnd_wprev==-1)
	{	if((hnd_wprev = wind_create(attr_prev|(INFO*aprev_mode), xfe, yfe, wfe, hfe)) <= 0)
		{	super_alert(1,3,T_NO_MORE_WINDOWS);
			hnd_wprev = -1;
			Mfree(prev_img);
			return;
		}
	}
	
	/* fixe les infos */
	wind_set(hnd_wprev, WF_NAME, T_PREVIEW);
			
	cur_aprev=1;
	aprev_pos=projet.preview_in;

	if(aprev_mode==1 && mnt_registered)	/* mode precalc */
	{	
			/* preparation de la boite de boot */
		rsrc_gaddr(0,BOOTING,&adr_boot);
	
		form_center(adr_boot , &xb ,&yb, &wb ,&hb);		/* centre formulaire */
		form_dial (FMD_START, 0, 0, 0, 0, xb, yb, wb, hb);		/* prepare l'affichage */
		adr_boot[BOOT_ASS].ob_width=1;
		strcpy(adr_boot[BOOT_TEXT].ob_spec.tedinfo->te_ptext,T_CREAT_PREV);
		objc_draw (adr_boot, 0, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);

		strcpy(str,chemin_key);
		strcat(str,"TMP_PREV.BIN");
		hnd=(int)Fcreate(str,0);
		if(hnd>=0)
		{	/*dummy = projet.preview_out - projet.preview_in;
			Fwrite(hnd,2,&dummy);
			Fwrite(hnd,2,&px);
			Fwrite(hnd,2,&py);*/
			taille = (long)py*r_x_mod16*2L;
			cur = 0;
			tot = (projet.preview_out - projet.preview_in + 1)*2;
			for(;aprev_pos<=projet.preview_out; aprev_pos++)
			{	/* avance de la barre */
				adr_boot[BOOT_ASS].ob_width = (int)((328L * ++cur) / tot);
				objc_draw (adr_boot, BOOT_ASS, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
				/* calcul de l'image */
				calc_frame(aprev_pos, prev_img, px, py, r_x_mod16);
				/* avance de la barre */
				adr_boot[BOOT_ASS].ob_width = (int)((328L * ++cur) / tot);
				objc_draw (adr_boot, BOOT_ASS, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
				/* savegarde de l'image */
				count = Fwrite(hnd,taille,prev_img);
				if(count < taille)
				{	super_alert(1,3,T_DISK_2_ERROR);
					Fclose(hnd);
					Fdelete(str);
					stop_aprev();
					form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */
					goto end_aprev;
				}
			}
			form_mem(UPDT_MEM);
			end_preview(); /* libere de la RAM */
			Fclose(hnd);
			hnd=(int)Fopen(str,FO_READ);
			if(hnd>=0)
			{	wind_set(hnd_wprev, WF_INFO, T_SPC2REPLAY);
				wind_open(hnd_wprev,xfe,yfe,wfe,hfe);
				wind_get(hnd_wprev,WF_WORKXYWH,&xfeu, &yfeu, &wfeu, &hfeu);

				/* Zone source dans l'image en m‚moire : */
				pxy[0] = 0;
				pxy[1] = 0;	
				pxy[2] = px-1;	
				pxy[3] = py-1;	
			/* Zone destination dans la fenˆtre … l'‚cran */
				pxy[4] = xfeu;							
				pxy[5] = yfeu;							
				pxy[6] = pxy[4] + px - 1;	
				pxy[7] = pxy[5] + py - 1;	

				v_hide_c(vdi_handle);
				set_clip(0,0);
				taille_tot = (projet.preview_out-projet.preview_in +1)*taille;
				mem_replay=0;
				if(((long)Malloc(-1L)-64000L) > taille_tot)
				{	buffer = Malloc(taille_tot);
					if(buffer)
					{	mem_replay = TRUE;
						Fread(hnd,taille_tot,buffer);
						Fclose(hnd);
					}
				}
				form_mem(UPDT_MEM);
				speed = 200/projet.speed;
				replay:;
				if(!mem_replay)
					Fseek(0,hnd,0);
				for(aprev_pos=projet.preview_in;aprev_pos<=projet.preview_out; aprev_pos++)
				{	timer_c = get_timerC()+speed;
					if(mem_replay)
						mfdb_prev.fd_addr = buffer + ((long)(aprev_pos-projet.preview_in))*taille;
					else
						Fread(hnd,taille,prev_img);
					/* AFFICHAGE DIRECT */
					tnt_cpyfm(pxy,&mfdb_prev);
					if(aprev_curseur)
						set_curseur(aprev_pos);
					while(timer_c > get_timerC());  /* attend la syncro */
				}
				do
				{	/*dummy=(int)Crawio(0xFF);*/
					dummy=(int)Cnecin();
					if((dummy&0xff)==' ') goto replay;
				}while((dummy&0xff)!=27);
				
				if(mem_replay)
					Mfree(buffer);
				else
					Fclose(hnd);
			}else
				super_alert(1,3,T_DISK_ERROR);
			form_dial (FMD_FINISH, 0, 0, 0, 0, xb - 4, yb - 4, wb + 8, hb + 8);	/* efface */
	
			v_show_c(vdi_handle,1);
			stop_aprev();
	
			Fdelete(str);
		}
	}else		/* mode temps reel */
	{	wind_open(hnd_wprev,xfe,yfe,wfe,hfe);
		wind_get(hnd_wprev,WF_WORKXYWH,&xfeu, &yfeu, &wfeu, &hfeu);

		/* la premiere (pour les redraw) */
		calc_frame(aprev_pos, prev_img, px, py, r_x_mod16);

		if(aprev_preemptif) 
		{	/* MODE PREEMPTIF KI BLOQUE TOUT */
			/* Zone source dans l'image en m‚moire : */
			pxy[0] = 0;
			pxy[1] = 0;	
			pxy[2] = px-1;	
			pxy[3] = py-1;	
		/* Zone destination dans la fenˆtre … l'‚cran */
			pxy[4] = xfeu;							
			pxy[5] = yfeu;							
			pxy[6] = pxy[4] + px - 1;	
			pxy[7] = pxy[5] + py - 1;	

			v_hide_c(vdi_handle);
	
			for(;aprev_pos<=projet.preview_out; aprev_pos++)
			{	if(aprev_pos!=projet.preview_in) /* si pas deja calcule */
					calc_frame(aprev_pos, prev_img, px, py, r_x_mod16);
				Vsync();
			
				/* CURSEUR DANS LES PISTES */
				if(aprev_curseur)
				{	/* mode trasparent */
					vswr_mode(vdi_handle,MD_XOR);
					set_clip(0,0);
					/* effacement de l'ancien curseur */
					if(old_curseur != -1)
						ligne(img2pix(old_curseur),yfpu,1,hfpu,BLACK);
					if(aprev_pos>=deb_img && aprev_pos<=(deb_img+nb_img_in_wind_pist))	/* dans la fenetre ! */
					{	ligne(img2pix(aprev_pos),yfpu,1,hfpu,BLACK);
						old_curseur = aprev_pos;
					}
					set_curseur(aprev_pos);
				}

				/* AFFICHAGE DIRECT */
				tnt_cpyfm(pxy,&mfdb_prev);
			
				graf_mkstate(&dummy, &dummy, &dummy,&key);
				if(key & (K_LSHIFT|K_RSHIFT|K_CTRL|K_ALT))
					goto end_paprev;
			}
			end_paprev:
			v_show_c(vdi_handle,1);
			stop_aprev();
		}else
		{	/* MODE TIMER EN "TACHE DE FOND" */
			/* INSTALL UNE NOUVELLE DEMANDE D'APPTACHE DE FOND */
			install_tache_fond(TACHE_APREV);
		}
	}
	end_aprev:;
	
}

void aprev_timer(void)
{	int x_base = xfpu+18,dummy;

	if(aprev_pos!=projet.preview_in) /* si pas deja calcule */
		calc_frame(aprev_pos, prev_img, px, py, r_x_mod16);
	
	Vsync();
		/* CURSEUR DANS LES PISTES */
	if(aprev_curseur)
	{	/* mode trasparent */
		vswr_mode(vdi_handle,MD_XOR);
		set_clip(0,0);
		/* effacement de l'ancien curseur */
		if(old_curseur != -1)
			ligne(img2pix(old_curseur),yfpu,1,hfpu,BLACK);
		if(aprev_pos>=deb_img && aprev_pos<=(deb_img+nb_img_in_wind_pist))	/* dans la fenetre ! */
		{	ligne(img2pix(aprev_pos),yfpu,1,hfpu,BLACK);
			old_curseur = aprev_pos;
		}
		set_curseur(aprev_pos);
	}
		/* AFFICHAGE REDRAW */
	redraw_prev(hnd_wprev,x_desk,y_desk,w_desk,h_desk);
	
	/* faut bien avancer */
	aprev_pos++;
	
	graf_mkstate(&dummy, &dummy, &dummy,&key);
	if( (key & (K_LSHIFT|K_RSHIFT|K_CTRL|K_ALT)) || aprev_pos>projet.preview_out)
		stop_aprev();
	
}

static void stop_aprev(void)
{	int x_base = xfpu+18;
	remove_tache_fond(TACHE_APREV);
	end_preview();
	close_aprev();
/* effacement de l'ancien curseur */
	if(old_curseur != -1 && aprev_curseur)
	{	vswr_mode(vdi_handle,MD_XOR);
		set_clip(0,0);
		ligne(img2pix(old_curseur),yfpu,1,hfpu,BLACK);
	}
}
void window_prev(int event)
{		
	if (event & MU_MESAG)
	{
		switch(buff[0])
		{
			case WM_REDRAW :
				redraw_prev(buff[3], buff[4], buff[5], buff[6], buff[7]);
				break;
			case WM_TOPPED :
				if(!modal_flag)
					wind_set(buff[3], WF_TOP);
				break;
			case WM_CLOSED : 
				if(!modal_flag)
				{	if(cur_prev)
						close_prev();
					else if(cur_aprev)
						close_aprev();
				}
				break;
			case WM_MOVED :
				wind_set(buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7]);
				wind_get(hnd_wprev,WF_CURRXYWH,&xfe, &yfe, &wfe, &hfe);
				wind_get(hnd_wprev,WF_WORKXYWH,&xfeu, &yfeu, &wfeu, &hfeu);
				def_x_pos_prev = xfe;
				def_y_pos_prev = yfe;
				break;
			
		}
	}
}

void redraw_prev(int w_handle, int x, int y, int w, int h)
{
	GRECT r, rd,rimg;
	int pxy[8],efface_mouse=0,mx,my,dum;
	
	rd.g_x = x;
	rd.g_y = y;
	rd.g_w = w;
	rd.g_h = h;
	
	if(w_handle<1)
		goto end_redraw;
		
	graf_mkstate(&mx, &my, &dum,&dum);
	if( mx>(xfeu-16) && mx<(xfeu+wfeu) && my>(yfeu-16) && my<(yfeu+hfeu))
	{	efface_mouse = 1;
		v_hide_c(vdi_handle);
	}
	wind_update(BEG_UPDATE);
		
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while (r.g_w && r.g_h)
	{	
		if (rc_intersect(&rd, &r))
		{	
			set_clip(1, &r); 
			
			rimg.g_x = xfeu;
			rimg.g_y = yfeu;
			rimg.g_w = px;
			rimg.g_h = py;
			
			if(rc_intersect(&r, &rimg))
			{	
			/* Zone source dans l'image en m‚moire : */
				pxy[0] = rimg.g_x - xfeu;	
				pxy[1] = rimg.g_y - yfeu;	
				pxy[2] = pxy[0] + rimg.g_w - 1;	
				pxy[3] = pxy[1] + rimg.g_h - 1;	
			/* Zone destination dans la fenˆtre … l'‚cran */
				pxy[4] = rimg.g_x;							
				pxy[5] = rimg.g_y;							
				pxy[6] = pxy[4] + rimg.g_w - 1;	
				pxy[7] = pxy[5] + rimg.g_h - 1;	
				tnt_cpyfm ( pxy, &mfdb_prev);
			}
		}
		wind_get(w_handle, WF_NEXTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	}
	wind_update(END_UPDATE);
	if(efface_mouse)
		v_show_c(vdi_handle, 1);
	end_redraw:;
}

void form_opre(int event)
{
	WINDFORM_VAR *ptr_var = &opre_var;
	int choix;
	
	if (event == OPEN_DIAL)
	{	if(ptr_var->w_handle < 1)
		{	init_radio(ptr_var,APRV_TREEL+aprev_mode);
			wf_change(ptr_var,APRV_PREEM,aprev_preemptif,0);
			wf_change(ptr_var,APRV_CURS,aprev_curseur,0);
		}
		open_dialog(ptr_var, T_OPT_A_PREV, 0, 1);
	}
	else 
	{	choix = windform_do(ptr_var, event);
		if (choix != 0) 
		{	if(choix == CLOSE_DIAL)
				choix = APRV_ANN;
			wf_change(ptr_var,choix,0,1);
			switch(choix)
			{		
				case APRV_OK:
					aprev_mode = ptr_var->adr_form[APRV_PREC].ob_state & SELECTED;
					aprev_preemptif = ptr_var->adr_form[APRV_PREEM].ob_state & SELECTED;
					aprev_curseur = ptr_var->adr_form[APRV_CURS].ob_state & SELECTED;
				case APRV_ANN:
					close_dialog(ptr_var);
					break;
			}
		}
	}
}
