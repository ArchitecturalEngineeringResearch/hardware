#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial simSerial(9, 8);//自定义软串口 RX TX
SoftwareSerial gpsSerial(10, 11);//自定义软串口 RX TX
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  simSerial.begin(9600);

  Serial.println("Initing...");
  delay(13000);

  initModule();
}

void loop() {
  checkSIMState();

  gpsSerial.listen();
  String gpsString = "info=" + getGPSInfo();
  Serial.println(gpsString);

  simSerial.listen();
  simPrintLine("AT+HTTPINIT", 3, "");
  simPrintLine("AT+HTTPPARA=\"URL\",\"http://121.4.188.127:4000/info/\"", 1, "");
  simPrintLine("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"", 1, "");
  simPrintLine("AT+HTTPDATA=" + String(gpsString.length()) + ",3000", 1, "");
  simPrintLine(gpsString, 1, "");
  simPrintLine("AT+SAPBR=2,1", 1, "");
  String httpResults = simPrintLine("AT+HTTPACTION=1", 5, "");
  Serial.println(httpResults.indexOf("200"));
  simPrintLine("AT+HTTPTERM", 2, "close");
  if (httpResults.indexOf("200") < 0) {
    Serial.println("reset");
    simPrintLine("AT+SAPBR=0,1", 2, "");
    initModule();
  }
}

//初始化方法
void initModule() {
  checkSIMState();

  //设置承载参数
  simPrintLine("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 5, "open GPRS");
  //打开承载，打开网络通道
  simPrintLine("AT+SAPBR=1,1", 5, "open GPRS");
}

void checkSIMState () {
  // 当前信号
  simPrintLine("AT+CSQ", 3, "signal");
  // 是否有注册运营商
  simPrintLine("AT+CREG?", 5, "operator");
  // ip
  simPrintLine("AT+SAPBR=2,1", 1, "");
}

String simPrintLine (String command, int delayNumber, String remark) {
  simSerial.println(command);
  delay(delayNumber * 1000);
  return printLine(remark);
}

String getGPSInfo () {
  delay(1000);
  String data;
  while (gpsSerial.available() > 0) {
    // 每次读取一个字符
    if (gps.encode(gpsSerial.read()) || gps.location.isValid()) {
      // 经度,纬度,卫星数,速度,海拔

      data = String(gps.location.lat()) + "," + String(gps.location.lng()) + "," + String(gps.satellites.value()) + "," +  String(gps.speed.kmph()) + "" + String(gps.altitude.meters()) + "," +  String(gps.course.value());
    }
  }

  return data;
}

//读取串口返回信息
String printLine(String info) {
  delay(1000);
  simSerial.listen();
  String data;
  while (simSerial.available() > 0)
  {
    // 每次读取一个字符
    char c = simSerial.read();

    if (c == '\n') {
      c = 32;
    }
    data += c;
  }
  Serial.println(info + " " + data);
  return data;
}
