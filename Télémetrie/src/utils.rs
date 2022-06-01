/// **Print a vector of bytes as binary**
pub fn print_bytes(bytes: &Vec<u8>) {
    println!("================BIT==================");
    let mut int = 0;
    for i in 0..bytes.len() {
        int += bytes[i] as u64 * 2u64.pow(8 * (bytes.len() - 1 - i) as u32);
    }
    println!("bytes = {} = {:?}", int, bytes);
    for byte in bytes {
        let mut byte_str = String::new();
        for i in 0..8 {
            let bit = ((*byte >> i) & 1) as u8;
            byte_str.push_str(&format!("{}", bit));
        }
        println!("{}", byte_str);
    }
}

/// **Byte to binary string**
pub fn byte_to_binary_str(byte: u8) -> String {
    let mut byte_str = String::new();
    for i in 0..8 {
        let bit = ((byte >> i) & 1) as u8;
        byte_str.push_str(&format!("{}", bit));
    }
    byte_str
}

pub fn byte_to_bool_vec(byte: u8) -> Vec<bool> {
    let mut bool_vec = Vec::new();
    for i in 0..8 {
        let bit = ((byte >> i) & 1) as u8;
        bool_vec.push(bit == 1);
    }
    bool_vec
}

pub fn bool_vec_to_byte(source: Vec<bool>) -> u8 {
    let mut result: u8 = 0;
    // This assumes the array never contains more than 8 elements!
    let mut index: usize = 8 - source.len();

    // Loop through the array
    for b in source {
        // if the element is 'true' set the bit at that position
        if b {
            result |= 1 << index;
        }

        index += 1;
    }

    result
}