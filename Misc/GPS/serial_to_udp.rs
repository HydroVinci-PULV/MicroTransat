use std::env;

fn main() {

}

// mutable global variables
// serial port
let mut serial_port:str = "";
// baud rate
let mut baud_rate:u32 = 0;
// UDP port
let mut udp_port:u16 = 0;
// UDP host
let mut udp_host:str = "";

// Function that opens a UDP socket on a given port and host
fn open_udp_socket(int port, str host) {
    // create a UDP socket
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // set the socket options
    setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, 1);
    // bind the socket to the given port and host
    let sockaddr_in addr = {
        sin_family: AF_INET,
        sin_port: htons(port),
        sin_addr: inet_addr(host),
        sin_zero: { 0, 0, 0, 0, 0, 0, 0, 0 }
    };
    bind(udp_socket, addr);
}