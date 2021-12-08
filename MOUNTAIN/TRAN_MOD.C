/********************MOUTAIN*********************/
/*****          MODULES DE TRANSITION       *****/
/*****            Par Vanel R‚mi            *****/
/************************************************/

#include "windform.h"  /* et vdi.h et aes.h */
#include "transit.h"
#include "time_spl.h"
#include "some_fnc.h"
#include "flh_lib.H"
#include "effets1.H"
#include "txtran.H"



TRAN_DESC_PARAM desc_param_zbox =
{	3,0,{TT_AVANCEE,TT_ZBOX_P2,
	TT_ZBOX_P3}, 0, {0,1,1}, {10000, 30, 30}, 
	{-12345,2,2}, 0, {"",""}, {{0,0,0},{0,0,0}},1,{TT_ZBOX_S1,"","",""},
	0,0,{"",""},{{0,0},{0,0}}
};

TRAN_DESC_PARAM desc_param_isqr =
{	2,0,{TT_AVANCEE,TT_CONTOUR_SIZE,
	""}, 0, {0,0,0}, {10000, 20, 0}, 
	{-12345,2,0}, 1, {TT_CONTOUR_COLOR,""}, {{255,0,0},{0,0,0}},0,{"","","",""},
	0,1,{TT_CENTRE,""},{{500,500},{0,0}}
};

TRAN_DESC_PARAM desc_param_slde =
{	1,0,{TT_AVANCEE,"",""}, 0, {0,0,0}, {10000, 0, 0}, 
	{-12345,0,0}, 0, {"",""}, {{255,0,0},{0,0,0}},1,{TT_DEP_HORIZ,"","",""},
	0,0,{"",""},{{0,0},{0,0}}
};

TRAN_DESC_PARAM desc_param_strh =
{	1,0,{TT_AVANCEE,"",""}, 0, {0,0,0}, {10000, 0, 0}, 
	{-12345,0,0}, 0, {"",""}, {{255,0,0},{0,0,0}},1,{TT_DEP_HORIZ,"","",""},
	0,0,{"",""},{{0,0},{0,0}}
};

TRAN_DESC_PARAM desc_param_push =
{	1,0,{TT_AVANCEE,"",""}, 0, {0,0,0}, {10000, 0, 0}, 
	{-12345,0,0}, 0, {"",""}, {{255,0,0},{0,0,0}},1,{TT_DEP_HORIZ,"","",""},
	0,0,{"",""},{{0,0},{0,0}}
};

TRAN_DESC_PARAM desc_param_pshz =
{	1,0,{TT_AVANCEE,"",""}, 0, {0,0,0}, {10000, 0, 0}, 
	{-12345,0,0}, 0, {"",""}, {{255,0,0},{0,0,0}},2,{TT_DEP_HORIZ,TT_A_ECRAS,"",""},
	0,0,{"",""},{{0,0},{0,0}}
};


int tran_ZBOX(TRAN_PARA *adr_para)
{	int avancee,x,y,xb,yb,wb,hb,i,j;
	int max_x,max_x2,max_y,nb_x,nb_y;
	int amin,amax,avancee2;
	avancee = adr_para->tran_real_param->vari[0];
	nb_x =  max(1,adr_para->tran_real_param->vari[1]);
	nb_y =  max(1,adr_para->tran_real_param->vari[2]);
	x = adr_para->x;
	y = adr_para->y;
	max_x = x/nb_x;
	max_y = y/nb_y;
	copy_tc2(adr_para->img_a, adr_para->img_dst, y, x, adr_para->larg_img_dst-x,0);
	for(j=0;j<nb_y;j++)
	{	yb = j*max_y;
		max_x2=max_x;
		if(j==nb_y-1)		/* rectif pour la derniere On comble !*/
			max_y = max_y + y-(max_y*nb_y);
		if(adr_para->tran_real_param->switchs & 1)
		{	/* MODE APPARITION PROGRESSIVE */
			for(i=0;i<nb_x;i++)
			{	if(nb_y!=1)
				{	amin = (int)((long)(i+j*nb_x)*(10000-10000/nb_y)/(nb_x*nb_y));
					amax = amin + 10000/nb_y;
				}else
				{	amin = (int)((long)(i+j*nb_x)*(10000-10000/nb_x)/(nb_x*nb_y));
					amax = amin + 10000/nb_x;
				}
				avancee2 = max(0,avancee-amin);
				avancee2 = min(avancee2,amax-amin);
				xb = i*max_x;
				if(i==nb_x-1)		/* rectif pour la derniere */
					max_x2 = max_x + x-(max_x*nb_x);
				wb = (int)( ((long)max_x2*avancee2)/(amax-amin));
				hb = (int)( ((long)max_y*avancee2)/(amax-amin));
				
				if(wb && hb)
					zoom(adr_para->img_b+xb+(long)x*yb, adr_para->img_dst+(xb+max_x2/2-wb/2)+(long)adr_para->larg_img_dst*(yb+max_y/2-hb/2), max_x2,max_y,x,
						wb,hb,adr_para->larg_img_dst,1024);
			}
		}else
		{
			for(i=0;i<nb_x;i++)
			{	xb = i*max_x;
				if(i==nb_x-1)		/* rectif pour la derniere */
					max_x2 = max_x + x-(max_x*nb_x);
				wb = (int)( ((long)max_x2*avancee)/10000);
				hb = (int)( ((long)max_y*avancee)/10000);

				if(wb && hb)
					zoom(adr_para->img_b+xb+(long)x*yb, adr_para->img_dst+(xb+max_x2/2-wb/2)+(long)adr_para->larg_img_dst*(yb+max_y/2-hb/2), max_x2,max_y,x,
						wb,hb,adr_para->larg_img_dst,1024);
			}
		}
	}
	return 1;
}

int tran_ISQR(TRAN_PARA *adr_para)
{	int centre_x,centre_y,avancee,x,y;
	int cpy_x,cpy_y,cpy_w,cpy_h,epaisseur;
	int couleur;
	
	x=adr_para->x;
	y=adr_para->y;
	copy_tc2(adr_para->img_a, adr_para->img_dst, y, x, adr_para->larg_img_dst-x, 0);

	centre_x = (int)(((long)adr_para->tran_real_param->coords[0][0]*x)/1000);
	centre_y = (int)(((long)adr_para->tran_real_param->coords[0][1]*y)/1000);
	
	avancee = adr_para->tran_real_param->vari[0];
	epaisseur = adr_para->tran_real_param->vari[1];
	couleur = conv_col(adr_para->tran_real_param->color[0][0],adr_para->tran_real_param->color[0][1],adr_para->tran_real_param->color[0][2]);

	cpy_x = (int)(((long)(10000-avancee)*centre_x)/10000);
	cpy_y = (int)(((long)(10000-avancee)*centre_y)/10000);
	cpy_w = (int)(((long)avancee*x)/10000);
	cpy_h = (int)(((long)avancee*y)/10000);
	
	if(cpy_w && cpy_h)
		copy_tc2(adr_para->img_b+cpy_x+(long)cpy_y*x, adr_para->img_dst+cpy_x+cpy_y*adr_para->larg_img_dst,
				 cpy_h, cpy_w,adr_para->larg_img_dst-cpy_w, x-cpy_w);
	
	if(epaisseur)
	{	fill(adr_para->img_dst+cpy_x+cpy_y*adr_para->larg_img_dst,
			cpy_w,epaisseur,adr_para->larg_img_dst,couleur);
		fill(adr_para->img_dst+cpy_x+(cpy_y+cpy_h-epaisseur)*adr_para->larg_img_dst,
			cpy_w,epaisseur,adr_para->larg_img_dst,couleur);
		fill(adr_para->img_dst+cpy_x+cpy_y*adr_para->larg_img_dst,
			epaisseur,cpy_h,adr_para->larg_img_dst,couleur);
		fill(adr_para->img_dst+(cpy_x+cpy_w-epaisseur)+cpy_y*adr_para->larg_img_dst,
			epaisseur,cpy_h,adr_para->larg_img_dst,couleur);
	}
	return 1;
}

int tran_SLDE(TRAN_PARA *adr_para)
{	int x,y, largeur,avancee, debut_b,hauteur;
	
	x=adr_para->x;
	y=adr_para->y;
	avancee = adr_para->tran_real_param->vari[0];

	copy_tc2(adr_para->img_a, adr_para->img_dst, y, x, adr_para->larg_img_dst-x, 0);
	
	if(adr_para->tran_real_param->switchs & 1)
	{	
		largeur = (int)( ((long)avancee*x)/10000L);
		debut_b = x-largeur;
		if(largeur)
			copy_tc2(adr_para->img_b+debut_b, adr_para->img_dst,
					 y, largeur, adr_para->larg_img_dst-largeur, x-largeur);
	}else
	{	hauteur = (int)( ((long)avancee*y)/10000L);
		debut_b = y-hauteur;
		if(hauteur)
			copy_tc2(adr_para->img_b+(long)debut_b*x, adr_para->img_dst,
					 hauteur, x, adr_para->larg_img_dst-x, 0);
	}
	return 1;
}

int tran_STRH(TRAN_PARA *adr_para)
{	int x,y, largeur,avancee, hauteur;
	
	x=adr_para->x;
	y=adr_para->y;
	avancee = adr_para->tran_real_param->vari[0];

	copy_tc2(adr_para->img_a, adr_para->img_dst, y, x, adr_para->larg_img_dst-x, 0);
	
	if(adr_para->tran_real_param->switchs & 1)
	{	
		largeur = (int)( ((long)avancee*x)/10000L);
		if(largeur)
			zoom(adr_para->img_b, adr_para->img_dst,x,y,x,largeur,y,adr_para->larg_img_dst,1024);
	}else
	{	hauteur = (int)( ((long)avancee*y)/10000L);
		if(hauteur)
			zoom(adr_para->img_b, adr_para->img_dst,x,y,x,x,hauteur,adr_para->larg_img_dst,1024);
	}
	return 1;
}

int tran_PUSH(TRAN_PARA *adr_para)
{	int x,y, largeur,avancee, debut_b,hauteur;
	
	x=adr_para->x;
	y=adr_para->y;
	avancee = adr_para->tran_real_param->vari[0];

	
	if(adr_para->tran_real_param->switchs & 1)
	{	
		largeur = (int)( ((long)avancee*x)/10000L);
		debut_b = x-largeur;
		
		if(debut_b)
			copy_tc2(adr_para->img_a, adr_para->img_dst+largeur, y,
				 debut_b, adr_para->larg_img_dst-debut_b, x-debut_b);
		
		if(largeur)
			copy_tc2(adr_para->img_b+debut_b, adr_para->img_dst,
					 y, largeur, adr_para->larg_img_dst-largeur, x-largeur);
	}else
	{	hauteur = (int)( ((long)avancee*y)/10000L);
		debut_b = y-hauteur;

		if(debut_b)
			copy_tc2(adr_para->img_a, adr_para->img_dst+(long)hauteur*adr_para->larg_img_dst, debut_b,
				 x, adr_para->larg_img_dst-x, 0);
		if(hauteur)
			copy_tc2(adr_para->img_b+(long)debut_b*x, adr_para->img_dst,
					 hauteur, x, adr_para->larg_img_dst-x, 0);
	}
	return 1;
}

int tran_PSHZ(TRAN_PARA *adr_para)
{	int x,y, largeur,avancee, debut_b,hauteur;
	
	x=adr_para->x;
	y=adr_para->y;
	avancee = adr_para->tran_real_param->vari[0];

	
	if(adr_para->tran_real_param->switchs & 1)
	{	
		largeur = (int)( ((long)avancee*x)/10000L);
		debut_b = x-largeur;
		
		if(adr_para->tran_real_param->switchs & 2)
		{	if(largeur)
				copy_tc2(adr_para->img_b+debut_b, adr_para->img_dst,
					 y, largeur, adr_para->larg_img_dst-largeur, x-largeur);
		
			if(debut_b)
				zoom(adr_para->img_a, adr_para->img_dst + largeur,x,y,x,debut_b,y,adr_para->larg_img_dst,1024);
		}else
		{	if(debut_b)
				copy_tc2(adr_para->img_a, adr_para->img_dst+largeur, y,
					 debut_b, adr_para->larg_img_dst-debut_b, x-debut_b);
		
			if(largeur)
				zoom(adr_para->img_b, adr_para->img_dst,x,y,x,largeur,y,adr_para->larg_img_dst,1024);
		}
	}else
	{	hauteur = (int)( ((long)avancee*y)/10000L);
		debut_b = y-hauteur;

		if(adr_para->tran_real_param->switchs & 2)
		{	if(hauteur)
				copy_tc2(adr_para->img_b+(long)debut_b*x, adr_para->img_dst,
					 hauteur, x, adr_para->larg_img_dst-x, 0);
		
			if(debut_b)
				zoom(adr_para->img_a, adr_para->img_dst + (long)hauteur*adr_para->larg_img_dst,x,y,x,x,debut_b,adr_para->larg_img_dst,1024);
		}else
		{	if(debut_b)
				copy_tc2(adr_para->img_a, adr_para->img_dst+(long)hauteur*adr_para->larg_img_dst, debut_b,
					 x, adr_para->larg_img_dst-x, 0);
			if(hauteur)
				zoom(adr_para->img_b, adr_para->img_dst,x,y,x,x,hauteur,adr_para->larg_img_dst,1024);
		}
	}
	return 1;
}
