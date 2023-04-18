#include <FastLED.h>
#include <IRremote.h>


#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 60
#define DATA_PIN 7
#define IR_TOLERANCE 17

int BRIGHTNESS=255;

//sound variables
int sound=0;
int sound_pin=3;
int previous_sound=0;

const int SAMPLE_TIME = 20;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
int sampleBufferValue = 0;

//sound_variables


int channel_minus=12339;
int channel=12348;
int channel_plus=12351;
int prev=12336;
int next=12288;
int play_pause=12303;
int minus=63;
int plus=819;
int EQ=195;
int zero=828;
int hundred_plus=963;
int two_hundred_plus=243;
int one=240;
int two=960;
int three=13308;
int four=192;
int five=1008;
int six=13260;
int seven=12300;
int eight=13068;
int nine=12492;

int controller_value;
int previous_control=0;
int lights_on=0;
unsigned long previous_time=0;
unsigned long current_time;
struct random_colors{
  int one;
  int two;
  int three;
};

struct random_colors get_colors(){
  struct random_colors which;
  which.one=rand() % 255;
  which.two=rand() % 255;
  which.three=rand() % 255;
  return which;
}

struct random_colors struct_holder;
int low_light_val;

int IRpin=11;

CRGB leds[NUM_LEDS];

IRrecv irrecv(IRpin);
decode_results signals;

int i=0;
int led=4;


void setup(){
//  srand(time(NULL));
  randomSeed(analogRead(A0));
	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  irrecv.enableIRIn();
  FastLED.clear();
  FastLED.show();
  pinMode(sound_pin,INPUT);
  //setup led and turn it off, we're gonna use it when the song is playing to see if the sensor is working
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  Serial.begin(9600);
}
void loop(){

      
      
      millisCurrent = millis();
      millisElapsed = millisCurrent - millisLast;
//      elapsed_time=millis()-last_time;
//      last_time+=elapsed_time;
      update_control_value(millisElapsed);

      
//    leds[0]=CRGB(255,0,0);
//    leds[1]=CRGB(0,255,0);
//    leds[2]=CRGB(0,0,255);
//    FastLED.setBrightness(50);
//    FastLED.show();
//    leds[3]=CRGB::Black;
//    leds[3]=CRGB(255,255,255);
//    leds[4]=CHSV(HUE_YELLOW,50,BRIGHTNESS);
//    leds[5]=CRGB(0,255,255);
//    FastLED.show();
//    Serial.println(controller_value);

      

    if (controller_value==zero){
      both_sides(30,CRGB(255,0,0),CRGB(255,0,255));
    }

    else if (controller_value==one){
      lights_on=1;
     	turn_leds(1,CRGB(255,0,0));
	  }
   else if (controller_value==two){
      lights_on=1;
      turn_leds(1,CRGB(0,255,0));
   }
   else if (controller_value==three){
      turn_leds(1,CRGB(0,0,255));
   }
   else if (controller_value==four){
    back_forth(CRGB(255,0,255),50,25);
   }
   else if (controller_value==five){
    back_forth(CRGB(40,200,213),50,25);
   }
   else if (controller_value==six){
    back_forth(CRGB(0,0,0),50,25);
   }
   else if (controller_value==seven){
    snake(8,75,CRGB(0,0,255));
   }
   else if (controller_value==eight){
    fade(60);
   }
   else if (controller_value==nine){
    snake(4,15,CRGB(255,50,255));
   }
   else if (controller_value==channel_minus){
      change_brightness(-51);
      controller_value=0;
   }
   else if (controller_value==channel){
     specific_fade(10);
   }
   else if (controller_value==channel_plus){
      change_brightness(51);
      controller_value=0;
   }
   else if (controller_value==prev){
    lights_on=1;
    turn_leds(1,CRGB(125,255,125));
   }
   else if (controller_value==next){
    lights_on=1;
    turn_leds(1,CRGB(255,125,0));
   }
   
   else if (controller_value==play_pause){//this turns on and off the LEDS
    if (lights_on==1){
      lights_on=0;
      turn_leds(0,CRGB(0,0,0));
    }
    else{
      lights_on=1;
      turn_leds(1,CRGB(255,255,255));
      BRIGHTNESS=0;
      change_brightness(255);
    }
    controller_value=0;
   }
   
   else if (controller_value==minus){
      change_brightness(-51);
      controller_value=0;
   }
   else if (controller_value==plus){
      change_brightness(51);
      controller_value=0;
   }
   else if (controller_value==EQ){
//    listen_for_sound(millisElapsed);
     music_v2(30);
//      music_v3();
//    music_v4();
   }
   else if (controller_value==hundred_plus){
    one_pattern();
   }
   else if (controller_value==two_hundred_plus){
    both_sides(150,CRGB(0,255,0),CRGB::Cyan);
   }
}

int number_in_buttons(int num){
//  Serial.println(num);
  if (num==one || num==two || num==three || num==four || num==five || num==six || num==seven ||num==eight || num==nine ||
      num==zero || num==EQ || num==play_pause || num==channel || num==channel_minus || num==channel_plus ||
      num==hundred_plus || num==two_hundred_plus || num==plus || num==minus || num==prev || num==next){
        return 1;
      }
  return 0;
}

int update_control_value(unsigned long current_time){
  previous_control=controller_value;
  if (irrecv.decode(&signals) && ((current_time-previous_time)>=250)){
    uint64_t irVal = 0;
    for(int i=3; i<(signals.rawlen-1); i++)
    {
        int Delta = signals.rawbuf[i] - signals.rawbuf[i+1];
        if (Delta < 0) Delta = -Delta;
        uint8_t b = (Delta < IR_TOLERANCE) ? 0 : 1;

        if ((i-3) < 63)
        {
            irVal = irVal | ((int64_t)b << (int64_t)(i-3));
        }
    }

    uint64_t x1 = irVal;
    uint32_t x = x1 >> 32;
    if ((int)(x)!=0){
      controller_value=(int)(x);
      if (previous_control!=controller_value){
        digitalWrite(led,HIGH);
        delay(55);
        digitalWrite(led,LOW);
      }
    }
//    Serial.println("Value is: ");
//    Serial.println(controller_value);
    previous_time=current_time;
    irrecv.resume(); // get the next signal
  }
  return 0;
}

int turn_leds(int state, CRGB color){
  if (state){
    for (i=0;i<NUM_LEDS;i++){
      leds[i]=color;
    }
    FastLED.show();
  }
  else{
    FastLED.clear();
    FastLED.show();
  }
  return 0;
}

int check_break(){
  previous_control=controller_value;
  millisCurrent = millis();
  millisElapsed = millisCurrent - millisLast;
  update_control_value(millisElapsed);
  if (controller_value==previous_control){
    return 1;
  }
  return 0;
}

int when_break(){
  FastLED.clear();
  FastLED.show();
  turn_leds(1,CRGB(125,0,125));
  BRIGHTNESS=0;
  change_brightness(255);
  return 0;
}


int both_sides(int speedd, CRGB color1, CRGB color2){

 turn_leds(1,color1);
 FastLED.show();
 if (check_break()!=1){
        when_break();
        return 0;
    }
 delay(250);
 for (unsigned int k=0;k<NUM_LEDS;k++){
    if (k==(NUM_LEDS/2)){
      break;
    }
    else{
    leds[k]=color2;
    leds[NUM_LEDS-k]=color2;
    FastLED.show();
    if (check_break()!=1){
        when_break();
        return 0;
    }
    delay(speedd);
    }
 }
  return 0;
}

int change_brightness(int value){
    if (BRIGHTNESS!=255 && value>0){
      lights_on=1;
      BRIGHTNESS+=value;
      FastLED.setBrightness(BRIGHTNESS);
      FastLED.show();
    }
    else if (BRIGHTNESS!=0 && value<0){
      BRIGHTNESS+=value;
      if (BRIGHTNESS==0){
        lights_on=0;
      }
      else{
        lights_on=1;
      }
      FastLED.setBrightness(BRIGHTNESS);
      FastLED.show();
    }
    return 0;
  }
int snake(int largo,int speedd, CRGB color){
  for (i=0;i<largo;i++){
    leds[i]=color;
    FastLED.show();
    if (check_break()!=1){
        when_break();
        return 0;
    }
    delay(250);
  }
  if (check_break()!=1){
        when_break();
        return 0;
    }
  delay(200);
  for (i=0;i<NUM_LEDS;i++){
    leds[largo+i]=color;
    leds[i]=CRGB(0,0,0);
    FastLED.show();
    if (check_break()!=1){
        when_break();
        return 0;
    }
    delay(speedd);
    if (i==NUM_LEDS-largo-1){
        FastLED.clear();
        FastLED.show();
        break;
    }
  }
  return 0;
}



int back_forth(CRGB color,int forth_delay,int back_delay){
  for(i=0;i<NUM_LEDS;i++){
    if (color==CRGB(0,0,0)){
      //we are going to do a random back_forth
      struct_holder=get_colors();
      leds[i]=CRGB(struct_holder.one,struct_holder.two,struct_holder.three);
      FastLED.show();
      if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(forth_delay);
    }
    else{
      leds[i]=color;
      FastLED.show();
      if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(forth_delay);
    }
     }
  for(i=NUM_LEDS-1;i>=0;i--){
    leds[i]=CRGB(0,0,0);
    FastLED.show();
    if (check_break()!=1){
        when_break();
        return 0;
    }
    delay(back_delay);
    }
  return 0;
}


int listen_for_sound(unsigned long millisElapsed){
  if (digitalRead(sound_pin)==1){
    sampleBufferValue++;
  }
  if (millisElapsed > SAMPLE_TIME) {
    sound_leds(sampleBufferValue);
    sampleBufferValue = 0;
    millisLast = millisCurrent;
  }
  if (check_break()!=1){
        when_break();
        return 0;
    }
  return 0;
}
int sound_leds(int sound){
  Serial.println(sound);
  if (check_break()!=1){
        when_break();
        return 0;
    }
//  delay(5);
  if (sound==0){
    return 0;
  }
  if (sound<0){
    sound=sound*-1;
  }
  if (sound>0 && sound<5){
    turn_with_hsv(CHSV(HUE_GREEN,255,100));
//    turn_leds(1,CRGB::Green);
  }
  else if (sound>5 && sound<10){
    turn_with_hsv(CHSV(HUE_BLUE,255,150));
//    turn_leds(1,CRGB::Blue);
  }
  else if (sound>10 && sound<15){
    turn_with_hsv(CHSV(180,255,180));
//    turn_leds(1,CRGB::Cyan);
  }
  else if (sound>15 && sound<20){
    turn_with_hsv(CHSV(282,255,190));
//    turn_leds(1,CRGB::Magenta);
  }
  else if (sound>20 && sound<25){
    turn_with_hsv(CHSV(HUE_ORANGE,255,200));
//    turn_leds(1,CRGB::Orange);
  }
  else if (sound>25 && sound<40){
    turn_with_hsv(CHSV(64,255,210));
//    turn_leds(1,CRGB(64,128,128));
  }
  else if (sound>40 && sound<50){
    turn_with_hsv(CHSV(HUE_YELLOW,150,220));
//    turn_leds(1,CRGB(0,128,192));
  }
  else if (sound>60 && sound<70){
    turn_with_hsv(CHSV(25,255,230));
//    turn_leds(1,CRGB::Cyan);
  }
  else if (sound>70 && sound<100){
    turn_with_hsv(CHSV(100,255,240));
//    turn_leds(1,CRGB::Brown);
  }
  else if (sound>100){
    turn_with_hsv(CHSV(HUE_RED,255,255));
//    turn_leds(1,CRGB::Red);
  }
  return 0; 
}

int music_v2(int delay_time){
   low_light_val=170;
   for (int j=0;j<256;j++){
      turn_with_hsv(CHSV(j,low_light_val,low_light_val));
      if (digitalRead(sound_pin)==1){
          turn_with_hsv(CHSV(j,255,255));
        }
      if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(delay_time);
    }
    for (int j=255;j>=0;j--){
        turn_with_hsv(CHSV(j,low_light_val,low_light_val));
        if (digitalRead(sound_pin)==1){
          turn_with_hsv(CHSV(j,255,255));
        }
        if (check_break()!=1){
          when_break();
          return 0;
    }
        delay(delay_time);
      }
  return 0;
  
}

int music_v3(){
  if (digitalRead(sound_pin)==1){
     turn_with_hsv(CHSV(HUE_RED,255,255));
      if (check_break()!=1){
        when_break();
        return 0;
    }
     delay(85);
}
  else{
    turn_with_hsv(CHSV(HUE_BLUE,255,150));
  }
  return 0;
}

int music_v4(){
  

  if (digitalRead(sound_pin)==1){
    struct_holder=get_colors();
    turn_leds(1,CRGB(struct_holder.one,struct_holder.two,struct_holder.three));
//     turn_with_hsv(CHSV(HUE_RED,255,255));
//     delay(50);
}
  else{
    turn_with_hsv(CHSV(HUE_BLUE,120,255));
  }

  return 0;
}

int one_pattern(){
  CRGB color1=CRGB(255,0,0);
  CRGB color2=CRGB(0,255,0);
  
  for (i=0;i<NUM_LEDS;i++){
    if (i%2==0){
      leds[i]=color1;
    }
    else{
      leds[i]=color2;
    }
  }
  FastLED.show();
  if (check_break()!=1){
        when_break();
        return 0;
    }
  delay(250);
  int num_iterations=5;
  for (i=0;i<num_iterations;i++){
    for (int j=0;j<NUM_LEDS;j++){
      if (leds[j]==color1){
        leds[j]=color2;
      }
      else{
        leds[j]=color1;
      }
      if (check_break()!=1){
        when_break();
        return 0;
    }
     if (check_break()!=1){
        when_break();
        return 0;
    }
    delay(15);
    }
    FastLED.show();
  }
  
  if (check_break()!=1){
        when_break();
        return 0;
    }
  delay(150);
  return 0;  
}

int fade(int delay_time){

    for (int j=0;j<256;j++){
      turn_with_hsv(CHSV(j,255,255));
       if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(delay_time);
    }

    for (int j=255;j>=0;j--){
        turn_with_hsv(CHSV(j,255,255));
        if (check_break()!=1){
          when_break();
          return 0;
        }
        delay(delay_time);
      }
  BRIGHTNESS=0;
  change_brightness(255);
  return 0;
}

int turn_with_hsv(CHSV color){
  for (i=0;i<NUM_LEDS;i++){
    leds[i]=color;
  }
  FastLED.show();
  return 0;
}





int specific_fade(int speedd){
  speedd=speedd/2;
  CRGB last_color=leds[1];
   if (last_color.r!=0 || last_color.g!=0 || last_color.b!=0){
    for (i=255;i>=0;i--){
      FastLED.setBrightness(i);
      FastLED.show();
      if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(speedd);
    }
    for (i=0;i<256;i++){
      FastLED.setBrightness(i);
      FastLED.show();
      if (check_break()!=1){
        when_break();
        return 0;
    }
      delay(speedd);
    }
   }
  BRIGHTNESS=0;
  change_brightness(255);
  return 0;
}


