import serial
import socket
import argparse

if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='Serial to UDP')
    parser.add_argument('-p', '--port', type=str, default='/dev/ttyUSB0',
                        help='serial port')
    parser.add_argument('-b', '--baud', type=int, default=115200,
                        help='baud rate')
    parser.add_argument('-u', '--udp', type=str, default=10101,
                        help='UDP port')

    args = parser.parse_args()

    ser = serial.Serial(args.port, args.baud)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:
        data = []
        for i in range(9):
            data.append(ser.readline())
        data = b''.join(data)
        sock.sendto(data, ('localhost', int(args.udp)))