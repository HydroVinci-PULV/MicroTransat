extern crate serial;

use std::os::unix::io::FromRawFd;
use std::os::unix::io::AsRawFd;
use std::env;
use std::time::Duration;
use std::net::UdpSocket;
use std::io::prelude::*;
use serial::prelude::*;
// usage -S=<serial port> -B=<baud rate> -H=<remote host> -P<remote port>
fn main() {
    let args: Vec<String> = env::args().collect();
    let mut serial_port = String::from("/dev/ttyUSB0");
    let mut baud_rate = String::from("9600");
    let mut remote_host = String::from("127.0.0.1");
    let mut remote_port = String::from("2947");

    // if there are less than 4 arguments, print usage and exit
    if args.len() < 4 {
        println!("usage -S=<serial port> -B=<baud rate> -H=<remote host> -P<remote port>");
        return;
    }

    for arg in args {
        if arg.starts_with("-S=") {
            serial_port = arg.replace("-S=", "");
        } else if arg.starts_with("-B=") {
            baud_rate = arg.replace("-B=", "");
        } else if arg.starts_with("-H=") {
            remote_host = arg.replace("-H=", "");
        } else if arg.starts_with("-P=") {
            remote_port = arg.replace("-P=", "");
        }
    }

    let mut serial = serial::open(&serial_port).unwrap();

    // convert baud_rate from string to serial::BaudRate enum
    let baud_rate = match baud_rate.as_ref() {
        "9600" => serial::Baud9600,
        "19200" => serial::Baud19200,
        "38400" => serial::Baud38400,
        "57600" => serial::Baud57600,
        "115200" => serial::Baud115200,
        _ => serial::Baud9600,
    };

    serial.reconfigure(&|settings| {
        settings.set_baud_rate(baud_rate).unwrap();
        settings.set_char_size(serial::Bits8);
        settings.set_parity(serial::ParityNone);
        settings.set_stop_bits(serial::Stop1);
        settings.set_flow_control(serial::FlowNone);
        Ok(())
    }).unwrap();

    let udp_socket;

    // udp socket bind string
    let bind_string = format!("{}:{}", remote_host, remote_port);

    // create udp socket
    match UdpSocket::bind(&bind_string) {
        Ok(socket) => {
            udp_socket = socket.as_raw_fd();
        },
        Err(e) => {
            println!("Error: {}", e);
            return;
        }
    }

    // read from serial port
    let mut buffer = [0; 1024];
    let mut n;
    
    loop {
        n = serial.read(&mut buffer).unwrap();
        if n > 0 {
            let mut data = String::from("");
            for i in 0..n {
                data.push(buffer[i] as char);
            }
        //    send data to udp socket
            let socket = unsafe { UdpSocket::from_raw_fd(udp_socket) };
            socket.send_to(&data.as_bytes(), &bind_string).unwrap();
        }
        std::thread::sleep(Duration::from_millis(100));
    }

}

