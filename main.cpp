#include "mbed.h"
#include "MMA8451Q.h"
#include "DebounceIn.h"
#include <string>
#include <sstream>
#include "USBKeyboard.h"
 
//USBKeyboard
USBKeyboard keyboard;
DigitalOut myled(LED1);
DebounceIn capTouch(D8);
DebounceIn middleTouch(D9);

// define I2C Pins and address for KL25Z. Taken from default sample code.
PinName const SDA = PTE25;
PinName const SCL = PTE24;
#define MMA8451_I2C_ADDRESS (0x1d<<1)

//serial connection to PC via USB
Serial pc(USBTX, USBRX);

int default_delay = 10;
int prev_delay = 10;
int curr_delay = 10;
//float curr_delay = 0.3;

int main(void)
{
    pc.printf("test2");
    //configure on-board I2C accelerometer on KL25Z
    MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);
    
    float y;
    int tilt_state = 0; //0 if straight, 1 if tilted left, 2 if tilted right
    
    int oldCapTouch=1;
    int newCapTouch;
    wait(.001);
    
    int oldMiddleTouch = 1;
    int newMiddleTouch;
    wait(.001);
    
    middleTouch.mode(PullUp);
    capTouch.mode(PullUp);
    
    
    while (1) {
        int curr_tilt_state;
        newCapTouch = capTouch;
        newMiddleTouch = middleTouch;
        myled = capTouch;
        
        if ((newCapTouch==1) && (oldCapTouch==0)) {
            keyboard.keyCode('w');
        }
        if (( newMiddleTouch == 0) && (oldMiddleTouch == 1)){
            keyboard.keyCode('\n');
            }
        
        oldCapTouch = newCapTouch;
        oldMiddleTouch = newMiddleTouch;
        
        y = acc.getAccY() * 100;
        if ( y >= 15 ) {
            curr_tilt_state = 1;
        } else if ( y <= -15) {
            curr_tilt_state = 2;
        } else {
            curr_tilt_state = 0;
            }
        if ((tilt_state != curr_tilt_state) && curr_delay <= 0) {
            tilt_state = curr_tilt_state;
            if (tilt_state == 1){
                pc.printf("tilted left, delay reset");
                curr_delay = default_delay; 
                prev_delay = default_delay;  
                keyboard.keyCode('a');
            } else if (tilt_state == 2){
                pc.printf("tilted right, delay reset");
                curr_delay = default_delay; 
                prev_delay = default_delay;    
                keyboard.keyCode('d');
            } else {
                pc.printf("straight, delay reset"); 
                curr_delay = default_delay; 
                prev_delay = default_delay;    
                }
            pc.printf("\r\n");    
        } else if (curr_delay <= 0){
            //here we start to reduce the delay
                if (tilt_state == 1){
                pc.printf("tilted left"); 
                curr_delay = prev_delay - 2;
                prev_delay = curr_delay;    
                keyboard.keyCode('a');
                pc.printf(", set delay to %d", curr_delay);
            } else if (tilt_state == 2){
                pc.printf("tilted right");
                curr_delay = prev_delay - 2;
                prev_delay = curr_delay;
                pc.printf(", set delay to %d", curr_delay);
                keyboard.keyCode('d');
            }
            } 
            else {
                curr_delay -= 1;
                pc.printf("loop ");   
        }

        
        wait(0.02); // wait 50ms (20Hz update rate)
    }
}