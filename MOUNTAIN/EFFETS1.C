/*******************MOUNTAIN********************/
/*****           EFFETS une Image           ****/
/*****             Version 0.1              ****/
/*****            Par Vanel R‚mi            ****/
/*****         Premier code : 4/5/97        ****/
/***********************************************/



#include <math.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include "flh_lib.h"
#include "transit.h"
#include "time_spl.h"
#include "some_fnc.h"
#include "geffets.h"
#include "effets1.h"

typedef unsigned long   size_t;

/* dans mountain.s effet.s z_intens.s */
void asm_zoom(int *scr, int *dst, int x_src, int x_dest);
void asm_50_zoom(int *scr, int *dst, int x_src, int x_dest);
void asm_25_zoom(int *scr, int *dst, int x_src, int x_dest);
void asm_75_zoom(int *scr, int *dst, int x_src, int x_dest);
void a_tr0_zoom(int *scr, int *dst, int x_src, int x_dest, int col);
void opt_tr_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int mask);
void tr_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int pour_64_toler);
void tr_50_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int pour_64_toler);
void tr_25_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int pour_64_toler);
void tr_75_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int pour_64_toler);
void asm_motif(int *scr, int *dst, int x_src, int x_dest);
void a_50_tr0_zoom(int *scr, int *dst, int x_src, int x_dest, int col);
void a_25_tr0_zoom(int *scr, int *dst, int x_src, int x_dest, int col);
void a_75_tr0_zoom(int *scr, int *dst, int x_src, int x_dest, int col);
void asm_int_zoom(int *scr, int *dst, int x_src, int x_dest, int intens);
void tr0_int_zoom(int *scr, int *dst, int x_src, int x_dest, int intens, int col);
void tr_int_zoom(int *scr, int *dst, int x_src, int x_dest, int col, int pour_64_toler, int intens);
void asm_rot(int *src,int *dst,int x,int y,long asm_cos,long asm_sin,int larg, int x_centre, int y_centre, int couleur);
void asm2_rot(int *src,int *dst,int x,int y,long asm_cos,long asm_sin,int larg, int x_centre, int y_centre);


void zoom(int *img_src, int *img_dest, int x_src, int y_src, int x_src_img, int x_dest, int y_dest, int x_dest_img,int intens)
{	int i;
	float cur_y,skip;
	size_t icur_y;
	
	if(x_dest<=1) return;		/* sinon bug */
	skip = (float)y_src / (float)y_dest;
	cur_y=0;
	if(intens==1024)	/* 100 % */
	{	for(i=0;i<y_dest;i++)
		{	icur_y = (size_t)cur_y;
			asm_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest);
			cur_y += skip;
		}
	}else if(intens==512)	/* 50 % */
	{	for(i=0;i<y_dest;i++)
		{	icur_y = (size_t)cur_y;
			asm_50_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest);
			cur_y += skip;
		}
	}else if(intens==768)	/* 75 % */
	{	for(i=0;i<y_dest;i++)
		{	icur_y = (size_t)cur_y;
			asm_75_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest);
			cur_y += skip;
		}
	}else if(intens==256)	/* 25 % */
	{	for(i=0;i<y_dest;i++)
		{	icur_y = (size_t)cur_y;
			asm_25_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest);
			cur_y += skip;
		}
	}else if(intens==0)	/* 0 % */
		return;
	else /* % libre */
	{	for(i=0;i<y_dest;i++)
		{	icur_y = (size_t)cur_y;
			asm_int_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,intens);
			cur_y += skip;
		}
	}
	
}

/* avec trasparence et intensite */
void zoom_tr(int *img_src, int *img_dest, int x_src, int y_src, int x_src_img, int x_dest, int y_dest, int x_dest_img, int col_tran, int toler, int intens)
{	int i;
	float cur_y,skip;
	size_t icur_y;
	
	if(x_dest<=1) return;		/* sinon bug */
	skip = (float)y_src / (float)y_dest;
	cur_y=0;
		
	if(toler == 0)		/* mode FAST */
	{	if(intens == 1024)	/* 100 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				a_tr0_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran);
				cur_y += skip;
			}
		}else if(intens == 512)	/* 50 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				a_50_tr0_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran);
				cur_y += skip;
			}
		}else if(intens == 768)	/* 75 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				a_75_tr0_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran);
				cur_y += skip;
			}
		}else if(intens == 256)	/* 25 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				a_25_tr0_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran);
				cur_y += skip;
			}
		}else if(intens==0)	/* 0 % */
			return;
		else /* intens libre */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr0_int_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,intens,col_tran);
				cur_y += skip;
			}
		}	
	}else	/* au % pre */
	{	toler = (int)( ((long)toler*64L)/1000L);
		if(intens == 1024)	/* 100 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran,toler);
				cur_y += skip;
			}
		}else if(intens == 512)	/* 50 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr_50_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran,toler);
				cur_y += skip;
			}
		}else if(intens == 256)	/* 25 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr_25_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran,toler);
				cur_y += skip;
			}
		}else if(intens == 768)	/* 75 % */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr_75_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran,toler);
				cur_y += skip;
			}
		}else if(intens==0)	/* 0 % */
			return;
		else		/* intensite libre */
		{	for(i=0;i<y_dest;i++)
			{	icur_y = (size_t)cur_y;
				tr_int_zoom(img_src+(icur_y*x_src_img),img_dest+((size_t)i*x_dest_img),x_src,x_dest,col_tran,toler,intens);
				cur_y += skip;
			}
		}
	}
}

void motif(int *img_src, int *img_dest, int x_src, int y_src, int x_src_img, int x_dest, int y_dest, int x_dest_img)
{	int y;

	for(y=0;y<y_dest ; y++)
		asm_motif(img_src+((y%y_src)*(size_t)x_src_img),img_dest+((size_t)y*x_dest_img),x_src,x_dest);
	
}

void fill(int *img_dest, int x_dest, int y_dest, int x_dest_img, int coul)
{	int i,j,*adr_ligne=img_dest;

	for(i=0;i<y_dest;i++)
	{	for(j=0;j<x_dest;j++)
			*(adr_ligne++) = coul;
		img_dest += x_dest_img;
		adr_ligne=img_dest;
	}
}	

int effet_ROTS(EFFET_PARA *effet_para)
{	int *src,*dst,x,y,xs,ys,angle,x_centre,y_centre,larg;
	int couleur,cx,cy,xi,yi,mosaique;
	double angle_rad,c,s,zoom;
	long asm_cos,asm_sin;
	
	src=effet_para->img_src;
	dst=effet_para->img_dst;
	xi=effet_para->x;
	yi=effet_para->y;
	mosaique = effet_para->effet_real_param->switchs&2;
	couleur=conv_col(effet_para->effet_real_param->color[0][0],effet_para->effet_real_param->color[0][1],effet_para->effet_real_param->color[0][2]);	
	zoom = (double)effet_para->effet_real_param->vari[1]/100;
	if(zoom > 0.01)
		zoom = 1/zoom;
	else zoom = 100;
	angle = effet_para->effet_real_param->vari[0];
	angle_rad = (M_PI*angle)/180;
	c = cos(angle_rad)*zoom;
	s = sin(angle_rad)*zoom;
	
	cx = effet_para->effet_real_param->coords[0][0];
	cy = effet_para->effet_real_param->coords[0][1];
	x_centre = (int)(((long)xi*cx)/1000L);
	y_centre = (int)(((long)yi*cy)/1000L);
	larg = effet_para->larg_img;
	
	if(effet_para->effet_real_param->switchs&1)	/* mode rapide en asm */
	{	asm_cos = (long)(c*0xffff);
		asm_sin = (long)(s*0xffff);
		if(mosaique)	/* MOSAIQUE */
			asm2_rot(src,dst,xi,yi, asm_cos, asm_sin, larg, 
						x_centre,y_centre);
		else 
			asm_rot(src,dst,xi,yi, asm_cos, asm_sin, larg, 
					x_centre,y_centre,couleur);
	}else
	{	for(y=0;y<yi;y++)
		{	for(x=0;x<xi;x++)
			{	/* MATRICE DE ROTATION */
				xs = x_centre + (int)(c*(x-x_centre) - s*(y-y_centre));
				ys = y_centre + (int)(s*(x-x_centre) + c*(y-y_centre));
				if(mosaique)	/* MOSAIQUE */
				{	if(xs<0)
					{	cx=xs/xi;
						xs = xs - cx*xi;
						xs=xi+xs;
					}else if(xs>xi)
					{	cx=xs/xi;
						xs = xs - cx*xi;
					}
					if(ys<0)
					{	cy=ys/yi;
						ys = ys - cy*yi;
						ys=yi+ys;
					}else if(ys>xi)
					{	cy=ys/yi;
						ys = ys - cy*yi;
					}
					*(dst + x + (long)y*larg) = *(src + xs + (long)ys*larg);
				}else
				{	if(xs<0 || ys<0 || xs>xi || ys>yi)
						*(dst + x + (long)y*larg) = couleur;		/* SI CA SORT : COULEUR CHOISIE */
					else
						*(dst + x + (long)y*larg) = *(src + xs + (long)ys*larg);
				}
			}
		}
	}
	
	return 1;
}

int effet_SPIR(EFFET_PARA *effet_para)
{	int *src,*dst,x,y,xs,ys,x_centre,y_centre,larg;
	int cx,cy,xi,yi,dist_max;
	double angle_max,c,s,dist;
	
	src=effet_para->img_src;
	dst=effet_para->img_dst;
	xi=effet_para->x;
	yi=effet_para->y;
	angle_max = (M_PI*effet_para->effet_real_param->vari[0])/180;
	
	cx = effet_para->effet_real_param->coords[0][0];
	cy = effet_para->effet_real_param->coords[0][1];
	x_centre = (int)(((long)xi*cx)/1000L);
	y_centre = (int)(((long)yi*cy)/1000L);
	dist_max = min(x_centre,min(y_centre,min(xi-x_centre,yi-y_centre)));
	larg = effet_para->larg_img;
	
	for(y=0;y<yi;y++)
	{	for(x=0;x<xi;x++)
		{	/* DISTANCE AU CENTRE */
			cx = x-x_centre;
			cy = y-y_centre;
			dist = sqrt((double)cx*cx + (double)cy*cy);
			if((int)dist>=dist_max)
			{	c=1; s=0; }
			else
			{	dist = dist_max - dist;
				c = cos(angle_max*dist/dist_max);
				s = sin(angle_max*dist/dist_max);
			}
			/* MATRICE DE ROTATION */
			xs = x_centre + (int)(c*cx - s*cy);
			ys = y_centre + (int)(s*cx + c*cy);
			if(xs<0 || ys<0 || xs>xi || ys>yi)
				*(dst + x + (long)y*larg) = 0;
			else
				*(dst + x + (long)y*larg) = *(src + xs + (long)ys*larg);
		}
	}
	
	return 1;
}

int effet_WOBV(EFFET_PARA *effet_para)
{	int col,*ligne,y;
	TIME_SPLINE *spline_gauche,*spline_droite;
	TRAN_REAL_PARAM *param=effet_para->effet_real_param;
	char mode_g,mode_d;	/* 0->fixe, 1->spline lineaire 2->spline addy */
	int decal_g,decal_d,time,decalage;
	long max = effet_para->x/2;
	
	col=conv_col(param->color[0][0],param->color[0][1],param->color[0][2]);
	fill(effet_para->img_dst,effet_para->x,effet_para->y,effet_para->larg_img,col);
	
	if(param->mode_vari[0] == 1)	/* mode spline */
	{	if(param->time[0]==0)		/* la linea */
			mode_g=1;
		else
		{	spline_gauche=find_time_spline(param->time[0]);
			if(spline_gauche==0)
				mode_g=1; else mode_g=2;
		}
	}else 
	{	mode_g=0;
		decal_g = (int)( ((long)param->vari[0] * max)/10000);
	}
	
	if(param->mode_vari[1] == 1)	/* mode spline */
	{	if(param->time[1]==0)		/* la linea */
			mode_d=1;
		else
		{	spline_droite=find_time_spline(param->time[1]);
			if(spline_droite==0)
				mode_d=1; else mode_d=2;
		}
	}else
	{	mode_d=0;
		decal_d = (int)( ((long)param->vari[1] * max)/10000);
	}	
	
	decalage = (int)( ((long)effet_para->y * param->vari[2])/10000);
	
	for(y=0;y<effet_para->y;y++)
	{	time = y + decalage;
		if(time>effet_para->y) time -= effet_para->y;
		if(mode_g == 1)
			decal_g = (int)( ((long)time*max)/effet_para->y);
		else if(mode_g == 2)
			decal_g = (int)spline_comput(-1,spline_gauche,time,effet_para->y,max);
		if(mode_d == 1)
			decal_d = (int)( ((long)time*max)/effet_para->y);
		else if(mode_d == 2)
			decal_d = (int)spline_comput(-1,spline_droite,time,effet_para->y,max);
		
		ligne = effet_para->img_dst + (long)y*effet_para->larg_img;
		ligne += decal_g;
		asm_zoom(effet_para->img_src + (long)y*effet_para->larg_img,ligne,effet_para->x,effet_para->x-decal_g-decal_d);
	}	
	return 1;
}
