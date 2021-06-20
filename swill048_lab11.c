/*	Author: Silas Willis
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link:https://youtu.be/C6Nlv3rKvrU
 */
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0 ; 
unsigned long _avr_timer_M = 1 ; 
unsigned long _avr_timer_cntcurr = 0 ; 

void TimerOn(){
TCCR1B = 0x0B ; 
OCR1A = 125 ; 
TIMSK1 = 0x02 ; 
TCNT1 = 0 ; 

_avr_timer_cntcurr = _avr_timer_M ; 
SREG |= 0x80 ;

}

void TimerOff(){
TCCR1B = 0x00 ; 
}
void TimerISR(){
TimerFlag = 1 ; 
}

ISR(TIMER1_COMPA_vect){
_avr_timer_cntcurr-- ; 

if(_avr_timer_cntcurr == 0 ){
TimerISR() ; 
_avr_timer_cntcurr = _avr_timer_M ; 
	}
}

void TimerSet(unsigned long M){
_avr_timer_M = M ; 
_avr_timer_cntcurr = _avr_timer_M ; 

}

typedef struct task{
	signed char state ; 
	unsigned long int period ; 
	unsigned long int elapsedTime ; 
	int (*TickFct)(int) ; 
}task ; 

unsigned long int findGCD(unsigned long int a , unsigned long int b ){
unsigned long int c ; 
while(1){
	c = a%b ; 
	if(c==0){
	return b ; 
	}
	a = b ; 
	b = c ; 

}
return 0 ; 
}

void set_PWM(double frequency){
static double current_frequency ; 

if(frequency != current_frequency){
	if(!frequency){TCCR3B &= 0x08 ; }
	else{TCCR3B |= 0x03 ; }

	if(frequency <0.954){OCR3A = 0xFFF ; }
	else if(frequency > 31250){OCR3A = 0x0000 ; } 
	else{ OCR3A = (short)(8000000 / (128* frequency)) -1 ; }
	TCNT3 = 0  ;
	current_frequency = frequency ; 

}

}
void PWM_on(){
TCCR3A = (1 << COM3A0) ; 
TCCR3B = (1 <<WGM32)| (1<<CS31) | (1<<CS30) ; 
set_PWM(0) ; 


}



//PORTA 0-3 --> Column 1-4  LED Matrix --> Left, pins 1-4, 13-16
//PORTA 4-7 --> Column 5-8  LED Matrix --> Right, pins 9-16 // use matrix pins 1-8 for red LEDs
//For LED activation: PORTA: HIGH PORTD: LOW
//PORTD 0-7 --> rows 1-8

unsigned int SPACES_HI[8] = {0} ; //start off 
unsigned int SPACES_LO[8] = {0} ; //start off // dont forget to invert!
unsigned char tmpA = 0x00 ; 
unsigned char tmpD = 0x00 ; // dont forget to invert --> low side on 

enum LED_STATES {WAIT, SET }MATRIX  ; //sets LED state in matrix 
void LED_S(){
	switch(MATRIX){
	case WAIT: // dont do stuff
	break ; 
	case SET: // set LEDs to current array values
	//read each array (HI & LO), smoosh each into PORTA & PORTD 
	for(int i = 0 ; i < 8 ; ++i){
		tmpA = (tmpA<<1) + SPACES_HI[i] ;
		tmpD = (tmpD<<1) + SPACES_LO[i] ; 
	}
		break ; 
	default: break ; 
	}
	switch(MATRIX){
	case WAIT:

		break ; 
	case SET:

		break ; 
	default: break ; 
	}
}

unsigned char tmp = 0x00 ; 

enum LOGIC{HOLD, INC }INCREMENT ; //change place of HIGH LEDs 
void LOGIC_S(){
	switch(INCREMENT){
	case HOLD:

		break ; 
	case INC:

if(tmp == 0){
	PORTA = (tmpA) ; 
	PORTD = 0x00 ; 
	tmp = 1 ; 
	tmpA = tmpA<<1 ; 

	if(tmpA == 0x80){
	tmpA = 0x01 ; 
	}
}
else if(tmp == 1){
	PORTA = 0x00 ; 
	PORTD = 0xFF ; 
	tmp = 0 ; 
}
	INCREMENT = INC ; 
		break ; 
	default: break ; 
	}
	switch(INCREMENT){
	case HOLD: break ;
	case INC:  break ; 
	default:   break ; 
	}
}

enum DISP{HOLDON, ON}DISPLAY ; 
unsigned char countUP = 8 ; 
unsigned char countDOWN = 8 ; 
unsigned char LEDA = 0x00 ; 
unsigned char LEDD = 0xFF ; 

unsigned int PLACE[8][8]  =    {{0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0 }, 
				{0,0,0,0,0,0,0,0 },   			 
				{0,0,0,0,0,0,0,0 },
				{0,0,0,0,0,0,0,0 }, 
				{0,0,0,0,0,0,0,0 }, 
				{0,0,0,0,0,0,0,0 }, 
				{0,0,0,0,0,0,0,0 }}  ; 

int DISP_FCT(int state  ){
	switch(DISPLAY){
	case HOLDON: break ; 
	case ON:
	LEDA = 0x00 ; 
	LEDD = 0xFF ; 

	PORTA = 0x00 ;//cols  
	PORTD = 0xFF ; //rows
	for(int i = 0 ; i < 8 ; ++i){
		for(int j = 0 ; j < 8 ; ++j ){ 
			if(PLACE[i][j] == 1){
			LEDA = LEDA | (0x01<<j) ; 
			LEDD = 0x80>>i ; 	
	
			}
		}
		PORTA = LEDA ; 
		PORTD = ~LEDD; 
		LEDA = 0x00 ; 
		LEDD = 0xFF ; 
	}
		break ;
	default: break ; 
	}

	switch(DISPLAY){
	case HOLDON: break ;
	case ON: break ; 

	}
	return state ; 	
}


enum CTRLS{ WAIT2, UP, DOWN, LEFT, RIGHT}CONTROLS ; 
unsigned char tmpB = 0x00 ; 
unsigned char headUP = 0 ;//cols 
unsigned char headSIDE = 0 ; //rows
unsigned char tailUP = 0 ; 
unsigned char tailSIDE = 0 ; 
unsigned char length = 2;  //gives actual length of (length - 1) for some reason?
unsigned char FOOD_UP ; 
unsigned char FOOD_SIDE ; 
unsigned char TAIL_ROW[64] = {0} ; 
unsigned char TAIL_COL[64] = {0} ; 
unsigned char dec_flag = 0 ; //fix this!! ~should~ be temporary
//set dec_flag to high once a button is pressed / can change this to 1 to "pop" the pseudo-queue
//


unsigned char food_flag = 1 ; //set to 1 to generate new bit of food
enum food{NO_FOOD, FOOD}HUNGRY ; 
unsigned char food_ok = 1  ; // makes sure the food coords are ok 
unsigned char pwm_flag = 0 ; //set to 1 in order to play loss sound 


//head gets re set after each food 
//each head needs to stay lit for LENGTH amount of time
// have 2 'queues', one for ROWS one for COLS 
// each HEAD gets pushed to the back of the 'queue'
// once it reaches the front, the LED at the given coords is turned off

// after each food length++


//headUP >= 1 & headUP <=6 set the bounds for the matrix.
// change these bounds in order to set a 'dead zone' outside of the matrix
// currently the matrix acts as a barrier 
int LED_CTRL(int state ){
	tmpB = PINB & 0x0F ; 
	switch(CONTROLS){
	case WAIT2:
	
		if(tmpB != 0x0F){ // do stuff 
			if((tmpB == 0x0E) && (headUP >= 1)){
			//go up
			headUP-- ; 
			CONTROLS = UP ; 
			if((PLACE[headUP][headSIDE] == 1) && (headUP != FOOD_UP) && (headSIDE != FOOD_SIDE)){ 
			//if checks for reset conditions: bumping into itself, make sure its not food & the head cant
			//immediately turn on itself 
	
				for(int i = 0 ; i < 8 ; ++i ){
				for(int j = 0 ; j < 8 ; ++j){
				PLACE[i][j] = 0 ; 
				}
			}
		length = 2 ; 
		food_flag =  1 ; 	
		pwm_flag = 1 ; 
			}
			}
			else if((tmpB == 0x0D) && (headUP <= 6)){
			//go down 
			headUP++ ; 
			CONTROLS = DOWN ; 
			if((PLACE[headUP][headSIDE] == 1) && (headUP != FOOD_UP) && (headSIDE != FOOD_SIDE)){ 
			for(int i = 0 ; i < 8 ; ++i ){
				for(int j = 0 ; j < 8 ; ++j){
				PLACE[i][j] = 0 ; 
				}
			}	
		length = 2 ; 
		food_flag = 1 ;
	        pwm_flag = 1 ; 	
			}
			}
			else if((tmpB == 0x0B) && (headSIDE <= 6 )) {
			//go right
			headSIDE++ ; 
			CONTROLS = RIGHT ; 
			if((PLACE[headUP][headSIDE] == 1) && (headUP != FOOD_UP) && (headSIDE != FOOD_SIDE)){ 
			for(int i = 0 ; i < 8 ; ++i ){
				for(int j = 0 ; j < 8 ; ++j){
				PLACE[i][j] = 0 ; 
				}
			}
		length = 2 ; 
		food_flag = 1 ;
		pwm_flag = 1  ; 	
	
			}
			}
			else if((tmpB == 0x07) && (headSIDE >= 1))   {
			headSIDE-- ; 
			CONTROLS = LEFT ; 
			if((PLACE[headUP][headSIDE] == 1) && (headUP != FOOD_UP) && (headSIDE != FOOD_SIDE)){ 
			for(int i = 0 ; i < 8 ; ++i ){
				for(int j = 0 ; j < 8 ; ++j){
				PLACE[i][j] = 0 ; 
				}
			}	
		length = 2 ; 
		food_flag = 1 ; 
		pwm_flag = 1 ; 
			}
			}
		}
		else{
		CONTROLS = WAIT2 ; 
		}


			break ; 
	case UP:
		if(tmpB == 0x0F ){// go back to wait ; 
		PLACE[headUP][headSIDE] = 1 ; 
		TAIL_ROW[length-1] = headUP ; 
		TAIL_COL[length-1] = headSIDE ;  
		dec_flag = 1 ; //FIXME

		CONTROLS = WAIT2 ; 
		}
		else{ //stay

		CONTROLS = UP ; 
		}

			break ; 
	case DOWN:
			if(tmpB == 0x0F ){// go back to wait ; 
		PLACE[headUP][headSIDE] = 1 ; 
		TAIL_ROW[length-1] = headUP ; 
		TAIL_COL[length-1] = headSIDE ; 
		dec_flag = 1 ; //FIXME

		CONTROLS = WAIT2 ; 
		}
		else{ //stay

		CONTROLS = DOWN ; 
		}	


			break ;
	case LEFT: 
		if(tmpB == 0x0F ){// go back to wait ; 
		PLACE[headUP][headSIDE] = 1 ; 
		TAIL_ROW[length-1] = headUP ; 
		TAIL_COL[length-1] = headSIDE ; 
		dec_flag = 1 ; //FIXME

		CONTROLS = WAIT2 ; 
		}
		else{ //stay

		CONTROLS = LEFT  ; 
		}
			break ;
	case RIGHT:
		if(tmpB == 0x0F ){// go back to wait ; 
		PLACE[headUP][headSIDE] = 1 ; 
		TAIL_ROW[length-1] = headUP ; 
		TAIL_COL[length-1] = headSIDE ; 
		dec_flag = 1 ; //FIXME

		CONTROLS = WAIT2 ; 
		}
		else{ //stay

		CONTROLS = RIGHT ; 
		}
			break ; 


	}
	return state ; 
}


enum LED_CHAIN{HOLD_INC, MOVE_INC}ONES ; 

int LED_ZEROS(int state){

	switch(ONES){
	case HOLD_INC:
		if(dec_flag == 1){
		ONES = MOVE_INC ; 
		dec_flag = 0 ; 
		}
		else{
		ONES = HOLD_INC ; 
		}
			
		break ; 
	case MOVE_INC:
		PLACE[TAIL_ROW[0]][TAIL_COL[0]] = 0  ; 

		for(int i = 0 ; i <63  ; ++i){//increment all indexes in BOTH TAIL_ROW & TAIL_COL
		
		if(i < 62){
		TAIL_ROW[i] = TAIL_ROW[i+1] ; 
		TAIL_COL[i] = TAIL_COL[i+1] ; 
		}
		else{
		TAIL_COL[i] = 0 ; 
		TAIL_ROW[i] = 0 ; 
		}

		}
		ONES = HOLD_INC ; 

		break ; 
	default: break ; 
	}

return state ; 
}



int SET_FOOD(int state){
	switch(HUNGRY){
	case NO_FOOD:
	//check if food has been eaten 
	if((headUP == FOOD_UP) && (headSIDE == FOOD_SIDE)){//food has been chomped
	food_flag = 1 ;
	++length ; 
	}

		if(food_flag == 1){//do stuff
		food_flag = 0 ; 
		food_ok = 1 ; 
		HUNGRY = FOOD ; 
		}
		else{
		HUNGRY = NO_FOOD ; 
		}

		break ; 
	case FOOD:

		
		while(food_ok){	
		FOOD_UP = (rand() % 8) ; 
		FOOD_SIDE = (rand()% 8) ; 
		
		if(FOOD_UP < 8 && FOOD_SIDE < 8){//food is within indexes
		if(PLACE[FOOD_UP][FOOD_SIDE] == 0 ){ // check that space isnt occupied
			food_ok = 0 ; 
			PLACE[FOOD_UP][FOOD_SIDE] = 1 ; // generate food bit

		}	
		}
		
		HUNGRY = NO_FOOD; // food had been generated & displayed 
			

		}
		break ; 
	default: break ; 
	}
	return state ; 
}

double notes[5] = {261.63, 329.63,493.88,370.0,329.33  } ; 
unsigned char note_counter = 0 ; 

//do stuff based on pwm_flag
enum PLAY{NO_SOUND, SONG}PLAY_PWM ; 
int PWM_STUFF(int state){
	switch(PLAY_PWM){
	case NO_SOUND:
		set_PWM(0) ; 
		if(pwm_flag == 1){
		pwm_flag = 0 ; 
		note_counter = 0 ; 
		PLAY_PWM = SONG ; 
		}
		else{
		PLAY_PWM = NO_SOUND ; 
		}

		break ; 
	case SONG:
		
		set_PWM(notes[note_counter]) ; 
		++note_counter ; 

		if(note_counter == 5){//return 
		PLAY_PWM = NO_SOUND ;	
		}
		else{
		PLAY_PWM = SONG ; 
		}
		
		break ; 
	}
	return state ; 
}


int main(void) {
    /* Insert DDR and PORT initializations */
DDRA = 0xFF ; PORTA = 0x00 ; 
DDRD = 0xFF ; PORTD = 0x00 ; 
DDRB = 0xF0 ; PORTB = 0x0F ; 

static task task1, task2, task3, task4 , task5 ; 
task *tasks[] = {&task1 , &task2, &task3, &task4, &task5 } ; 
const unsigned short numTasks = sizeof(tasks)/sizeof(task*) ; 
const char start = -1 ; 

task1.state = start ; 
task1.period = 1 ; 
task1.elapsedTime = task1.period ; 
task1.TickFct = &DISP_FCT  ; 

task2.state = start ; 
task2.period = 100  ; 
task2.elapsedTime = task2.period ; 
task2.TickFct = &LED_CTRL ; 

task3.state = start ; 
task3.period = 100 ; 
task3.elapsedTime = task3.period ; 
task3.TickFct = &LED_ZEROS ; 

task4.state = start ; 
task4.period = 100 ; 
task4.elapsedTime = task4.period ; 
task4.TickFct = &SET_FOOD ; 

task5.state = start ; 
task5.period = 100 ; 
task5.elapsedTime = task5.period ; 
task5.TickFct = &PWM_STUFF ; 

unsigned long GCD = tasks[0]->period ; 
for(unsigned int i = 1 ; i < numTasks ; ++ i ){
GCD = findGCD(GCD, tasks[i]->period) ; 
}

TimerSet(GCD ) ; 
TimerOn()  ; 
set_PWM(0) ; 
PWM_on() ; 
MATRIX = WAIT  ; 
INCREMENT = INC ;  // FIXME!!!
DISPLAY = ON ; 
CONTROLS = WAIT ; 
ONES = HOLD_INC ; 
HUNGRY = NO_FOOD ; 
unsigned short i ;
    /* Insert your solution below */
    while (1) {
	for(i = 0 ; i < numTasks ; ++i ){
		if(tasks[i]->elapsedTime == tasks[i]->period){
		tasks[i]->state = tasks[i]->TickFct(tasks[i]->state) ; 
		tasks[i]->elapsedTime = 0 ; 
		}
		tasks[i]->elapsedTime += GCD ; 
	}



	while(!TimerFlag){} ; 
	TimerFlag = 0 ;


    }
    return 1;
}
