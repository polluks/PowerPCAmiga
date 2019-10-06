## Copyright (c) 2019 Dennis van der Boon
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.

.include    constantsppc.i

.global     _setupPPC, _Reset, _SetIdle


.section "setupppc","acrx"

            bl	.SkipCom		#0x3000	System initialization

.long		0					#Used for initial communication
.long		0					#MemStart
.long		0					#MemLen
.long		0					#RTGBase
.long		0					#RTGLen
.long		0					#RTGType
.long		0					#RTGConfig
.long		0					#Options1
.long		0					#Options2
.long		0					#Options3
.long		0					#XMPI Address
.long		0					#StartBAT
.long		0					#SizeBAT

.SkipCom:	mflr	r3
		subi    r3,r3,4
		subi    r1,r3,256
		b   _setupPPC
		
_Reset:		mflr    r6
		mfmsr	r5
		andi.	r5,r5,PSL_IP
		mtmsr	r5				#Clear MSR, keep Interrupt Prefix for now
		isync
							    #Zero-out registers
		li      r3,0
		mtsprg0	r3
		mtsprg1 r3
		mtsprg2	r3
		mtsprg3	r3

		loadreg	r3,HID0_NHR		#Set HID0 to known state
		mfspr	r4,HID0
		and     r3,r4,r3 		#Clear other bits
		mtspr	HID0,r3
		sync

		loadreg	r3,PSL_FP	    #Set MPU/MSR to a known state. Turn on FP
		or      r3,r5,r3
		mtmsr 	r3
		isync
						    	#Init the floating point control/status register
	 	mtfsfi  7,0
		mtfsfi  6,0
		mtfsfi  5,0
		mtfsfi  4,0
		mtfsfi  3,0
		mtfsfi  2,0
		mtfsfi  1,0
		mtfsfi  0,0
		isync
						    	#Initialize floating point data regs to known state
		bl	ifpdr_value

.long	0x3f800000				#Value of 1.0

ifpdr_value:	mflr	r3
		lfs     f0,0(r3)
		lfs     f1,0(r3)
		lfs     f2,0(r3)
		lfs     f3,0(r3)
		lfs     f4,0(r3)
		lfs     f5,0(r3)
		lfs     f6,0(r3)
		lfs     f7,0(r3)
		lfs     f8,0(r3)
		lfs     f9,0(r3)
		lfs     f10,0(r3)
		lfs     f11,0(r3)
		lfs     f12,0(r3)
		lfs     f13,0(r3)
		lfs     f14,0(r3)
		lfs     f15,0(r3)
		lfs     f16,0(r3)
		lfs     f17,0(r3)
		lfs     f18,0(r3)
		lfs     f19,0(r3)
		lfs     f20,0(r3)
		lfs     f21,0(r3)
		lfs     f22,0(r3)
		lfs     f23,0(r3)
		lfs     f24,0(r3)
		lfs     f25,0(r3)
		lfs     f26,0(r3)
		lfs     f27,0(r3)
		lfs     f28,0(r3)
		lfs     f29,0(r3)
		lfs     f30,0(r3)
		lfs     f31,0(r3)
		sync
							#Clear BAT and Segment mapping registers
		li      r3,0
        	mtibatu 0,r3
        	mtibatu 1,r3
        	mtibatu 2,r3
        	mtibatu 3,r3
        	mtibatl 0,r3
        	mtibatl 1,r3
        	mtibatl 2,r3
        	mtibatl 3,r3
        	mtdbatu 0,r3
        	mtdbatu 1,r3
        	mtdbatu 2,r3
        	mtdbatu 3,r3
        	mtdbatl 0,r3
        	mtdbatl 1,r3
        	mtdbatl 2,r3
        	mtdbatl 3,r3

		mfpvr	r4
		rlwinm	r5,r4,16,16,31
		cmplwi	r5,0x8083
		beq     .ExtraBats

		rlwinm	r5,r4,8,24,31
		cmpwi	r5,0x70
		bne     .SkipExtraBats

.ExtraBats:	mtspr	ibat4u,r3
		mtspr	ibat5u,r3
		mtspr	ibat6u,r3
		mtspr	ibat7u,r3
		mtspr	ibat4l,r3
		mtspr	ibat5l,r3
		mtspr	ibat6l,r3
		mtspr	ibat7l,r3
		mtspr	dbat4u,r3
		mtspr	dbat5u,r3
		mtspr	dbat6u,r3
		mtspr	dbat7u,r3
		mtspr	dbat4l,r3
		mtspr	dbat5l,r3
		mtspr	dbat6l,r3
		mtspr	dbat7l,r3

.SkipExtraBats:	isync
		sync
		sync

		mtsr	0,r3
		mtsr	1,r3
		mtsr	2,r3
		mtsr	3,r3
		mtsr	4,r3
		mtsr	5,r3
		mtsr	6,r3
		mtsr	7,r3
		mtsr	8,r3
		mtsr	9,r3
		mtsr	10,r3
		mtsr	11,r3
		mtsr	12,r3
		mtsr	13,r3
		mtsr	14,r3
		mtsr	15,r3
		
		isync
		sync
		sync

		mtlr	r6
		blr

#*********************************************************

_SetIdle:      	mflr    r4
        	bl      .End

.Start:		nop
		b      .Start

.End:		mflr	r3
        	mtlr    r4
        	blr

#*********************************************************
