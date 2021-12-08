MODE68000 = 1

EXPORT nova_cpyfm
EXPORT nova2_cpyfm
EXPORT matrix_cpyfm
EXPORT trm_16_cpyfm
EXPORT trm_mono
EXPORT xrvb_cpyfm
EXPORT bvr_cpyfm
EXPORT rvb_cpyfm
EXPORT xbvr_cpyfm

	
*TRAMAGE MONO BAYER
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	8
*W -> largeur source		10
*W -> largeur dest		12
*W -> hauteur				d3
*d2 -> larg ligne dest en octets
*a3 -> adr next ligne dest
*d6 -> compteur x ligne

trm_mono:
	movem.l	d0-d7/a0-a4,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	a0,a4				;sav
	move.w	10(a0),d3		;larg source
	move.w	8(a0),d4			;larg a copier
	sub.w		d4,d3
	lsl.w		d3
	
	move.w	8(a0),d4			;larg a copier
	
	move.w	14(a0),bcl_y		;haut
	lea 		matrice,a3
	move.l	4(a0),a1			; dst
	move.l	(a0),a0			; src

ligne:
	move.w	d4,d2
	bra.s		fin8

pixels_8:
	moveq		#7,d1
	move.l	a3,a2
	moveq		#-1,d0

loop:
	move.w	(a0)+,d5

IFEQ MODE68000
	bfextu	d5{16:5},d6		; extrait composante rouge	0..64
	lsl.w		#1,d6
	bfextu	d5{21:6},d7		; extrait composante verte 0..128
	lsl.w		#1,d7
	add.w		d7,d6
	bfextu	d5{27:5},d7		; extrait composante bleue 0..64
ELSE
	move.w	d5,d6
	and.w		#%1111100000000000,d6
	lsr.w		#8,d6
	lsr.w		#2,d6
	move.w	d5,d7
	and.w		#%0000011111100000,d7
	lsr.w		#4,d7
	add.w		d7,d6
	move.w	d5,d7
	and.w		#%0000011111,d7
ENDIF

	lsl.w		#1,d7
	add.w		d7,d6				; total max 256
	lsr.w		#2,d6				; on a une valeur de gris de 0->64

	cmp.b		(a2)+,d6
	bmi.s		noir
	bclr		d1,d0

noir:
	dbf		d1,loop
	move.b	d0,(a1)+
fin8:
	subq.w	#8,d2
	bpl.s		pixels_8
	
	addq.w	#7,d2
	bmi.s		fin_ligne
	moveq		#-1,d0
	moveq		#7,d1
	sub.w		d2,d1
	move.l	a3,a2
loop2:
	move.w	(a0)+,d5
IFEQ MODE68000
	bfextu	d5{16:5},d6		; extrait composante rouge	0..64
	lsl.w		#1,d6
	bfextu	d5{21:6},d7		; extrait composante verte 0..128
	lsl.w		#1,d7
	add.w		d7,d6
	bfextu	d5{27:5},d7		; extrait composante bleue 0..64
ELSE
	move.w	d5,d6
	and.w		#%1111100000000000,d6
	lsr.w		#8,d6
	lsr.w		#2,d6
	move.w	d5,d7
	and.w		#%0000011111100000,d7
	lsr.w		#4,d7
	add.w		d7,d6
	move.w	d5,d7
	and.w		#%0000011111,d7
ENDIF
	lsl.w		#1,d7
	add.w		d7,d6				; total max 256
	lsr.w		#2,d6				; on a une valeur de gris de 0->64
	
	cmp.b		(a2)+,d6
	bmi.s		noir2
	bclr		d2,d0

noir2:
	dbf		d2,loop2
	rol.b		d1,d0
	move.b	d0,(a1)+

fin_ligne:
	addq.l	#8,a3
	cmp.l		#matrice+64,a3
	bne.s		pas_8
	sub.w		#64,a3
pas_8:
	add.w		d3,a0
boucle:
	subq.w	#1,bcl_y
	bne.w		ligne
	
	movem.l	(sp)+,d0-d7/a0-a4
	rts


*FALC 2 ecran 16
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3
*d2 -> larg ligne dest en octets
*a3 -> adr next ligne dest
*d6 -> compteur x ligne

trm_16_cpyfm:
	movem.l	d0-d7/a0-a3,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),bcl_y	; haut
	move.w	16(a0),pos_x_dst
	move.l	(a0),a0			; src
	
	lsr.w		d2					; d2 /2 -> largeur d'un ligne 16 couls
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	
	add.l		d1,d1				;en word

	
trm_y:
	move.w	d2,d0		; compteur dbf x
	lsr.w		#3,d0
	subq		#1,d0
	move.l	a1,a3
	add.l		d2,a3				; prepare en a3 l'adr de la next ligne dest
trm_x:
REPT	16
	move.w	(a0)+,d3
	jsr		trame_16c(pc)		; retour en d3
	lsr.b		#1,d3
	roxl.w	#1,d7
	lsr.b		#1,d3
	roxl.w	#1,d6
	lsr.b		#1,d3
	roxl.w	#1,d5
	lsr.b		#1,d3
	roxl.w	#1,d4
ENDM
	movem.w	d4-d7,(a1)
	lea		8(a1),a1
	
	dbf		d0,trm_x
	
	add.w		d1,a0				; saut ligne suivant src
	move.l	a3,a1				; saut ligne suivant dst
	
	subq		#1,bcl_y
	bne		trm_y
	
	movem.l	(sp)+,d0-d7/a0-a3
	rts

*TRAME 
*D3-> pixel 16bit
*retour d3 -> pixel 16 coul
trame_16c:	
	movem.l	d0-d2,-(sp)	; save context	

IFEQ MODE68000
	bfextu	d3{16:2},d0		; extrait composante rouge 4 niv
	bfextu	d3{21:2},d1		; extrait composante verte sauf 4 niv
	bfextu	d3{27:2},d2		; extrait composante bleue 4 niv
ELSE
	move.w	d3,d0
	and.w		#%1111100000000000,d0
	lsr.w		#8,d0
	lsr.w		#6,d0
	move.w	d3,d1
	and.w		#%0000011111100000,d1
	lsr.w		#4,d1
	lsr.w		#5,d1
	move.w	d3,d2
	and.w		#%0000011111,d2
	lsr.w		#3,d2

ENDIF
	
	lsl.w		#4,d0
	lsl.w		#2,d2
	or.w		d1,d0	
	or.w		d2,d0
	
	move.l	#tab_rvb,a2
;	moveq		#0,d3
	move.b	d0(a2),d3
	
	movem.l	(sp)+,d0-d2
	rts


	*FALC 2 NOVA
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

nova_cpyfm:
	movem.l	d0-d5/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	add.l		d2,d2
	
nova_y:
nova_x:
	move.w	(a0)+,d5
	ror.l		#5,d5
	lsr		#1,d5
	rol.l		#5,d5
	ror		#8,d5
	move.w	d5,(a1)+
	
	dbf		d0,nova_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,nova_y
	
	movem.l	(sp)+,d0-d5/a0-a1
	rts


*FALC 2 MATRIX
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

matrix_cpyfm:
	movem.l	d0-d6/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	add.l		d2,d2
	
matrix_y:
matrix_x:
	move.w	(a0)+,d5
	move.w	d5,d6
	lsr.w		#1,d5
	bfins		d6,d5{27:5}
	move.w	d5,(a1)+
	
	dbf		d0,matrix_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,matrix_y
	
	movem.l	(sp)+,d0-d6/a0-a1
	rts

*FALC 2 NOVA BIS (hades)
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

nova2_cpyfm:
	movem.l	d0-d5/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	add.l		d2,d2
	
nova2_y:
nova2_x:
	move.w	(a0)+,d5
	rol.w		#8,d5
	move.w	d5,(a1)+
	
	dbf		d0,nova2_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,nova2_y
	
	movem.l	(sp)+,d0-d5/a0-a1
	rts

	*FALC 2 XRVB (matrix et mac)
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

xrvb_cpyfm:
	movem.l	d0-d7/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	moveq		#0,d7
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter src
	sub.w		d0,d2				; a sauter dst
	
	add.l		d1,d1				;en word
	lsl.w		#2,d2				;en 32bits : *4
	
xrvb_y:
xrvb_x:
	move.w	(a0)+,d5
	
	move.b	d7,(a1)+			; le "X"
	
	bfextu	d5{16:5},d6		; extrait composante rouge 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+
	
	bfextu	d5{21:6},d6		; extrait composante verte 6 bits
	lsl.b		#2,d6
	move.b	d6,(a1)+

	bfextu	d5{27:5},d6		; extrait composante bleue 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+
	
	dbf		d0,xrvb_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,xrvb_y
	
	movem.l	(sp)+,d0-d7/a0-a1
	rts

	*FALC 2 XBVR (Nova 32 bits)
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

xbvr_cpyfm:
	movem.l	d0-d7/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	moveq		#0,d7
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	lsl.w		#2,d2				;en 32bits : *4
	
xbvr_y:
xbvr_x:
	move.w	(a0)+,d5

	move.b	d7,(a1)+			; le "X"

	bfextu	d5{27:5},d6		; extrait composante bleue 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+

	bfextu	d5{21:6},d6		; extrait composante verte 6 bits
	lsl.b		#2,d6
	move.b	d6,(a1)+

	bfextu	d5{16:5},d6		; extrait composante rouge 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+
	
	
	dbf		d0,xbvr_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,xbvr_y
	
	movem.l	(sp)+,d0-d7/a0-a1
	rts

*FALC 2 BVR Hades
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

bvr_cpyfm:
	movem.l	d0-d6/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	mulu.w	#3,d2
	
bvr_y:
bvr_x:
	move.w	(a0)+,d5

IFEQ MODE68000
	bfextu	d5{27:5},d6		; extrait composante bleue 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+

	bfextu	d5{21:6},d6		; extrait composante verte 6 bits
	lsl.b		#2,d6
	move.b	d6,(a1)+

	bfextu	d5{16:5},d6		; extrait composante rouge 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+
ELSE
	move.w	d5,d6
	and.w		#%0000011111,d6
	lsl.b		#3,d6
	move.b	d6,(a1)+
	move.w	d5,d6
	and.w		#%0000011111100000,d6
	lsr.w		#3,d6
	move.b	d6,(a1)+
	move.w	d5,d6
	and.w		#%1111100000000000,d6
	lsr.w		#8,d6
	move.b	d6,(a1)+
ENDIF	
	
	dbf		d0,bvr_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,bvr_y
	
	movem.l	(sp)+,d0-d6/a0-a1
	rts

*FALC 2 RVB 
*a0 -> param block :
*L -> src		a0
*L -> dst		a1
*W -> largeur a copier	d0 et d4
*W -> largeur source		d1
*W -> largeur dest		d2
*W -> hauteur				d3

rvb_cpyfm:
	movem.l	d0-d6/a0-a1,-(sp)	; save context	
	
	moveq		#0,d0
	moveq		#0,d1
	moveq		#0,d2
	moveq		#0,d3
	
	move.l	4(a0),a1			; dst
	move.w	8(a0),d0			; larg cop
	move.w	10(a0),d1		; larg src
	move.w	12(a0),d2		; larg dst
	move.w	14(a0),d3		; haut
	move.l	(a0),a0			; src
	
	move.l	d0,d4				; save
	
	sub.w		d0,d1				; a sauter
	sub.w		d0,d2				; a sauter
	
	add.l		d1,d1				;en word
	mulu.w	#3,d2
	
rvb_y:
rvb_x:
	move.w	(a0)+,d5

IFEQ MODE68000
	bfextu	d5{16:5},d6		; extrait composante rouge 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+

	bfextu	d5{21:6},d6		; extrait composante verte 6 bits
	lsl.b		#2,d6
	move.b	d6,(a1)+

	bfextu	d5{27:5},d6		; extrait composante bleue 5 bits
	lsl.b		#3,d6
	move.b	d6,(a1)+
ELSE
	move.w	d5,d6
	and.w		#%1111100000000000,d6
	lsr.w		#8,d6
	move.b	d6,(a1)+
	move.w	d5,d6
	and.w		#%0000011111100000,d6
	lsr.w		#3,d6
	move.b	d6,(a1)+
	move.w	d5,d6
	and.w		#%0000011111,d6
	lsl.b		#3,d6
	move.b	d6,(a1)+
ENDIF		
	
	dbf		d0,rvb_x
	
	move.l	d4,d0
	
	add.w		d1,a0				; saut ligne suivant src
	add.w		d2,a1				; saut ligne suivant dst
	
	dbf		d3,rvb_y
	
	movem.l	(sp)+,d0-d6/a0-a1
	rts
	
pos_x_dst:	ds.l	1
bcl_y:		ds.w	1

*					  V0E1R2T3
tab_rvb:		dc.b	15,15,6,12		;ROUGE 0 BLEU 0
				dc.b	15,7,7,5			;ROUGE 0 BLEU 1
				dc.b	3,2,5,5			;ROUGE 0 BLEU 2
				dc.b	2,2,5,5			;ROUGE 0 BLEU 3

				dc.b	9,13,13,4		;ROUGE 1 BLEU 0
				dc.b	11,1,1,4			;ROUGE 1 BLEU 1
				dc.b	11,7,14,12		;ROUGE 1 BLEU 2
				dc.b	2,2,5,5			;ROUGE 1 BLEU 3

				dc.b	9,8,12,12		;ROUGE 2 BLEU 0
				dc.b	9,13,12,12		;ROUGE 2 BLEU 1
				dc.b	10,10,14,14		;ROUGE 2 BLEU 2
				dc.b	10,10,17,0		;ROUGE 2 BLEU 3

				dc.b	8,8,12,12		;ROUGE 3 BLEU 0
				dc.b	8,8,12,12		;ROUGE 3 BLEU 1
				dc.b	10,10,14,0		;ROUGE 3 BLEU 2
				dc.b	10,10,0,0		;ROUGE 3 BLEU 3

;Le bayer :
matrice:		dc.b 1,32,8,40,2,34,10,42
				dc.b 48,16,56,24,50,18,58,26
				dc.b 12,44,4,36,14,46,6,38
				dc.b 60,28,52,20,62,30,54,22
				dc.b 3,35,11,43,1,33,9,41
				dc.b 51,19,59,27,49,17,59,25
				dc.b 15,47,7,39,13,45,5,37
				dc.b 63,31,55,23,61,29,53,21
				