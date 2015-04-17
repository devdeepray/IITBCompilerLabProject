void func_0(){
pushi(ebp);
move(esp,ebp);
loadi(ind(ebp,4),eax);
cmpi(1, eax);
jge(label1);
label0:
pushi(0);
loadi(ind(ebp,4),ebx);
addi(-1, ebx);
pushi(ebx);
func_0();
popi(1);
loadi(ind(esp, 0), ebx);
popi(1);
loadi(ind(ebp,4),eax);
muli(ebx, eax);
storei(eax, ind(ebp,8));
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
j(label2);
label1:
storei(1, ind(ebp,8));
move(ebp,esp);
addi(I,esp);
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
print_string("Factorial is ");
pushi(0);
pushi(10);
func_0();
popi(1);
loadi(ind(esp, 0), eax);
popi(1);
print_int(eax);
print_string("\n");
label3:
move(ebp,esp);
addi(I,esp);
loadi(ind(ebp, 0), ebp);
return;
}
