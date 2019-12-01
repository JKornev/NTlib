.code

capture_context64 proc
	ret
capture_context64 endp

extern NtCrtReportGSFailure:proc
extern __security_cookie:qword

; ======================
;  From CRT source

__security_check_cookie proc
        cmp rcx, __security_cookie      ; check cookie value in frame
        jne ReportFailure               ; if ne, cookie check failure
        rol rcx, 16                     ; make sure high word is zero
        test cx, -1
        jne RestoreRcx
        db 0f3h                         ; (encode REP for REP RET)
        ret                             ; no overrun, use REP RET to avoid AMD
                                        ; branch prediction flaw after Jcc
; The cookie check failed.
RestoreRcx:
        ror rcx, 16
ReportFailure:
        jmp NtCrtReportGSFailure        ; overrun found
__security_check_cookie endp


_PAGESIZE_      equ     1000h

public  _alloca_probe

__chkstk proc
_alloca_probe    =  __chkstk

; Save r10, r11
	sub         rsp, 10h  
	mov         qword ptr [rsp], r10  
	mov         qword ptr [rsp+8], r11  
; Make 0
	xor         r11, r11  
; Calc stack peak
	lea         r10, [rsp+18h]  
	sub         r10, rax  
; Access it
	cmovb       r10, r11  
; Check is it under stack allocation
	mov         r11, qword ptr gs:[10h]  
	cmp         r10, r11  
	jae         __chkstk_return
; Access each page on stack for initialization purpose
	and         r10w, 0F000h  
__chkstk_loop:
	lea         r11,[r11-_PAGESIZE_]  
	mov         byte ptr [r11],0  
	cmp         r10,r11  
	jne         __chkstk_loop

__chkstk_return:
	mov         r10,qword ptr [rsp]  
	mov         r11,qword ptr [rsp+8]  
	add         rsp,10h  
	ret 
__chkstk endp


extern _RTC_Failure:proc

capture_context86 PROC
; TODO:
	ret
capture_context86 ENDP

public _RTC_CheckEsp
_RTC_CheckEsp:
    jne _RTC_CheckEsp_failed
	ret

_RTC_CheckEsp_failed:

    ret


end