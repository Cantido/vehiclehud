#ifndef OBD_H
#define OBD_H

//      PID_NAME            "code" // max,  min,    units,          formula
#define PID_SUPPORTED       "0100" //               Bit encoded
#define PID_ENGINELOAD      "0104" // 0%    100%    %               A*100/255
#define PID_COOLANTTEMP     "0105" // -40   215     degrees C       A-40
#define PID_SHORTTERMTRIMB1 "0106" // -100  99.22   %               (A-128) * 100/128
#define PID_LONGTERMTRIMB1  "0107" // -100  99.22   %               (A-128) * 100/128
#define PID_SHORTTERMTRIMB2 "0108" // -100  99.22   %               (A-128) * 100/128
#define PID_LONGTERMTRIMB2  "0109" // -100  99.22   %               (A-128) * 100/128
#define PID_FUELPRESSURE    "010A" // 0     765     kPa             A*3
#define PID_MANPRESSURE     "010B" // 0     255     kPa             A
#define PID_RPM             "010C" // 0     16383.5 rpm             ((A*256)+B)/4
#define PID_SPEED           "010D" // 0     255     km/h            A
#define PID_TIMINGADVANCE   "010E" // -64   63.5    deg, #1 cyl     A/2 - 64
#define PID_INTAKETEMP      "010F" // -40   215     degrees C       A - 40
#define PID_MAFAIRFLOW      "0110" // 0     655.35  grams/sec       ((A*256)+B) / 100
#define PID_THROTTLEPOS     "0111" // 0     100     %               A*100/255
#define PID_COMSECAIRSTATUS "0112" //               bit encoded
#define PID_O2SENSORS       "0113" //               bit encoded
#define PID_B1S1VOLTAGE     "0114" 
#define PID_B1S2VOLTAGE     "0115"
#define PID_B1S3VOLTAGE     "0116"
#define PID_B1S4VOLTAGE     "0117"
#define PID_B2S1VOLTAGE     "0118"
#define PID_B2S2VOLTAGE     "0119"
#define PID_B2S3VOLTAGE     "011A"
#define PID_B2S4VOLTAGE     "011B"
#define PID_OBDSTANDARDS    "011C" //               bit encoded
#define PID_O2SENSORS2      "011D" //               bit encoded
#define PID_AUXINSTATUS     "011E" //               bit encoded
#define PID_RUNTIME         "011F" // 0     65535   seconds         (A*256)+B

#define PID_SUPPORTED2      "0120"
#define PID_SUPPORTED3      "0140"
#define PID_SUPPORTED4      "0160"
#define PID_SUPPORTED5      "01A0"




int get_RPM(int obd_fd);

#endif
