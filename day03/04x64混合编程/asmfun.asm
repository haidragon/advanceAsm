
.code

;加法操作 rcx:参数1，rdx：参数2
add_fun proc
	xor rax,rax
	mov rax,rcx
	add rax,rdx
	ret 
add_fun endp

end



