MODE68000 = 1


EXPORT asm_zoom
EXPORT a_tr0_zoom
EXPORT opt_tr_zoom
EXPORT tr_zoom
EXPORT copy_intens_img
EXPORT noir_et_blanc
EXPORT strict_netb
EXPORT invers_video
EXPORT horz_mir
EXPORT vert_mir
EXPORT asm_rvbc
EXPORT asm_rot
EXPORT asm2_rot

* A0:src A1:dst D0:x D1:yD2:larg_img
vert_mir:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	move.w	d2,d7		*larg->d7
	move.w	d2,d6		*larg->d6
	sub.w		d0,d6		*saut->d6
	lsl.l		#1,d6		* d6 * 2
	
	lsl.l		#2,d7		* d7 * 4
	
	move.w	d0,d2		*x->d2
	lsl.l		#1,d2		* d2 * 2
	add.w		d2,a0
	lsl.l		#1,d2		* d2 * 2
	subq		#1,d0
	subq		#1,d1
	move.w	d0,d3		* sav X
vmir_bcl:
	move.w	-(a0),(a1)+
	
	dbf		d0,vmir_bcl
	add.w		d7,a0			*vrai saut de  lignes
	add.w		d6,a1			*saut de pixel
	move.w	d3,d0
	dbf		d1,vmir_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y
horz_mir:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	move.w	d0,d2		*x->d2
	mulu.w	d1,d2		*x*y->d2
	lsl.l		#1,d2		* d2 * 2
	add.l		d2,a0		* on se met a la fin
	move.w	d0,d2		*x->d2
	lsl.w		#1,d2		* d2 * 2
	sub.w		d2,a0		* debut avant derniere ligne
	lsl.w		#1,d2		* d2 * 2
	subq		#1,d0
	subq		#1,d1
	move.w	d0,d3		* sav X
hmir_bcl:
	move.w	(a0)+,(a1)+
	
	dbf		d0,hmir_bcl
	sub.w		d2,a0		* une ligne en arrier
	move.w	d3,d0
	dbf		d1,hmir_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y
invers_video:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	subq		#1,d0
	subq		#1,d1
	move.w	d0,d3				* sav X
invv_bcl:
	move.w	(a0)+,d5
	eori.w	#$ffff,d5
	move.w	d5,(a1)+
	
	dbf		d0,invv_bcl
	move.w	d3,d0
	dbf		d1,invv_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y d2:seuil SP1: COL1 SP2: col2
strict_netb:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	subq		#1,d0
	move.w	d1,bc_y
	move.w	d0,d3				* sav X
	move.w	48(sp),d1   ; col1
	move.w	50(sp),d4   ; col2
snetb_bcl:
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
	
	cmp.w		d2,d6
	bmi		snetb_noir
	move.w	d4,(a1)+
	bra.w		snetb_suite
snetb_noir:
	move.w	d1,(a1)+
snetb_suite:
	
	dbf		d0,snetb_bcl
	move.w	d3,d0
	subq.w	#1,bc_y
	bne 		snetb_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y d2:c_r SP1: c_v SP2: c_b
asm_rvbc:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	subq		#1,d0
	move.w	d1,bc_y
	move.w	d0,bc_x				* sav X
	move.w	48(sp),d3   ; c_v
	move.w	50(sp),d4   ; c_b

	and.l		#$ffff,d2
	and.l		#$ffff,d3
	and.l		#$ffff,d4
IFEQ MODE68000
	lsl.l		#7,d2
	lsl.l		#7,d3
	lsl.l		#7,d4
ENDIF

; D2 : Rouge   D3 : Vert   D4 : Bleu
; D1, D5, D6 et D7 libres

rvbc_bcl:
	move.w	(a0)+,d1
IFEQ MODE68000
	bfextu	d1{16:5},d5		; extrait composante rouge	0..32
	mulu.l	d2,d5				; ponderation
	swap		d5
	bfextu	d1{21:6},d6		; extrait composante verte 0..64
	mulu.l	d3,d6				; ponderation
	swap		d6
	bfextu	d1{27:5},d7		; extrait composante bleue 0..32
	mulu.l	d4,d7				; ponderation
	swap		d7
ELSE
	move.w	d1,d5
	and.w		#%1111100000000000,d5
	lsr.w		#8,d5
	lsr.w		#3,d5
	mulu.w	d2,d5
	lsr.w		#8,d5
	lsr.w		#1,d5
	move.w	d1,d6
	and.w		#%0000011111100000,d6
	lsr.w		#5,d6
	mulu.w	d3,d6
	lsr.w		#8,d6
	lsr.w		#1,d6
	move.w	d1,d7
	and.w		#%0000011111,d7
	mulu.w	d4,d7
	lsr.w		#8,d7
	lsr.w		#1,d7
ENDIF

	cmp.w		#31,d7
	bmi		no_cb
	moveq		#31,d7	; limitation a 31max
no_cb:
	cmp.w		#63,d6
	bmi		no_cv
	moveq		#63,d6	; limitation a 63max
no_cv:
	cmp.w		#31,d5
	bmi		no_cr
	moveq		#31,d5	; limitation a 31max
no_cr:
IFEQ MODE68000
	bfins		d6,d7{21:6}		; met le vert
	bfins		d5,d7{16:5}		; met le rouge
ELSE
	lsl.w		#8,d5
	lsl.w		#3,d5
	or.w		d5,d7
	lsl.w		#5,d6
	or.w		d6,d7
ENDIF	
	move.w	d7,(a1)+
	
	dbf		d0,rvbc_bcl
	move.w	bc_x,d0
	subq.w	#1,bc_y
	bne 		rvbc_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts


* A0:src A1:dst D0:x D1:y d2:cos SP1: sin SP2 -> larg
* sp3-> centre x * sp4 -> centre y * sp5->color
* ROTATION SANS MOSAIQUE
asm_rot:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	move.w	d1,bc_y
	move.w	d0,bc_x			* sav X
	move.w	d0,x				* sav X
	move.w	d1,y				* sav Y
	move.w	52(sp),d3   ; larg
	lsl.w		#1,d3			; en octet
	move.w	d3,larg
	lsl.w		#1,d0			; calcul de la fin de l'image
	add.w		d0,a1
	subq.w	#1,d1
	mulu.w	d3,d1
	add.l		d1,a1			; a3 = a0 + 2*x + 2*y*larg
	
	move.l	d2,d1			; cos ds d1
	move.l	48(sp),d2   ; sin ds d2
	move.w	54(sp),x_centre 
	move.w	56(sp),y_centre 
	move.w	58(sp),d3	;la couleur
	
* d0->tmp/couleur * d1->cos * d2 -> sin * d3-> color * d4-> x * d5-> y
* d6->xs * d7-> ys
rot_bcl:
	moveq		#0,d4
	move.w	bc_x,d4
	subq		#1,d4
	sub.w		x_centre,d4   	*(x-x_centre) -> d4
	ext.l		d4
	moveq		#0,d5
	move.w	bc_y,d5
	subq		#1,d5
	sub.w		y_centre,d5		*(y-y_centre) -> d5
	ext.l		d5

;CALCUL XS	
	move.l	d4,d6	; x->xs
IFEQ MODE68000
	muls.l	d1,d6 ; x*cos(a)
	move.l	d5,d0 ; y->tmp
	muls.l	d2,d0 ; y*sin(a)
ELSE
	movem.l	d1-d2,-(sp)
	move.w	d6,d2
	mulu.w	d1,d6
	swap		d1
	muls.w	d1,d2
	swap		d2
	add.l		d2,d6
	movem.l	(sp)+,d1-d2
	
	move.l	d5,d0 ; y->tmp

	movem.l	d2-d3,-(sp)
	move.w	d0,d3
	mulu.w	d2,d0
	swap		d2
	muls.w	d2,d3
	swap		d3
	add.l		d3,d0
	movem.l	(sp)+,d2-d3
ENDIF
	sub.l		d0,d6 ; xs=x*cos(a)-y*sin(a)
	swap		d6		; entier
	move.w	d3,d0 ; couleur par defaut
	add.w		x_centre,d6
	bmi		abord
	cmp.w		x,d6
	bpl		abord
	
;CALCUL YS	
	move.l	d4,d7	; x->xs
IFEQ MODE68000
	muls.l	d2,d7 ; x*sin(a)
	move.l	d5,d0 ; y->tmp
	muls.l	d1,d0 ; y*cos(a)
ELSE
	movem.l	d1-d2,-(sp)
	move.w	d7,d1
	mulu.w	d2,d7
	swap		d2
	muls.w	d2,d1
	swap		d1
	add.l		d1,d7
	movem.l	(sp)+,d1-d2
	
	move.l	d5,d0 ; y->tmp

	movem.l	d1-d2,-(sp)
	move.w	d0,d2
	mulu.w	d1,d0
	swap		d1
	muls.w	d1,d2
	swap		d2
	add.l		d2,d0
	movem.l	(sp)+,d1-d2
ENDIF
	add.l		d0,d7 ; ys=x*sin(a)+y*cos(a)
	swap		d7		; entier
	move.w	d3,d0 ; couleur par defaut
	add.w		y_centre,d7
	bmi		abord
	cmp.w		y,d7
	bpl		abord

;PRISE DE LA COULEUR SUR LA SRC
	move.l	a0,a2
	mulu.w	larg,d7			* y*2*larg_img
	add.l		d7,a2
	lsl.w		#1,d6
	add.w		d6,a2			* scr + x*2 + y*2*larg_img
	move.w	(a2),d0
	
abord:
;PIXEL_DESTINATION
	move.w	d0,-(a1)

	subq.w	#1,bc_x
	bne 		rot_bcl
	move.w	x,bc_x
	move.w	larg,d0
	lsr.w		#1,d0
	sub.w		x,d0
	lsl.w		#1,d0
	sub.w		d0,a1			* eleve la difference larg-x a a3
	subq.w	#1,bc_y
	bne 		rot_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y d2:cos SP1: sin SP2 -> larg
* sp3-> centre x * sp4 -> centre y
* ROTATION AVEC MOSAIQUE
asm2_rot:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	move.w	d1,bc_y
	move.w	d0,bc_x			* sav X
	move.w	d0,x				* sav X
	move.w	d1,y				* sav Y
	move.w	52(sp),d3   ; larg
	lsl.w		#1,d3			; en octet
	move.w	d3,larg
	lsl.w		#1,d0			; calcul de la fin de l'image
	add.w		d0,a1
	subq.w	#1,d1
	mulu.w	d3,d1
	add.l		d1,a1			; a3 = a0 + 2*x + 2*y*larg
	
	move.l	d2,d1			; cos ds d1
	move.l	48(sp),d2   ; sin ds d2
	move.w	54(sp),x_centre 
	move.w	56(sp),y_centre 
	
* d0->tmp/couleur * d1->cos * d2 -> sin * d3-> larg * d4-> x * d5-> y
* d6->xs * d7-> ys
rot2_bcl:
	moveq		#0,d4
	move.w	bc_x,d4
	subq		#1,d4
	sub.w		x_centre,d4   	*(x-x_centre) -> d4
	ext.l		d4
	moveq		#0,d5
	move.w	bc_y,d5
	subq		#1,d5
	sub.w		y_centre,d5		*(y-y_centre) -> d5
	ext.l		d5

;CALCUL XS	
	move.l	d4,d6	; x->xs
	muls.l	d1,d6 ; x*cos(a)
	move.l	d5,d0 ; y->tmp
	muls.l	d2,d0 ; y*sin(a)
	sub.l		d0,d6 ; xs=x*cos(a)-y*sin(a)
	clr.w		d6
	swap		d6		; entier
	add.w		x_centre,d6
	move.w	x,d0
	divs.w	d0,d6
	swap		d6
	
;CALCUL YS	
	move.l	d4,d7	; x->xs
	muls.l	d2,d7 ; x*sin(a)
	move.l	d5,d0 ; y->tmp
	muls.l	d1,d0 ; y*cos(a)
	add.l		d0,d7 ; ys=x*sin(a)+y*cos(a)
	clr.w		d7
	swap		d7		; entier
	add.w		y_centre,d7
	move.w	y,d0
	divs.w	d0,d7
	swap		d7

;PRISE DE LA COULEUR SUR LA SRC
	move.l	a0,a2
	mulu.w	d3,d7			* y*2*larg_img
	add.l		d7,a2
	lsl.w		#1,d6
	add.w		d6,a2			* scr + x*2 + y*2*larg_img
	move.w	(a2),d0
	
;PIXEL_DESTINATION
	move.w	d0,-(a1)

	subq.w	#1,bc_x
	bne 		rot2_bcl
	move.w	x,bc_x
	move.w	d3,d0		* d3=larg*2
	lsr.w		#1,d0
	sub.w		x,d0
	lsl.w		#1,d0
	sub.w		d0,a1			* eleve la difference larg-x a a3
	subq.w	#1,bc_y
	bne 		rot2_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* A0:src A1:dst D0:x D1:y
noir_et_blanc:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	subq		#1,d0
	subq		#1,d1
	move.w	d0,d2				* sav X
netb_bcl:
	move.w	(a0)+,d5
IFEQ MODE68000
	bfextu	d5{16:5},d6		; extrait composante rouge	0..64
	lsl.w		#1,d6
	bfextu	d5{21:6},d7		; extrait composante verte 0..128
	lsl.w		#1,d7
	add.w		d7,d6
	bfextu	d5{27:5},d7		; extrait composante bleue 0..64
	lsl.w		#1,d7
	add.w		d7,d6				; total max 256
	lsr.w		#2,d6				; on a une valeur de gris de 0->64
	move.w	d6,d5
	lsr.w		#1,d5				; on a une valeur de gris de 0->32
	
	move.w	d5,d3				; met le bleu
	bfins		d6,d3{21:6}		; met le vert
	bfins		d5,d3{16:5}		; met le rouge
	
ELSE

	move.w	d5,d6
	and.w		#%1111100000000000,d6
	lsr.w		#5,d6
	lsr.w		#5,d6							; extrait composante rouge	0..64
	
	move.w	d5,d7
	and.w		#%11111100000,d7
	lsr.w		#4,d7							;  extrait composante verte 0..128

	add.w		d7,d6
	
	move.w	d5,d7
	and.w		#%11111,d7
	lsl.w		#1,d7
	add.w		d7,d6				; total max 256
	lsr.w		#2,d6				; on a une valeur de gris de 0->64
	move.w	d6,d5
	lsr.w		#1,d5				; on a une valeur de gris de 0->32
	
	move.w	d5,d3				; met le bleu
	lsl.w		#5,d5
	lsl.w		#6,d5
	or.w		d5,d3				; met le rouge
	lsl.w		#5,d6
	or.w		d6,d3
ENDIF

	move.w	d3,(a1)+
	
	dbf		d0,netb_bcl
	move.w	d2,d0
	dbf		d1,netb_bcl
	
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts
	

* (src,dst,img_para,x,y,x_dst_to_skip)
* d0:x,d1:tempo, d2:sav x , d3 tempo, d4 intens, d4f -intens
copy_intens_img:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	
	move.l	48(sp),a2		* img param
	lsl.w		#1,d2
	move.w	d2,x_dst_skip
	subq		#1,d0
	move.w	d0,d2				* sav X
	move.w	d1,bc_y
	
cii_bcl_y:
	move.w	d2,d0
cii_bcl_x:
	move.w	(a2)+,d3
IFEQ MODE68000
	bfextu	d3{21:6},d4	* le vert
ELSE
	move.w	d3,d4
	and.w		#%11111100000,d4
	lsr.w		#5,d4
ENDIF
	move.w	d4,d3
	lsl.w		#5,d4
	lsl.w		#5,d4			* sur 65536
;	addq.l	#1,d4
	swap		d4
	moveq		#63,d5
	sub.w		d3,d5
	move.w	d5,d4
	lsl.w		#5,d4
	lsl.w		#5,d4			* sur 65536
;	addq.l	#1,d4
	swap		d4
	
	move.w	(a0)+,d3		* src
IFEQ MODE68000
	bfextu	d3{16:5},d6	* le rouge
ELSE
	move.w	d3,d6
	and.w		#%1111100000000000,d6
	lsr.w		#5,d6
	lsr.w		#6,d6
ENDIF
	mulu.w	d4,d6			* ponderation
	move.w	(a1),d1		* dest
IFEQ MODE68000
	bfextu	d1{16:5},d5	* le rouge
ELSE
	move.w	d1,d5
	and.w		#%1111100000000000,d5
	lsr.w		#5,d5
	lsr.w		#6,d5
ENDIF
	swap		d4
	mulu.w	d4,d5			* ponderation
	add.l		d6,d5
	swap		d5				* on a la composante rouge
	move.w	d5,d7			* on stoque
	lsl.w		#8,d7
	lsl.w		#3,d7			* oblig‚ !!!
	swap		d4
;VERT
IFEQ MODE68000
	bfextu	d3{21:6},d6	* le vert
ELSE
	move.w	d3,d6
	and.w		#%11111100000,d6
	lsr.w		#5,d6
ENDIF
	mulu.w	d4,d6			* ponderation
IFEQ MODE68000
	bfextu	d1{21:6},d5	* le vert
ELSE
	move.w	d1,d5
	and.w		#%11111100000,d5
	lsr.w		#5,d5
ENDIF
	swap		d4
	mulu.w	d4,d5			* ponderation
	add.l		d6,d5
	swap		d5				* on a la composante verte
	lsl.w		#5,d5
	or.w		d5,d7			* on stoque
	swap		d4
;BLUE
IFEQ MODE68000
	bfextu	d3{27:5},d6	* le bleu
ELSE
	move.w	d3,d6
	and.w		#%11111,d6
ENDIF
	mulu.w	d4,d6			* ponderation
IFEQ MODE68000
	bfextu	d1{27:5},d5	* le bleu
ELSE
	move.w	d1,d5
	and.w		#%11111,d5
ENDIF
	swap		d4
	mulu.w	d4,d5			* ponderation
	add.l		d6,d5
	swap		d5				* on a la composante bleu
	or.w		d5,d7			* on stoque
	swap		d4

 	move.w 	d7,(a1)+

	
	dbf		d0,cii_bcl_x
	add.w		x_dst_skip,a1
	subq.w	#1,bc_y
	bne 		cii_bcl_y
	movem.l	(sp)+,d0-d7/a0-a2	; restore context	
	rts

* (adr_source,adr_dest,x_src, x_dest)
* d0: increment virgule fixe
* d2: position dans src
asm_zoom:
	movem.l	d0-d3,-(sp)	; save context	
	
	
	moveq		#0,d2
	move.w	d1,d2			*x_dest dans d2	
	swap		d0				*x_src en virgule fixe


IFEQ MODE68000
	divu.l	d2,d0			*increment = x_src/x_dest
ELSE
	lsr.l		#6,d0
	divu.w	d2,d0
	and.l		#$ffff,d0
	lsl.l		#6,d0
ENDIF

	moveq		#0,d2			* au debut dans src
	moveq		#0,d3			* il nous faut un reg vide pour l'opt addx
	
	subq		#2,d1			; -1 pour dbf et -1 le premier
			
	swap		d2				*d2 en entier
	swap		d0				*d0 en entier
	move.w	(a0),(a1)+
zoom:
	add.l		d0,d2			*incerment la src
	addx.w	d3,d2			* on add juste le bit de poid fort cree
IFEQ MODE68000
	move.w	(d2*2,a0),(a1)+
ELSE
	add.w		d2,d2
	move.w	(d2,a0),(a1)+
	lsr.w		#1,d2
ENDIF
		
	dbf 		d1,zoom

	movem.l	(sp)+,d0-d3
	rts


* Zoom avec transparence de tolerance nulle
* (a0:adr_source,a1:adr_dest,d0:x_src,d1:x_dest,d2:trans_color)
* d0: increment virgule fixe
* d4: position dans src
a_tr0_zoom:
	movem.l	d0-d7,-(sp)	; save context	
	
;	subq		#1,d0
	
	moveq		#0,d4
	move.w	d1,d4			*x_dest dans d2	
	swap		d0				*x_src en virgule fixe
IFEQ MODE68000
	divu.l	d4,d0			*increment = x_src/x_dest
ELSE
	lsr.l		#6,d0
	divu.w	d4,d0
	and.l		#$ffff,d0
	lsl.l		#6,d0
ENDIF
	moveq		#0,d4			* au debut dans src
	moveq		#0,d3			* on a besoin d'1 reg vide
	
	subq		#2,d1			; -1 pour dbf et -1 le premier
			
	move.w	(a0),d7	* le premier pixel
	cmp.w		d7,d2					* le test
	beq		no_frist_pix0			* s'il sont egaux, on saut le pixel
	move.w	d7,(a1)

no_frist_pix0:
	addq.w	#2,a1
	swap		d0						*d0 en virgule f

tr0_zoom:
	add.l		d0,d4					*incerment la src
	addx.w	d3,d4
IFEQ MODE68000
	move.w	(d4*2,a0),d7	*pixel src
ELSE
	add.w		d4,d4
	move.w	(d4,a0),d7
	lsr.w		#1,d4
ENDIF

	cmp.w		d7,d2					* le test
	beq		suite0_trans			* s'il sont egaux, on saut le pixel
	move.w	d7,(a1)
suite0_trans:
	addq.w	#2,a1
	dbf 		d1,tr0_zoom

	movem.l	(sp)+,d0-d7
	rts

* Zoom avec transparence de tolerance avec masque logique
* NON UTILISEE
* (a0:adr_source,a1:adr_dest,d0:x_src,d1:x_dest,d2:trans_color,SP:mask)
* d0: increment virgule fixe
* d4: position dans src
* d3: position dans dest
* d5: mask
* d6: pixel masqu‚
opt_tr_zoom:
	movem.l	d0-d7,-(sp)	; save context	
	
	move.w	36(sp),d5   ; le masque
	and.w		d5,d2			; on masque deja la couleur de trans !
	
;	subq		#1,d0
	
	moveq		#0,d4
	move.w	d1,d4			*x_dest dans d2	
	swap		d0				*x_src en virgule fixe
IFEQ MODE68000
	divu.l	d4,d0			*increment = x_src/x_dest
ELSE
	lsr.l		#6,d0
	divu.w	d4,d0
	and.l		#$ffff,d0
	lsl.l		#6,d0
ENDIF
	moveq		#0,d4			* au debut dans src
	moveq		#0,d3			* on a besoin d'1 reg vide
	
	subq		#2,d1			; -1 pour dbf et -1 le premier
			
	move.w	(a0),d7		* le premier pixel
	move.w	d7,d6
	and.w		d5,d6					* le masquage
	cmp.w		d6,d2					* la comparaison
	beq		tro_zoom			* s'il sont egaux, on saut le pixel
	move.w	d7,(a1)+
	swap		d0						*d0 en virgule f
tro_zoom:
	add.l		d0,d4					*incerment la src
	addx.w	d3,d4
IFEQ MODE68000
	move.w	(d4*2,a0),d7	*pixel src
ELSE
	add.w		d4,d4
	move.w	(d4,a0),d7
	lsr.w		#1,d4
ENDIF
	move.w	d7,d6
	and.w		d5,d6					* le masquage
	cmp.w		d6,d2					* la comparaison
	beq		suiteo_trans			* s'il sont egaux, on saut le pixel
	move.w	d7,(a1)
suiteo_trans:
	addq.w	#2,a1
	dbf 		d1,tro_zoom

	movem.l	(sp)+,d0-d7
	rts

* Zoom avec transparence de tolerance libre 
* (a0:adr_source,a1:adr_dest,d0:x_src,d1:x_dest,d2:trans_color,SP:toler*64)
* d0: increment virgule fixe
* d1: faible:compteur fort:(rouge,coul)
* d4: position dans src
* d3: fort:(vert,coul) faible:(bleu,coul)
* d5: toler
* d6: pixel src
* d7 tempo
* d2 tempo

tr_zoom:
	movem.l	d0-d7,-(sp)	; save context	
	
	moveq		#0,d4
	move.w	d1,d4			*x_dest dans d4
;	subq		#1,d0
	
	swap		d0				*x_src en virgule fixe
IFEQ MODE68000
	divu.l	d4,d0			*increment = x_src/x_dest
ELSE
	lsr.l		#6,d0
	divu.w	d4,d0
	and.l		#$ffff,d0
	lsl.l		#6,d0
ENDIF

	move.w	36(sp),d5   ; la toler
IFEQ MODE68000
	bfextu	d2{16:5},d1	* le rouge
ELSE
	move.w	d2,d1
	and.w		#%1111100000000000,d1
	lsr.w		#5,d1
	lsr.w		#6,d1
ENDIF
	lsl.w		#1,d1			* sur 64
	swap		d1				* on range le rouge ds le poid fort
	move.w	d4,d1			* on remet le compteur
IFEQ MODE68000
	bfextu	d2{21:6},d3 * le vert
ELSE
	move.w	d2,d3
	and.w		#%11111100000,d3
	lsr.w		#5,d3
ENDIF
	swap		d3				* le vert ds le poids fort
IFEQ MODE68000
	bfextu	d2{27:5},d7	* le bleu
ELSE
	move.w	d2,d7
	and.w		#%11111,d7
ENDIF
	lsl.w		#1,d7			* sur 64
	move.w	d7,d3	

	moveq		#0,d4			* au debut dans src
	
	subq		#2,d1			; -1 pour dbf et -1 le premier
			
	move.w	(a0),d6		* le premier pixel
IFEQ MODE68000
	bfextu	d6{16:5},d2	* le rouge
ELSE
	move.w	d6,d2
	and.w		#%1111100000000000,d2
	lsr.w		#5,d2
	lsr.w		#6,d2
ENDIF
	lsl.w		#1,d2				* *2 pour base 64
	swap		d1				*met le red en poid faible
	sub.w		d1,d2			* difference avec coul de ref
	bpl		pno_neg_r
	neg.w		d2
pno_neg_r:
	swap		d1				*met le red en poid fort
	cmp.w		d2,d5				* la comp
	bmi		ppix_diff			* pix differents

IFEQ MODE68000
	bfextu	d6{21:6},d2 * le vert
ELSE
	move.w	d6,d2
	and.w		#%11111100000,d2
	lsr.w		#5,d2
ENDIF
	swap		d3					*met le vert en poid faible
	sub.w		d3,d2				* difference avec coul de ref
	bpl		pno_neg_v
	neg.w		d2
pno_neg_v:
	swap		d3					*met le vert en poid fort
	cmp.w		d2,d5				* la comp
	bmi		ppix_diff			* pix differents

IFEQ MODE68000
	bfextu	d6{27:5},d2	* le bleu
ELSE
	move.w	d6,d2
	and.w		#%11111,d2
ENDIF
	lsl.w		#1,d2				* *2 pour base 64
	sub.w		d3,d2				* difference avec coul de ref
	bpl		pno_neg_b
	neg.w		d2
pno_neg_b:
	cmp.w		d2,d5				* la comp
	bmi		ppix_diff			* pix differents
; si on est ici, les pix sont pareils
	addq		#2,a1
	bra		btr_zoom
ppix_diff:
	move.w	d6,(a1)+

btr_zoom:
	add.l		d0,d4					*incerment la src
	swap		d4						*d2 en entier
IFEQ MODE68000
	move.w	(d4*2,a0),d6	*pixel src
ELSE
	add.w		d4,d4
	move.w	(d4,a0),d6
	lsr.w		#1,d4
ENDIF
	
IFEQ MODE68000
	bfextu	d6{16:5},d2	* le rouge
ELSE
	move.w	d6,d2
	and.w		#%1111100000000000,d2
	lsr.w		#5,d2
	lsr.w		#6,d2
ENDIF
	lsl.w		#1,d2				* *2 pour base 64
	swap		d1				*met le red en poid faible
	sub.w		d1,d2			* difference avec coul de ref
	bpl		no_neg_r
	neg.w		d2
no_neg_r:
	swap		d1				*met le red en poid fort
	cmp.w		d2,d5				* la comp
	bmi		pix_diff			* pix differents

IFEQ MODE68000
	bfextu	d6{21:6},d2 * le vert
ELSE
	move.w	d6,d2
	and.w		#%11111100000,d2
	lsr.w		#5,d2
ENDIF
	swap		d3					*met le vert en poid faible
	sub.w		d3,d2				* difference avec coul de ref
	bpl		no_neg_v
	neg.w		d2
no_neg_v:
	swap		d3					*met le vert en poid fort
	cmp.w		d2,d5				* la comp
	bmi		pix_diff			* pix differents

IFEQ MODE68000
	bfextu	d6{27:5},d2	* le bleu
ELSE
	move.w	d6,d2
	and.w		#%11111,d2
ENDIF
	lsl.w		#1,d2				* *2 pour base 64
	sub.w		d3,d2				* difference avec coul de ref
	bpl		no_neg_b
	neg.w		d2
no_neg_b:
	cmp.w		d2,d5				* la comp
	bmi		pix_diff			* pix differents
; si on est ici, les pix sont pareils
	addq		#2,a1
	bra		suite_trans
pix_diff:
	move.w	d6,(a1)+
suite_trans:
	swap		d4						*d2 en virgule fixe
	
	dbf 		d1,btr_zoom

	movem.l	(sp)+,d0-d7
	rts

bc_y: ds.w	1
bc_x: ds.w	1
x_centre: ds.w	1
y_centre: ds.w	1
y: ds.w	1
x: ds.w	1
larg: ds.w	1
x_dst_skip:	ds.w	1