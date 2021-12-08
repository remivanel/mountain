;      /| __    _    /  __      
;     / |/ _ __| \  <  <    _   
;    /__|\ // _| / / \  \ |/ \  |
;   <   | \ \ /|< <_  >  >|\_/|\|
;    \  |    \ | \ \ / _/ |   | | 
;     \        |    /
;

	XDEF diffusion

;----------------------------------
; DIFFUUSI FADE VEKE
;----------------------------------
diffusion:
	movem.l	d0-d7/a0-a6,-(sp)	; save context	
	move.w	d0,d7
	lsl.w		#1,d7
	move.w	d7,size_2x
	move.w	d1,d7
	lsl.w		#2,d7
	move.w	d7,size_4y
	subq.w	#1,d0
	subq.w	#1,d1
	move.w	d0,size_x
	move.w	d1,size_y

  MOVEQ   #0,d0
  MOVEQ   #0,d1
;  MOVEQ   #0,d2
; MOVEQ   #0,d3
  lsl.l   #1,d2
  move.w  d2,d3
  swap    d2

  LEA     wxtable,a1
  LEA     wytable,a2
  LEA     wrndxtable,a3
  LEA     wrndytable,a4

  MOVE.w  size_y,d7
wdiff_yloop:
  SWAP    d7
  MOVE.w  size_x,d7
wdiff_xloop:
  MOVE.w  0(a1,d0.w),a5       ;x offset
  ADD.l   0(a2,d1.w),a5       ;y  offset
  ADD.l   a0,a5               ;current point

  MOVE.l  a5,a6
  MOVEQ   #0,d4
  move.w  0(a3,d2.w),d4       ;x rnd offset
  ADD.w   0(a4,d3.w),d4       ;y rnd offset
  EXT.l	  d4
  ADD.l   d4,a6               ;other point

  MOVE.w  (a5),d4             ;halve current colour
  LSR.w   #1,d4
  AND.w   #$7bef,d4
  MOVE.w  d4,(a5)

  MOVE.w  (a6),d5             ;add to other colour
  LSR.w   #1,d5
  AND.w   #$7bef,d5
  ADD.w   d4,d5
  MOVE.w  d5,(a6)

  ADDQ    #2,d0
  ADDQ    #4,d1
  ADDQ    #2,d2
  ADDQ    #2,d3

;  CMP.w   size_2x,d0
  CMP.w   #320*2,d0
  BNE     wno_xptr_loop
  MOVEQ   #0,d0
wno_xptr_loop:
;  CMP.w   size_4y,d1
  CMP.w   #253*4,d1
  BNE     wno_yptr_loop
  MOVEQ   #0,d1
wno_yptr_loop:
  CMP.w   #59*2,d2
  BNE     wno_xrndptr_loop
  MOVEQ   #0,d2
wno_xrndptr_loop:
  CMP.w   #53*2,d3
  BNE     wno_yrndptr_loop
  MOVEQ   #0,d3
wno_yrndptr_loop:

  DBRA    d7,wdiff_xloop
  SWAP    d7
  DBRA    d7,wdiff_yloop
	movem.l	(sp)+,d0-d7/a0-a6	; restore context	
  RTS

wxtable:
  IncBin "difxtabl.bin"
wytable:
  IncBin "difytabl.bin"
wrndxtable:
  IncBin "rndxtabl.bin"
wrndytable:
  IncBin "rndytabl.bin"

size_x:
	ds.w	1
size_2x:
	ds.w	1
size_y:
	ds.w	1
size_4y:
	ds.w	1
	