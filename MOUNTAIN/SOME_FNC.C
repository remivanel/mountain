/********************MOUTAIN********************/
/*****       ROUTINES COMPLEMENTAIRES       ****/
/*****             Version 0.1              ****/
/*****            Par Vanel R‚mi            ****/
/*****         Premier code : 1/5/97        ****/
/***********************************************/

#include <tos.h>
#include <string.h>
#include <ctype.h>
#include <ldg.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include "some_fnc.h"
#include "gest_flh.h"
#include "mountain.h"
#include "preview.h"
#include "visu.h"
#include "scrn_ldg.h"

void nova_cpyfm(CPY_PARAM *cpy);
void nova2_cpyfm(CPY_PARAM *cpy);
void matrix_cpyfm(CPY_PARAM *cpy);
void xrvb_cpyfm(CPY_PARAM *cpy);
void bvr_cpyfm(CPY_PARAM *cpy);
void rvb_cpyfm(CPY_PARAM *cpy);
void xbvr_cpyfm(CPY_PARAM *cpy);
void trm_16_cpyfm(CPY_PARAM *cpy);
void trm_mono(CPY_PARAM *cpy);
void	get_bkgr(int of_x, int of_y, int of_w, int of_h);
void	put_bkgr(int of_x, int of_y, int of_w, int of_h);
void wait_mk(void);

extern WINDFORM_VAR alr_var;

#define USR_INDICATOR	0x800
#define USR_ACTIVATOR	0x1800


/* gestion des tache de fond */
int nb_tache=0;
int list_tache[MAX_TACHE];
int current_tache = FALSE;

/* Screen LDG */
long cdecl (*screen_detect)(long vdihandle, char *answer);
long cdecl (*screen_init)(long vdihandle, videoinf *display, char *type_ecran, sortievideo *more_infos, long flag);
static void cdecl (*fix_palette)(sortievideo *more_infos, long mode, long vdihandle)=NULL;
void cdecl (*screen_display)(long vdihandle, videoinf *display);
videoinf mydisplay;
sortievideo myinfos;
LDG *screenldg;
int ldg_install_ok = FALSE;
int ldg_screen_ok = FALSE;
 

extern int use_nvdi;
extern int gemdos_version;
extern int int_rel;
extern int use_n_alert;
extern int carte;	/* carte graphique */
extern int force_vdi_use;	/* passe par un buffer intermediaire */
GRECT clip;

long mem_free(void)
{	long mem;
	
	
	if(gemdos_version >= 0x1900)		/* Gemdos TT ou plus */
	{	mem =  (long)Mxalloc(-1,0); 	/* ST Ram libre */
		mem += (long)Mxalloc(-1,1);	/* TT Ram libre */
	}else
		mem =  (long)Malloc(-1L);		/* ST Ram libre */

	return mem;
}


char check_key(char *chm, char *nom, int x, int y, int nb_img)
{	int handle;
	KEY_HEAD key;
	
	handle = (int)Fopen(chm,FO_READ);
	if(handle < 0)
		return 0;
	Fread(handle,SIZE_HKEY,&key);
	Fclose(handle);
	if(strcmp(key.nom_anim,nom))
		return 0;
	if(key.x != x)
		return 0;
	if(key.y != y)
		return 0;
	if(key.nb_img_in_flh != nb_img)
		return 0;
	return 1;
}
	
/* FIND PATH : trouve le path dans un chemin*/
void find_path(char *str)
{	size_t bcl;

	bcl = strlen(str);

	do
	{	if(str[bcl--] == '\\')
		{	str[bcl+2] = 0; bcl = 0; }
	}while(bcl>0);
		
}

/* FIND NAME : trouve le nom dans un chemin et vire l'extention*/
void find_name(char *str, char *dest)
{	size_t bcl;

	bcl = strlen(str);

	do
	{	if(str[bcl--] == '\\')
		{	str = &str[bcl+2]; bcl = 0; }
	}while(bcl>0);
		
	while(*str != 0 && *str != '.')
		*dest++ = *str++;

	*dest = 0;
	
}

void vire_ext(char *str)		/* eleve l'extention */
{	size_t bcl,len;

	bcl = len = strlen(str);
	
	do
	{	if(str[--bcl] == '.')
		{	str[bcl]=0;
			bcl=0;	/* pour sortir */
		}
	}while(bcl>len-4);
}
	
/* FIND NAME EXT : trouve le nom dans un chemin*/
void find_name_ext(char *str, char *dest)
{	size_t bcl;

	bcl = strlen(str);

	do
	{	if(str[bcl--] == '\\')
		{	str = &str[bcl+2]; bcl = 0; }
	}while(bcl>0);
	
	strcpy(dest,str);
	
}

void set_clip(int clip_flag, GRECT *area)
{	int pxy[4];
	GRECT screen;
	
	if(clip_flag)
	{	screen.g_x = x_desk;
		screen.g_y = y_desk;
		screen.g_w = w_desk;
		screen.g_h = h_desk;
		rc_intersect(area,&screen);

		clip.g_x = screen.g_x;
		clip.g_y = screen.g_y;
		clip.g_w = screen.g_w;
		clip.g_h = screen.g_h;
	
		pxy[0] = screen.g_x;
		pxy[1] = screen.g_y;
		pxy[2] = screen.g_w + screen.g_x - 1;
		pxy[3] = screen.g_h + screen.g_y - 1;
	}else
	{	clip.g_x = x_desk;
		clip.g_y = y_desk;
		clip.g_w = w_desk;
		clip.g_h = h_desk;
	}
	vs_clip(vdi_handle, clip_flag, pxy);
}

void ligne(int x, int y,int w, int h,int color)
{	int xy[4];

	vsl_type(vdi_handle,1);
	vsl_color(vdi_handle,color);
	xy[0]=x; xy[1]=y; 
	xy[2]=x+w-1; xy[3]=y+h-1; 
	v_pline(vdi_handle,2,xy);
}

void rect_3d(int x, int y,int w, int h)
{	w -= 1;
	h -= 1;
	if(int_rel)
	{	ligne(x,   y,   w ,1, WHITE);			/* H */
		ligne(x,   y,   1, h, WHITE);			/* G */
		ligne(x+1, y+h, w, 1, color_3d2);	/* B */
		ligne(x+w, y+1, 1, h, color_3d2);	/* D */
	}else
	{	ligne(x,   y, w+1 ,1, BLACK);			/* H */
		ligne(x,   y,   1, h, BLACK);			/* G */
		ligne(x, y+h, w+1, 1, BLACK);			/* B */
		ligne(x+w, y,   1, h+1, BLACK);		/* D */
	}
}

void rect(int x, int y,int w, int h, int color)
{	w -= 1;
	h -= 1;
	ligne(x,   y, w+1 ,1, color);			/* H */
	ligne(x,   y,   1, h, color);			/* G */
	ligne(x, y+h, w+1, 1, color);			/* B */
	ligne(x+w, y,   1, h+1, color);		/* D */
	
}

void rect_full(int x, int y,int w, int h,int color)
{	int xy[4];

	xy[0]=x; xy[1]=y; 
	xy[2]=x+w-1; xy[3]=y+h-1; 

	vsf_color(vdi_handle, color);
	vr_recfl(vdi_handle, xy);
}

void text_vert(int x, int y, const char *str)
{	int xy[8],dum;
	char car[2] = {0,0};
	size_t len;
	
	car[0] = str[0];
	
	vswr_mode(vdi_handle,MD_TRANS);
	vqt_extent(vdi_handle,car,xy);
	
	len = strlen(str);
	for(dum = 0;dum<len;dum++)
	{	car[0] = str[dum];
		text_center_x(x,y,car);
		y += xy[5]-2;
	}
}	
void text_center_x(int x, int y, char *str)
{	int xy[8];
	
	vqt_extent(vdi_handle,str,xy);
	v_gtext(vdi_handle,x - xy[2]/2, y + xy[5]+2, str);
}

void text_center(int x, int y, char *str)
{	int xy[8];
	
	vqt_extent(vdi_handle,str,xy);
	v_gtext(vdi_handle,x - xy[2]/2, y + xy[7]/2-3, str);
}

void screen_copy(int x_src, int y_src, int w_src, int h_src, int x_dst, int y_dst)
{	MFDB nul={0};
	int pxy[8];
	
	/* zone source */
	pxy[0] = x_src;
	pxy[1] = y_src;
	pxy[2] = pxy[0] + w_src - 1;
	pxy[3] = pxy[1] + h_src - 1;
	/* zone dest */
	pxy[4] = x_dst;
	pxy[5] = y_dst;
	pxy[6] = pxy[4] + w_src - 1;
	pxy[7] = pxy[5] + h_src - 1;
	
	vro_cpyfm (vdi_handle, S_ONLY, pxy, &nul, &nul);
}


void tnt_cpyfm(int pxy[8], MFDB *src)
{	MFDB nul = {0},src2;
	CPY_PARAM cpy;
	GRECT dst;
	int *src_intermediaire,dst_mod16,col_nb[2]={1,0};
	
	if(carte == 0)		/* Falcon */
		vro_cpyfm (vdi_handle, S_ONLY, pxy, src, &nul);
	else if(carte == 1 || carte == 2 || carte == 3)	/* nova 16 bits */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{		/* zone dest */
			pxy[4] = dst.g_x;
			pxy[5] = dst.g_y;
			pxy[6] = pxy[4] + dst.g_w - 1;
			pxy[7] = pxy[5] + dst.g_h - 1;
			
			if(force_vdi_use)		/* ON PASSE PAR UN BUFFER INTERMEDIAIRE */
			{	dst_mod16 = (dst.g_w + 15)/16;
				dst_mod16 *= 16;
				src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)*2L+8192L);	/* securitee */
				
				if(src_intermediaire)
				{	cpy.x_dst = dst_mod16-1;
					cpy.x_src = src->fd_w-1;
					cpy.y = pxy[3] - pxy[1]+1;
					cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
					cpy.dst = src_intermediaire;
					cpy.larg_cpy =  min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
					cpy.pos_x_dst = 0;
				
					if(carte == 1)
						nova_cpyfm(&cpy);
					else if(carte == 2)
						matrix_cpyfm(&cpy);
					else
						nova2_cpyfm(&cpy);
	
					src2.fd_addr = src_intermediaire; 
					src2.fd_w = dst_mod16;
					src2.fd_h = dst.g_h;
					src2.fd_wdwidth = dst_mod16/16;
					src2.fd_stand = 0;
					src2.fd_nplanes = 16;
				
					pxy[0] = 0;
					pxy[1] = 0;
					pxy[2] = dst.g_w-1;
					pxy[3] = dst.g_h-1;
					
					vro_cpyfm (vdi_handle, S_ONLY, pxy, &src2, &nul);
					Mfree(src_intermediaire);
				}
			}else						/* AFFICHAGE DIRECT A L'ECRAN */
			{	

				cpy.x_dst = x_desk+w_desk-1;
				cpy.x_src = src->fd_w-1;
				cpy.y = pxy[3] - pxy[1];
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
				cpy.dst = (int *)Logbase() + pxy[4] + (long)pxy[5]*(cpy.x_dst+1);
				cpy.larg_cpy = min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
				if(carte == 1)
					nova_cpyfm(&cpy);
				else if(carte == 2)
					matrix_cpyfm(&cpy);
				else
					nova2_cpyfm(&cpy);
			}
		}
	}else if(carte == 4)	/* 24 bits BVR */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{		/* zone dest */
			pxy[4] = dst.g_x;
			pxy[5] = dst.g_y;
			pxy[6] = pxy[4] + dst.g_w - 1;
			pxy[7] = pxy[5] + dst.g_h - 1;

			if(force_vdi_use)		/* ON PASSE PAR UN BUFFER INTERMEDIAIRE */
			{	dst_mod16 = (dst.g_w + 15)/16;
				dst_mod16 *= 16;
				src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)*3L+8192L); /* secu */
				
				if(src_intermediaire)
				{	cpy.x_dst = dst_mod16-1;
					cpy.x_src = src->fd_w-1;
					cpy.y = pxy[3] - pxy[1]+1;
					cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1); /* la src est 16 bits */
					cpy.dst = src_intermediaire;
					cpy.larg_cpy =  min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
					cpy.pos_x_dst = 0;
				
					bvr_cpyfm(&cpy);
	
					src2.fd_addr = src_intermediaire; 
					src2.fd_w = dst_mod16;
					src2.fd_h = dst.g_h;
					src2.fd_wdwidth = dst_mod16/16;
					src2.fd_stand = 0;
					src2.fd_nplanes = 24;
				
					pxy[0] = 0;
					pxy[1] = 0;
					pxy[2] = dst.g_w-1;
					pxy[3] = dst.g_h-1;
					
					vro_cpyfm (vdi_handle, S_ONLY, pxy, &src2, &nul);
					Mfree(src_intermediaire);
				}
			}else						/* AFFICHAGE DIRECT A L'ECRAN */
			{	
				cpy.x_dst = x_desk+w_desk-1;
				cpy.x_src = src->fd_w-1;
				cpy.y = pxy[3] - pxy[1];
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
				cpy.dst = (char *)Logbase() + pxy[4]*3 + (long)pxy[5]*(cpy.x_dst+1)*3;
				cpy.larg_cpy = min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
			
				bvr_cpyfm(&cpy);
			}
		}
	}else if(carte == 5 || carte == 6)	/* 32 bits XBVR et XRVB */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{		/* zone dest */
			pxy[4] = dst.g_x;
			pxy[5] = dst.g_y;
			pxy[6] = pxy[4] + dst.g_w - 1;
			pxy[7] = pxy[5] + dst.g_h - 1;

			if(force_vdi_use)		/* ON PASSE PAR UN BUFFER INTERMEDIAIRE */
			{	dst_mod16 = (dst.g_w + 15)/16;
				dst_mod16 *= 16;
				src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)*4L+8192L); /* secu */
				
				if(src_intermediaire)
				{	cpy.x_dst = dst_mod16-1;
					cpy.x_src = src->fd_w-1;
					cpy.y = pxy[3] - pxy[1]+1;
					cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
					cpy.dst = src_intermediaire;
					cpy.larg_cpy =  min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
					cpy.pos_x_dst = 0;
				
					if(carte == 5)
						xbvr_cpyfm(&cpy);		/* nova */
					else
						xrvb_cpyfm(&cpy);    /* matrix/mmac */
	
					src2.fd_addr = src_intermediaire; 
					src2.fd_w = dst_mod16;
					src2.fd_h = dst.g_h;
					src2.fd_wdwidth = dst_mod16/16;
					src2.fd_stand = 0;
					src2.fd_nplanes = 32;
				
					pxy[0] = 0;
					pxy[1] = 0;
					pxy[2] = dst.g_w-1;
					pxy[3] = dst.g_h-1;
					
					vro_cpyfm (vdi_handle, S_ONLY, pxy, &src2, &nul);
					Mfree(src_intermediaire);
				}
			}else						/* AFFICHAGE DIRECT A L'ECRAN */
			{	
				cpy.x_dst = x_desk+w_desk-1;
				cpy.x_src = src->fd_w-1;
				cpy.y = pxy[3] - pxy[1];
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
				cpy.dst = (char *)Logbase() + pxy[4]*4 + (long)pxy[5]*(cpy.x_dst+1)*4;
				cpy.larg_cpy = min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
			
				if(carte == 5)
					xbvr_cpyfm(&cpy);		/* nova */
				else
					xrvb_cpyfm(&cpy);    /* matrix/mmac */
			}
		}
	}else if(carte == 7)	/* ecran 16 couleurs */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{	dst_mod16 = (dst.g_w + 15)/16;
			dst_mod16 *= 16;
			src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)/2L);
			if(src_intermediaire)
			{		/* zone dest */
				pxy[4] = dst.g_x;
				pxy[5] = dst.g_y;
				pxy[6] = pxy[4] + dst.g_w - 1;
				pxy[7] = pxy[5] + dst.g_h - 1;
				
				cpy.x_dst = dst_mod16;
				cpy.x_src = src->fd_w-1;
				cpy.y = pxy[3] - pxy[1]+1;
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1);
				cpy.dst = src_intermediaire;
				cpy.larg_cpy = dst_mod16-1;
/*				cpy.larg_cpy = min(pxy[2] - pxy[0], pxy[6] - pxy[4]); */
				cpy.pos_x_dst = 0;
				trm_16_cpyfm(&cpy);
				
				src2.fd_addr = src_intermediaire; 
				src2.fd_w = dst_mod16;
				src2.fd_h = dst.g_h;
				src2.fd_wdwidth = dst_mod16/16;
				src2.fd_stand = 0;
				src2.fd_nplanes = 4;
				
				pxy[0] = 0;
				pxy[1] = 0;
				pxy[2] = dst.g_w-1;
				pxy[3] = dst.g_h-1;
				
				vro_cpyfm (vdi_handle, S_ONLY, pxy, &src2, &nul);
				Mfree(src_intermediaire);
			}
		}
	}else if(carte == 8)	/* Monochrome pass partout */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{	dst_mod16 = (dst.g_w + 15)/16;
			dst_mod16 *= 16;
			src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)/4L);
			if(src_intermediaire)
			{		/* zone dest */
				pxy[4] = dst.g_x;
				pxy[5] = dst.g_y;
				pxy[6] = pxy[4] + dst.g_w - 1;
				pxy[7] = pxy[5] + dst.g_h - 1;
				
				cpy.x_dst = dst_mod16;
				cpy.x_src = src->fd_w;
				cpy.y = pxy[3] - pxy[1]+1;
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*cpy.x_src;
				cpy.dst = src_intermediaire;
				cpy.larg_cpy = dst_mod16;
/*				cpy.larg_cpy = min(pxy[2] - pxy[0], pxy[6] - pxy[4]); */
				cpy.pos_x_dst = 0;
				trm_mono(&cpy);
				
				src2.fd_addr = src_intermediaire; 
				src2.fd_w = dst_mod16;
				src2.fd_h = dst.g_h;
				src2.fd_wdwidth = dst_mod16/16;
				src2.fd_stand = 0;
				src2.fd_nplanes = 1;
				
				pxy[0] = 0;
				pxy[1] = 0;
				pxy[2] = dst.g_w-1;
				pxy[3] = dst.g_h-1;
				
				vrt_cpyfm (vdi_handle, 1, pxy, &src2, &Screen_MFDB,col_nb);
				Mfree(src_intermediaire);
			}
		}
	}else if(carte == 9)	/* Screen LDG */
	{	dst.g_x = pxy[4];
		dst.g_y = pxy[5];
		dst.g_w = pxy[6] - pxy[4] + 1;
		dst.g_h = pxy[7] - pxy[5] + 1;
		if(rc_intersect(&clip, &dst))		/* gestion du cliping */
		{		/* zone dest */
			pxy[4] = dst.g_x;
			pxy[5] = dst.g_y;
			pxy[6] = pxy[4] + dst.g_w - 1;
			pxy[7] = pxy[5] + dst.g_h - 1;

			/* ON PASSE PAR UN BUFFER INTERMEDIAIRE */
			dst_mod16 = (dst.g_w + 15)/16;
			dst_mod16 *= 16;
			src_intermediaire = Malloc(((long)dst_mod16*dst.g_h)*3L+8192L); /* secu */
				
			if(src_intermediaire)
			{	cpy.x_dst = dst_mod16-1;
				cpy.x_src = src->fd_w-1;
				cpy.y = pxy[3] - pxy[1]+1;
				cpy.src = (int *)src->fd_addr + pxy[0] + (long)pxy[1]*(cpy.x_src+1); /* la src est 16 bits */
				cpy.dst = src_intermediaire;
				cpy.larg_cpy =  min(pxy[2] - pxy[0], pxy[6] - pxy[4]);
				cpy.pos_x_dst = 0;
				
				rvb_cpyfm(&cpy);
			
					/* addresse image */
	       	mydisplay.add_RGB=(long)src_intermediaire;
			   	/* on renseigne sur l'image source  */
      	 	mydisplay.largeur_RGB=dst_mod16;
	       	mydisplay.hauteur_RGB=pxy[3] - pxy[1]+1;
      	 		/* on veut l'image sur l'‚cran */
	       	mydisplay.position_X=pxy[4]; 
	       	mydisplay.position_Y=pxy[5];
       			/* … partir du coin haut gauche de l'image */
	       	mydisplay.X_voulu=0;
   	    	mydisplay.Y_voulu=0;
	       		/* sur toute la largeur et hauteur de l'image (ne 
   		   	 	peut depasser cette taille)*/
	       	mydisplay.largeur_voulue=dst.g_w;
   	    	mydisplay.hauteur_voulue=dst.g_h;
      	 	screen_display(vdi_handle, &mydisplay); /* affichage */
				Mfree(src_intermediaire);
      	}
      }
	}	
}

int super_popup(WINDFORM_VAR *ptr_var,int objet,int index,int old,char *str,int taille_str,int nb_str)
{	int i,old_y,ay, ah,nb;
	int deb_view=0;
	OBJECT *adr_pop;
	OBJECT *adr_form;
	int x, y, w, h, etat;
	int xm, ym, km, dummy, sortie = FALSE,redraw;
	int event, old_opt = -1, option = 0;
	
	wind_update(BEG_MCTRL);				/* Bloquer menu */
	
	rsrc_gaddr(R_TREE, index, &adr_pop);/* Adresse formulaire pop-up */
	adr_form = ptr_var->adr_form;
		
	objc_offset(adr_form, objet, &x, &y);/* Position bouton cliqu‚ */
	y += (adr_form[objet].ob_height);	/* Se placer juste dessous par d‚faut */
	adr_pop->ob_x = x;					/* Positionner le */
	adr_pop->ob_y = y;					/* formulaire pop_up. */
	w = adr_pop->ob_width;				/* Prendre ses dimensions: */
	h = adr_pop->ob_height;
	
	if(old)
	{	if(old >= TXT1+10)
			deb_view = min(old-1,nb_str-10);

		x = adr_pop->ob_x = x - adr_pop[old-deb_view].ob_x;
		y = adr_pop->ob_y = y - adr_pop[old-deb_view].ob_y - adr_form[objet].ob_height;
		adr_pop[old-deb_view].ob_state |= CHECKED;	/* On "Checke" l'option */
	}
	
	/* Si on sort du bureau, d‚caler le pop-up avec une marge de 5 pixels */
	if (x + w > x_desk + w_desk - 5)
		x = adr_pop->ob_x = w_desk + x_desk - w - 5;
	if (x < x_desk + 5)
		x = adr_pop->ob_x = x_desk + 5;
	if (y + h > y_desk + h_desk - 5)
		y = adr_pop->ob_y = h_desk + y_desk - h - 5;
	if (y < y_desk + 5)
		y = adr_pop->ob_y = y_desk + 5;

	get_bkgr(x, y, w, h);	/* Sauver l'image de fond */
		
	for(i=0;i<10;i++)			/* vide les champs */
		adr_pop[TXT1+i].ob_spec.tedinfo->te_ptext[2] = 0;
	
	for(i=0; i<min(10,nb_str) ;i++)
		strpcpy_force(adr_pop[TXT1+i].ob_spec.tedinfo->te_ptext+2, str + taille_str * (deb_view + i),33, ' ' );
	
	
	/* met la taille de d'ass proportionnel */
	adr_pop[CASS].ob_height = (10 * adr_pop[CFON].ob_height) / max(nb_str,10);
	adr_pop[CASS].ob_height = max(adr_pop[CASS].ob_height,16); /*pas trop petit !! */
	
	/* met l'ass au bon endroit */
	if(nb_str <= 10)
		adr_pop[CASS].ob_y = 0;
	else
		adr_pop[CASS].ob_y = (deb_view *(adr_pop[CFON].ob_height - adr_pop[CASS].ob_height )) / (nb_str-10);
	
		/* Dessiner le pop-up : */
	objc_draw(adr_pop, ROOT, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
	
	/* ay base de l'ass */
	ay = y + adr_pop[CFON].ob_y;
	/* ah haut de l'ass */
	ah = adr_pop[CASS].ob_height;
	
	wait_mk();				/* NEW au 29/11/1996, J delavoix	*/
	do		/* BOUCLE PRINCIPALE DE GESTION DU POP-UP */
	{
		event = evnt_multi(MU_BUTTON | MU_TIMER,
							1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* buffer	*/				0L, 40, 0, &xm, &ym, &km, &dummy, &dummy, &dummy);
		if (event & MU_BUTTON)
		{	redraw=FALSE;
			option = objc_find(adr_pop, ROOT, MAX_DEPTH, xm, ym);
			if(option == CUP)
			{	if(deb_view > 0)
				{	adr_pop[old-deb_view].ob_state &= ~CHECKED;
					deb_view--;
					redraw=TRUE;
				}
			}else if(option == CDOWN)
			{	if(deb_view < (nb_str - 10))
				{	adr_pop[old-deb_view].ob_state &= ~CHECKED;
					deb_view++;
					redraw=TRUE;					
				}
			}else if(option == CFON)
			{	adr_pop[old-deb_view].ob_state &= ~CHECKED;	
				if( (ym - y - adr_pop[CFON].ob_y) < adr_pop[CASS].ob_y)
					deb_view = max(0,deb_view-5);
				else
					deb_view = min(nb_str - 10,deb_view+5);
				redraw=TRUE;					
			}else if(option == CASS && nb_str>10)
			{	old_y = deb_view;
				graf_mouse (FLAT_HAND, 0);
				do  
				{	/* prise des coordon‚es de la souris : */
					graf_mkstate(&dummy, &ym, &km,&dummy); 
					nb = ym - ay - ah/2;
					if (nb <  0 )
						nb = 0;
					if (nb > adr_pop[CFON].ob_height - ah )
						nb = adr_pop[CFON].ob_height - ah;
					
					deb_view = ((nb_str-10)*nb) / (adr_pop[CFON].ob_height - ah);

					adr_pop[CASS].ob_y = (deb_view *(adr_pop[CFON].ob_height - adr_pop[CASS].ob_height )) / (nb_str-10);
					
					if(deb_view != old_y)
					{	
						adr_pop[old-old_y].ob_state &= ~CHECKED;

						old_y = deb_view;
						/* redraw */
						
						if(old-deb_view >= TXT1	&& old-deb_view < TXT1+10)
							adr_pop[old-deb_view].ob_state |= CHECKED;	/* On "Checke" l'option */
						
						for(i=0;i<10;i++)
						{	strpcpy_force(adr_pop[TXT1+i].ob_spec.tedinfo->te_ptext+2, str + taille_str * (deb_view + i),33, ' ' );
							objc_draw(adr_pop, TXT1+i, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
						}
						/* Dessiner le pop-up : */
						objc_draw(adr_pop, CUP, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
						objc_draw(adr_pop, CDOWN, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
						objc_draw(adr_pop, CFON, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
					}
				}while (km !=0);
				graf_mouse (ARROW, 0);
				
			}else
			{	
				while (km)	/* attend relƒchement bouton souris. */
					graf_mkstate(&dummy, &dummy, &km, &dummy);
				sortie = TRUE;	/* On peut sortir, puisqu'on a cliqu‚ */
			}
			if(redraw)
			{	
				if(old-deb_view >= TXT1	&& old-deb_view < TXT1+10)
					adr_pop[old-deb_view].ob_state |= CHECKED;	/* On "Checke" l'option */

				for(i=0;i<10;i++)
				{	strpcpy_force(adr_pop[TXT1+i].ob_spec.tedinfo->te_ptext+2, str + taille_str * (deb_view + i),33, ' ' );
					objc_draw(adr_pop, TXT1+i, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
				}
	
					/* met l'ass au bon endroit */
				if(nb_str <= 10)
					adr_pop[CASS].ob_y = 0;
				else
					adr_pop[CASS].ob_y = (deb_view *(adr_pop[CFON].ob_height - adr_pop[CASS].ob_height )) / (nb_str-10);
					
						/* Dessiner le pop-up : */
				objc_draw(adr_pop, CUP, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
				objc_draw(adr_pop, CDOWN, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
				objc_draw(adr_pop, CFON, MAX_DEPTH, x - 3, y - 3, w + 6, h + 6);
			}

		}
		else if (event == MU_TIMER)
		{
/*			graf_mkstate(&xm, &ym, &dummy, &dummy);*/

/* Demander l'option de pop-up … cette position : */
			option = objc_find(adr_pop, ROOT, MAX_DEPTH, xm, ym);
			
			if(option < TXT1 || option >= TXT1+min(10,nb_str))
				option = -1;

			if (old_opt != option)	/* Si l'option a chang‚ */
			{
				if (old_opt > ROOT)	/* On d‚s‚lectionne l'ancienne */
				{
					etat = adr_pop[old_opt].ob_state & ~SELECTED;
					objc_change(adr_pop, old_opt, 0, x, y, w, h, etat, TRUE);
				}
		/* Si l'on est bien dans le Pop-up et l'option non DISABLED : */
					if (option > ROOT && (! (adr_pop[option].ob_state & DISABLED)))
				{
				/* On la s‚lectionne  : */
					etat = adr_pop[option].ob_state | SELECTED;
					objc_change(adr_pop, option, 0, x, y, w, h, etat, TRUE);
				}
				old_opt = option; /* Actualise l'option. */
			}
		}
	} while (sortie == FALSE);

	put_bkgr(x, y, w, h);				/* Restaurer le fond */
	adr_pop[old-deb_view].ob_state &= ~CHECKED;	/* D‚-checker l'option de d‚part */

	if (old_opt >= TXT1)
	{
		if (! (adr_pop[old_opt].ob_state & DISABLED))
		{
			etat = adr_pop[old_opt].ob_state &= ~SELECTED;
			objc_change(adr_pop, old_opt, 0, x, y, w, h, etat, FALSE);
			if((ptr_var->adr_form[objet].ob_type & 0xff) == G_TEXT || (ptr_var->adr_form[objet].ob_type & 0xff) == G_BOXTEXT)
				strcpy(ptr_var->adr_form[objet].ob_spec.tedinfo->te_ptext,str + (old_opt+deb_view-1) * taille_str );
			else
				strcpy(ptr_var->adr_form[objet].ob_spec.free_string,str + (old_opt+deb_view-1) * taille_str );
		}
		else
			old_opt = -1;
	}
	
	wf_change(ptr_var, objet, NORMAL, TRUE);
	wind_update(END_MCTRL);	 /* D‚bloquer menu */

	if (old_opt != -1)
		return old_opt+deb_view;
	return old;
	
}

char *strpcpy_force(char *dest, char *start, int len, char fill)
{
	char *ptr_dest = dest;
	int count=0;
	
	while (count < len && (*(ptr_dest)++ = *(start++)) != 0) count++;
	
	ptr_dest--;
	
	while (count < len ) 
	{	*ptr_dest++ = fill;
		count++;
	}
	
	*ptr_dest = '\0';
	return dest;
}

int super_alert(int defaut, int icone, const char *str)
{	int nb_txt, nb_bout,choix=0,envt;
	char *sep,*sep2,*texte=(char *)str;
	int x, y, w, h,len[3],tot_len_b,tot_len_t=0,entre_bouton;
	int h_bout;
	char chaine[120], *rac,asc;
	char clav[3]={0,0,0};
	OBJECT *adr;
	int mx,my,mk,dummy;
	int buff[8], key_s, key;
	
	if(!use_n_alert)		/* old alert */
	{	sprintf(chaine,"[%d]",icone);
		rac = chaine + 3;
		dummy=0;
		while(str[dummy] != ']') *rac++ = str[dummy++];
		*rac++ = str[dummy++];
		*rac++ = str[dummy];
		while(str[++dummy] != ']')
		{	if(str[dummy] != '[')
		 		*rac++ = str[dummy];
		}
		*rac++ = str[dummy++];
		*rac++ = str[dummy++];
		return(form_alert(defaut,chaine));
	}
		
	adr = alr_var.adr_form;
	
	wind_update(BEG_MCTRL);				/* Bloquer menu */

	/* init */
	alr_var.adr_form[POINT].ob_flags = HIDETREE;
	alr_var.adr_form[INTERRO].ob_flags = HIDETREE;
	alr_var.adr_form[STOP].ob_flags = HIDETREE;
	if(icone--)
		alr_var.adr_form[POINT+icone].ob_flags = NORMAL;
	
	alr_var.adr_form[ALB1].ob_flags = EXIT|SELECTABLE|HIDETREE|USR_INDICATOR|USR_ACTIVATOR;
	alr_var.adr_form[ALB1+1].ob_flags = EXIT|SELECTABLE|HIDETREE|USR_INDICATOR|USR_ACTIVATOR;
	alr_var.adr_form[ALB1+2].ob_flags = EXIT|SELECTABLE|HIDETREE|USR_INDICATOR|USR_ACTIVATOR;
	
	if(defaut--)
		alr_var.adr_form[ALB1+defaut].ob_flags = EXIT|SELECTABLE|HIDETREE|DEFAULT|USR_INDICATOR|USR_ACTIVATOR;
	
	for(nb_txt=0;nb_txt<5;nb_txt++)
		alr_var.adr_form[ALRT1+nb_txt].ob_flags = HIDETREE;
		
	texte++;
	nb_txt=0;
	do
	{	sep=strchr(texte,'|');
		sep2=strchr(texte,']');
		if(sep2<sep)
			sep=0;
		if(sep)
		{	strpcopy(alr_var.adr_form[ALRT1+nb_txt].ob_spec.free_string,texte,sep);
			tot_len_t = max(tot_len_t,(int)(sep-texte)*8);
			alr_var.adr_form[ALRT1+nb_txt++].ob_flags = NORMAL;
			texte = ++sep;
		}	
		else
		{	
			strpcopy(alr_var.adr_form[ALRT1+nb_txt].ob_spec.free_string,texte,sep2);
			alr_var.adr_form[ALRT1+nb_txt++].ob_flags = NORMAL;
			tot_len_t = max(tot_len_t,(int)(sep2-texte)*8);
			texte = ++sep2;
		}
	}while(sep);
	
	tot_len_t += alr_var.adr_form[ALRT1+nb_txt].ob_x+10;	/* de base */
	
	len[0] = 0;
	len[1] = 0;
	len[2] = 0;
	texte++;
	nb_bout=0;
	do
	{	sep=strchr(texte,'|');
		sep2=strchr(texte,']');
		if(sep2<sep)
			sep=0;
		if(sep)
		{	strpcopy(chaine,texte,sep);
			strcpy((char *)alr_var.adr_form[ALB1+nb_bout].ob_spec.userblk->ub_parm,chaine);
			/* rac clavier */
			rac = strchr(chaine,'[');
			if(rac)
				clav[nb_bout] = toupper(rac[1]);
			
			alr_var.adr_form[ALB1+nb_bout].ob_flags &= EXIT|SELECTABLE|DEFAULT|USR_INDICATOR|USR_ACTIVATOR;
			len[nb_bout] = (int)(sep - texte -(rac!=0) )*9+15;
			alr_var.adr_form[ALB1+nb_bout].ob_width = len[nb_bout];
			texte = ++sep;
			nb_bout++;
		}	
		else
		{	strpcopy(chaine,texte,sep2);
			strcpy((char *)alr_var.adr_form[ALB1+nb_bout].ob_spec.userblk->ub_parm,chaine);
			/* rac clavier */
			rac = strchr(chaine,'[');
			if(rac++)
				clav[nb_bout] = toupper(*rac);
			alr_var.adr_form[ALB1+nb_bout].ob_flags &= EXIT|SELECTABLE|DEFAULT|USR_INDICATOR|USR_ACTIVATOR;
			len[nb_bout] = (int)(sep2 - texte -(rac!=0) )*9+15;
			alr_var.adr_form[ALB1+nb_bout].ob_width = len[nb_bout];
			texte = ++sep2;
			nb_bout++;
		}
	}while(sep);
	
	tot_len_b = len[0] + len[1] + len[2] + nb_bout*20;

	alr_var.adr_form[0].ob_width = max(tot_len_b,tot_len_t);
	
	/* centrage des boutons */
	entre_bouton = (max(tot_len_b,tot_len_t) - len[0] - len[1] - len[2])/(nb_bout + 1);
	alr_var.adr_form[ALB1].ob_x = entre_bouton;
	x = len[0] + entre_bouton*2;
	alr_var.adr_form[ALB1+1].ob_x = x;
	x += len[1] + entre_bouton;
	alr_var.adr_form[ALB1+2].ob_x = x;
	
	h_bout = alr_var.adr_form[ALRT1+nb_txt-1].ob_y + 35;
	alr_var.adr_form[ALB1].ob_y = h_bout;
	alr_var.adr_form[ALB1+1].ob_y = h_bout;
	alr_var.adr_form[ALB1+2].ob_y = h_bout;
	h_bout += alr_var.adr_form[ALB1].ob_height + 10;
	alr_var.adr_form[0].ob_height = h_bout;

	alr_var.adr_form[POINT+icone].ob_y = (alr_var.adr_form[ALB1].ob_y-32)/2;

	
	form_center (adr , &x ,&y, &w ,&h);
	x -= 3;	y -= 3;	w += 6;	h += 6;
	form_dial (FMD_START, 0, 0, 0, 0, x, y, w, h);
	objc_draw (adr, 0, MAX_DEPTH, x, y, w, h);
	
	
	/* form do parait bugg‚, … nous le control */
	do
	{	envt=evnt_multi((MU_TIMER|MU_BUTTON|MU_KEYBD), 2, 1, 1,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								buff, 200, 0,
								&mx, &my, &mk, &key_s, &key, &dummy);
		if(envt == MU_KEYBD && key)
		{	asc = stdkey(key);
			if(asc == 13 || asc == 10)
				choix = ALB1 + defaut;
			asc = toupper(asc);
			if(key_s == K_ALT)
			{	if(asc == clav[0]) choix = ALB1;
				if(asc == clav[1]) choix = ALB1+1;
				if(asc == clav[2]) choix = ALB1+2;
			}
		}
		if(mk == 1 && envt & MU_BUTTON)
		{	choix = objc_find(adr,0,8,mx,my);
			if(adr[choix].ob_flags & SELECTABLE)
			{
				adr[choix].ob_state |= SELECTED;
				objc_draw (adr, choix, MAX_DEPTH, x, y, w, h);
				do
				{	graf_mkstate(&mx, &my, &mk, &dummy);
					dummy = objc_find(adr,0,8,mx,my);
					if(dummy != choix && (adr[choix].ob_state & SELECTED))
					{	adr[choix].ob_state &= ~SELECTED;
						objc_draw (adr, choix, MAX_DEPTH, x, y, w, h);
					}else if(dummy == choix && !(adr[choix].ob_state & SELECTED))
					{	adr[choix].ob_state |= SELECTED;
						objc_draw (adr, choix, MAX_DEPTH, x, y, w, h);
					}
				}while(mk == 1);
				if(dummy != choix)
					choix = 0;
			}
		}
	}while( (adr[choix].ob_flags & SELECTABLE) == 0);
		
/*	choix = form_do(adr,0);*/
	alr_var.adr_form[choix].ob_state &= ~SELECTED;
	choix = choix - ALB1 + 1;
	
	wind_update(END_MCTRL);	 /* D‚bloquer menu */

	form_dial (FMD_FINISH, 0, 0, 0, 0, x, y, w, h);
	return choix;
}

void set_text(	WINDFORM_VAR *ptr_var, int ob,char *str)
{
	int taille_max = ptr_var->adr_form[ob].ob_spec.tedinfo->te_txtlen - 1;

	strpcopy(ptr_var->adr_form[ob].ob_spec.tedinfo->te_ptext,str,str+taille_max);

	if(ptr_var->w_handle > 0)
		wf_draw(ptr_var,ob);
					 
}

void install_tache_fond(int tache)
{	
	if(!list_tache[tache])
	{	list_tache[tache] = TRUE;
		nb_tache++;
	}
	current_tache = TRUE;
}

void remove_tache_fond(int tache)
{
	if(list_tache[tache])
	{	list_tache[tache] = FALSE;
		nb_tache--;
		if(nb_tache == 0)
			current_tache = FALSE;
	}
}

void launch_tache(void)
{	int i;
	for(i=0;i<MAX_TACHE ; i++)
	{	if(list_tache[i])
		{	switch(i)
			{	case TACHE_APREV :
					aprev_timer();
					break;
				case TACHE_VISU :
					visu_play();
					break;
				case TACHE_CALCUL :
					break;
			}
		}
	}
}

void barre(int cur,int tot,OBJECT *adr,const char *txt)
{	adr[BOOT_ASS].ob_width = (int)((328L * (long)cur) / (long)tot);
	strcpy(adr[BOOT_TEXT].ob_spec.tedinfo->te_ptext,txt);
	Vsync();
	objc_draw (adr, BOOT_TEXT, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
	Vsync();
	objc_draw (adr, BOOT_FON, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
}

void ldg_mnt_init(void)
{	char retourneecc[50];
	
	if( (screenldg = ldg_exec ( app_id, "screen.ldg")) == NULL)  /* Chargement de la lib screen */
	 	/* ‚chec */
 		return;
 	
 	ldg_install_ok = TRUE;
 	
		/* recharche des fonctions de screen.ldg pour les utiliser */
	screen_detect=ldg_find("screen_detect", screenldg);
	screen_init=ldg_find("screen_init", screenldg);
	fix_palette=ldg_find("fix_palette", screenldg);
	screen_display=ldg_find("screen_display", screenldg);
	
	if((screen_detect!=NULL)&&(screen_init!=NULL)&&(fix_palette!=NULL)&&(screen_display!=NULL)) /* les fonctions ont ‚t‚ trouv‚es */
   {	retourneecc[0]=0;
		
		if(screen_detect((long)vdi_handle,retourneecc)>=0) 
		{  /* ‚cran reconnu mais peut ˆtre pas support‚! on n'en sait 
				rien jusque l… */
			if(screen_init((long)vdi_handle, &mydisplay,retourneecc,&myinfos,0L)>0) 
			{ /* ‚cran support‚ ca rempli tout par d‚faut … notre place */
				
				fix_palette(&myinfos, 1L, (long) vdi_handle); /* l… je fixe la pallette n'est en fait n‚cessaire que pour les r‚solution <= 8 plans */
				ldg_screen_ok = TRUE;
			}
		}
	}
}

void ldg_uninstall(void)
{	if(ldg_install_ok)
		ldg_term(app_id, screenldg); /* lib‚ration du LDG */
}