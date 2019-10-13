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

.global     _ExcFPUnav, _ExcAlignment, _ExcInsStor,	 _ExcDatStor, _ExcTrace
.global     _ExcBreakPoint,	_ExcDec, _ExcPrIvate, _ExcMachCheck, _ExcSysCall
.global     _ExcPerfMon, _ExcSysMan, _ExcTherMan, _ExcVMXUnav, _ExcITLBMiss
.global     _ExcDLoadTLBMiss, _ExcDStoreTLBMiss, _ExcExternal

.section "kernel","acrx"

.ExceptionTable:

	        b	_ExcFPUnav		       	   #0
	    	b	_ExcAlignment	       	   #4
	    	b	_ExcInsStor		       	   #8
	    	b	_ExcDatStor		       	   #c
	    	b	_ExcTrace		       	   #10
	    	b	_ExcBreakPoint	       	   #14
	    	b	_ExcDec			    	   #18
	    	b	_ExcPrIvate			       #1c
	    	b	_ExcMachCheck	           #20
	    	b	_ExcSysCall		       	   #24
	    	b	_ExcPerfMon		       	   #28
	    	b	_ExcSysMan	       		   #2c
	    	b	_ExcTherMan	       		   #30
	    	b	_ExcVMXUnav	       		   #34
	    	b	_ExcITLBMiss    		   #38
	    	b	_ExcDLoadTLBMiss		   #3c
	    	b	_ExcDStoreTLBMiss		   #40
            b   _ExcExternal               #44
