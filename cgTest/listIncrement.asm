void main(){
pushi(ebp);
move(esp,ebp);
addi(-24,esp);
storei(0, ind(ebp, -24));
label0:
loadi(ind(ebp,-24),eax);
cmpi(5, eax);
jle(label3);
label1:
loadi(ind(ebp,-24),ebx);
muli(I*1, ebx);
addi(ebp, ebx);
loadi(ind(ebp,-24),eax);
storei(eax, ind(ebx,-20));
loadi(ind(ebp,-24),ebx);
muli(I*1, ebx);
addi(ebp, ebx);
loadi(ind(ebx,-20), eax);
addi(1,eax);
storei(eax, ind(ebx,-20));
label2:
loadi(ind(ebp,-24), eax);
addi(1,eax);
storei( eax,ind(ebp,-24));
j(label0);
label3:
storei(0, ind(ebp, -24));
label4:
loadi(ind(ebp,-24),eax);
cmpi(5, eax);
jle(label7);
label5:
print_string("");
loadi(ind(ebp,-24),eax);
muli(I*1, eax);
addi(ebp, eax);
loadi(ind(eax,-20), eax);
print_int(eax);
print_string("\n");
label6:
loadi(ind(ebp,-24), eax);
addi(1,eax);
storei( eax,ind(ebp,-24));
j(label4);
label7:
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
}
