#![no_std]
#![no_main]

#![feature(asm_experimental_arch)]

use core::arch::asm;
use panic_halt as _;

const LOOP_MS: u16 = 0x0a52;

fn delay(ms: u16) {
    let mut count = 0;
    while count < ms {
        let mut l = 0;
        while l < LOOP_MS {
            unsafe { asm!("nop\n\t"); }
            l += 1;
        }
        count += 1;
    }
}

#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);

    let mut led = pins.d13.into_output();

    loop {
        led.toggle();
        delay(500);
    }
}

