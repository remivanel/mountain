MODE68000 = 1

EXPORT asm_motif
EXPORT conv_color
EXPORT conv_col
EXPORT conv2_col
EXPORT get_mem
EXPORT Save_stack
EXPORT Put_stack
EXPORT MPL_calc
EXPORT back_memcpy
EXPORT trouve_text
EXPORT decode_text
EXPORT trouve_debut
EXPORT checkcode
EXPORT uncrypt
EXPORT boot_serial
EXPORT get_tt_ram
EXPORT get_timerC

IMPORT mountain_stack
IMPORT mpl_stack
IMPORT calc_for_mpl

_BOOTDEV EQU $446

get_timerC:
	;sup exec de la fonction
	pea 		timeC
	move.w	#38,-(sp)		* appel en sup_exec a timeC
	trap		#14
	addq.l	#6,sp
	rts
timeC:	
	move.l	$4ba,d0 		; prend le timer C
	rts



;a0 -> code
checkcode:
	movem.l	d1-d6/a0,-(sp)	; save context	
	move.l	d0,d1
	moveq		#0,d0
		
crcc_bcl:
	move.l	(a0)+,d2
	add.l		d2,d0
	dbf		d1,crcc_bcl
	
	movem.l	(sp)+,d1-d6/a0
	rts

* code la partie logiciel avec la cle en D0
* taille de d1
uncrypt:
	movem.l	d0-d7/a0-a4,-(sp)	; save context	
	
code_bcl:
	eor.l		d0,(a0)
	move.l	(a0)+,d2
	eor.l		d2,d0
	dbf		d1,code_bcl
	
	movem.l	(sp)+,d0-d7/a0-a4
rts

;a0 : adr_mod
trouve_debut:
	move.b	(a0)+,d0
	cmp.b		#'M',d0
	bne		trouve_debut
	move.b	(a0)+,d0
	cmp.b		#'N',d0
	bne		trouve_debut
	move.b	(a0)+,d0
	cmp.b		#'T',d0
	bne		trouve_debut
	move.b	(a0)+,d0
	cmp.b		#'1',d0
	bne		trouve_debut
	
	move.l	a0,d0
rts	


;a0: text
decode_text:
	movem.l	d0-d7/a0-a4,-(sp)	; save context	
	
	move.w	#10,d1			; LONGEUR PARTIE CODEE/4
	
uncode_bcl:
	eor.l		d0,(a0)+
	dbf		d1,uncode_bcl
	
	movem.l	(sp)+,d0-d7/a0-a4
rts
	
	
;a0 : adr_mod
trouve_text:
	move.b	(a0)+,d0
	cmp.b		#'F',d0
	bne		trouve_text
	move.b	(a0)+,d0
	cmp.b		#'M',d0
	bne		trouve_text
	move.b	(a0)+,d0
	cmp.b		#'T',d0
	bne		trouve_text
	move.b	(a0)+,d0
	cmp.b		#'1',d0
	bne		trouve_text
	
	move.l	a0,d0
rts	

;a0:dest a1:src d0.w;len
back_memcpy:
	tst.w		d0
	bne		go_copy
	rts
go_copy:
	subq.w	#1,d0
	move.b	(d0,a1),(d0,a0)
	tst.w		d0
	bne		go_copy
	rts	
	
Save_stack:
	move.l	sp,mountain_stack
	add.l		#4,mountain_stack
	rts

Put_stack:
	move.l	(sp),d0
	move.l	mountain_stack,sp
	move.l	d0,-(sp)
	rts

MPL_calc:
	move.l	sp,mpl_stack		; prend pile de mpl
	move.l	mountain_stack,sp		; met pile de mnt
	sub.l		#512,sp
	jsr		calc_for_mpl		; appel mnt
	move.l	mpl_stack,sp		; restaure pile de mpl
	rts


get_tt_ram:
	movem.l	d1-d6/a0-a3,-(sp)	; save context	
	move.l	#0,-(sp)
	move.w	#32,-(sp)
	trap		#1				*SUPERVISEUR
	addq.l	#6,sp
	move.l	d0,old_stack

	moveq		#0,d3
	move.l	$5A8,d2
	cmp.l		#$1357BD13,d2
	bne		no_tt
	move.l	$5A4,d3
	beq		no_tt
	sub.l		#$1000000,d3
	
no_tt:
	move.l	old_stack,-(sp)
	move.w	#32,-(sp)
	trap		#1					* UTILISATEUR
	addq.l	#6,sp

	move.l	d3,d0
	movem.l	(sp)+,d1-d6/a0-a3
	rts
	
get_mem:
	;sup exec de la fonction
	pea 	mem
	move.w	#38,-(sp)		* appel en sup_exec a mem
	trap		#14
	addq.l	#6,sp
	
	lsr.w		#4,d0
	
	rts
mem:	
	move.w	$42e,d0
	rts


conv_col:
	movem.l	d1-d6/a0-a3,-(sp)	; save context	
	
	moveq		#8,d6
	
	lsl.w		d6,d0				/* le rouge */
	and.w		#$f800,d0		/* sur 5 bits */

	moveq		#0,d5
	move.b	d1,d5			/* le vert */
	lsl.w		#3,d5				/* sur 6 bits au milieu */
	and.w		#$7e0,d5
	or.w		d5,d0
			
	moveq		#0,d5
	move.b	d2,d5			/* le bleu */
	lsr.b		#3,d5				/* sur 5 bits */
	and.w		#$1f,d5
	or.w		d5,d0

	movem.l	(sp)+,d1-d6/a0-a3
	rts

conv2_col:				/* TC 16 vers  24 bits r v b */
	movem.l	d1-d6/a0-a3,-(sp)	; save context	
	

IFEQ MODE68000
	bfextu	d0{16:5},d1	* le rouge
	bfextu	d0{21:6},d2		; extrait composante verte sauf1
	bfextu	d0{27:5},d3		; extrait composante bleue
ELSE
	move.w	d0,d1
	and.w		#%1111100000000000,d1
	lsr.w		#5,d1
	lsr.w		#6,d1
	move.w	d0,d2
	and.w		#%11111100000,d2
	lsr.w		#5,d2
	move.w	d0,d3
	and.w		#%11111,d3
ENDIF

	lsl.w		#3,d1				; passe en 8 bits
	move.w	d1,(a0)+
	lsl.w		#2,d2				; passe en 8 bits
	move.w	d2,(a0)+
	lsl.w		#3,d3				; passe en 8 bits
	move.w	d3,(a0)+
	
	movem.l	(sp)+,d1-d6/a0-a3
	rts

* CONV COLOR
* a0 -> ptr vers 3 coul rgb
* -> D0 couleur 16 bits

conv_color:
	movem.l	d1-d2/a0,-(sp)	; save context	

	moveq		#0,d0
	moveq		#8,d2
	
	move.w	(a0)+,d0			/* le rouge */
	lsl.w		d2,d0				/* sur 5 bits */
	and.w		#$f800,d0

	moveq		#0,d1
	move.w	(a0)+,d1			/* le vert */
	lsl.w		#3,d1				/* sur 6 bits au milieu */
	and.w		#$7e0,d1
	or.w		d1,d0
			
	moveq		#0,d1
	move.w	(a0)+,d1			/* le bleu */
	lsr.b		#3,d1				/* sur 5 bits */
	and.w		#$1f,d1
	or.w		d1,d0


	movem.l	(sp)+,d1-d2/a0
	rts

* (adr_source,adr_dest,x_src, x_dest)
* d2: position dans src
asm_motif:
	movem.l	d0-d3,-(sp)	; save context	
	
	subq		#1,d1		;pour le dbf
	moveq		#0,d2
	move.w	d0,d3
	add.w		d3,d3		; pour la substution
	subq		#1,d0		;pour le cmp
	
motif:
	move.w	(a0)+,(a1)+	
	addq.w	#1,d2
	cmp.w		d0,d2
	bne		no_modulo
	moveq		#0,d2	
	sub.w		d3,a0
no_modulo:
	dbf 		d1,motif

	movem.l	(sp)+,d0-d3
	rts

boot_serial:

 MOVEM.L D1-D2/A0-A2,-(SP)
 LINK A6,#-16
 	pea 		LECTEUR_BOOT
	move.w	#38,-(sp)		* appel en sup_exec a swap
	trap		#14
	addq.l	#6,sp

	move.w	d0,-(sp)
	move.w	#0,-(sp)
	move.w	#1,-(sp)
	pea.l	BUFFER_LECTURE
	move.w	#0,-(sp)
	move.w	#4,-(sp)
	trap		#13				; RWABS D0,#0,#1,BUFFER_LECTURE,#0	;lecture boot-secteur
	lea	14(sp),sp
 
 EXT.L D0
 BMI.S e1					;erreur
 MOVE.L BUFFER_LECTURE+8,D0;no de serie
 LSR.L #8,D0
e1: 
 UNLK A6
 MOVEM.L (SP)+,D1-D2/A0-A2
 RTS

LECTEUR_BOOT:

 MOVE _BOOTDEV,D0
 RTS
  
BUFFER_LECTURE:
	DS.w 4096

old_stack:	ds.l	1