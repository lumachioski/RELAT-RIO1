//Relatório 1 - MÁQUINA DE ESTADOS
//Maria Luiza Machioski e Luiza Tedeschi - S23
//portF
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))
//portD
#define GPIO_PORTD_DATA_R (*((volatile unsigned long *)0x40007020))
#define GPIO_PORTD_DIR_R (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_AMSEL_R (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R (*((volatile unsigned long *)0x4000752C))
// Function Prototypes
void PortF_Init(void);
void PortD_Init(void);
void Delay25(void);
void Delaypadrao(void);
//void EnableInterrupts(void);
//Global Variables
//contador usado para mudar os estados e fazer repetições no programa
//PF4 para ler a SW2, ou seja,
unsigned long cont=0, contdois=0, contres=0, LED, PD3;
//inicialização e rotina do programa
int main(void){
//inicializando as portas D e F usando as funções com suas definições
 PortF_Init();
PortD_Init();
//loop que o program ficará rodando
 while(1){
switch(cont)
 {
//ESTADO 0
 case 0:
//colocando as saídas PF1, PF2 e PF3 (leds) em zero, ou seja, desligados
 GPIO_PORTF_DATA_R = 0x00;
//aplicando nível lógico 0 na saída PD3 (só ela foi autorizada a ter mudanças)
GPIO_PORTD_DATA_R = 0x00;
PD3=GPIO_PORTD_DATA_R&0x08; //leitura da PD3
cont=1; //variável que soma os estados
Delaypadrao(); //pequeno delay de tempo (1 seg, apenas para controle)
 break;
//ESTADO 1
 case 1:
 GPIO_PORTD_DATA_R = 0x00; //PD3 continua desligado
contdois=10; //convencionei que o led azul irá piscar 10x em 2,5 Hz
while(contdois>0){ //while para piscar o LED azul
GPIO_PORTF_DATA_R = 0x04; //colocando PF2 em nível alto
LED=GPIO_PORTF_DATA_R&0x04;
Delay25(); //delay de 2/2,5 (meio período)
GPIO_PORTF_DATA_R = 0x00; //colocando PF2 em nível baixo
LED=GPIO_PORTF_DATA_R&0x04;
Delay25(); //delay de 2/2,5 (meio período)
contdois--; //uma piscada é contada
}
cont = 2; //ao final do estado 1, muda para o estado 2
 break;
//ESTADO 2
case 2:
//se ao final do est. 1 o botão sw2 estiver press., muda para o estado 3
if((GPIO_PORTF_DATA_R&0x01)==0x01){//
cont=3; //como é pull up, não press causa um nível lógico alto
}
//se SW2 não estiver press., muda para o estado 4
else{
cont=4; //se for pressionado, ou seja, nível 0 na PF0, vai para est. 4
}
 break;
//ESTADO 3
 case 3:
contdois=5; //led verde vai piscar 5x
while(contdois>0){ //mesmo raciocínio do estado 1, mas aqui a freq. não importa
GPIO_PORTF_DATA_R = 0x08; //verde é PF3
Delaypadrao();
GPIO_PORTF_DATA_R = 0x00;
Delaypadrao();
contdois--;
}
cont=1; //ao final do estado 3, a máq. volta para o estado 1
 break;
//ESTADO 4
 case 4:
contdois=2; //led vermelho pisca 2 x
while(contdois>0){ //mesmo esquema dos anteriores
GPIO_PORTF_DATA_R = 0x02; //vermelho é PF1
Delaypadrao();
GPIO_PORTF_DATA_R = 0x00;
Delaypadrao();
contdois--;
}
cont=5; //vai para o estado 5
 break;
//ESTADO 5
 case 5:
contdois=3; // led vermelho pisca 10x com F de 2,5 Hz
while(contdois>0){
GPIO_PORTF_DATA_R = 0x02;
Delay25();
GPIO_PORTF_DATA_R = 0x00;
Delay25();
contdois--;
}
contres++;
//na primeira vez contres = 1, vai para o break e continua cont = 5
//se for contres = 2, ou seja, é a segunda passagem, o estado muda para o 6
if(contres==2) //verifica se está na repetição, se sim, vai para o est. 6
{
cont=6;
contres=0;
}
 break;
//ESTADO 6
 case 6:
contdois=4; //4x repetições das piscadas
GPIO_PORTD_DATA_R = 0x08; //define a saída PD3 em nível lógico alto
PD3=GPIO_PORTD_DATA_R&0x08; //leitura da PD3
while(contdois>0){
GPIO_PORTF_DATA_R = 0x02; //led vermelho
Delaypadrao();
GPIO_PORTF_DATA_R = 0x04; //led azul
Delaypadrao();
GPIO_PORTF_DATA_R = 0x08; //led verde
Delaypadrao();
contdois--;
}
cont=0; //volta para o estado 0
 break;
return 0;
}
}
}
void PortF_Init(void){ volatile unsigned long delay;
//sem mudanças do exemplo do professor, porque atende aos requisitos
 SYSCTL_RCGC2_R |= 0x00000020; // 1) F clock
 delay = SYSCTL_RCGC2_R; // delay
 GPIO_PORTF_LOCK_R = 0x4C4F434B; // 2) unlock PortF PF0 
 GPIO_PORTF_CR_R = 0x1F; // allow changes to PF4-0
 GPIO_PORTF_AMSEL_R = 0x00; // 3) disable analog function
 GPIO_PORTF_PCTL_R = 0x0000FFFF; // 4) GPIO clear bit PCTL
 GPIO_PORTF_DIR_R = 0x0E; // 5) PF4,PF0 input, PF3,PF2,PF1 output
 GPIO_PORTF_AFSEL_R = 0x00; // 6) no alternate function
 GPIO_PORTF_PUR_R = 0x11; // enable pullup resistors on PF4,PF0
 GPIO_PORTF_DEN_R = 0x1F; // 7) enable digital pins PF4-PF0
}
void PortD_Init(void){ volatile unsigned long delay;
//sem mudanças do exemplo do professor, porque atende aos requisitos
//só será usado o PD3
 SYSCTL_RCGC2_R |= 0x00000008; // 1) D clock
 delay = SYSCTL_RCGC2_R; // delay é necessário desbloquear algum pinono D com o LOCK?
 GPIO_PORTD_AMSEL_R = 0x00; // 3) disable analog function
 GPIO_PORTD_PCTL_R = 0x00000000; // 4) GPIO configura como GPIO?
 GPIO_PORTD_DIR_R = 0x08; // 5) PD3 output
 GPIO_PORTD_AFSEL_R = 0x00; // 6) no alternate function
 GPIO_PORTD_DEN_R = 0x1F; // 7) enable digital pins PD4-PD0
}
void Delay25(void){unsigned long volatile time;
 time = 0.18184033*727240*200/91; // 1 sec
 while(time){
time--;
 }
}
void Delaypadrao(void){unsigned long volatile time;
 time = 0.9092016667*727240*200/91; // 1 sec
 while(time){
time--;
 }
}