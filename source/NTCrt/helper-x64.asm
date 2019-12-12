.code

PAGE_SIZE equ 1000h

extern NtCrtReportGSFailure:proc
extern __security_cookie:qword
extern _RTC_Failure:proc

; ======================
;  _security_check_cookie
__security_check_cookie proc
    cmp rcx, __security_cookie
    jne ReportFailure
    rol rcx, 16
    test cx, -1
    jne RestoreRcx
    db 0f3h                     ; (encode REP for REP RET)
    ret                         ; no overrun, use REP RET to avoid AMD
                                ; branch prediction flaw after Jcc
RestoreRcx:
    ror rcx, 16
ReportFailure:
    jmp NtCrtReportGSFailure
__security_check_cookie endp

; ======================
;  _chkstk
public _RTC_CheckEsp
_RTC_CheckEsp:
    jne _RTC_CheckEsp_failed
	ret

_RTC_CheckEsp_failed:
    ret
    
; ======================
;  _chkstk
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
	lea         r11, [r11 - PAGE_SIZE]  
	mov         byte ptr [r11],0  
	cmp         r10, r11  
	jne         __chkstk_loop

__chkstk_return:
	mov         r10, qword ptr [rsp]  
	mov         r11, qword ptr [rsp+8]  
	add         rsp, 10h  
	ret 
__chkstk endp

; ======================
;  capture_context64
capture_context64 proc
	ret
capture_context64 endp

end