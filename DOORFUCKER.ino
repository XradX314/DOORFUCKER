#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>
#define OPTpin 3
#define OKpin 4
RCSwitch mySwitch = RCSwitch();
LiquidCrystal_I2C lcd(0x27,16,2);


typedef struct{
unsigned long bitdata; //4 bytes//
unsigned int bl; //2 bytes//
unsigned long val;//4 bytes//
int prot;//2 bytes//
int pl;//2 bytes//
}_key;//14 bytes=112 bits//



int totalColumns = 16;
int totalRows = 2;

unsigned long tiempoInicio = 0;
unsigned long tiempoEspera = 500; // Tiempo en milisegundos para cada fase del semáforo
unsigned long tiempoAnterior = 0;
unsigned long tiempoActual = 0;
int state=0;    ////
short choice=0;
int mem_pos=0;
int mem_read=0;
int usage=0;
short mem_state=0;
int option=HIGH;
int select=HIGH;
int vis;
_key key[10];//140 Bytes//

void setup() {

mySwitch.enableReceive(0);
mySwitch.enableTransmit(10);
pinMode(OPTpin,INPUT_PULLUP);
pinMode(OKpin, INPUT_PULLUP);
int k=0;

for(int i=0;i<10;i++){
  EEPROM.get(0+(i*14), key[i].bitdata);
  EEPROM.get(4+(i*14), key[i].bl);
  EEPROM.get(6+(i*14), key[i].val);
  EEPROM.get(10+(i*14), key[i].prot);
  EEPROM.get(12+(i*14), key[i].pl);
  if(key[i].bl!=0){
    mem_pos++;
  }
}




lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("-- DOORFVCKER --");
delay(2000);
lcd.clear();


}

void loop() {
  tiempoActual = millis();
  tiempoAnterior = tiempoActual - tiempoInicio;
  option=digitalRead(OPTpin);
  select=digitalRead(OKpin);
  if (tiempoAnterior > (150)) {
    tiempoInicio = tiempoActual;
    if(option==LOW){
      lcd.clear();
      if(state==0){
      state++;
      }else{
      if(state==1){
        state=4;
      }else{
        if(state==4){
          state=0;
        }
      }

      }

      if(state==3){
        if(mem_read<mem_pos-1){
            mem_read++;
        }else{
          mem_read=0;
        }


      }

      if(state==2){
        if(mem_read<=mem_pos){
          mem_read++;
        }else{
          mem_read=0;
        }
        
      }

      if(state==5){
          if(mem_read==0){
            mem_read++;

          }else{
          mem_read=0;
        }

      }

    }
      if(select==LOW){
        usage=0;

      lcd.clear();
      if(state==2){
        choice=1;
      }
      if(state==0){
      mySwitch.resetAvailable();
      state=2;
      }
      if(state==3){
        choice=1;
      }
      if(state==1){
        state=3;
      }
      if(state==4){
        state=5;
      }else{
      if(state==5&&mem_read==1){
        choice=1;
      }else{
      if(state==5&&mem_read==0){
        state=0;
      }

      }}

    }

  }


switch(state){
      case 0:
      
      lcd.setCursor(0, 0);
      lcd.print("SCAN 433MHZ KEY");
      


      break;
      case 1:
      
      lcd.setCursor(0, 0);
      lcd.print("PLAY 433MHZ KEY");
      

      break;
      case 2:

        if(mem_pos==10){
          mem_state=1;
          lcd.setCursor(0, 0);
          lcd.print("MEMORY FULL");
          lcd.setCursor(0, 1);
          lcd.print("REWRITE:");
          if(mem_read==10){
            lcd.print(" EXIT");
          }else{
          vis=mem_read+1;
          lcd.print(vis); 
          }
          if(choice){
            mem_pos=mem_read;
            choice=0;
          }
          
        }
        if (mySwitch.available()) {
          key[mem_pos].bitdata=mySwitch.getReceivedRawdata();
          key[mem_pos].bl=mySwitch.getReceivedBitlength();
          key[mem_pos].val=mySwitch.getReceivedValue();
          key[mem_pos].prot=mySwitch.getReceivedProtocol();
          key[mem_pos].pl=mySwitch.getReceivedDelay();
          EEPROM.put(0+(mem_pos*14), key[mem_pos].bitdata);
          EEPROM.put(4+(mem_pos*14), key[mem_pos].bl);
          EEPROM.put(6+(mem_pos*14), key[mem_pos].val);
          EEPROM.put(10+(mem_pos*14), key[mem_pos].prot);
          EEPROM.put(12+(mem_pos*14), key[mem_pos].pl);
          lcd.setCursor(0, 0);
          lcd.print("RECIEVED ");
          lcd.print(key[mem_pos].bl);
          lcd.print(" BITS");
          lcd.setCursor(0, 1);
          scrollMessage(1, dec2binWzerofill(key[mem_pos].val,key[mem_pos].bl), 100, totalColumns);
          mem_pos++;

          state=0;
        }else{
      lcd.setCursor(0, 0);
      lcd.print("    LISTENING   ");
        }

      
      break;

      case 3:
      if (mem_pos!=0) {
        if(choice==0){
          lcd.setCursor(0, 0);
          lcd.print("MEMORIA: ");
          vis=mem_read+1;
          lcd.print(vis);  
        }else{
      lcd.setCursor(0, 0);
      lcd.print("TRANSMITTING");
      mySwitch.setProtocol(key[mem_read].prot);
      mySwitch.setPulseLength(key[mem_read].pl);
      lcd.setCursor(0, 1);
      scrollMessage(1, dec2binWzerofill(key[mem_read].val,key[mem_read].bl), 70, totalColumns);
      mySwitch.setRepeatTransmit(15);
      mySwitch.send(key[mem_read].val,key[mem_read].bl);
      choice=0;
      state=0;
      }
      }else{
        lcd.setCursor(0, 0);
        lcd.print("     NO MEM     ");
        delay(500);
        state=0;
      }
      break;
      case 4:
      lcd.setCursor(0, 0);
      lcd.print("DELETE EXISTING");
      lcd.setCursor(0, 1);
      lcd.print("KEYS");
      break;
      case 5:
        if (mem_pos!=0) {
          if(choice==0){
          lcd.setCursor(0, 0);
          lcd.print("SEGURO?: ");
          vis=mem_read;
          lcd.print(vis);  
        }else{
      lcd.setCursor(0, 0);
      lcd.print("BORRANDO: ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
        key[i].bitdata=0;
        key[i].bl=0;
        key[i].pl=0;
        key[i].prot=0;
        key[i].val=0;
      }
      mem_pos=0;
      
      delay(800);
      choice=0;
      state=0;
      }
      }else{
        lcd.setCursor(0, 0);
        lcd.print("     NO MEM     ");
        delay(500);
        state=0;
      }
      break;

      default:

      lcd.setCursor(0, 0);
      

      break;






}

}


static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}


void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}
