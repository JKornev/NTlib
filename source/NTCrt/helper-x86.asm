.386
.model flat, c

.code

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
    push ebp
    mov ebp, esp
    pusha

    push 0
    push dword ptr [esp + 4]
    call _RTC_Failure
    add esp, 8

    popa
    pop ebp
    ret

end