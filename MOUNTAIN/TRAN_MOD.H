/* tran mod.h */

int tran_ZBOX(TRAN_PARA *adr_para);
int tran_ISQR(TRAN_PARA *adr_para);
int tran_SLDE(TRAN_PARA *adr_para);
int tran_STRH(TRAN_PARA *adr_para);
int tran_PUSH(TRAN_PARA *adr_para);
int tran_PSHZ(TRAN_PARA *adr_para);

extern TRAN_DESC_PARAM desc_param_zbox;
extern TRAN_DESC_PARAM desc_param_isqr;
extern TRAN_DESC_PARAM desc_param_slde;
extern TRAN_DESC_PARAM desc_param_strh;
extern TRAN_DESC_PARAM desc_param_push;
extern TRAN_DESC_PARAM desc_param_pshz;
