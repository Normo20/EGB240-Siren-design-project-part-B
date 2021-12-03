//////////////////////////////////////////
//                                     //
//          actual code               //
//          do not delete            //
//////////////////////////////////////


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Serial debug
#include "serial.h"
#include <stdio.h>

uint8_t pushbuttons_db=0;
volatile uint8_t reg1, reg2;
#define S1 0x10
#define S2 0x20
#define S3 0x40
#define S4 0x80
volatile uint16_t fmin1 = 2660; 
volatile uint16_t fmax1 = 5375;  
int mode=1;
volatile uint32_t num_ticks= 0;


void pwm_init() {
	// Timer configuration here
	TCCR1A = 0b00110011;
	TCCR1B = 0b00011000;

	OCR1A = 8000;
	OCR1B = 4000;
	TCCR1B |= 1;

	TIMSK1 =0b0000001; 

	DDRB |=(1<<PINB6);
}

void init() {
	cli();  //Disable interrupts

	//Set sysclk to 16 MHz
	CLKPR = 0x80; // Prescaler change enable
	CLKPR = 0x00; // Set prescaler to zero

	//Configure PORT D4-D7 (LEDs) as outputs
	PORTD = 0x00;
	DDRD = 0xF0;

	TCCR0A = 0x02;
	TCCR0B = 0x04;
	OCR0A= 0x7D;
	TIMSK0 = 0x02;

	OCR1A = fmax1;
	OCR1B = OCR1A/2; 


	pwm_init();
	serial_init();

	sei();  //Enable interrupts
}


volatile uint16_t isrCount = 0;
ISR(TIMER1_OVF_vect) {
	
	//Interrupt-driven frequency toggle example
	isrCount++;
	if (isrCount == 1000) {
		OCR1A = 5826; // 2.7 kHz
	} else if (isrCount > 1999) {
		OCR1A = 7800; //2KHz
		isrCount = 0;
	}
	
	/*
	if (OCR1A == fmin1) {	// 2.7 kHz
		OCR1A = fmax1 ; 	// 5.4 kHz
		OCR1B = OCR1A/2;
	} else {
		OCR1A=OCR1A-0.2;
		OCR1B = OCR1A/2;
	}
	*/
	
   
	 
	
	 /*
	uint16_t fspan= fmax1-fmin1;
	
   uint16_t fdev = (fspan * (uint64_t) num_ticks)/16e6;
   uint16_t curr_f = fmax1-fdev;
  uint16_t top_next = (16e6)/(curr_f);
	
    if (mode==1){
   OCR1A= top_next;
   OCR1B = top_next/2;
   num_ticks += top_next;
  
 if (num_ticks> (16e6)){
	   num_ticks=0;
	   
   } 
	}

	else if (mode==2){
		 
     OCR1A=fmin1;
	 OCR1B = fmin1/2;
	 
	}

	else if (mode==3){
       OCR1A=fmax1;
	   OCR1B=fmax1/2;

	}

*/
}

int main(void) {
	uint8_t pb_now=0, pb_prev=0, pb_pressed;

	// Initialisation
	init();
    
	//Pin set example
	//PORTD |= (1<<PIND7);

	// Main loop (loop forever)
	for(;;) {
 pushbuttons_db = ~PINF;           // read pushbuttons

      // determine if any pushbutton pressed
      pb_now = pushbuttons_db;
	  pb_pressed = pb_now & (pb_now ^ pb_prev);
	  pb_prev = pb_now;

	  PORTD = pushbuttons_db & 0xF0;    // output PBs to LEDs


//S4 resets mode to run and resets all the frequencies (need to change frequencies)
      if (pb_pressed & S4) {
		  mode=1;
       fmin1 = 2660;
       fmax1 = 5375;

		

	
}


//S3 Decreases the frequency of fmax or fmin depending on what mode it is in (not done)
	   else if (pb_pressed & S3) { 
		   
		    if (mode==2){
            
			if (fmin1<3200){ //changed to three increments when raised didn't change the fmax
			 fmin1=fmin1+137; //changed the starting fmax value when lowered
			}
			}

		   if (mode==3){
             
			 
			  if (fmax1<7265){ //up 15 
				  fmax1=fmax1+500; //
			 }
		  
	   } 
		   
			
			 }



		  
		




//S2 increases the frequency of fmax or fmin depending on what mode it is in (not done)
	   else if (pb_pressed & S2) { 
		   
		   if (mode==2){
			   if (fmin1>2760){
             fmin1=fmin1-137;
			   }
			  
			}

		  if (mode==3){ 
			  if (fmax1>5340){ //down 50
				fmax1=fmax1-500; 
			 }
		  
	   } 
	   
	   }


//Cycles through modes 
        else if (pb_pressed & S1) { 
         mode++;
	    if (mode==4){
			mode=1;
		}
		
		
	  } 



	}
}

ISR(TIMER0_COMPA_vect)
{
uint8_t pb0;
uint8_t delta;
pb0 =~PINF;
delta =pb0 ^pushbuttons_db;
pushbuttons_db^=(reg2 & delta);
reg2= (reg1 & delta);
reg1= delta;

}