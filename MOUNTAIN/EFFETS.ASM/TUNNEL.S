;
; VLM EXAMPLE MODULE                                             NO/Escape'98
;

                ;OUTPUT 'E:\VOODOO\VLM\EXAMPLE.VLM'
                TEXT

*** VLM STRUCTURE **************************************************************

                DC.B "VLM2"     ;vlm module type
                DC.L infotext   ;pointer to infotext
                DC.L settings   ;pointer to settings-structure
                DC.L init       ;pointer to init routine
                DC.L deinit     ;pointer to deinit routine
                DC.L main       ;pointer to mainloop routine

;SERVICE ROUTINES PROVIDED BY THE CALLING PROGRAM

set_vblrout:
;a0: new vbl subroutine
                movea.l service_struct,A1
                movea.l (A1),A1
                jsr     (A1)
                rts

wait_vbl:
                movea.l service_struct,A1
                movea.l 4(A1),A1
                jsr     (A1)
                rts

set_scradr:
;a0: new screen adress
                movea.l service_struct,A1
                movea.l 8(A1),A1
                jsr     (A1)
                rts

set_resolution:
;d0: number of the wanted resolution
                movea.l service_struct,A1
                movea.l 12(A1),A1
                jsr     (A1)
                rts

get_left_spec:
                movea.l service_struct,A1
                movea.l 16(A1),A1
                jsr     (A1)            ;returns in a0 the adress of left spec
                rts

get_right_spec:
                movea.l service_struct,A1
                movea.l 20(A1),A1
                jsr     (A1)            ;returns in a0 the adress of right spec
                rts

get_left_volume:
                movea.l service_struct,A1
                movea.l 24(A1),A1
                jsr     (A1)            ;returns in d0 the left volume value
                rts

get_right_volume:
                movea.l service_struct,A1
                movea.l 28(A1),A1
                jsr     (A1)            ;returns in d0 the right volume value
                rts

get_left_osci:
                movea.l service_struct,A1
                movea.l 32(A1),A1
                jsr     (A1)            ;returns in a0 the adress of osci data
                rts

get_right_osci:
                movea.l service_struct,A1
                movea.l 36(A1),A1
                jsr     (A1)            ;returns in a0 the adress of osci data
                rts

service_struct: DC.L 0          ;must be set in 'init'

********************************************************************************
infotext:       
                DC.B 'tunnel scroller',0
                DC.B 'author: evil ',0
                DC.B 'version: 1.0',0
                DC.B 'date:    11.06.99',0
                DC.B 0
                EVEN

settings:       dc.l	7

		dc.l	scrname
		dc.l	1
screnabled:	dc.l	1
		dc.l	0


		dc.l	rotname
		dc.l	1
rotenabled:	dc.l	1
		dc.l	0

		dc.l	scr_speedname
		dc.l	3
scr_speed:	dc.l	32
		dc.l	scr_speedstruct

		dc.l	rot_speedname
		dc.l	3
rot_speed:	dc.l	2048
		dc.l	rot_speedstruct


		dc.l	rottriggername
		dc.l	3
rottrigval:	dc.l	14000
		dc.l	rottrigstruct

		dc.l	scrtriggername
		dc.l	3
scrtrigval:	dc.l	14000
		dc.l	scrtrigstruct


		dc.l	timeslicename
		dc.l	3
timeslice:	dc.l	4
		dc.l	timeslicestr


timeslicestr:	dc.l	1,20
rottrigstruct:	dc.l	1000,20000
scrtrigstruct:	dc.l	1000,20000
scr_speedstruct:dc.l	0,128
rot_speedstruct:dc.l	0,16384


timeslicename:	dc.b	'TOGGLE TIMESLICE:',0
		even
scr_speedname:	dc.b	'SCROLL SPEED:',0
		even
rot_speedname:	dc.b	'ROTATION SPEED:',0
		even

rottriggername:	dc.b	'ROTATION TRIGGER:',0
		even
scrtriggername:	dc.b	'SCROLL TRIGGER:',0
		even
rotname:	dc.b	'TOGGLE ROTATION: ',0
		even
scrname:	dc.b	'TOGGLE SCROLLDIR:',0
		even



init:           
;a0: service structure
                move.l  A0,service_struct

		bsr.w	tunnel_init
		bsr.w	clear_screens

                movea.l scradr2,A0      ;set new screen adress
                bsr     set_scradr

                move.l  #2,D0           ;set resolution to 320x100
                bsr     set_resolution
                rts
deinit:         
                rts
main:           
                jsr     wait_vbl
                jsr     switch_screens

		bsr.w	tunnel_main

                rts                     ;end of the mainroutine

clear_screens:
		move.l	scradr1,a0
		move.l	scradr2,a1
		move.l	scradr3,a2
		move.w	#320*100*2/64-1,d7
.loop:		rept	16
		clr.l	(a0)+
		clr.l	(a1)+
		clr.l	(a2)+
		endr
		dbra	d7,.loop
		rts



tunnel_main:

		subq.l	#1,countdown
		bne.w	.done

		move.l	timeslice,countdown



.leftcol:	tst.l	screnabled
		beq.s	.rightvol

		bsr.w	get_left_volume
		move.l	scrtrigval,d1
		cmp.l	d1,d0
		bgt.s	.changescr
		bra.s	.rightvol
.changescr:	not.w	scrdir
.rightvol:	tst.l	rotenabled
		beq.s	.done

		bsr.w	get_right_volume
		move.l	rottrigval,d1
		cmp.l	d1,d0
		bgt.s	.changerot
		bra.s	.done
.changerot:	not.w	rotdir

.done:
		tst.w	scrdir
		beq.s	.scrollsub
		move.l	scr_speed,d0
		bclr	#0,d0
		add.l	d0,tunnel_scrl		;scroll
		bra.s	.scrolldone
.scrollsub:	move.l	scr_speed,d0
		bclr	#0,d0
		sub.l	d0,tunnel_scrl		;scroll


.scrolldone:	tst.w	rotdir
		beq.s	.rotsub
		move.l	rot_speed,d0
		and.l	#%11111111111111111111110000000000,d0
		add.l	d0,tunnel_scrl	;rot
		bra.s	.alldone
.rotsub:	move.l	rot_speed,d0
		and.l	#%11111111111111111111110000000000,d0
		sub.l	d0,tunnel_scrl	;rot

.alldone:

		and.l	#$1ffff,tunnel_scrl

		move.l	scradr1,a0
		lea.l	64(a0),a0
		lea.l	tunnel_lut,a1
		lea.l	tunnel_txt,a2
		add.l	tunnel_scrl,a2

		clr.l	d0
		
		move.w	#100-1,d7
yloop:		swap	d7
		move.w	#128/16-1,d7
xloop:
		rept	16
		move.w	(a1)+,d0
		move.l	(a2,d0.l*2),(a0)+
		endr


		dbra	d7,xloop
		lea.l	128(a0),a0
		swap	d7
		dbra	d7,yloop


		rts



countdown:	dc.l	1
tunnel_scrl:	ds.l	1
scrdir:		ds.w	1
rotdir:		ds.w	1



tunnel_init:
		lea.l	tunnel_rawtxt,a0
		lea.l	tunnel_txt,a1
		lea.l	tunnel_txt+512,a2
		lea.l	tunnel_txt+512*255,a3
		lea.l	tunnel_txt+512*255+512,a4

		move.w	#128-1,d7
.loop:		move.w	#128-1,d6
.loop2:		move.w	(a0)+,d0
		move.w	d0,(a1)+
		move.w	d0,-(a2)
		move.w	d0,(a3)+
		move.w	d0,-(a4)

		dbra	d6,.loop2
		lea.l	256(a1),a1
		lea.l	768(a2),a2
		lea.l	-768(a3),a3
		lea.l	-256(a4),a4
		dbra	d7,.loop


		lea.l	tunnel_txt,a0
		lea.l	tunnel_txt+256*256*2,a1
		move.w	#256*256*2/64-1,d7
.loop3:		rept	16
		move.l	(a0)+,(a1)+
		endr
		dbra	d7,.loop3
		rts

		even

tunnel_lut:	incbin	'tunnel.dat'
		even

tunnel_rawtxt:	incbin	'tunnel.16b'

		even









;ROUTINE SWITCHES VISIBLE SCREEN AND CYCLES THE SCREENADRESSES
switch_screens:
                move.l  scradr3,D0
                move.l  scradr2,scradr3
                move.l  scradr1,scradr2
                move.l  D0,scradr1
                movea.l scradr2,A0
                jsr     set_scradr
                rts



scradr1:        Dc.L scr1+256
scradr2:        Dc.L scr2+256
scradr3:        Dc.L scr3+256

                BSS

tunnel_txt:	ds.w	256*512


scr1:		ds.w	320*100
scr2:		ds.w	320*100
scr3:		ds.w	320*100

                END
