import processing.serial.*; // استدعاء مكتبة السيريال
import java.awt.event.KeyEvent; 
import java.io.IOException;

Serial myPort; 
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;

void setup() {
 size (1200, 700); // حجم الشاشة
 smooth();
 // عدل اسم البورت هنا حسب اللي ظاهر عندك في اللينكس (مثلاً /dev/ttyUSB0)
 myPort = new Serial(this,"/dev/ttyUSB0", 9600); 
 myPort.bufferUntil('.'); 
}

void draw() {
  fill(98,245,31);  
  // تأثير الشفافية عشان يبان زي الرادار القديم
  noStroke();
  fill(0,4); 
  rect(0, 0, width, height-height*0.074); 
  
  fill(98,245,31); // لون أخضر

  drawRadar(); 
  drawLine();
  drawObject();
  drawText();
}

void serialEvent (Serial myPort) { 
  // استقبال البيانات من الأردوينو
  data = myPort.readStringUntil('.');
  data = data.substring(0,data.length()-1);
  
  index1 = data.indexOf(","); 
  angle= data.substring(0, index1); 
  distance= data.substring(index1+1, data.length());
  
  // تحويل النصوص لأرقام
  iAngle = int(angle);
  iDistance = int(distance);
}

void drawRadar() {
  pushMatrix();
  translate(width/2,height-height*0.074); 
  noFill();
  strokeWeight(2);
  stroke(98,245,31);
  // رسم الأقواس
  arc(0,0,(width-width*0.0625),(width-width*0.0625),PI,TWO_PI);
  arc(0,0,(width-width*0.27),(width-width*0.27),PI,TWO_PI);
  arc(0,0,(width-width*0.479),(width-width*0.479),PI,TWO_PI);
  arc(0,0,(width-width*0.687),(width-width*0.687),PI,TWO_PI);
  // رسم خطوط الزوايا
  line(-width/2,0,width/2,0);
  line(0,0,(-width/2)*cos(radians(30)),(-width/2)*sin(radians(30)));
  line(0,0,(-width/2)*cos(radians(60)),(-width/2)*sin(radians(60)));
  line(0,0,(-width/2)*cos(radians(90)),(-width/2)*sin(radians(90)));
  line(0,0,(-width/2)*cos(radians(120)),(-width/2)*sin(radians(120)));
  line(0,0,(-width/2)*cos(radians(150)),(-width/2)*sin(radians(150)));
  line((-width/2)*cos(radians(30)),0,width/2,0);
  popMatrix();
}

void drawObject() {
  pushMatrix();
  translate(width/2,height-height*0.074); 
  strokeWeight(9);
  stroke(255,10,10); // لون أحمر للجسم المكتشف
  
  // تحويل المسافة (سم) لبكسلات على الشاشة
  pixsDistance = iDistance*22.5; 
  
  // لو المسافة أقل من 40 سم، ارسم خط أحمر
  if(iDistance<40 && iDistance>0){ 
    line(pixsDistance*cos(radians(iAngle)),-pixsDistance*sin(radians(iAngle)),(width-width*0.505)*cos(radians(iAngle)),-(width-width*0.505)*sin(radians(iAngle)));
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(30,250,60); // الخط الأخضر الماسح
  translate(width/2,height-height*0.074); 
  line(0,0,(height-height*0.12)*cos(radians(iAngle)),-(height-height*0.12)*sin(radians(iAngle))); 
  popMatrix();
}

void drawText() { 
  // كتابة البيانات على الشاشة
  pushMatrix();
  if(iDistance>40) {
    noObject = "Out of Range";
  }
  else {
    noObject = "In Range";
  }
  fill(0,0,0);
  noStroke();
  rect(0, height-height*0.0648, width, height);
  fill(98,245,31);
  textSize(25);
  
  text("10cm",width-width*0.3854,height-height*0.0833);
  text("20cm",width-width*0.281,height-height*0.0833);
  text("30cm",width-width*0.177,height-height*0.0833);
  text("40cm",width-width*0.0729,height-height*0.0833);
  textSize(40);
  text("Object: " + noObject, width-width*0.875, height-height*0.0277);
  text("Angle: " + iAngle +" °", width-width*0.48, height-height*0.0277);
  text("Dist: " + iDistance +" cm", width-width*0.26, height-height*0.0277);
  popMatrix(); 
}
