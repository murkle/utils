// https://github.com/VernierST/godirect-js/blob/main/src/constants.js
// https://raw.githubusercontent.com/VernierST/godirect-js/main/LICENSE
// BSD 3-Clause License
// sent like 88, 25, 254, 63
//  // Populate the packet header bytes
//    command[1] = command.length;
//    command[2] = this._decRollingCounter();
//    command[3] = this._calculateChecksum(command);
//const HEADER = new Uint8Array([0x58, 0x00, 0x00, 0x00]);
const INIT = [
    0x1a, // 26
    0xa5, // 165
    0x4a,
    0x06,
    0x49,
    0x07,
    0x48,
    0x08,
    0x47,
    0x09,
    0x46,
    0x0a,
    0x45,
    0x0b,
    0x44,
    0x0c,
    0x43,
    0x0d,
    0x42,
    0x0e,
    0x41, // 65
];

function getStartMeasurementCommand(channelMask) {
    return [
        0x18,
        0xff,
        0x01, (channelMask >> 0) & 0xff, (channelMask >> 8) & 0xff, (channelMask >> 16) & 0xff, (channelMask >> 24) & 0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    ];
}

const STOP_MEASUREMENTS = [0x19, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff];

function getMeasurementPeriodCommand(measurementPeriodInMicroseconds) {
    // 32 + (161<<8) + (7<<16) + (0<<24) = 500,000 microseconds
    return [
        0x1b,
        0xff,
        0x00, (measurementPeriodInMicroseconds >> 0) & 0xff, // eg 32
        (measurementPeriodInMicroseconds >> 8) & 0xff, // eg 161
        (measurementPeriodInMicroseconds >> 16) & 0xff, // eg 7
        (measurementPeriodInMicroseconds >> 24) & 0xff, //eg 0
        0x00,
        0x00,
        0x00,
        0x00,
    ];
}

const DISCONNECT = [0x54];
const GET_INFO = [0x55];
const GET_STATUS = [0x10];
const GET_SENSOR_IDS = [0x51];

// TODO: param what is it?
// param = 1 for Temperature probe
function getSensorInfoCommand(param) {
        return [0x50, param];
    }
    //const GET_SENSOR_INFO = [0x50, 0x01];

const GET_DEFAULT_SENSORS_MASK = [0x56];

/*
const commands = {
  HEADER,
  INIT,
  DISCONNECT,
  START_MEASUREMENTS,
  STOP_MEASUREMENTS,
  SET_MEASUREMENT_PERIOD,
  GET_INFO,
  GET_STATUS,
  GET_SENSOR_IDS,
  GET_SENSOR_INFO,
  GET_DEFAULT_SENSORS_MASK,
};*/

const NORMAL_REAL32 = 0x06;
const WIDE_REAL32 = 0x07;
const APERIODIC_REAL32 = 0x0a;
const SINGLE_CHANNEL_REAL32 = 0x08;
const SINGLE_CHANNEL_INT32 = 0x09;
const APERIODIC_INT32 = 0x0b;
const START_TIME = 0x0c;
const DROPPED = 0x0d;
const PERIOD = 0x0e;

const measurementType = {
    NORMAL_REAL32,
    WIDE_REAL32,
    APERIODIC_REAL32,
    SINGLE_CHANNEL_REAL32,
    SINGLE_CHANNEL_INT32,
    APERIODIC_INT32,
    START_TIME,
    DROPPED,
    PERIOD,
};

const MEASUREMENT = 0x20;

const responseType = {
    MEASUREMENT,
};

const IDLE = 0;
const CHARGING = 1;
const COMPLETE = 2;
const ERROR = 3;

const chargingState = {
    IDLE,
    CHARGING,
    COMPLETE,
    ERROR,
};


function _calculateChecksum(buff) {

    const length = buff[1];
    let checksum = -1 * buff[3];

    for (let i = 0; i < length; ++i) {
        checksum += buff[i];
        checksum &= 0xff;
    }

    if (checksum < 0 || checksum > 255) {
        console.error('Checksum failed!');
        return 0;
    }

    return checksum;
}

var rollingCounter = 255;

function _decRollingCounter() {
    this.rollingCounter -= 1;
    return this.rollingCounter;
}

// header 4 bytes starting 0x58
// result 1 byte length, 4 bytes header, command
function constructCommand(command) {

    // 4 for header
    commandLength = 4 + command.length;

    var result = [0x58, commandLength, _decRollingCounter(), 1 /*checksum placeholder */ ];

    // put command on the end
    result = result.concat(command);

    result[3] = _calculateChecksum(result);

    //console.log(result);
    return [commandLength, ...result];

}