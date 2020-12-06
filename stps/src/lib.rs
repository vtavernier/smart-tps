#![no_std]
#![feature(core_intrinsics, lang_items)]

use core::panic::PanicInfo;

#[cfg(target = "thumbv7em-none-eabi")]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // TODO: Print a panic message somewhere before aborting
    loop {}
}

#[cfg(not(target = "thumbv7em-none-eabi"))]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // On x86_64, just abort
    core::intrinsics::abort()
}

#[lang = "eh_personality"] extern fn rust_eh_personality() {}

#[no_mangle]
pub extern "C" fn get_message() -> *const u8 {
    b"Hello, world!\0".as_ptr()
}
