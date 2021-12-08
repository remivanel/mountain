	EXPORT fli_delta
	EXPORT fli_lc
	EXPORT fli_brun
	EXPORT fli_color_64
	EXPORT fli_color_256
	EXPORT decompi
	IMPORT pal
		
MACRO imovew p1, p2
	move.w	p1,p2
	rol.w		#8,p2
	ENDM

MACRO imovel p1, p2
	move.l	p1,p2
	rol.w	#8,p2
	swap	p2
	rol.w	#8,p2
	ENDM

		
;FLI COLOR 64
;A0 -> ptr buffer FLI
;A3 -> ptr palette TC 16bit
;*******
;D0 -> packet
;D1 -> bcl packet
;D2 -> saut de coul / au debut
;D3 -> nb coul a changer
;D4 -> pixel de couleur
;D5 -> tempo
;D6 -> 10

fli_color_64:
	movem.l	d0-d6/a0-a3,-(sp)	; save context	
	
	imovew	(a0)+,d0		; nb packets
	beq		fin_fli_color_64
	subq		#1,d0			; pour le DBF
	moveq		#0,d1
	moveq		#10,d6
	
bcl_pac_fli64:
	moveq		#0,d2			; init
	moveq		#0,d3

	move.b	(a0)+,d2		;saut de couleur
	move.b	(a0)+,d3		;nb coul a changer
	sub.b		#1,d3			; pour dbf

	add.w		d2,d2			; on saut une pal 16bits
	add.w		d2,a3			; le saut
	
bcl_copy_fli64:
	moveq		#0,d4
	
	move.b	(a0)+,d4			/* le rouge */
	lsl.w		d6,d4				/* sur 5 bits */
	and.w		#$f800,d4

	moveq		#0,d5
	move.b	(a0)+,d5			/* le vert */
	lsl.w		#5,d5				/* sur 6 bits au milieu */
	and.w		#$7e0,d5
	or.w		d5,d4
			
	moveq		#0,d5
	move.b	(a0)+,d5			/* le bleu */
	lsr.b		#1,d5				/* sur 5 bits */
	and.w		#$1f,d5
	or.w		d5,d4

	move.w	d4,(a3)+
	
	dbf		d3,bcl_copy_fli64
	dbf		d0,bcl_pac_fli64
	
fin_fli_color_64:
	movem.l	(sp)+,d0-d6/a0-a3
	rts

;FLI COLOR 256
;A0 -> ptr buffer FLI
;A3 -> ptr palette TC 16bit
;*******
;D0 -> packet
;D1 -> bcl packet
;D2 -> saut de coul / au debut
;D3 -> nb coul a changer
;D4 -> pixel de couleur
;D5 -> tempo
;D6 -> 8

fli_color_256:
	movem.l	d0-d6/a0-a3,-(sp)	; save context	
	
	imovew	(a0)+,d0		; nb packets
	beq		fin_fli_color_256
	subq		#1,d0			; pour le DBF
	moveq		#0,d1
	moveq		#8,d6
	
bcl_pac_fli256:
	moveq		#0,d2			; init
	moveq		#0,d3

	move.b	(a0)+,d2		;saut de couleur
	move.b	(a0)+,d3		;nb coul a changer
	sub.b		#1,d3			; pour dbf

	add.w		d2,d2			; on saut une pal 16bits
	add.w		d2,a3			; le saut
	
bcl_copy_fli256:
	moveq		#0,d4
	
	move.b	(a0)+,d4			/* le rouge */
	lsl.w		d6,d4				/* sur 5 bits */
	and.w		#$f800,d4

	moveq		#0,d5
	move.b	(a0)+,d5			/* le vert */
	lsl.w		#3,d5				/* sur 6 bits au milieu */
	and.w		#$7e0,d5
	or.w		d5,d4
			
	moveq		#0,d5
	move.b	(a0)+,d5			/* le bleu */
	lsr.b		#3,d5				/* sur 5 bits */
	and.w		#$1f,d5
	or.w		d5,d4

	move.w	d4,(a3)+
	
	dbf		d3,bcl_copy_fli256
	dbf		d0,bcl_pac_fli256
	
fin_fli_color_256:
	movem.l	(sp)+,d0-d6/a0-a3
	rts

; FLI_LC
; D0 -> x : largeur d'un image
; D1 -> y : h d'une image  ##inutilise##
; D2 -> x : largeur de l'ecran dest
; A0 -> ptr : pointeur sur le flh
; A1 -> img : destination
; A3 -> pal : destination
;*************************
; D1 -> lignes a decompac‚es
; D2 -> nb de packets
; D5 -> nb pix to skip for each line
; D6 -> x : largeur de l'ecran dest sav
; D7 -> tempo

fli_lc:
	movem.l	d0-d7/a0-a3,-(sp)	; save context	
	
	move.w	d2,d6
	move.w	d2,d5
	sub.w		d0,d5			* nb pix to skip
	add.w		d5,d5
	add.w		d6,d6

	and.l		#$0000ffff,d0
	add.w		d0,d0			* x *2 une fois pour toutes
	imovew	(a0)+,d1  	* nb de lignes a sauter
	mulu.w	d6,d1		* packets * x = octets a sauter
	add.l		d1,a1		* saut
	
	imovew	(a0)+,d1  	* nb de lignes a decomp
	beq		end_ln_lc * ca peut arriver...
	subq.w	#1,d1			* pour le dbf
	
	
loop_lc_lignes:	
	move.l	a1,a2			* sauve les ptr image
	moveq		#0,d2
	move.b	(a0)+,d2		* le word compteur de ligne
;	imovew	(a0)+,d2		* le word compteur de ligne
	beq		ln_lc_saut  * si == 0, on saut la ligne direct

	* on va decompacter les d2 packets
	subq.w	#1,d2			* pour le dbf

looplc_packets:
	moveq		#0,d3
	move.b	(a0)+,d3
	add.w		d3,d3			* nb de pixels a sauter
	add.l		d3,a1			* saut
	
	moveq		#0,d3
	move.b	(a0)+,d3		* compteur rle
	bmi		rle_lc_pack
* now on a d3 pixel a copier direct
	beq		pack_lc_suivant
	subq.w	#1,d3			* pour le dbf

nolcpackpix:
	moveq		#0,d7
	move.b	(a0)+,d7			* pixel en char 256 coul
	move.w	(d7*2,a3),(a1)+
	dbf		d3,nolcpackpix
	jmp		pack_lc_suivant

rle_lc_pack:
	moveq		#0,d4
	sub.b		d3,d4
;	beq		packet_suivant.w			* si flh mal pck
	subq.w	#1,d4			* pour le dbf
	moveq		#0,d3
	move.b	(a0)+,d3
	move.w	(d3*2,a3),d7
unpacklcpix:
	move.w	d7,(a1)+
	dbf		d4,unpacklcpix.w

pack_lc_suivant:
	dbf		d2,looplc_packets.w
	
ln_lc_saut:
	add.l		d0,a2
	move.l	a2,a1		* recale sur la ligne suivante
	
	add.w		d5,a1
	dbf		d1,loop_lc_lignes

end_ln_lc:
	movem.l	(sp)+,d0-d7/a0-a3
	rts

; FLI DELTA
; D0 -> x : largeur d'un image
; D1 -> y : h d'une image  ##inutilise##
; D2 -> x : largeur de l'ecran dest
; A0 -> ptr : pointeur sur le flh
; A1 -> img : destination
; A3 -> pal : destination
;*************************
; D1 -> lignes a decompac‚es
; D2 -> nb de packets
; D5 -> nb pix to skip for each line
; D6 -> x : largeur de l'ecran dest sav
; D7 -> tempo

fli_delta:
	movem.l	d0-d7/a0-a3,-(sp)	; save context	
	
	move.w	d2,d6
	move.w	d2,d5
	sub.w		d0,d5			* nb pix to skip
	add.w		d5,d5
	add.w		d6,d6

	and.l		#$0000ffff,d0
	add.w		d0,d0			* x *2 une fois pour toutes
	imovew	(a0)+,d1  	* nb de lignes a decomp
	beq		end_ln_delta * ca peut arriver...
	subq.w	#1,d1			* pour le dbf
	
	
loop_lignes:	
	imovew	(a0)+,d2
	bmi		ln_saut
	move.l	a1,a2			* sauve les ptr image

	* on va decompacter les d2 packets
	subq.w	#1,d2			* pour le dbf

loop_packets:
	moveq		#0,d3
	move.b	(a0)+,d3
	add.w		d3,d3			* nb de pixels a sauter
	add.l		d3,a1			* saut
	
	moveq		#0,d3
	move.b	(a0)+,d3		* compteur rle
	bmi		rle_pack
* now on a d3*2 pixel a copier direct
	beq		packet_suivant
	add.w		d3,d3
	subq.w	#1,d3			* pour le dbf

nopackpix:
	moveq		#0,d7
	move.b	(a0)+,d7			* pixel en char 256 coul
	move.w	(d7*2,a3),(a1)+
	dbf		d3,nopackpix
	jmp		packet_suivant

rle_pack:
	moveq		#0,d4
	sub.b		d3,d4
;	beq		packet_suivant.w			* si flh mal pck
;	add.w		d4,d4
	subq.w	#1,d4			* pour le dbf
	moveq		#0,d3
	move.b	(a0)+,d3
	move.w	(d3*2,a3),d7
	swap		d7
	move.b	(a0)+,d3
	move.w	(d3*2,a3),d7
unpackpix:
	move.l	d7,(a1)+
	dbf		d4,unpackpix.w

packet_suivant:
	dbf		d2,loop_packets.w
	
	add.l		d0,a2
	move.l	a2,a1		* recale sur la ligne suivante
	
	jmp		ligne_suivante
ln_saut:
	addq.w	#1,d1		* on a pas vraiment decompact‚ des lignes

	move.w	d2,d3
	and.w		#$ff00,d3
	cmp.w		#$8000,d3
	beq		no_saut_ligne		* un code bidon
	moveq		#0,d3		
	sub.w		d2,d3		* enleve le signe
	mulu.w	d6,d3		* packets * x = octets a sauter
	add.l		d3,a1		* saut
no_saut_ligne:	
	dbf		d1,loop_lignes

ligne_suivante:
	add.w		d5,a1
	dbf		d1,loop_lignes

end_ln_delta:
	movem.l	(sp)+,d0-d7/a0-a3
	rts
	
; FLI BRUN
; D0 -> x : largeur d'un image
; D1 -> y : hauteur d'un image
; D2 -> x : largeur de l'ecran dest
; A0 -> ptr : pointeur sur le flh
; A1 -> img : destination
; A3 -> pal : destination
;*************************
; D2 -> nb de packets
; D6 -> tempo
; A2 -> dest + 1 ligne

fli_brun:
	movem.l	d0-d6/a0-a3,-(sp)	; save context	
	
	and.l		#$0000ffff,d0

	move.w	d2,d5
	sub.w		d0,d5			* nb pix to skip
	add.w		d5,d5
	
	add.w		d0,d0			* x *2 une fois pour toutes
	subq.w	#1,d1			* pour le dbf

loop_ligney:
; on dervrait faire :
;	moveq		#0,d2
;	move.b	(a0)+,d2		* nb_de packets
;	bne		no_apex
	addq.l	#1,a0

	* ON CONSIDERE QUE LES ANIM APEX, AVEC TOUJOURS 0 PACKETS
	
	move.l	a1,a2			
	add.w		d0,a2			* a2 contiend la fin de la ligne
	
loop_packety:
	moveq		#0,d3
	move.b	(a0)+,d3		* compteur rle
	bpl		rle_packy
* now on a -d3 pixel a copier direct

	moveq		#0,d4
	sub.b		d3,d4			* d4 = -d3
	subq.w	#1,d4			* pour le dbf
	
nopackpiy:
	moveq		#0,d6
	move.b	(a0)+,d6			* pixel en char 256 coul
	move.w	(d6*2,a3),(a1)+		* pixel pris ds le tabl de pal
	dbf		d4,nopackpiy
	jmp		packet_suivanty

rle_packy:
	subq.w	#1,d3			* pour le dbf
	moveq		#0,d4
	move.b	(a0)+,d4
	move.w	(d4*2,a3),d6
unpackpiy:
	move.w	d6,(a1)+
	dbf		d3,unpackpiy

packet_suivanty:
	cmp.l		a2,a1			* on a fini la ligne ??
	bmi		loop_packety

	move.l	a2,a1		* recale sur la ligne suivante
	
;no_apex:
	add.w		d5,a1
	dbf		d1,loop_ligney
	
	movem.l	(sp)+,d0-d6/a0-a3
	rts
	
	
;DECOMP une frame
; D0-> x
; D1-> y
; D2-> x : largeur de l'ecran dest
;------
; A0-> ptr FLH
; A1-> img

decompi:
	movem.l	d0-d7/a0-a6,-(sp)	; save context	
	
	and.l		#$0000ffff,d0
	and.l		#$0000ffff,d1
	and.l		#$0000ffff,d2
	
	move.l	pal,a3			* la palette
	
;	move.l	a0,a5				* save adr
	imovel	(a0)+,d3			* size frame
;	add.l		d3,a5				* fait point‚ la sauvegarde sur la next frame
	move.w	(a0)+,d6			* magic
	cmp.w		#$faf1,d6
	bne      end_decomp		* maivaise frame

	imovew	(a0)+,d6			* nombre de chunks
	beq		end_decomp		* !!! bug du 0 chunk, assez dangereux
	subq.w	#1,d6				* pour le dbf
	addq.l	#8,a0				* octets reserv‚s
	
loop_chunk:
	move.l	a0,a6				* sav adr chunk
	imovel	(a0)+,d4			* sizechunk
	add.l		d4,a6				* fait point‚ la sauvegarde sur le next chunk
	imovew	(a0)+,d5			* type chunk
	
	
	cmp.w		#15,d5			* fli brun
	bne		decomp_delta
	jsr 		fli_brun
	jmp		end_chunk
	
decomp_delta:
	cmp.w		#7,d5			* fli delta
	bne		color_64
	jsr 		fli_delta

color_64:
	cmp.w		#11,d5			* fli col 64
	bne		color_256
	jsr 		fli_color_64

color_256:
	cmp.w		#4,d5				* fli col 256
	bne		lc
	jsr 		fli_color_256

lc:
	cmp.w		#12,d5				* fli col 256
	bne		end_chunk
	jsr 		fli_lc

	
end_chunk:
	; restore les parms
	move.l	a6,a0				* adr next chunk

	dbf		d6,loop_chunk
	
	
end_decomp:
;	move.l		a5,a0
	movem.l	(sp)+,d0-d7/a0-a6
	rts