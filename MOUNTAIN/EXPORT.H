/* export.h */

int export_anim(void);
void form_exp(int event);
void form_mpl(int event);
int find_exp_mod(long id);

#define EXP_PRE_W 160
#define EXP_PRE_H 160

typedef struct
{	void (*rout_mpl)(void);
	long	num_frame;
	int	*ptr_img;
	int	format;
}MPL_PARA;
