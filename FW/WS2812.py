# Example using PIO to drive a set of WS2812 LEDs.

import array, time
from machine import Pin
import rp2

# Configure the number of WS2812 LEDs.
NUM_LEDS = 64
PIN_NUM = 22
brightness = 0.1

@rp2.asm_pio(sideset_init=rp2.PIO.OUT_LOW, out_shiftdir=rp2.PIO.SHIFT_LEFT, autopull=True, pull_thresh=24)
def ws2812():
    T1 = 2
    T2 = 5
    T3 = 3
    wrap_target()
    label("bitloop")
    out(x, 1)               .side(0)    [T3 - 1]
    jmp(not_x, "do_zero")   .side(1)    [T1 - 1]
    jmp("bitloop")          .side(1)    [T2 - 1]
    label("do_zero")
    nop()                   .side(0)    [T2 - 1]
    wrap()


# Create the StateMachine with the ws2812 program, outputting on pin
sm = rp2.StateMachine(0, ws2812, freq=8_000_000, sideset_base=Pin(PIN_NUM))

# Start the StateMachine, it will wait for data on its FIFO.
sm.active(1)

# Display a pattern on the LEDs via an array of LED RGB values.
ar = array.array("I", [0 for _ in range(NUM_LEDS)])

##########################################################################
def pixels_show():
    dimmer_ar = array.array("I", [0 for _ in range(NUM_LEDS)])
    for i,c in enumerate(ar):
        r = int(((c >> 8) & 0xFF) * brightness)
        g = int(((c >> 16) & 0xFF) * brightness)
        b = int((c & 0xFF) * brightness)
        dimmer_ar[i] = (g<<16) + (r<<8) + b
    sm.put(dimmer_ar, 8)
    time.sleep_ms(10)

def pixels_set(i, color):
    ar[i] = (color[1]<<16) + (color[0]<<8) + color[2]

def pixels_fill(color):
    for i in range(len(ar)):
        pixels_set(i, color)

def color_chase(color, wait):
    for i in range(NUM_LEDS):
        pixels_set(i, color)
        time.sleep(wait)
        pixels_show()
    time.sleep(0.2)
 
def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    if pos < 85:
        return (255 - pos * 3, pos * 3, 0)
    if pos < 170:
        pos -= 85
        return (0, 255 - pos * 3, pos * 3)
    pos -= 170
    return (pos * 3, 0, 255 - pos * 3)
 
 
def rainbow_cycle(wait):
    for j in range(255):
        for i in range(NUM_LEDS):
            rc_index = (i * 256 // NUM_LEDS) + j
            pixels_set(i, wheel(rc_index & 255))
        pixels_show()
        time.sleep(wait)

BLACK = (0, 0, 0)
RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
WHITE = (255, 255, 255)

GRAY = (0X1F, 0XaF, 0X3F)

COLORS = (BLACK, RED, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE)

def PixelFadeInOut(number, color):
    fadedColor = [0,0,0]
    flag = 0;
    for i in range (255):
        if(fadedColor[0] < color[0]):
            fadedColor[0] += 1
        if(fadedColor[1] < color[1]):
            fadedColor[1] += 1
        if(fadedColor[2] < color[2]):
            fadedColor[2] += 1
        if((fadedColor[2] == color[2]) and (fadedColor[1] == color[1]) and (fadedColor[0] == color[0])):
            break
        pixels_set(number, fadedColor)
        pixels_show()
        time.sleep(0.1)
    flag = 0
    for i in range(255):
        if(fadedColor[0] > 0):
            fadedColor[0] -= 1
        if(fadedColor[1] > 0):
            fadedColor[1] -= 1
        if(fadedColor[2] > 0):
            fadedColor[2] += 1
        if((fadedColor[2] == 0) and (fadedColor[1] == 0) and (fadedColor[0] == 0)):
            break
        pixels_set(number, fadedColor)
        pixels_show()
        time.sleep(0.1)

class ColorSet:
        
    def __init__(pixel, number, color, rs, fs):
        pixel.color = color
        pixel.number = number
        pixel.rs = rs
        pixel.fs = fs
        pixel.pixelColor = [0,0,0]
        pixel.idFlag = 0
    
    
    def SetColor(color):
        
        if((color.pixelColor[0] >= color.color[0]) and (color.pixelColor[1] >= color.color[1]) and (color.pixelColor[2] >= color.color[2])):
            color.idFlag = 1
            
        if(color.idFlag == 0):
            if(color.pixelColor[0] < color.color[0]):
                color.pixelColor[0] += color.rs
            if(color.pixelColor[1] < color.color[1]):
                color.pixelColor[1] += color.rs
            if(color.pixelColor[2] < color.color[2]):
                color.pixelColor[2] += color.rs
        elif(color.idFlag == 1):
            if((color.pixelColor[0] < 1) or (color.pixelColor[0] < 1) or (color.pixelColor[0] < 1)):
                color.pixelColor[0] = 0
                color.pixelColor[1] = 0
                color.pixelColor[2] = 0
                color.idFlag = 0
            else:
                color.pixelColor[0] -= color.fs
                color.pixelColor[1] -= color.fs
                color.pixelColor[2] -= color.fs
                
        pixels_set(color.number, color.pixelColor)

        
def RGBFadeInOut():
    for i in range(64):
        pixels_set(i, (0X0F, 0X0F, 0X0F))
    pixels_show()
    p1 = ColorSet(1, GREEN, 1, 1)
    p2 = ColorSet(2, RED, 2, 5)
    p3 = ColorSet(3, BLUE, 7, 2)
    p30 = ColorSet(30, PURPLE, 5, 5)
    p40 = ColorSet(40, GRAY, 3, 6)
    while True:
        p1.SetColor()
        p2.SetColor()
        p3.SetColor()
        p30.SetColor()
        p40.SetColor()
        pixels_show()
        #time.sleep(0.1)


RGBFadeInOut()
