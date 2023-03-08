#include <Arduino.h>

//Hard code output pins for LED
#define RED_PIN 26
#define YELLOW_PIN 13
#define GREEN_PIN 12

//Hard code enumerator
#define RED_STATE 0
#define RED_YELLOW_STATE 1
#define YELLOW_STATE 2
#define GREEN_STATE 3

//Hard code light timers in ms
#define RED_LIGHT_TIMER 10000
#define YELLOW_LIGHT_TIMER 2000 
#define RED_YELLOW_TIMER 2000
#define PEDESTRIAN_WALK_BUFFER 5000

#define BUZZ_FREQUENCY 1000

int tl_state;                                           //Traffic light state
unsigned long tl_timer;                                 //Traffic light timer
unsigned long green_end_interval_timer = 0;
unsigned long red_end_interval_timer = 0;


int buzzer_pin = 15;
int button_pin = GPIO_NUM_36;
int counter = 0;

//activates buzzer noise (buzzer_pin, <sound level in khz>, <OPTIONAL: duration ms >)
void Buzz(int buzz_frequency, int buzz_time){           
  tone(buzzer_pin, buzz_frequency, buzz_time);          
}

//stops any current buzz
void StopBuzz(){
  noTone(buzzer_pin);
}

//resets lights by turning all lights off
void ResetLights(){                                     
  digitalWrite(RED_PIN, LOW); 
  digitalWrite(YELLOW_PIN, LOW); 
  digitalWrite(GREEN_PIN, LOW); 
}

//Buzzes for 250ms and stops for 250ms 
void RedStateBuzzer(){                                  
  if((int)tl_timer >= (int)red_end_interval_timer){
    Buzz(BUZZ_FREQUENCY, 250);
    red_end_interval_timer = millis() + 500;            
  }
  tl_timer = millis(); 
}

//Buzzes for 500ms and stops for 2000ms
void GreenStateBuzzer(){                                 
  if((int)tl_timer >= (int)green_end_interval_timer){
    Buzz(BUZZ_FREQUENCY, 500); 
    green_end_interval_timer = millis() + 2000;
  } 
  tl_timer = millis();
}

//green light and buzzer is on for 5000ms then changes state to yellow
void StartPedestrianSequence(){                         
  unsigned long end_green_signal = millis() + 5000;
  while((int)tl_timer < (int)end_green_signal){         //waits for 5 seconds (stays in greenlight for 5 sec)
    GreenStateBuzzer();
  }

  tl_state = YELLOW_STATE;                              //changes state to turn on yellow light
  StopBuzz();                                           //makes sure buzzer stops 

}

//Calls RedStateBuzzer() to buzz for 10000 ms 
void StartRedStateSequence(){                           
  unsigned long end_red_signal = millis() + 10000; 
  while(tl_timer <= end_red_signal){
    RedStateBuzzer(); 
    tl_timer = millis(); 
  }

  StopBuzz(); 
}

void setup() {
  Serial.begin(9600);
  //Configuring buzzer and button 
  pinMode(buzzer_pin, OUTPUT);
  pinMode(button_pin, INPUT);

  //Configuring LED pins as output 
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode (GREEN_PIN, OUTPUT);

  //initial state for states and timers
  tl_state = RED_STATE; 
  // tl_timer = millis() + RED_LIGHT_TIMER; 
  tl_timer = millis();
  ResetLights(); 
}

void loop() {
  //TL state machine
  switch(tl_state)
  {
    case RED_STATE:   
      digitalWrite(RED_PIN, HIGH);          //turns red light on
      StartRedStateSequence();              //Calls buzzer sequence for red light 
      tl_state = RED_YELLOW_STATE;          //changes state after 10 seconds
      ResetLights(); 
      break; 
    
    case RED_YELLOW_STATE:
      digitalWrite(RED_PIN, HIGH);          //turns red light on
      digitalWrite(YELLOW_PIN, HIGH);       //turns yellow light on
      delay(RED_YELLOW_TIMER);              //waits for 2000ms
      tl_state = GREEN_STATE;             
      ResetLights(); 
      break; 

    case YELLOW_STATE: 
      digitalWrite(YELLOW_PIN, HIGH);       //turns yellow light on
      delay(YELLOW_LIGHT_TIMER);            //waits for 2000ms
      tl_state = RED_STATE; 
      ResetLights(); 
      
      break; 

    case GREEN_STATE: 
      digitalWrite(GREEN_PIN, HIGH);        //turns green light on
      GreenStateBuzzer();                   //calls buzzer sequence for green light
      if(digitalRead(button_pin) == HIGH){  //checks if button is pressed
        StartPedestrianSequence();          //calls function to change state  
        ResetLights();
      }

      break; 
  }
  tl_timer = millis();
}