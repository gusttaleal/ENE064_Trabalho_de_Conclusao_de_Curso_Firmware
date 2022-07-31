/*
 *  Para permitir acesso a leitura e escrita na porta USB0:
 *  $ sudo chmod a+rw /dev/ttyUSB0
 */

String backEndURL = "http://192.168.0.109:5000/data/";
String deviceId = "gHWN3hIeNDDC96o166mI";
int blink = 250;
int rateLimit = 1000 - blink;
