/*
 * Project hht-particle
 * Description: MVP code to decode HD44780 display signals to "scrap" a display for remote viewing/storage
 * Author: Danny Salzman
 * Date:8/23/18
 */



// Control signals
uint8_t rs = D5;
uint8_t enable = D6;
uint8_t rw = D4;

// Data lines
uint8_t d4 = D0;
uint8_t d5 = D1;
uint8_t d6 = D2;
uint8_t d7 = D3;

void decode(void);

bool gotUpNib = false;

uint8_t lowerNibble;
uint8_t upperNibble;

uint8_t command;

int dataPinValues;

char LCD[32] = { 32 };

uint8_t charIndex = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("HD44870 Decoder Starting...");
    pinMode(rs, INPUT);
    pinMode(enable, INPUT);
    pinMode(rw, INPUT);
    pinMode(d4, INPUT_PULLDOWN);
    pinMode(d5, INPUT_PULLDOWN);
    pinMode(d6, INPUT_PULLDOWN);
    pinMode(d7, INPUT_PULLDOWN);
    
    attachInterrupt(enable, decode, RISING);
    

    
}

void loop() {

    // delay(500);
    // Serial.println("LCD Screen:");
    // for(int i = 0; i < 16; i++){
    //      Serial.print(LCD[i]);
    // }
    // Serial.println();
    // for(int j = 16; j < 32; j++){
    //      Serial.print(LCD[j]);
    // }
    // Serial.println();


}

void decode(){
    // command received
    if(pinReadFast(rs) == 0){
        if(gotUpNib == false){
        	upperNibble = (GPIOB -> IDR & 0B0000000011110000) >> 4;
        	gotUpNib = true;
        	return;
        }else{
        	lowerNibble = (GPIOB -> IDR & 0B0000000011110000) >> 4;
        	command = (upperNibble << 4) | lowerNibble;
        	gotUpNib = false;
            evalCommand(command);
            // Serial.println();
            // Serial.print("Command: ");
            // Serial.println(command, HEX);
        	return;
        }
    }
    // print character received 
    if(pinReadFast(rs) == 1){
    	if(gotUpNib == false){
            upperNibble = (GPIOB -> IDR & 0B0000000011110000) >> 4;
            gotUpNib = true;
            // Serial.print("upper: ");
            // Serial.println(upperNibble, BIN);
            return;
    
        }
        else{
            lowerNibble = (GPIOB -> IDR & 0B0000000011110000) >> 4;
            gotUpNib = false;
            LCD[charIndex] = (char) (upperNibble << 4 | lowerNibble);
            Serial.print( LCD[charIndex]);
            charIndex = (charIndex + 1) % 31;
            // Serial.print("lower: ");
            // Serial.println(lowerNibble, BIN);
        }

    }
}

const uint8_t LCD_CLEAR =  0x01;
const uint8_t LCD_CLEAR_MASK = 0xFF;

const uint8_t LCD_SETDDRAMADDR = 0x80;
const uint8_t LCD_SETDDRAMADDR_MASK = 0x80;

void evalCommand(uint8_t command){
    if((command & LCD_CLEAR_MASK) == LCD_CLEAR){
        for(int i = 0; i < 32; i++){LCD[i]=32;}
        charIndex = 0;
        return;
    }
    if((command & LCD_SETDDRAMADDR_MASK) == LCD_SETDDRAMADDR){
        if((command & 0x7f) < 16){
            charIndex = (command & 0x7f);
        }else{
            charIndex = (command & 0x7f) - 48;
        }
        return;
    }

}
