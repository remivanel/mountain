/* PREVIEW.H */

int init_wprev(void);
void window_prev(int event);
void close_prev(void);
int new_preview(int frame);
void end_preview(void);
void redraw_prev(int w_handle, int x, int y, int w, int h);
int calc_frame(int frame,int *prev_img, int px, int py, int r_x_mod16);
void put_ani_pis(int piste, int anim_p,int frame, int *ptr_img, int px, int py, int x_mod16);
void aprev_start(void);
void close_aprev(void);
void form_opre(int event);
void aprev_timer(void);
