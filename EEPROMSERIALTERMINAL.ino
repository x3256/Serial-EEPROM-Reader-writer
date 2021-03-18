 const int DO = 4, DI = 5, CLK = 3, CS = 2;
 const int T=20;
  void setup() {
  //D2=cs
  //D3=CLK
  //D4=DO
  //D5=DI

  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
  Serial.begin(9600);
  digitalWrite(CLK,LOW);
  Serial.println("Hello PC");

}

void serWrite(int data,int bits){
  digitalWrite(CLK,LOW);
  while(bits>0){
    bits--;
    if(0x8000 & data){
      digitalWrite(DI,HIGH);
      //Serial.print("1");
    }else{
      //Serial.print("0");
      digitalWrite(DI,LOW);
    }
    delayMicroseconds(T/4);
    digitalWrite(CLK,HIGH);
    delayMicroseconds(T/2);
    digitalWrite(CLK,LOW);
    delayMicroseconds(T/4);
    data=data<<1;
  }
  //Serial.println();
}
//send data
byte serRead(){
    digitalWrite(CLK,LOW);
    byte bits = 8;
    byte data = 0;
  while(bits>0){
    bits--;
    data=data<<1;
    delayMicroseconds(T/4);
    digitalWrite(CLK,HIGH);
    delayMicroseconds(T/4);
    if(digitalRead(DO)){
      //Serial.print("1");
      data+=1;
    }else{
      //Serial.print("0");
      data+=0;
    }
    delayMicroseconds(T/4);
    digitalWrite(CLK,LOW);
    delayMicroseconds(T/4);  
  }
  //Serial.println();
  return(data);
}
//WriteEn
void writeEn(){
  byte add = 0xC0;
  int data=0x0000;
  digitalWrite(CS,HIGH);
  byte mode = 0x80; //100_11 WREN
  data= data|mode;
  data = data<<3;
  data = data|(add);
  data = data<<5;
  //Serial.println("WRITE ENABLE:");
  serWrite(data,9);
  digitalWrite(CS,LOW);
}
//Write Disable
void writeDs(){
  byte add = 0xC0;
  int data=0x0000;
  digitalWrite(CS,HIGH);
  byte mode = 0x80; //100_11 WREN
  data= data|mode;
  data = data<<3;
  data = data|(add);
  data = data<<5;
  //Serial.println("WRITE DISABLE:");
  serWrite(data,9);
  digitalWrite(CS,LOW);
}
//write data abs
void writeData(int wd,byte ad){
  writeEn();
  //Writing data
  int data=0x0000;
  digitalWrite(CS,HIGH);
  int mode = 0xA0;
  data= data|mode;
  data = data<<3;
  data = data|(ad);
  data = data<<5;
  serWrite(data,9); //setting mode
  //Serial.print("WRITING TO ADDR:");
  //Serial.println((char)ad);
  //Serial.println(data,BIN);
  //delayMicroseconds(100);
  //Serial.print("data is :");
  //Serial.print((char)(wd>>8));
  //Serial.println((char)wd);
  serWrite(wd,16);
  digitalWrite(CS,LOW);
  writeDs();
}
//Read Data
short readData(byte add){
  //considering 6 bit address
  digitalWrite(CS,HIGH);
  short data = 0x0000;
  byte mode = 0xc0;
  data = data|mode;
  data = data<<3;
  data = data|(add);
  data = data<<5;
  serWrite(data,9);
  byte r1 = serRead();
  byte r2 = serRead();
  //Serial.print(r1);
  //Serial.println(r2);
  short ret = 0x0000;
  ret = ret|r1;
  ret = ret<<8;
  ret = ret|r2;
  digitalWrite(CS,LOW);
  return ret;
}
void readAll(byte mx){
  for(byte add=0;add<mx;add+=4){
    Serial.print("data at: ");
    Serial.print((add>>2),HEX);
    Serial.print("is :");
    int tmp = readData(add);
    Serial.print((char)(tmp>>8));
    Serial.println((char)tmp);
  }
    Serial.print("data at: ");
    Serial.print(mx,HEX);
    Serial.print("is :");
    int tmp = readData(mx);
    Serial.print((char)(tmp>>8));
    Serial.println((char)tmp);
}
byte state=0;
byte inadd;
int wrdat=0;
void loop() {
  int indat;
  if(Serial.available()>0){
    indat = Serial.read();
    
    switch(state){
      case 0:{
      switch(indat){
      case 'r':state = 1;
      break;
      case 'w':state = 2;
      break;
      case 'a':readAll(0xFC);
      }
      break;
      case 1:{
        //Serial.print("data at: ");
        //Serial.println((char)indat);
        int tmp=readData(indat);
        Serial.write((char)(tmp>>8));
        Serial.write((char)(tmp));
        Serial.write('\n');
        state=0;
      }
      break;
      case 2:{
        inadd = indat;
        state=3;
      }
      break;
      case 3:{
        wrdat = wrdat|indat;
        wrdat = wrdat<<8;
        state=4;
      }
      break;
      case 4:{
        wrdat = wrdat|indat;
        state=0;
        writeData(wrdat,inadd);
        wrdat =0; //this line wasted my 3-5hrs 
      }
      break;
      default:state=0;
    }
    }
  }
}
