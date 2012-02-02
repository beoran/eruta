/*
use std;
fn main(args: [str]) {
    std::io::println("hello world from '" + args[0] + "'!");
}
*/

use std;


native mod crypto {    
    fn SHA1(src: *u8, sz: ctypes::c_uint, out: *u8) -> *u8;
}

fn as_hex(data: [u8]) -> str {
    let acc = "";
    for byte in data { acc += #fmt("%02x", byte as uint); }
    ret acc;
}

fn sha1(data: str) -> str unsafe {
    let bytes = str::bytes(data);
    let hash = crypto::SHA1(vec::unsafe::to_ptr(bytes),
                            vec::len(bytes), ptr::null());
    ret as_hex(vec::unsafe::from_buf(hash, 20u));
}

fn main(args: [str]) {
    std::io::println(sha1(args[1]));
}

