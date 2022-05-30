const { SerialPort } = require('serialport');
const GPS = require('gps');
const { DelimiterParser } = require('@serialport/parser-delimiter')

const port = new SerialPort({
    path: '/dev/ttyACM0',
    baudRate: 9600,
  });

  const parser = port.pipe(new DelimiterParser({ delimiter: '\r\n' }))

  const gps = new GPS;

  parser.on('data', data => {
    // convert buffer to string
    const str = data.toString();
    gps.update(str);
  });

  gps.on('data', data => {
    console.log(data.lat);
  });