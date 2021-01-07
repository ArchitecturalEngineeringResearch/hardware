#include <SoftwareSerial.h>
SoftwareSerial softSerial(9, 8);//自定义软串口 RX TX
SoftwareSerial gpsSerial(10, 11);//自定义软串口 RX TX

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  softSerial.begin(9600);


  Serial.println("Initing...please wait.");
  delay(13000);

  initModule();
}

void loop() {
  checkSIMState();

  gpsSerial.listen();
  String gpsString = "info=" + getGPSInfo();
  Serial.println(gpsString);

  softSerial.listen();
  softSerial.println("AT+HTTPINIT");
  delay(3000);
  printLine("open HTTP");

  softSerial.println("AT+HTTPPARA=\"URL\",\"http://121.4.188.127:4000/info/\"");
  printLine("set url");

  softSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(1000);
  printLine("CONTENT");


  softSerial.println("AT+HTTPDATA=" + String(gpsString.length()) + ",3000");
  delay(1000);
  printLine("CONTENT");

  softSerial.println(gpsString);
  delay(1000);
  printLine("CONTENT");


  softSerial.println("AT+HTTPACTION=1");
  delay(5000);
  printLine("method post");

  softSerial.println("AT+HTTPTERM");
  delay(2000);
  printLine("close http");
}

String getGPSInfo () {
  delay(1000);
  String data;
  while (gpsSerial.available()) {
    // 每次读取一个字符
    char c = gpsSerial.read();

    if (c == '\n') {
      c = 45;
    }
    data += c;
  }

  return data;
}

//初始化方法
void initModule() {
  checkSIMState();

  //设置承载参数
  softSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(5000);
  printLine("open GPRS");

  //打开承载，打开网络通道
  softSerial.println("AT+SAPBR=1,1");
  delay(5000);
  printLine("open GPRS");
}

void checkSIMState () {
  // 当前信号
  softSerial.println("AT+CSQ");
  delay(3000);
  printLine("signal");

  // 是否有注册运营商
  softSerial.println("AT+CREG?");
  delay(5000);
  printLine("operator");
}

//读取串口返回信息
void printLine(String info) {
  delay(1000);
  softSerial.listen();
  String data;
  while (softSerial.available() > 0)
  {
    // 每次读取一个字符
    char c = softSerial.read();

    if (c == '\n') {
      c = 32;
    }
    data += c;
  }
  Serial.println(info + ": " + data);
}
