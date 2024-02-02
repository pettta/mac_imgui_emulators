typedef struct Ports {
    uint8_t read1; // read 1 -- coin, start, shoot, joystick 
    uint8_t read2; // read 2 -- dip switches 
    uint8_t shift1; // read 3 -- shift register 1 
    uint8_t shift0; // read 3 -- shift register 2 
    uint8_t shift_offset; // write 2 -- shift register offset
    uint8_t sound; // write 3 -- sound 1 TODO 
    uint8_t sound2; // write 5 -- sound 2 TODO 
    uint8_t write4; // write 4 -- fill shift register TODO 
    uint8_t write6; // write 6 -- debug port TODO? 
} Ports;
