const avr = struct {
    pub const ddrb = @as(*volatile u8, @ptrFromInt(0x24));
    pub const portb = @as(*volatile u8, @ptrFromInt(0x25));
};

const led_pin: u8 = 5;
const led_bit: u8 = 1 << led_pin;

const loop_ms = 0x0a52;

fn delay(ms: u16) void {
    var count: u16 = 0;
    while (count < ms) : (count += 1) {
        var loop: u16 = 0;
        while (loop < loop_ms) : (loop += 1) {
            asm volatile ("" ::: "memory");
            
        }
        
    }
}


export fn main() noreturn {
    avr.ddrb.* = led_bit;
    avr.portb.* = led_bit;
    
    
    
    while (true) {
        avr.portb.* ^= led_bit;
        delay(500);
    }
}

