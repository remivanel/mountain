; module DL pour MOUNTAIN
; Executable (28 bytes de header)
; entierement relogeable!

	output "DL_1_2.IMP"
	opt P=68000,w+
	
	dc.l 'DL12'		; type
	dc.l '1.00'		; version
	dc.l 788			; taille buffer de travail souhait‚e
          	dc.l 1		;flags

	; en entree A0 pointe sur une structure:
	; 0	WORD handle du fichier
	; 2	LONG taille totale du fichier
	; 6	WORD $FFFF -> pas un fichier DL(erreur), $0000 fichier DL
	; 8	WORD X
	; 10	WORD Y
	; 12	LONG nombre d'images
	; 16	LONG taille buffer demandee pour charger une image
	; 20	LONG adresse buffer de travail (entree)
	; 24	LONG adr id_string
	;
	; donc en entree, seuls les parametres handle et taille sont fournis
	; en sortie, 'init_dl' aura rempli le reste
	
	bra init_dl
	
	; en entree, A0 contient l'adresse d'une structure:
	; 0	WORD handle
	; 2	LONG numero image deja dans buffer
	; 6	LONG numero image demandee
	; 10	LONG adresse du buffer dont la taille a ete specifiee par init_dl
	; 14	LONG adresse ecran (virtuel ou non)
	; 18	WORD largeur ecran
	; 20  LONG buffer de travail
	;
	; en entree ces 4 paramŠtres sont fix‚s, la valeur numero image dans buffer
	; n'est utilisee que si il y a egalite, en effet, il n'y a pas de compression
	; delta ni de key frames.
	;
	; en sortie, le buffer est rempli avec une image au format True Color
	; Falcon: RRRRRVVV VVVBBBBB (2 octets par pixel)
	
	bra read_frame
	bra close_anim				; non utilise
	
	
GEMDOS MACRO    ; fn,stack
   move #\1,-(sp)
   trap #1
   if \2<=8
      addq.l #\2,sp
   else
      add.w #\2,sp
   endif
   ENDM
   
SEEK MACRO	; size, mode (0=start, 1=current, 2=end)
	move #\2,-(sp)			; mode
	move (a6),-(sp)		; handle
	move.l \1,-(sp)		; size (offset)
	move #66,-(sp)			; fseek
	trap #1
	lea 10(sp),sp
	ENDM

INTEL MACRO ; a data register (long)
	rol #8,\1
	swap \1
	rol #8,\1
	ENDM

init_dl:
	movem.l d0-d7/a0-a6,-(sp)
	move.l a0,a6
	move.l 20(a6),a5		; buffer de travail
	bsr dl_read_header
	move d0,6(a6)
	bne.s .end
	move (a5),8(a6)		; x
	move 2(a5),10(a6)		; y
	move.l #64000,16(a6)
	bsr load_dl_palette
	move.l 8(a5),d0		; size of screens
	SEEK d0,1				; skip them
	bsr dl_frame_table	; computes the max image number
	addq.l #1,d4			; number, not index!
	move.l d4,4(a5)
	move.l d4,12(a6)		; sample num
.end:
	movem.l (sp)+,d0-d7/a0-a6
	rts	

dl_read_header:
	SEEK #0,0		; start of file
	lea 20(a5),a3	; 768 bytes free (for palette)
	moveq #64,d3
	pea (a3)
	move.l d3,-(sp)
	move (a6),-(sp)
	GEMDOS 63,12
	cmp d0,d3
	beq.s .ok
.bug:
	moveq #-1,d0
	rts
.ok:
	moveq #0,d3
	move.b (a3)+,d3		; 1 or 2
	move d3,18(a5)			; file type
	lea file_type1(pc),a0
	cmp.b #1,d3
	bne.s .lb0
	moveq #1,d4				; 160x100
	moveq #24,d0			; header size
	bra.s .lb1
.lb0:
	lea file_type2(pc),a0
	cmp.b #2,d3
	bne.s .bug
	moveq #0,d4
	move.b (a3)+,d4
	moveq #47,d0			; header size
	lea 20(a3),a3			; one zone
.lb1:
	move.l a0,24(a6)		; string ID
	lea 20(a3),a3			; second zone
	moveq #0,d5
	move.b (a3)+,d5		; screens
	muls #250,d5
	lsl.l #8,d5				; size of screens (250x256=64000/scr)
	move.l d5,8(a5)
	move.l d0,14(a5)		; offset for palette
	add.l #768,d0			; plus the palette
	add.l d5,d0				; plus the screens
	cmp.l 2(a6),d5
	bpl.s .bug				; not enough bytes!
	cmp.b #1,d3
	bne.s .long_fr
	move (a3)+,d0
	ror #8,d0
	bra.s .common
.long_fr:
	move.l (a3)+,d0
	INTEL d0
.common:
	move.l d0,4(a5)		; virtual number of images
	move #320,d0
	move #200,d1
	moveq #1,d2				; one image per screen
	subq #1,d4
	bmi.s .good_size
	lsr d0
	lsr d1
	moveq #4,d2				; 4 images per screen
	subq #1,d4
	bmi.s .good_size
	lsr d0
	lsr d1
	moveq #16,d2			; 16 images per screen
.good_size:
	move d0,(a5)
	move d1,2(a5)
	move d2,12(a5)			; images per screen
	moveq #0,d0
	rts	
	
load_dl_palette:
	move.l 14(a5),d3
	SEEK d3,0	; start of palette
	move.l #768,d0
	add.l d0,d3	; new offset
	move.l d3,14(a5)
	lea 20(a5),a3	; palette
	pea (a3)
	move.l d0,-(sp)
	move (a6),-(sp)
	GEMDOS 63,12
	move.l a3,a0
	move.l a3,a1
	move #255,d0
.lb0:
	move.b (a0)+,d1
	lsl #7,d1
	move.b (a0)+,d1
	lsl.b #2,d1
	lsl.l #6,d1
	move.b (a0)+,d1
	lsl.b #2,d1
	lsr.l #3,d1
	move d1,(a1)+
	dbf d0,.lb0
	rts

dl_frame_table:
	moveq #0,d4			; max frame number
	lea 4(a5),a3		; as a buffer
	move.l (a3),d3		; images number (virtual)
	cmp #1,18(a5)
	beq.s .lb0
.lb1:
	pea (a3)
	moveq #2,d0
	move.l d0,-(sp)
	move (a6),-(sp)
	GEMDOS 63,12
	move (a3),d0
	ror #8,d0						; intel!!
	bmi.s .not_plus				; command ($FFFF, $FFFE, etc...)
	cmp d4,d0
	bmi.s .not_plus
	move d0,d4
.not_plus:
	subq.l #1,d3
	bne.s .lb1
	rts
.lb0:
	pea (a3)
	moveq #1,d0
	move.l d0,-(sp)
	move (a6),-(sp)
	GEMDOS 63,12
	moveq #0,d1
	move.b (a3),d1
	divs #10,d1
	subq #1,d1
	move d1,d0						; nø of the screen
	swap d1
	subq #1,d1						; nø of image inside screen
	muls 12(a5),d0
	add d1,d0						; global nø of image
	cmp d4,d0
	bmi.s .not_plus2
	move d0,d4
.not_plus2:
	subq.l #1,d3
	bne.s .lb0
	rts

read_frame:
	movem.l d0-d7/a0-a6,-(sp)
	move.l a0,a6
	move.l 20(a6),a5		; buffer de travail
	move.w #0,24(a6)		; par defaut, tout va bien
	move.l 2(a6),d1		; old image
	move.l 6(a6),d0		; new
	cmp.l d0,d1
	beq.s .end				; yet in buffer, nothing to load!!!
	bsr dl_read_data
.end:
	movem.l (sp)+,d0-d7/a0-a6
	rts	

dl_read_data:
	move 12(a5),d3	; i per screen
	divs d3,d0	; screen and index
	moveq #0,d1
	move d0,d1
	swap d0
	move d0,-(sp)
	muls #250,d1
	lsl.l #8,d1		; offset for that screen
	add.l 14(a5),d1	; offset in file!
	move.l 10(a6),a3	; buffer
	SEEK d1,0			; good position
	pea (a3)
	move.l #64000,-(sp)
	move (a6),-(sp)
	GEMDOS 63,12		; load one image
	move (sp)+,d0
	lea .offsets(pc),a0
	cmp #16,d3
	bne.s .lb1
	lea 16(a0),a0
.lb1:
	add d0,d0
	add d0,d0
	add.l 0(a0,d0.w),a3	; absolute address in buffer
	move.l 14(a6),a4		; screen ptr
	move 18(a6),d7			; screen W
	move (a5),d4			; x
	sub d4,d7
	add d7,d7				; screen inc for one line
	move #320,d5
	sub d4,d5				; buffer inc for one line
	move 2(a5),d6			; y
	subq #1,d6				; for the loop
	lea 20(a5),a2			; the palette
	lsr #2,d4
	subq #1,d4				; 4 pixels at a time
	moveq #0,d0
.line:
	move d4,d1
.pixels:
	rept 4
		moveq #0,d0
		move.b (a3)+,d0
		add d0,d0
		move 0(a2,d0.w),(a4)+
	endr
	dbf d1,.pixels
	add d5,a3
	add d7,a4
	dbf d6,.line
	rts
.offsets: dc.l 0,160,32000,32160
.offsets2: dc.l 0,80,160,240,16000,16080,16160,16240
			  dc.l 32000,32080,32160,32240,48000,48080,48160,48240	


close_anim:
	rts

	data

file_type1: dc.b "DL type 1",0,"          "
file_type2: dc.b "DL type 2",0,"          "	

	; buffer de travail (788 octets)
	; 0	max_imagex: dc.w 1
	; 2	max_imagey: dc.w 1
	; 4	sample_num: dc.l 1
	; 8	size_screens: dc.l 1
	; 12	dl_i_per_screen: dc.w 1
	; 14	sound_seek: dc.l 1
	; 18	file_type: dc.w 1
	; 20	cvid_fix0: dcb.b 768,0

	end