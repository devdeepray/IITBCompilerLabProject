void main(){
pushi(ebp);
move(esp,ebp);
addi(-48,esp);
storei(0, ind(ebp, -4));
label0:
loadi(ind(ebp,-4),eax);
cmpi(10, eax);
jle(label3);
label1:
loadi(ind(ebp,-4),eax);
addi(1, eax);
loadi(ind(ebp,-4),ebx);
muli(I*1, ebx);
addi(ebp, ebx);
storei(eax, ind(ebx,-48));
label2:
loadi(ind(ebp,-4), eax);
addi(1,eax);
storei( eax,ind(ebp,-4));
j(label0);
label3:
storei(0, ind(ebp, -4));
label4:
loadi(ind(ebp,-4),eax);
cmpi(5, eax);
jle(label7);
label5:
loadi(ind(ebp,-4),eax);
muli(-1,eax);
addi(9, eax);
muli(I*1, eax);
addi(ebp, eax);
loadi(ind(eax,-48), eax);
storei(eax, ind(ebp, -8));
loadi(ind(ebp,-4),eax);
muli(I*1, eax);
addi(ebp, eax);
loadi(ind(eax,-48), eax);
loadi(ind(ebp,-4),ebx);
muli(-1,ebx);
addi(9, ebx);
muli(I*1, ebx);
addi(ebp, ebx);
storei(eax, ind(ebx,-48));
loadi(ind(ebp,-4),ebx);
muli(I*1, ebx);
addi(ebp, ebx);
loadi(ind(ebp,-8),eax);
storei(eax, ind(ebx,-48));
label6:
loadi(ind(ebp,-4), eax);
addi(1,eax);
storei( eax,ind(ebp,-4));
j(label4);
label7:
storei(0, ind(ebp, -4));
label8:
loadi(ind(ebp,-4),eax);
cmpi(10, eax);
jle(label11);
label9:
print_string("");
loadi(ind(ebp,-4),eax);
muli(I*1, eax);
addi(ebp, eax);
loadi(ind(eax,-48), eax);
print_int(eax);
print_string(" \n");
label10:
loadi(ind(ebp,-4), eax);
addi(1,eax);
storei( eax,ind(ebp,-4));
j(label8);
label11:
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
}
