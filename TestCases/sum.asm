void func_0(){
pushi(ebp);
move(esp,ebp);
loadi(ind(ebp,4),eax);
cmpi(0,eax);
je(label1);
label0:
loadi(ind(ebp,4),eax);
pushi(eax);
pushi(0);
loadi(ind(ebp,4),eax);
pushi(eax);
pushi(1);
loadi(ind(esp, 0), eax);
loadi(ind(esp, I), ebx);
muli(-1, eax);
addi(ebx, eax);
popi(2);
pushi(eax);
func_0();
popi();
loadi(ind(esp, 0), eax);
loadi(ind(esp, I), ebx);
addi(ebx, eax);
popi(2);
pushi(eax);
loadi(ind(esp, 0), eax);
storei(eax, ind(ebp,8));
move(ebp,esp)
addi(I,esp)
loadi(ind(ebp, 0), ebp);
return;
j(label2);
label1:
pushi(0);
loadi(ind(esp, 0), eax);
storei(eax, ind(ebp,8));
move(ebp,esp)
addi(I,esp)
loadi(ind(ebp, 0), ebp);
return;
label2:
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
}
void main(){
pushi(ebp);
move(esp,ebp);
pushi(0);
pushi(10);
func_0();
popi();
popi();
label3:
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
}
