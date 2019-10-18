.code

_capture_context64 PROC
	ret
_capture_context64 ENDP

extern __GSHandlerCheckCommon:PROC

; Source: MSCRT.lib
__GSHandlerCheck PROC
	sub rsp, 28h
	mov r8, [r9 + 38h]
	mov rcx, rdx
	mov rdx, r9
	call __GSHandlerCheckCommon
	mov eax, 1h
	add rsp, 28h
	ret
__GSHandlerCheck ENDP

end