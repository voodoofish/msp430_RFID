 // RFidRead.c
//VCC
//GND
//P1.0 EN
//P1.4 SOUT

#include "msp430g2211.h" //change as needed

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
unsigned char match; 
unsigned char key[12]; //array to hold keys read in through reader

// Disable watchdog
WDTCTL = WDTPW + WDTHOLD;
//disabled as I now just put this code into a loop without initialization via button press
//P1IE = 0x04; // P1.3 interrupt enabled
//P1IES |= 0x04; // P1.3 Hi/lo edge
//P1IFG &= ~0x04; // P1.3 IFG cleared

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
//need a delay here so linux doesn't bork out. at least 3 seconds.
/*//for some reason, this borked the chip....
Red_On();
for(delay=0;delay<2;delay++)
{Wait();}
puts("\r\nRFID Read...\r\n");
Red_Off();
*/
//if data comes out garbled check the baud rate....try hitting reset as well.
//_BIS_SR(LPM0_bits + GIE); //disabling for now as I don't need this.
while(1){
   int i;
   for(i = 0;i<11;i++) {           // Do forever
   key[i]=getc();
   }
   Red_On();
   //Check for 0F or 04
   puts("{\"keycard\":");
   puts("{\"key\":\"");
   for(i = 1;i<11;i++) {   //Start with i=1 as i=0 will return char 0xa which is a linefeed...this breaks stuff.         
   putc(key[i]);
   }
   puts("\",");
   puts("\"match\":\"");
   int x;
   for(x=0;x<keyCount; x++){ 
   if (diffAR(key, ID[x])==1)
  	{match=49;
  	break;}
   else
	{match=48;}
   }
	putc(match);
	puts("\"}}\r\n");
	Wait(); //wait for a little bit.
	Red_Off(); //Re-enable RFID reader.
	}

}
//not used atm, left in just in case though...
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{	
	
	char c;
	c = getc();
    putc(c);
	P1IFG &= ~0x04; // P1.3 IFG cleared
}

unsigned char diffAR(unsigned char a[],const unsigned char b[]){
	unsigned char diff = 0;
	unsigned char t = 0;
	unsigned char count = 0;
	/*
	unsigned char i;
	
	for(i = 0;i<11;i++) {
    putc(b[i]);
    }
    */
    //A do while loop...WOO...
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

