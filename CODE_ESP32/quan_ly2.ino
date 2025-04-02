//------PHAN KHAI BAO THU VIEN CAN DUNG-------------
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
//----------TẠO ĐỐI TƯƠNG CHO CHƯƠNG TRÌNH(WIFI, CLIENT KET NOI DEN THINGSBOARD, THE RFID, UART MỀM)-------------
  WiFiClient espClient;
  PubSubClient client(espClient);
  MFRC522 rfid(5,22);
  SoftwareSerial mySerial(16,17);
//--------KHAI BAO TÊN WIFI , PASS, VA MÃ TOKEN CỦA THIẾT BỊ CẦN KẾT NỐI------------
  const char* ssid = "OPPO Reno7 Z 5G";
  const char* password = "29032006";
  const char* token = "3haIi4qJGlq7k0XBrXQf";
//---KHAI BAO TIMER ĐỂ THỰC HIỆN BÁO ĐỘNG KHI ĐIỀU KIỆN MÔI TRƯỜNG VƯỢT MỨC----
hw_timer_t *My_timer = NULL;                          
volatile bool ledState = false;

const int RS = 32, EN = 33, D4 = 25, D5 = 26, D6 = 27, D7 = 14;           //gan chan cho LCD che do 4 bit
LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);                                      //tao doi tuong lcd với các chân đã gán

float R0 = 0,alarm_hum = 85.0,alarm_temp = 36.0,alarm_gas = 500.0;
float get_ppm, Hum, Temp;

void IRAM_ATTR onTimer(){       //hàm xử lý ngắt và các nhiệm vụ mỗi lần thực hiện ngắt
    ledState = !ledState;
    digitalWrite(2,ledState);
}
//
#define RATIO_CLEAN_AIR 3.8
#define VCC 5.0
#define RL 1000.0
#define ANALOG_PIN 35 

#define UNLOCK "U"              // định nghĩa kí tự  U là UNLOCK( mở cửa bằng thingsboard )
#define AUTO "A"                //định nghĩa kí tự A là AUTO (bật chê độ cửa tự động)
#define N_AUTO "N"              //định nghĩa kí tự N là N_AUTO (tắt chế độ cửa tự động)
#define CHANGE "C"              // định nghĩa kí tự C là CHANGE (cho phép đổi mật khẩu)
#define DATA "D"                // định nghĩa kí tự D là DATA (gửi đến 8051 để lấy dữ liệu từ DHT11)
#define SCAN "S"                // định nghĩa kí tự S là SCAN (quét thẻ mở cửa)
#define FAILED "F"              // định nghĩa kí tự S là SCAN (quét thẻ mở cửa)

//TẠO KIỂU STRUCT ĐỂ QUẢN LÝ NHIỀU KIỂU DỮ LIỆU ĐI CÙNG NHAU-------
struct card_id{
  String uid;         //KHAI BÁO BIẾN UID
  String name;        //KHAI BAO BIẾN TÊN
  bool status = 1;
  unsigned long time_check_in, time_check_out;
   //KHAI BÁO BIẾN TRẠNG THÁI QUÉT THẺ
};
card_id uid[15];    //TẠO MẢNG STRUCT ĐỂ QUẢN LÝ NHIỀU THẺ
int cnt = 0;        // BIẾN ĐẾM SỐ THẺ ĐÃ DÙNG
unsigned long time_get_data = 0;
//-------------PHẦN SETUP CHO CHƯƠNG ESP32-------------------
//----PHẦN SETUP PHỤ----
void setup_WiFi(){
  pinMode(2,OUTPUT);
  Serial.begin(115200);                           //KHỞI ĐỘNG UART GIAO TIẾP VỚI MÁY TÍNH
  mySerial.begin(9600);                          //KHỞI ĐỘNG UART MỀM GIAO TIẾP VỚI 8051(TỐC ĐỘ BAUT PHẢI BẰNG NHAU LÀ 9600)
  SPI.begin();                                  //KHƠI ĐỘNG GIAO TIẾP SPI ĐỂ DÙNG CHO RFID
  rfid.PCD_Init();                             //KHỞI ĐỘNG ĐỐI TƯỢNG rfid
  WiFi.begin(ssid,password);                  //KHỞI ĐỘNG KẾT NỐI ĐỂN WIFI
  client.setServer("thingsboard.cloud",1883);//KET NOI ĐẾN THINGSBOARD
  client.setCallback(callback);             //LIÊN KẾT HÀM CALLBACK ĐỂ NHẬN LỆNH ĐƯỢC GỬI TỪ THINGSBOARD
  reconnect();                             // HÀM KẾT NỐI LẠI VỚI THINGSBOARD KHI MẤT KẾT NỐI
}
//------PHẦN SETUP CHÍNH--------
void setup(){
  pinMode(2,OUTPUT);
  setup_WiFi();
  pinMode(ANALOG_PIN, INPUT);
  //Hieu chinh R0 cua MQ135
  R0 = calibrateR0() ;
  Serial.print("R0 : ");
  Serial.println(R0);
  lcd.begin(16,2);
  My_timer = timerBegin(0,80,true);
  timerAttachInterrupt(My_timer,&onTimer,true);
  timerAlarmWrite(My_timer,100000,true);
}
//------------HÀM LOOP ĐỂ THỰC HIỆN LẶP ĐI LẶP LẠI-------------------------
void loop(){
  reconnect();
  client.loop();                        //DUY TRÌ KẾT NỐI VÀ GỬI LIỆU ĐI VỀ VỚI THINGSBOARD
  quet_the();                           //HÀM QUÉT THẺ CỦA RC522 VÀ SO SÁNH VỚI CÁC UID HỢP LỆ
  if(millis()-time_get_data>=3000){
    mySerial.write(DATA);
    get_ppm = gas_ppm();
    lcd.setCursor(9, 0);
    lcd.print("GAS:"); lcd.print(String(get_ppm));
    if(get_ppm>alarm_gas){
      timerAlarmEnable(My_timer);
      client.publish("v1/devices/me/telemetry","{\"warning\":\"Canh bao Khi Gas\"}");
    }
    String h = "{\"gasPPM\":" + String(get_ppm) + "}";
    client.publish("v1/devices/me/telemetry",h.c_str());
    time_get_data = millis();
  }                        
  if(mySerial.available()>0){           //ĐỌC GIÁ TRỊ GỬI TỪ 8051 QUA UART
    String l = mySerial.readString();
    Serial.println(l);
    if(l == AUTO) client.publish("v1/devices/me/telemetry","{\"value_auto\":true}");
    else if (l == N_AUTO) client.publish("v1/devices/me/teletry","{\"value_auto\":false}");
    else{
      DynamicJsonDocument data(256);
      deserializeJson(data,l);
      Hum = data["Hum"];
      Temp = data["Temp"];
      lcd.setCursor(0, 0);
      lcd.print("H:"); lcd.print(String(Hum));
      lcd.setCursor(0, 1);
      lcd.print("T:"); lcd.print(String(Temp));
      String n;
      if(Hum >= alarm_hum){
        timerAlarmEnable(My_timer);
        n = "{\"warning\":\"Canh bao do am\"}";
        client.publish("v1/devices/me/telemetry",n.c_str());
      }
      if(Temp>=alarm_temp){
        timerAlarmEnable(My_timer);
        n = "{\"warning\":\"Canh bao nhiet do\"}";
        client.publish("v1/devices/me/telemetry",n.c_str());
      }
      client.publish("v1/devices/me/telemetry",l.c_str());
    }
  }
  if(Hum < alarm_hum && Temp < alarm_temp && get_ppm < alarm_gas){
    digitalWrite(2,0);
    timerAlarmDisable(My_timer);
  }
  
}
//----------PHẦN HÀM CALLBACK GỬI NHẬN LỆNH ĐIỀU KHIỂN TỪ THINGSBOARD
void callback(char* topic, byte* payLoad, unsigned int length){
  String str;
  for(int i = 0; i < length; i++){
    str+=(char)payLoad[i];        //CHUYỂN CHUỖI DỮ LIỆU GỬI TỪ THINGSBOARD QUA 1 CHUỖI KHÁC
  }
  Serial.println(str);            //IN CHUỖI VỪA NHẬN RA MONITOR
  DynamicJsonDocument doc(256);   //KHAI BÁO TẠO ĐỐI TƯỢNG JSON
  DeserializationError error = deserializeJson(doc,str);  //CHUYỂN TỪ CHUỐI JSON SANG ĐỐI TƯƠNG JSON
  if(error){
    Serial.println("Loi chuyen doi Json!!");//NGỪNG THỰC HIỆN NẾU QUÁ TRÌNH CHUYỂN ĐỐI BỊ LỖI
    return;
  }
  //--------KHOI ĐỘNG NHẬN THUỘC TÍNH TỪ THINGSBOARD VÀ TÁCH TỪNG KEY , SAU ĐÓ SẼ GÁN VÀO MẢNG STRUCT ĐÃ TẠO--------------
  if(doc.containsKey("shared")){
    JsonObject shared = doc["shared"];                       // NẾU TRONG doc CHỨA shared THÌ TIẾP TỤC TẠO 1 ĐỐI TƯỢNG JSON KHÁC ĐỂ LẤY GIÁ TRỊ BÊN TRONG
    if(shared.containsKey("card")){     
        JsonObject card = shared["card"];                  // NẾU TRONG shared CHỨA card THÌ TIẾP TỤC LẤY GIÁ TRỊ BÊN TRONG
         if(card.containsKey("card2")){
             JsonObject cardList = card["card2"];        //NẾU TRONG card CHỨA card2 TIẾP TỤC TÁCH
             for(JsonPair kv: cardList){                // ĐẾN ĐÂY ĐÃ NHẬN ĐƯỢC MẢNG ĐỐI TƯỢNG JSON GỒM CÁC CẶP GIÁ TRỊ GỒM UID:NAME
              uid[cnt].uid = kv.key().c_str();         // GÁN KEY CHO BIẾN uid;
              uid[cnt].name = kv.value().as<String>();// GÁN VALUE CHO BIẾN name
              cnt++;                                 // TĂNG CNT LÊN 1 CHỈ SỐ TRONG MẢNG STRUCT(TƯƠNG ỨNG SỐ LƯỢNG THẺ ĐÃ CÓ LÀ CNT+1)
          }//
       }
    }
  }
//-------hoan thanh kiem tra va nhan gia tri UID phan hoi tu thingsboard
    else if(doc.containsKey("method")){         //KEY method NHẬN ĐƯỢC TỪ THINGSBOARD DO CÁC NÚT NHẤN ĐIỀU KHIỂN
      String method = doc["method"];           //KEY
      if(method=="setDoor") {
        mySerial.write(UNLOCK);
        client.publish("v1/devices/me/telemetry","{\"closeDoor\":false}");
      }
        else if(method =="setAuto"){
          bool sta = doc["params"];               //VALUE
          if(sta == true) mySerial.write(AUTO);
            else mySerial.write(N_AUTO);
      }
        else if(method =="EnableChange"){
          bool sta = doc["params"];
          if(sta == true) {
            mySerial.write(CHANGE);
            Serial.println("Cho phep doi pass");
            client.publish("v1/devices/me/telemetry","{\"setOff\":false}");
          }
        }
        else if(method == "setHum"){
          alarm_hum  = doc["params"];
        }
        else if(method == "setTemp"){
          alarm_temp  = doc["params"];
        }
        else if(method == "setGas"){
          alarm_gas  = doc["params"];
        }
    }
}
//------PHẦN HÀM QUÉT THẺ, ĐỌC ID CỦA THẺ VÀ SO SÁNH VỚI CÁC UID HỢP LỆ, NẾU ĐÚNG GỬI TRẠNG THÁI LÊN THINGSBOARD
void quet_the(){
  if(!rfid.PICC_IsNewCardPresent()) return;
  if(!rfid.PICC_ReadCardSerial()) return;
  String ma_the="";
  for(byte i = 0 ; i < rfid.uid.size;i++){
    ma_the += String(rfid.uid.uidByte[i]);                                                      //NHẬN ID TỪ THẺ VÀ LƯU VÀO 1 CHUỖI 
  }
  for(int i = 0; i < 15; i++){                                                                // DUYỆT QUA TẤT CẢ THẺ VÀ SO SÁNH VỚI THẺ VỪA QUÉT
    if(ma_the==uid[i].uid) {
      Serial.println("The hop le " +uid[i].uid +" ten nguoi mo la: " + uid[i].name);        // GỬI LÊN MONITOR
      mySerial.write(SCAN);                                                                 //GỬI QUA UART ĐỂ 8051 NHẬN VÀ MỞ CỬA
      String info="";
      if(uid[i].status == 1){ 
         uid[i].time_check_in = millis();
        info = "{\"UID\":\"" + uid[i].uid + "\", \"Name\":\"" + uid[i].name +"\", \"Status\": \"check in\"}";// TẠO CHUỖI JSON ĐỂ GỬI ĐẾN THINGSBOARD VÀ HIỂN THỊ RA BẢNG
      }
      else {
         uid[i].time_check_out = millis();
         unsigned long work_time =uid[i].time_check_out - uid[i].time_check_in;
         unsigned long seconds = work_time/1000;
         int hours = seconds / 3600;
         int minutes = (seconds%3600) / 60;
         seconds = (seconds%3600) % 60;
        
        info = "{\"UID\":\"" + uid[i].uid + "\", \"Name\":\"" + uid[i].name +"\", \"Status\": \"check out\",\"WorkTime\":\""+String(hours)+"h:"+String(minutes)+"m:"+String(seconds)+"s\"}";
      }
        client.publish("v1/devices/me/telemetry",info.c_str());   // GỬI CHUỖI JSON LÊN THINGSBOARD
        uid[i].status = !uid[i].status;
        rfid.PICC_HaltA();                                       //tam ngung doc the TRÁNH ĐỌC NHIỀU LẦN
        rfid.PCD_StopCrypto1();                                 // ngung su dung do tuong My_mfRC522
        return;
    }
    else if(i == cnt-1){                                      //KHI QUÉT QUA TẤT CẢ THẺ MÀ KHÔNG CÓ THẺ ĐÚNG
        Serial.println("Ma the:" + ma_the+" khong hop le!");
        mySerial.write(FAILED);
        rfid.PICC_HaltA();                                   //tam ngung doc the
        rfid.PCD_StopCrypto1();                             // ngung su dung do tuong My_mfRC522
    }
  }
}
//------HÀM KẾT NỐI ĐẾN THINGSBOARD VÀ TIẾN HÀNH KẾT NỐI LẠI KHI MẤT KẾT NỐI
void reconnect(){
  while(!client.connected()){
    while(WiFi.status()!= WL_CONNECTED){
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.println("Ket noi thanh cong den WiFi!!");
  while(!client.connected()){
    Serial.println("Dang ket noi den thingsboard...");
    delay(500);
    if(client.connect("esp32",token,NULL)){                                                 //KHI KẾT ĐÚNG SẼ THỰC HIỆN CÁC LÊNH SAU
      client.subscribe("v1/devices/me/attributes");                                         //ESP ĐĂNG KÍ NHẬN THUỘC TÍNH ĐƯỢC GỬI TỪ THINGSBOARD
      client.subscribe("v1/devices/me/rpc/request/+");                                      //ESP ĐĂNG KÍ NHẬN LỆNH TỪ THINGSBOARD
      client.subscribe("v1/devices/me/attributes/response/+");                              //ESP ĐĂNG KÍ NHẬN THUỘC TÍNH PHẢN HỒI SAU KHI YÊU CẦU ĐẾN THINGSBOARD 
      client.publish("v1/devices/me/attributes/request/1","{\"sharedKeys\":\"card\"}");     //ESP GỬI YÊU CẦU THINGBOARD GỬI KEY: sharedKeys VÀ VALUE: card 
      Serial.println("Da ket noi thanh cong den Thingsboard!!");                            // THÔNG BÁO ĐÃ KẾT NỐI THÀNH CÔNG
      }
    }
  }
}
float calibrateR0() {
    double  Rs_sum = 0;
    for (int i = 0; i < 100; i++) {  // Lấy trung bình 100 mẫu
        int adc = analogRead(ANALOG_PIN);
        float Vout = adc * (VCC / 4095.0);
        float Rs = ((VCC - Vout) / Vout) * RL;
        Rs_sum += Rs;
        delay(10);
    }
    double Rs_avg = Rs_sum / 100.0;
    return Rs_avg / RATIO_CLEAN_AIR; // R0 = Rs / 3.8 (theo datasheet)
}
float gas_ppm(){
  int adc = analogRead(ANALOG_PIN);
    float Vout =adc * (VCC / 4095.0);
    float RS = ((VCC-Vout)/Vout)*RL;
    float ratio = RS / R0;
    float ppm = 105.064 * pow(ratio, -2.773);
    Serial.print("PPM: "); Serial.println(ppm);
    return ppm;
}