// RFidRead.c
#include "msp430g2231.h" //change as needed

#define keyCount 3
#define idc 11

static const unsigned char ID[][12] =
{
 {0x0A, 0x30, 0x46, 0x30, 0x33, 0x30, 0x32, 0x38, 0x31, 0x33, 0x42, 0x0D} // 0F0302813B
 ,{0x0A, 0x30, 0x34, 0x31, 0x35, 0x42, 0x30, 0x43, 0x34, 0x34, 0x34, 0x0D} // 0415B0C444
 ,{0x0A, 0x30, 0x46, 0x30, 0x33, 0x30, 0x32, 0x38, 0x30, 0x35, 0x31, 0x0D} // 0F03028051
};
// oPossums function code
void serial_setup(unsigned out_mask, unsigned in_mask, unsigned duration);
void putc(unsigned);
void puts(char *);
unsigned getc(void);
//new functions to enable and disable the Red LED
void Red_Off(void);
void Red_On(void);
void Wait(void);

//my diff funciton. var 1 is the input key(read in from the rfid reader), var 2 is the diff key from the ID Array.
unsigned char diffAR(unsigned char a[],const unsigned char b[]);

void main(void)
{
unsigned char key[12]; //array to hold keys read in through reader

// Disable watchdog
WDTCTL = WDTPW + WDTHOLD;
//disabled as I now just put this code into a loop without initialization via button press
//P1IE = 0x0c; // P1.3 interrupt enabled
//P1IES |= 0x0c; // P1.3 Hi/lo edge
//P1IFG &= ~0x0c; // P1.3 IFG cleared

// Use 1 MHz DCO factory calibration
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ;
DCOCTL = CALDCO_1MHZ;
//I'm wondering if it's possible to set up 3 pins for serial com....
// Setup the serial port
// Serial out: P1.1 (BIT1)
// Serial in:  P1.4 (BIT2)
// Bit rate:   2400 (CPU freq / bit rate) 
serial_setup(BIT1, BIT4, 1000000 / 2400);

//Start of read code.
puts("\r\nRFID Read...\r\n");
//if data comes out garbled check the baud rate....try hitting reset as well.
//_BIS_SR(LPM0_bits + GIE); //disabling for now as I don't need this.
while(1){
   int i;
   for(i = 0;i<11;i++) {           // Do forever
   //c = getc();     // Get a char
   key[i]=getc();
   //putc(c);        // Echo it back
   }
   Red_On();
   //Check for 0F or 04
   for(i = 0;i<11;i++) {           
   putc(key[i]);
   }
   puts("\r\n");
   int x;
   for(x=0;x<keyCount; x++){ 
   if (diffAR(key, ID[x])==1)
   {puts("\r\nKey matches...\r\n");}
   else
	{puts("\r\nKey mismatch...\r\n");}
   }
//Some test code...figured maybe it would be useful for someone.
/*
   for(;;) {           // Do forever
   c = getc();     // Get a char
	switch(c)
	{
	case 'a' :
		putc(c);
		Red_On();
		break;
	case 'b' :
		putc(c);
		Red_Off();
		break;
	default :
		putc(c);
		Red_Off();
	}
   }
   */
	Wait(); //wait for a little bit.
	Red_Off(); //Re-enable RFID reader.
	}

}
//not used atm, left in just in case though...
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
	if (P1IFG&0x04){
	char c;
	//	char d;
	//c = getcInt();     // Get a char
    c = getc();
    c |= 0x1;
    putc(c);
//    putc(d);
	P1IFG &= ~0x04; // P1.3 IFG cleared
	}
	if (P1IFG&0x08){
	P1IFG &= ~0x08; // P1.3 IFG cleared
	int foo = 65;
	putc(foo);
	}
//P1OUT |=0x1; //sets P1.0 high
//P1OUT &= ~0x1; //Turn off P1.0
}

unsigned char diffAR(unsigned char a[],const unsigned char b[]){
	unsigned char diff = 0;
	unsigned char t = 0;
	unsigned char count = 0;
	unsigned char i;
	for(i = 0;i<11;i++) {
    putc(b[i]);
    }
    
    do {
    //compare arrays here	
    t=(b[count]==a[count])? 1:0;
    count++;
    if (count == 11)
    {diff = 1;}
    }
    while(t == 1);
    //a=(x==y)? 1:0;
	return diff;
}

