use std::cmp::min;

mod utils;

/// **Compress a double to a desired bits sized vector of bytes**
///
/// takes as parameters :
/// - a double
/// - max bit size to allow for the return value
/// - range of the double
/// and figure out the precision of the double to fit in the max bit size and return a bytes array
///
/// Return a vector of bytes
pub fn compress(value: f64, max_bit_size: u8, start: f64, end: f64) -> Vec<u8> {
    #[cfg(debug_assertions)]
    println!("Compress start");
    #[cfg(debug_assertions)]
    println!("compress({}, {}, {}, {})", value, max_bit_size, start, end);
    // number of possible values for the double
    let n_possible_values = 2u64.pow(max_bit_size as u32);
    #[cfg(debug_assertions)]
    println!("n_possible_values: {}", n_possible_values);
    // precision of the double
    let precision = (end - start).abs() / n_possible_values as f64;
    #[cfg(debug_assertions)]
    println!("precision = {}", precision);
    // number of bytes to store the max_bit_size bits
    let n_required_bytes: u8 = (max_bit_size + 7) / 8;
    #[cfg(debug_assertions)]
    println!("n_required_bytes = {}", n_required_bytes);
    // vector of bytes to store the max_bit_size bits
    let mut bytes: Vec<u8> = Vec::with_capacity(n_required_bytes as usize);
    // copy of the value to work on
    let mut w_value = value;
    // if the start value is negative, we need to padd the w_value with it to make the value unsigned
    w_value -= start;
    // find the next double that is a multiple of the step
    let remainder = w_value % precision;
    // if the remainder is not 0, we need to add the step to the value
    if remainder != 0.0 {
        // if remainder is inferior to step/2, we need to substract the remainder to the value
        if remainder < precision / 2.0 {
            w_value -= remainder;
        } else {
            // else we need to add the remainder to the value
            w_value += precision - remainder;
        }
    }
    #[cfg(debug_assertions)]
    println!("w_value = {}", w_value);
    let mut pos = min((w_value / precision) as u64, n_possible_values - 1);
    #[cfg(debug_assertions)]
    println!("pos = {}", pos);
    // convert the masked value to bytes
    #[cfg(debug_assertions)]
    println!("byte = {}", pos as u8);
    bytes.push(pos as u8);
    for _i in 1..n_required_bytes {
        pos >>= 8;
        #[cfg(debug_assertions)]
        println!("byte = {}", pos as u8);
        bytes.push(pos as u8);
    }
    // invert vector
    bytes.reverse();

    #[cfg(debug_assertions)]
    utils::print_bytes(&bytes);
    #[cfg(debug_assertions)]
    println!("Compress end\n");

    bytes


}

/// **Decompress a vector of bytes to a double**
///
/// takes as parameters :
/// - a vector of bytes
/// - max bit size of the value in the vector of bytes
/// - range of the double
///
/// Return a double
pub fn decompress(bytes: &Vec<u8>, max_bit_size: u8, start: f64, end: f64) -> f64 {
    #[cfg(debug_assertions)]
    println!(
        "decompress({:?}, {}, {}, {})",
        bytes, max_bit_size, start, end
    );
    // number of possible values for the double
    let n_possible_values = 2u64.pow(max_bit_size as u32);
    // precision of the double
    let precision = (end - start) / n_possible_values as f64;
    #[cfg(debug_assertions)]
    println!("precision = {}", precision);
    // number of bytes to store the max_bit_size bits
    let n_required_bytes: u8 = (max_bit_size + 7) / 8;
    // check that the number of bytes is correct
    assert!(bytes.len() == n_required_bytes as usize);
    // convert the bytes to a u64
    // invert vector
    let bytes = bytes.clone();
    let mut pos: u64 = 0;
    for i in 0..n_required_bytes {
        let byte = bytes[i as usize] as u64;
        pos += byte << (8 * (n_required_bytes - 1 - i));
    }
    #[cfg(debug_assertions)]
    println!("pos = {}", pos);
    // convert to a double
    let mut w_value = pos as f64 * precision;
    #[cfg(debug_assertions)]
    println!("w_value = {}", w_value);
    // if the start value is negative, we need to padd the w_value with it to restore the value
    w_value += start;
    #[cfg(debug_assertions)]
    println!("w_value = {}", w_value);

    w_value

}

/// **concatenate multiple vectors of bytes**
///
/// takes as parameters :
/// - a vector of vectors of bytes
/// - a vector of bits sizes
///
/// Return a vector of bytes
pub fn concat_bytes(vec_bytes: &mut Vec<Vec<u8>>, bits_sizes: &Vec<u8>) -> Vec<u8> {
    // convert vec_bytes into a vector of bool
    let mut vec_bool: Vec<bool> = Vec::new();
    for i in 0..vec_bytes.len() {
        let bytes = &mut vec_bytes[i as usize];
        bytes.reverse();
        let bits = bits_sizes[i as usize];
        let mut bools: Vec<bool> = Vec::new();
        for j in 0..bytes.len() {
            let byte = bytes[j as usize];
            // extend the vector of bool with the bits of the byte
            let b = utils::byte_to_bool_vec(byte);
            bools.extend(b);
        }
        // only keep n bits in bools
        bools.truncate(bits as usize);
        // add the bools to the vector of bool
        vec_bool.extend(bools);
    }
    // convert the vector of bool into a vector of bytes
    let mut w_bytes: Vec<u8> = Vec::new();
    while !vec_bool.is_empty() {
        // take the first 8 bits of the vector of bool
        let mut bools: Vec<bool> = Vec::new();
        for _ in 0..8 {
            bools.push(vec_bool.remove(0));
        }
        // convert the vector of bool into a byte
        let byte = utils::bool_vec_to_byte(bools);
        w_bytes.push(byte);
    }

    #[cfg(debug_assertions)]
    utils::print_bytes(&w_bytes);

    w_bytes

}

/// **Split a vector of bytes into multiple vectors of bytes**
///
/// takes as parameters :
/// - a vector of bytes
/// - a vector of bits sizes
///
/// Return a vector of vectors of bytes
pub fn split_bytes(bytes: &Vec<u8>, bits_sizes: &Vec<u8>) -> Vec<Vec<u8>> {
    #[cfg(debug_assertions)]
    println!("split_bytes({:?}, {:?})", bytes, bits_sizes);
    // convert bytes into a vector of bool
    let mut vec_bool: Vec<bool> = Vec::new();
    for byte in bytes {
        let bools = utils::byte_to_bool_vec(*byte);
        vec_bool.extend(bools);
    }
    // split the vector of bool into multiple vectors of bool
    let mut vec_vec_bool: Vec<Vec<bool>> = Vec::new();
    for bits in bits_sizes {
        let mut bools: Vec<bool> = Vec::new();
        for _ in 0..*bits {
            bools.push(vec_bool.remove(0));
        }
        vec_vec_bool.push(bools);
    }

    // convert the vector of bool into a vector of bytes
    let mut vec_bytes: Vec<Vec<u8>> = Vec::new();
    for vec_bool in vec_vec_bool {
        let mut vec_bool = vec_bool.clone();
        let mut w_bytes: Vec<u8> = Vec::new();
        while !vec_bool.is_empty() {
            // take the first 8 bits of the vector of bool
            let mut bools: Vec<bool> = Vec::new();
            for i in 0..8 {
                if vec_bool.is_empty(){
                    bools.extend(vec![false;8-i]);
                    break;
                }
                bools.push(vec_bool.remove(0));
            }
            // convert the vector of bool into a byte
            let byte = utils::bool_vec_to_byte(bools);
            w_bytes.push(byte);
        }
        w_bytes.reverse();
        vec_bytes.push(w_bytes);
    }

    #[cfg(debug_assertions)]
    for vec_bytes in &vec_bytes {
        utils::print_bytes(vec_bytes);
    }

    vec_bytes

}

pub fn hamming_code(bytes: &Vec<u8>) {
    
}


#[cfg(test)]
pub mod tests {
    use super::*;
    use rand::Rng;

    #[test]
    pub fn test_compress_decompress() {
        // generate random double range
        let start = rand::thread_rng().gen_range(-1000..1000);
        let end = rand::thread_rng().gen_range(start..(start + 1000));
        // convert them to double by dividing by 10
        let start2 = start as f64 / 10.0;
        let end2 = end as f64 / 10.0;
        // generate random double
        let value = rand::thread_rng().gen_range(start..end);
        // convert it to double by dividing by 10
        let value2 = value as f64 / 10.0;
        // generate random bit size
        let max_bit_size = rand::thread_rng().gen_range(4..40);
        // precision
        let precision = (end2 - start2) / (2u64.pow(max_bit_size as u32) as f64);
        // compress the double
        let bytes = compress(value2, max_bit_size, start2, end2);
        // decompress the double
        let value3 = decompress(&bytes, max_bit_size, start2, end2);
        // check that the decompressed value is the same as the original (+/- 0.2)
        assert!(value3 >= value2 - precision && value3 <= value2 + precision);
    }

    #[test]
    pub fn test_heading_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 3600;
        // convert them to double by dividing by 10
        let start2 = start as f64 / 10.0;
        let end2 = end as f64 / 10.0;

        for i in start..end {
            let value = i as f64 / 10.0;
            let bytes = compress(value, 8, start2, end2);
            let value3 = decompress(&bytes, 8, start2, end2);
            assert!(value3 >= value - 1.5 && value3 <= value + 1.5);
        }
    }

    #[test]
    pub fn test_voltage_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 5000;
        // convert them to double by dividing by 100
        let start2 = 0.0;
        let end2 = 50.0;
        for i in start..end {
            let value = i as f64 / 100.0;
            let bytes = compress(value, 12, start2, end2);
            let value3 = decompress(&bytes, 12, start2, end2);
            assert!(value3 >= value - 0.013 && value3 <= value + 0.013);
        }
    }

    #[test]
    pub fn test_humidity_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 10000;
        // convert them to double by dividing by 100
        let start2 = 0.0;
        let end2 = 100.0;
        for i in start..end {
            let value = i as f64 / 100.0;
            let bytes = compress(value, 7, start2, end2);
            let value3 = decompress(&bytes, 7, start2, end2);
            assert!(value3 >= value - 0.8 && value3 <= value + 0.8);
        }
    }

    #[test]
    pub fn test_inondation_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 8;
        // convert them to double by dividing by 100
        let start2 = 0.0;
        let end2 = 8.0;
        for i in start..end {
            let value = i as f64;
            let bytes = compress(value, 3, start2, end2);
            let value3 = decompress(&bytes, 3, start2, end2);
            assert!(value3 == value);
        }
    }

    #[test]
    pub fn test_gps_quality_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 4;
        // convert them to double by dividing by 100
        let start2 = 0.0;
        let end2 = 4.0;
        for i in start..end {
            let value = i as f64;
            let bytes = compress(value, 2, start2, end2);
            let value3 = decompress(&bytes, 2, start2, end2);
            assert!(value3 == value);
        }
    }

    #[test]
    pub fn test_ping_compress_decompress() {
        // generate random double range
        let start = 0;
        let end = 1;
        // convert them to double by dividing by 100
        let start2 = 0.0;
        let end2 = 1.0;
        for i in start..end {
            let value = i as f64;
            let bytes = compress(value, 1, start2, end2);
            let value3 = decompress(&bytes, 1, start2, end2);
            assert!(value3 == value);
        }
    }

    #[test]
    pub fn test_concat_split() {
        let start = rand::thread_rng().gen_range(-1000..1000);
        let end = rand::thread_rng().gen_range(start..(start + 1000));
        let value1 = rand::thread_rng().gen_range(start..end) as f64;
        let value2 = rand::thread_rng().gen_range(start..end) as f64;
        let size = 20;

        let bytes1 = compress(value1, size, start as f64, end as f64);
        let bytes2 = compress(value2, size, start as f64, end as f64);

        #[cfg(debug_assertions)]
        {
            println!("\n");
            utils::print_bytes(&bytes1);
            utils::print_bytes(&bytes2);
            println!("\n");
        }

        let concat = concat_bytes(&mut vec![bytes1, bytes2], &vec![size, size]);
        let split = split_bytes(&concat, &vec![size, size]);

        let res1 = decompress(&split[0], size, start as f64, end as f64);
        let res2 = decompress(&split[1], size, start as f64, end as f64);

        let precision = (end - start) as f64 / (2u64.pow(20) as f64);

        assert!(res1 <= value1 + precision && res1 >= value1 - precision);
        assert!(res2 <= value2 + precision && res2 >= value2 - precision);
    }

}
