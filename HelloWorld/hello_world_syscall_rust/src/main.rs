extern crate libc;

use libc::{SYS_write, syscall};

fn main() {    
    let _res = unsafe { syscall(SYS_write, 1, "Hello, World!\n", 14) };
}
