  /* Originally posted on www.bbangpan.com

  Program Description: HC-06 bluetooth 2.0 / HM-10 BLE test code

  DataPin : 2,3 to TX, RX of HC-06(cross connection)

  Tested on : Arduino 1.0.6, Arduino UNO R3, HC-06 or HM-10 BLE

   

  Copyright (c) 2015 www.bbangpan.com. All rights reserved.

  This program can be used for any non-commercial purpose freely. 

  **************************************************************
  *
  *  2016.07.09 Mazdah 수정
  *
  **************************************************************

  */

   

  #include <SoftwareSerial.h>
  #include <Servo.h>

  int frontLeftPin = 22;
  int frontRightPin = 24;
  int rearLeftPin = 26;
  int rearRightPin = 28;

  int frontLeftPin2 = 30;
  int frontRightPin2 = 32;
  int rearLeftPin2 = 34;
  int rearRightPin2 = 36;

  int frontLeftPin3 = 38;
  int frontRightPin3 = 40;
  int rearLeftPin3 = 42;
  int rearRightPin3 = 44;

  int minAngle = 60;                                    // 제 1 관절의 이동 범위 중 최소 각도
  int maxAngle = 120;                                   // 제 1 관절의 이동 범위 중 최대 각도
  int centerAngle = (minAngle + maxAngle) / 2;          // 제 1 관절의 이동 범위 중 중간 각도 : 90
  int changedVal = 3;                                   // 제 1 관절의 증감 단위
  int term = (maxAngle - centerAngle) / changedVal;     // 제 1 관절의 중간 각도와 최소 각도 또는 최대 각도 간의 차이 (실제 움직이는 범위) : 9
  int centerMinAngle = centerAngle - term * changedVal;
  int centerMaxAngle = centerAngle + term * changedVal;

  int secondStartAngle_FLRR = 80;                            // 제 2 관절의 초기 각도
  int secondChangeVal = 2;                              // 제 2 관절의 증감 단위
  int secondHighAngle_FLRR = secondStartAngle_FLRR + (secondChangeVal * changedVal * (term / 2)); // 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴) : 
  int secondLowAngle_FLRR = secondStartAngle_FLRR - (secondChangeVal * changedVal * (term / 2));  //// 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴) : 30

  int secondStartAngle_FRRL = 100;                            // 제 2 관절의 초기 각도
  int secondHighAngle_FRRL = secondStartAngle_FRRL + (secondChangeVal * changedVal * (term / 2)); // 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴) : 
  int secondLowAngle_FRRL = secondStartAngle_FRRL - (secondChangeVal * changedVal * (term / 2));  //// 제 2 관절의 리턴 포인트 (이 각도 이후에는 초기 각도로 되돌아옴) : 30

  int thirdMinAngle = 80;                                    // 제 3 관절의 이동 범위 중 최소 각도
  int thirdMaxAngle = 90;                                   // 제 3 관절의 이동 범위 중 최대 각도
  int thirdCenterAngle = (thirdMinAngle + thirdMaxAngle) / 2;          // 제 3 관절의 이동 범위 중 중간 각도
  int thirdChangedVal = 3;                                   // 제 3 관절의 증감 단위
 // int thirdTerm = (thirdMaxAngle - thirdCenterAngle) / thirdChangedVal;     // 제 3 관절의 중간 각도와 최소 각도 또는 최대 각도 간의 차이 (실제 움직이는 범위

  int tiltChangeVal = 1;
  int titlTermHigh = 135;
  int titlTermLow = 45;
  int tiltTerm = (90 - titlTermLow) / tiltChangeVal;

  Servo frontLeft;
  Servo frontRight;
  Servo rearLeft;
  Servo rearRight;

  Servo frontLeft2;
  Servo frontRight2;
  Servo rearLeft2;
  Servo rearRight2;

  Servo frontLeft3;
  Servo frontRight3;
  Servo rearLeft3;
  Servo rearRight3;

  bool stepPhase = true; // true frontLeft, false front right
  bool start = true;

  int angle = 0; // servo position in degreesint
  //SoftwareSerial BTSerial(2, 3); // SoftwareSerial(RX, TX)

  char type[1];
  char target[1];
  char action[1];
  char _direction[1];;
  char _speed[1];
  char distance[1];

  int forwardCount = 1;
  int backwardCount = 1;

  void setup()
  {

    Serial.begin(9600);

    frontLeft.attach(frontLeftPin);
    frontRight.attach(frontRightPin);
    rearLeft.attach(rearLeftPin);
    rearRight.attach(rearRightPin);

    frontLeft2.attach(frontLeftPin2);
    frontRight2.attach(frontRightPin2);
    rearLeft2.attach(rearLeftPin2);
    rearRight2.attach(rearRightPin2);

    frontLeft3.attach(frontLeftPin3);
    frontRight3.attach(frontRightPin3);
    rearLeft3.attach(rearLeftPin3);
    rearRight3.attach(rearRightPin3);

    frontLeft.write(centerAngle);
    delay(15);
    frontRight.write(centerAngle); 
    delay(15);
    rearLeft.write(centerAngle); 
    delay(15);
    rearRight.write(centerAngle); 
    delay(15);
    
    frontLeft2.write(secondStartAngle_FLRR);
    delay(15);
    frontRight2.write(secondStartAngle_FRRL); 
    delay(15);
    rearLeft2.write(secondStartAngle_FRRL); 
    delay(15);
    rearRight2.write(secondStartAngle_FLRR); 
    delay(15);

    frontLeft3.write(thirdCenterAngle);
    delay(15);
    frontRight3.write(thirdCenterAngle); 
    delay(15);
    rearLeft3.write(thirdCenterAngle); 
    delay(15);
    rearRight3.write(thirdCenterAngle); 
  // set the data rate for the BT port

 //   Serial3.begin(9600);


  }

  void loop(){
//    while (Serial3.available()){ // if BT sends something
//      byte rtype = Serial3.readBytes(type, 1);
//      byte rtarget = Serial3.readBytes(target, 1);
//      byte raction = Serial3.readBytes(action, 1);
//      byte r_direction = Serial3.readBytes(_direction, 1);
//      byte r_speed = Serial3.readBytes(_speed, 1);
//      byte rdistance = Serial3.readBytes(distance, 1);
      while (Serial.available()){ // if BT sends something
        byte rtype = Serial.readBytes(type, 1);
        byte rtarget = Serial.readBytes(target, 1);
        byte raction = Serial.readBytes(action, 1);
        byte r_direction = Serial.readBytes(_direction, 1);
        byte r_speed = Serial.readBytes(_speed, 1);
        byte rdistance = Serial.readBytes(distance, 1);

      
      if (type[0] == 0x72 && target[0] == 0x61) {
        if (action[0] == 0x6D) {
          if (_direction[0] == 0x66) {
            goForward(forwardCount);
            forwardCount++;
          } else if (_direction[0] == 0x62) {
            goBackward(backwardCount);
            backwardCount++;
          } else if (_direction[0] == 0x6C) {
            goLeft();
          } else if (_direction[0] == 0x72) {
            goRight();
          }
        } else if (action[0] == 0x72) {
          if (_direction[0] == 0x6C) {
            turnLeft();
          } else if (_direction[0] == 0x72) {
            turnRight();
          }
        } else if (action[0] == 0x73) {
          if (_direction[0] == 0x6C) {
            twistLeft();
          } else if (_direction[0] == 0x72) {
            twistRight();
          }
        } else if (action[0] == 0x68) {
          if (_direction[0] == 0x66) {
            hTiltForward();
          } else if (_direction[0] == 0x62) {
            hTiltBackward();
          } else if (_direction[0] == 0x6C) {
            hTiltLeft();
          } else if (_direction[0] == 0x72) {
            hTiltRight();
          }
        } else if (action[0] == 0x74) {
          if (_direction[0] == 0x66) {
            tiltForward();
          } else if (_direction[0] == 0x62) {
            tiltBackward();
          } else if (_direction[0] == 0x6C) {
            tiltLeft();
          } else if (_direction[0] == 0x72) {
            tiltRight();
          }
        } else if (action[0] == 0x65) {
          if (_direction[0] == 0x75) {
            up();
          } else if (_direction[0] == 0x64) {
            down();
          }
        } else if (action[0] == 0x69) {
          reset();
        }
      }
    }

   

//    while (Serial.available()){ // if Serial has input(from serial monitor)
//    
//      byte data = Serial.read();
//      
//      Serial3.write(data); // write it to BT
//    
//    }

//     // scan from 0 to 180 degrees
//    for(angle = 0; angle < 90; angle++) 
//    { 
//      rearRight2.write(angle); 
//      delay(15); 
//    } 
//    // now scan back from 180 to 0 degrees
//    for(angle = 90; angle > 0; angle--) 
//    { 
//      rearRight2.write(angle); 
//      delay(15); 
//    } 

  }

  void reset () {
    start = true;
    frontLeft.write(centerAngle);
    delay(15);
    frontRight.write(centerAngle); 
    delay(15);
    rearLeft.write(centerAngle); 
    delay(15);
    rearRight.write(centerAngle); 
    delay(15);
    
    frontLeft2.write(secondStartAngle_FLRR);
    delay(15);
    frontRight2.write(secondStartAngle_FRRL); 
    delay(15);
    rearLeft2.write(secondStartAngle_FRRL); 
    delay(15);
    rearRight2.write(secondStartAngle_FLRR); 
    delay(15);

    frontLeft3.write(thirdCenterAngle);
    delay(15);
    frontRight3.write(thirdCenterAngle); 
    delay(15);
    rearLeft3.write(thirdCenterAngle); 
    delay(15);
    rearRight3.write(thirdCenterAngle); 

    //Serial3.write("<- RESET END");
    Serial.write("<- RESET END");
  }

  // 로봇 전진
  void goForward2() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;
      
    int level = -1 * changedVal;

    for (int i = 0; i < term; i++) {

        if (frontLeftAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
        }

        frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
        delay(15);
    }

    level = -1 * changedVal;
    for (int i = 0; i < term; i++) {

        if (rearRightAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
        }

        rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
        delay(15);
    }

    level = changedVal;
    for (int i = 0; i < term; i++) {

        if (frontRightAngle2 == secondHighAngle_FRRL) {
            level = changedVal * -1;
        }

        frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
        delay(15);
    }

    level = changedVal;
    for (int i = 0; i < term; i++) {

        if (rearLeftAngle2 == secondHighAngle_FRRL) {
            level = changedVal * -1;
        }

        rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
        delay(15);
    }
  }
  
//  void goForward() {
//    Serial.println("go forward");
//
//    if (start) {
//      start = false;
//
//      int rearRightAngle2 = rearRight2.read();
//      int tempCenter = centerAngle;
//      int tempCenter2 = centerAngle;
//      int tempCenter3 = centerAngle;
//      int level = -1 * changedVal;
//
//      Serial.print("secondLowAngle : ");
//      Serial.println(secondLowAngle);
//      Serial.println("");
//
//      for (int i = 0; i < term; i++) {
//        
//        rearRight.write(tempCenter += changedVal);
//        rearRight3.write(tempCenter3 += changedVal);
//        delay(15);
//
//        frontRight.write(tempCenter -= changedVal);
//        frontRight3.write(tempCenter3 -= changedVal);
//        delay(15);
//
//  //      Serial.print("rearRightAngle2 : ");
//  //      Serial.print(i);
//  //      Serial.print(" : ");
//  //      Serial.println(rearRightAngle2);
//
//        Serial.print("rearRightAngle2 : ");
//        Serial.println(rearRightAngle2);
//        
//        if (rearRightAngle2 == secondLowAngle) {
//          level = changedVal;
//        }
//
//        rearRight2.write(rearRightAngle2 += (2 * level));
//      }
//
//      Serial.print("frontRightAngle : ");
//      Serial.println(frontRight.read());
//      Serial3.write("<- GO FORWARD FIRST STEP");
//    } else {
//      int frontLeftAngle = frontLeft.read();
//      int frontRightAngle = frontRight.read();
//      int rearLeftAngle = rearLeft.read();
//      int rearRightAngle = rearRight.read();
//
//      int frontLeftAngle2 = secondStartAngle;
//      int frontRightAngle2 = secondStartAngle;
//      int rearLeftAngle2 = secondStartAngle;
//      int rearRightAngle2 = secondStartAngle;
//
//      int frontLeftAngle3 = frontLeft3.read();
//      int frontRightAngle3 = frontRight3.read();
//      int rearLeftAngle3 = rearLeft3.read();
//      int rearRightAngle3 = rearRight3.read();
//
//      int tempCenterFL = centerAngle;
//      int tempCenterFR = centerAngle;
//      int tempCenterRL = centerAngle;
//      int tempCenterRR = centerAngle;
//
//      int tempCenterFL3 = centerAngle;
//      int tempCenterFR3 = centerAngle;
//      int tempCenterRL3 = centerAngle;
//      int tempCenterRR3 = centerAngle;
//
//      int tempMin = minAngle;
//      int tempMax = maxAngle;
//
//      int tempMin3 = thirdMinAngle;
//      int tempMax3 = thirdMaxAngle;
//
//      Serial.print("frontLeftAngle : ");
//      Serial.println(frontLeftAngle);
//
//      if (stepPhase) {
//          stepPhase = false;
//          int level = changedVal;
//
//          for (int i = 0; i < term; i++) {
//              frontLeft.write(tempCenterFL += changedVal);
//              frontLeft3.write(tempCenterFL3 += changedVal);
//              delay(15);
//
//              rearRight.write(tempMax -= changedVal);
//              rearRight3.write(tempMax3 -= changedVal);
//              delay(15);
//
//              //frontRight2 move  85 -> 39 -> 85
//              if (frontRightAngle2 == secondHighAngle) {
//                  level = changedVal * -1;
//              }
//
//              frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
//              delay(15);
//              frontRight.write(tempMin += changedVal);
//              frontRight3.write(tempMin3 += changedVal);
//              delay(15);
////              Serial.println(frontRight.read());
////              Serial.println(tempMin += changedVal);
//              //rearLeft2 move  85 -> 39 -> 85
//              rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
//              delay(15);
//              rearLeft.write(tempCenterRL -= changedVal);
//              rearLeft3.write(tempCenterRL3 -= changedVal);
//              delay(15);
//          }
//
//          Serial.print("PHASE 1 frontRightAngle : ");
//          Serial.println(frontRight.read());
//          Serial3.write("<- GO FORWARD PHASE 1");
//      } else {
//          stepPhase = true;
//          int level = -1 * changedVal;
//
//          for (int i = 0; i < term; i++) {
//              frontRight.write(tempCenterFR -= changedVal);
//              frontRight3.write(tempCenterFR3 -= changedVal);
//              delay(15);
//
//              rearLeft.write(tempMin += changedVal);
//              rearLeft3.write(tempMin3 += changedVal);
//              delay(15);
//
//              if (frontLeftAngle2 == secondLowAngle) {
//                  level = changedVal;
//              }
//
//              frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
//              delay(15);
//              frontLeft.write(tempMax -= changedVal);
//              frontLeft3.write(tempMax3 -= changedVal);
//              delay(15);
//
//              rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
//              delay(15);
//              rearRight.write(tempCenterRR += changedVal);
//              rearRight3.write(tempCenterRR3 += changedVal);
//              delay(15);
//          }
//
//          Serial.print("PHASE 2 frontRightAngle : ");
//          Serial.println(frontRight.read());
//          Serial3.write("<- GO FORWARD PHASE 2");
//      }
//    }
//  }

  void goForward(int cnt) {
    //Serial.println("go forward");

    int frontLeftAngle = frontLeft.read();
    int frontRightAngle = frontRight.read();
    int rearLeftAngle = rearLeft.read();
    int rearRightAngle = rearRight.read();

    Serial.print(cnt);
    Serial.print(" >>> ");
    Serial.println("PHASE START : ");
    Serial.print("frontLeftAngle = ");
    Serial.println(frontLeftAngle);
    Serial.print(" : frontRightAngle = ");
    Serial.println(frontRightAngle);
    Serial.print("centerMinAngle = ");
    Serial.println(centerMinAngle);
    Serial.print(" : centerMaxAngle = ");
    Serial.println(centerMaxAngle);

    if (frontLeftAngle == 90 && frontRightAngle == 90) {    //PHASE START
      int rearRightAngle2 = secondStartAngle_FLRR;
  
      int tempCenterFR = centerAngle;
      int tempCenterRR = centerAngle;
  
      int tempCenterRR3 = thirdCenterAngle;
    
      int level = -1 * changedVal;

      for (int i = 0; i < term; i++) {
        if (rearRightAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
        }
              
        frontRight.write(tempCenterFR -= changedVal);
        delay(15);
  
        rearRight.write(tempCenterRR += changedVal);
        rearRight3.write(tempCenterRR3 += changedVal);
        delay(15);
        rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
        delay(15);
      }

      //Serial3.write("<- FORWARD START");
      Serial.write("<- FORWARD START");
    } //else if (frontLeftAngle == 90 && frontRightAngle == centerMinAngle) {   //PHASE 1
      else if (frontRightAngle == centerMinAngle) {   //PHASE 1
//      Serial.println("===========================================================");
//      Serial.print("PHASE 1 : frontLeftAngle angle = ");
//      Serial.print(frontLeft.read());
//      Serial.print(" : frontRightAngle angle = ");
//      Serial.print(frontRight.read());
//      Serial.print(" : rearLeftAngle angle = ");
//      Serial.print(rearLeft.read());
//      Serial.print(" : rearRightAngle angle = ");
//      Serial.println(rearRight.read());
//      Serial.println("===========================================================");
      
      int frontRightAngle2 = secondStartAngle_FRRL;
      int rearLeftAngle2 = secondStartAngle_FRRL;
  
      int tempCenterFL = centerAngle;
      int tempCenterRL = centerAngle;
  
      int tempCenterFR3 = thirdCenterAngle;
      int tempCenterRR3 = thirdCenterAngle;
  
      int tempMinFL = minAngle;
      int tempMaxRR = maxAngle;
      int tempMinFR = minAngle;
  
      int tempMaxRL3 = thirdMaxAngle;
      int tempMinFL3 = thirdMinAngle;
      int tempMaxRR3 = thirdMaxAngle;
    
      int level = changedVal;

      for (int i = 0; i < term; i++) {
          if (frontRightAngle2 == secondHighAngle_FRRL) {
              level = changedVal * -1;
          }

          frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
          delay(15);
          frontRight.write(tempMinFR += changedVal);
          frontRight3.write(tempCenterFR3 += changedVal);
          delay(15);

          rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
          delay(15);
          rearLeft.write(tempCenterRL -= changedVal);
          rearLeft3.write(tempMaxRL3 -= changedVal);
          delay(15);

          frontLeft.write(tempCenterFL += changedVal);
          frontLeft3.write(tempMinFL3 += changedVal);
          delay(15);

          rearRight.write(tempMaxRR -= changedVal);
          rearRight3.write(tempMaxRR3 -= changedVal);
          delay(15);
      }

//      Serial.print("PHASE 1 : frontRight angle = ");
//      Serial.println(frontRight.read());
//      Serial3.write("<- GO FORWARD PHASE 1");
        Serial.write("<- GO FORWARD PHASE 1");
        Serial.write(frontRight2.read());
    } //else if (frontLeftAngle == centerMaxAngle && frontRightAngle == 90) {   //PHASE 2
      else if (frontLeftAngle == centerMaxAngle) {   //PHASE 2
//      Serial.println("===========================================================");
//      Serial.print("PHASE 2 : frontLeftAngle angle = ");
//      Serial.print(frontLeft.read());
//      Serial.print(" : frontRightAngle angle = ");
//      Serial.print(frontRight.read());
//      Serial.print(" : rearLeftAngle angle = ");
//      Serial.print(rearLeft.read());
//      Serial.print(" : rearRightAngle angle = ");
//      Serial.println(rearRight.read());
//      Serial.println("===========================================================");
      
      int frontLeftAngle2 = secondStartAngle_FLRR;
      int rearRightAngle2 = secondStartAngle_FLRR;
  
      int tempCenterFL = centerAngle;
      int tempCenterRR = centerAngle;
      int tempCenterFR = centerAngle;
  
      int tempCenterFL3 = thirdCenterAngle;
      int tempCenterRR3 = thirdCenterAngle;
  
      int tempMaxFL = maxAngle;
      int tempMinRL = minAngle;
  
      int tempMaxFR3 = thirdMaxAngle;
      int tempMaxRL3 = rearLeft3.read();
    
      int level = -1 * changedVal;

      for (int i = 0; i < term; i++) {
          if (frontLeftAngle2 == secondLowAngle_FLRR) {
              level = changedVal;
          }

//          Serial.print("PHASE 2 : rearLeft3 angle = ");
//          Serial.println(rearLeft3.read());

          frontRight.write(tempCenterFR -= changedVal);
          frontRight3.write(tempMaxFR3 -= changedVal);
          delay(15);

          rearLeft.write(tempMinRL += changedVal);
          rearLeft3.write(tempMaxRL3 += changedVal);
          delay(15);

          frontLeft.write(tempMaxFL -= changedVal);
          frontLeft3.write(tempCenterFL3 -= changedVal);
          delay(15);
          frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
          delay(15);

          rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
          delay(15);
          rearRight.write(tempCenterRR += changedVal);
          rearRight3.write(tempCenterRR3 += changedVal);
          delay(15);
      }
      
//      Serial.print("PHASE 2 : frontRight angle = ");
//      Serial.println(frontRight.read());
//      Serial3.write("<- GO FORWARD PHASE 2");
        Serial.write("<- GO FORWARD PHASE 2");
        Serial.write(frontRight2.read());
    } else {
//      Serial.print("PHASE NONE : frontLeftAngle = ");
//      Serial.print(frontLeft.read());
//      Serial.print(" : frontRightAngle = ");
//      Serial.println(frontRight.read());
    }
  }

    // 로봇 후진
  void goBackward(int cnt) {
    //Serial.println("go forward");

    int frontLeftAngle = frontLeft.read();
    int frontRightAngle = frontRight.read();
    int rearLeftAngle = rearLeft.read();
    int rearRightAngle = rearRight.read();

//    Serial.print(cnt);
//    Serial.print(" >>> ");
//    Serial.print("BACKWARD PHASE START");

    if (frontLeftAngle == 90 && frontRightAngle == 90) {    //PHASE START
      int frontLeftAngle2 = secondStartAngle_FLRR;
  
      int tempCenterRL = centerAngle;
      int tempCenterFL = centerAngle;
  
      int tempCenterFL3 = thirdCenterAngle;
    
      int level = -1 * changedVal;

      for (int i = 0; i < term; i++) {
        if (frontLeftAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
        }
              
        rearLeft.write(tempCenterRL -= changedVal);
        delay(15);
  
        frontLeft.write(tempCenterFL += changedVal);
        frontLeft3.write(tempCenterFL3 += changedVal);
        delay(15);
        frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
        delay(15);
      }

      //Serial3.write("<- BACKWARD START");
      Serial.write("<- BACKWARD START");
    } //else if (rearRightAngle == 90 && rearLeftAngle == centerMinAngle) {   //PHASE 1
      else if (rearLeftAngle == centerMinAngle) {   //PHASE 1
      int rearLeftAngle2 = secondStartAngle_FRRL;
      int frontRightAngle2 = secondStartAngle_FRRL;
  
      int tempCenterRR = centerAngle;
      int tempCenterFR = centerAngle;
  
      int tempCenterRL3 = thirdCenterAngle;
      int tempCenterFL3 = thirdCenterAngle;
  
      int tempMinRR = minAngle;
      int tempMaxFL = maxAngle;
      int tempMinRL = minAngle;
  
      int tempMaxFR3 = thirdMaxAngle;
      int tempMinRR3 = thirdMinAngle;
      int tempMaxFL3 = thirdMaxAngle;
    
      int level = changedVal;

      for (int i = 0; i < term; i++) {
          if (rearLeftAngle2 == secondHighAngle_FRRL) {
              level = changedVal * -1;
          }

          rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
          delay(15);
          rearLeft.write(tempMinRL += changedVal);
          rearLeft3.write(tempCenterRL3 += changedVal);
          delay(15);

          frontRight2.write(frontRightAngle2 += (secondChangeVal * level));
          delay(15);
          frontRight.write(tempCenterFR -= changedVal);
          frontRight3.write(tempMaxFR3 -= changedVal);
          delay(15);

          rearRight.write(tempCenterRR += changedVal);
          rearRight3.write(tempMinRR3 += changedVal);
          delay(15);

          frontLeft.write(tempMaxFL -= changedVal);
          frontLeft3.write(tempMaxFL3 -= changedVal);
          delay(15);
      }

//      Serial.print("BACKWARD PHASE 1 : frontRight angle = ");
//      Serial.println(frontRight.read());
//      Serial3.write("<- GO BACKWARD PHASE 1");
        Serial.write("<- GO BACKWARD PHASE 1");
    } //else if (rearRightAngle == centerMaxAngle && rearLeftAngle == 90) {   //PHASE 2
      else if (rearRightAngle == centerMaxAngle) {   //PHASE 2
      int rearRightAngle2 = secondStartAngle_FLRR;
      int frontLeftAngle2 = secondStartAngle_FLRR;
  
      int tempCenterFL = centerAngle;
      int tempCenterRR = centerAngle;
      int tempCenterRL = centerAngle;
  
      int tempCenterFL3 = thirdCenterAngle;
      int tempCenterRR3 = thirdCenterAngle;
  
      int tempMaxRR = maxAngle;
      int tempMinFR = minAngle;
  
      int tempMaxRL3 = thirdMaxAngle;
      int tempMaxFR3 = frontRight3.read();
    
      int level = -1 * changedVal;

      for (int i = 0; i < term; i++) {
          if (rearRightAngle2 == secondLowAngle_FLRR) {
              level = changedVal;
          }

//          Serial.print("BACKWARD PHASE 2 : frontRight3 angle = ");
//          Serial.println(frontRight3.read());

          rearLeft.write(tempCenterRL -= changedVal);
          rearLeft3.write(tempMaxRL3 -= changedVal);
          delay(15);

          frontRight.write(tempMinFR += changedVal);
          frontRight3.write(tempMaxFR3 += changedVal);
          delay(15);

          rearRight.write(tempMaxRR -= changedVal);
          rearRight3.write(tempCenterRR3 -= changedVal);
          delay(15);
          rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
          delay(15);

          frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
          delay(15);
          frontLeft.write(tempCenterFL += changedVal);
          frontLeft3.write(tempCenterFL3 += changedVal);
          delay(15);
      }
      
//      Serial.print("BACKWARD PHASE 2 : rearLeft angle = ");
//      Serial.println(rearLeft.read());
//      Serial3.write("<- GO BACKWARD PHASE 2");
        Serial.write("<- GO BACKWARD PHASE 2");
    } else {
//      Serial.print("PHASE NONE : frontLeftAngle = ");
//      Serial.print(frontLeft.read());
//      Serial.print(" : frontRightAngle = ");
//      Serial.println(frontRight.read());
    }
  }

  // 로봇 후진
//  void goBackward() {
//    Serial.println("go backward");
//
//    if (start) {
//      start = false;
//
//      int frontLeftAngle2 = frontLeft2.read();
//      int tempCenter = centerAngle;
//      int tempCenter2 = centerAngle;
//      int tempCenter3 = centerAngle;
//      int level = -1 * changedVal;
//
//      for (int i = 0; i < term; i++) {
//        
//        rearLeft.write(tempCenter -= changedVal);
//        rearLeft3.write(tempCenter3 -= changedVal);
//        delay(15);
//
//        frontLeft.write(tempCenter += changedVal);
//        frontLeft.write(tempCenter3 += changedVal);
//        delay(15);
//
//  //      Serial.print("rearRightAngle2 : ");
//  //      Serial.print(i);
//  //      Serial.print(" : ");
//  //      Serial.println(rearRightAngle2);
//        
//        if (frontLeftAngle2 == secondLowAngle_FLRR) {
//          level = changedVal;
//        }
//
//        frontLeft2.write(frontLeftAngle2 += (2 * level));
//      }
//
//      Serial.print("frontLeftAngle : ");
//      Serial.println(frontLeft.read());
//      
//      Serial3.write("<- GO BACKWARD START");
//    } else {
//      int frontLeftAngle = frontLeft.read();
//      int frontRightAngle = frontRight.read();
//      int rearLeftAngle = rearLeft.read();
//      int rearRightAngle = rearRight.read();
//
//      int frontLeftAngle2 = secondStartAngle_FLRR;
//      int frontRightAngle2 = secondStartAngle_FRRL;
//      int rearLeftAngle2 = secondStartAngle_FRRL;
//      int rearRightAngle2 = secondStartAngle_FLRR;
//
//      int tempCenterFL = centerAngle;
//      int tempCenterFR = centerAngle;
//      int tempCenterRL = centerAngle;
//      int tempCenterRR = centerAngle;
//
//      int tempCenterFL3 = centerAngle;
//      int tempCenterFR3 = centerAngle;
//      int tempCenterRL3 = centerAngle;
//      int tempCenterRR3 = centerAngle;
//
//      int tempMin = minAngle;
//      int tempMax = maxAngle;
//
//      int tempMin3 = thirdMinAngle;
//      int tempMax3 = thirdMaxAngle;
//
//      Serial.print("frontLeftAngle : ");
//      Serial.println(frontLeftAngle);
//        
//      if (stepPhase) {
//        stepPhase = false;
//        int level = changedVal;
//
//        for (int i = 0; i < term; i++) {
//          //frontRight2 move  85 -> 131 -> 85
//          if (frontRightAngle2 == secondHighAngle_FRRL) {
//            level = changedVal * -1;
//          }
//    
//          frontLeft.write(tempMax -= changedVal); 
//          frontLeft3.write(tempMax3 -= changedVal); 
//          delay(15);
//
//          rearRight.write(tempCenterRR += changedVal);
//          rearRight3.write(tempCenterRR3 += changedVal);
//          delay(15);
//
//          frontRight2.write(frontRightAngle2 += (secondChangeVal * level));  
//          delay(15);
//          frontRight.write(tempCenterFR -= changedVal);
//          frontRight3.write(tempCenterFR3 -= changedVal);
//          delay(15);
//
//          rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level)); 
//          delay(15);
//          rearLeft.write(tempMin += changedVal);
//          rearLeft3.write(tempMin3 += changedVal);
//          delay(15);
//        }
//
//        Serial.print("PHASE 1 frontLeftAngle : ");
//        Serial.println(frontLeft.read());
//        Serial3.write("<- GO BACKWARD PHASE 1");
//      } else {
//        stepPhase = true;
//        int level = changedVal * -1;
//
//        for (int i = 0; i < term; i++) {
//          if (frontLeftAngle2 == secondLowAngle_FLRR) {
//            level = changedVal;
//          }
//    
//          
//          frontRight.write(tempMin += changedVal);
//          frontRight3.write(tempMin3 += changedVal);
//          delay(15);
//
//          rearLeft.write(tempCenterRL -= changedVal);
//          rearLeft3.write(tempCenterRL3 -= changedVal);
//          delay(15);
//
//          frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));  
//          delay(15);
//          frontLeft.write(tempCenterFL += changedVal);
//          frontLeft3.write(tempCenterFL3 += changedVal);
//          delay(15);
//
//          //rearRight2 move  85 -> 131 -> 85
//          rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
//          delay(15);
//          rearRight.write(tempMax -= changedVal);
//          rearRight3.write(tempMax3 -= changedVal);
//          delay(15);
//        } 
//        Serial.print("PHASE 2 frontLeftAngle : ");
//        Serial.println(frontLeft.read());
//        Serial3.write("<- GO BACKWARD PHASE 2");
//      }
//    }
//  }

  // 로봇 좌로 이동
  void goLeft() {
//    Serial.println("go left");
    if (start) {
      start = false;

      int frontRightAngle2 = frontRight2.read();
      int tempCenter = centerAngle, tempCenter2 = centerAngle;
      int level = changedVal * -1;

      for (int i = 0; i < term; i++) {
        
        frontRight.write(tempCenter += changedVal);
        delay(15);

        frontLeft.write(tempCenter2 -= changedVal);
        delay(15);

  //      Serial.print("rearRightAngle2 : ");
  //      Serial.print(i);
  //      Serial.print(" : ");
  //      Serial.println(rearRightAngle2);
        
        if (frontRightAngle2 == secondLowAngle_FRRL) {
          level = changedVal;
        }

        frontRight2.write(frontRightAngle2 += (2 * level));  
      }

      //Serial3.write("<- GO LEFT SATRT");
      Serial.write("<- GO LEFT SATRT");
    } else {
      int frontLeftAngle = frontLeft.read();
      int frontRightAngle = frontRight.read();
      int rearLeftAngle = rearLeft.read();
      int rearRightAngle = rearRight.read();

      int frontLeftAngle2 = secondStartAngle_FLRR;
      int frontRightAngle2 = secondStartAngle_FRRL;
      int rearLeftAngle2 = secondStartAngle_FRRL;
      int rearRightAngle2 = secondStartAngle_FLRR;

      int tempCenterFL = centerAngle;
      int tempCenterFR = centerAngle;
      int tempCenterRL = centerAngle;
      int tempCenterRR = centerAngle;

      int tempMin = minAngle;
      int tempMax = maxAngle;

//      Serial.print("frontLeftAngle : ");
//      Serial.println(frontLeftAngle);
        
      if (stepPhase) {
        stepPhase = false;
        int level = -1 * changedVal;

        for (int i = 0; i < term; i++) {
          frontRight.write(tempMin += changedVal);
          delay(15);
    
          rearLeft.write(tempCenterRL -= changedVal);
          delay(15);
    
          if (frontLeftAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
          }
    
          frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));     
          frontLeft.write(tempMax -= changedVal);
          delay(15);
    
          rearRight2.write(rearRightAngle2 += (secondChangeVal * level)); 
          rearRight.write(tempCenterRR += changedVal);
          delay(15);
        } 

        //Serial3.write("<- GO LEFT PHASE 1");
        Serial.write("<- GO LEFT PHASE 1");
      } else {
        stepPhase = true;
        int level = changedVal;

        for (int i = 0; i < term; i++) {
          frontLeft.write(tempCenterFL += changedVal);
          delay(15);
    
          rearRight.write(tempMax -= changedVal);
          delay(15);
          
          //frontRight2 move  85 -> 39 -> 85
          if (frontRightAngle2 == secondHighAngle_FRRL) {
            level = changedVal * -1;
          }
    
          frontRight2.write(frontRightAngle2 += (secondChangeVal * level));       
          frontRight.write(tempCenterFR -= changedVal);
          delay(15);
    
          //rearLeft2 move  85 -> 39 -> 85
          rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
          rearLeft.write(tempMin += changedVal);
          delay(15);
        }

        //Serial3.write("<- GO LEFT PHASE 2");
        Serial.write("<- GO LEFT PHASE 2");
      }
    }
  }

  // 로봇 우로 이동
  void goRight() {
 //   Serial.println("go right");
    if (start) {
      start = false;

      int rearLeftAngle2 = rearLeft2.read();
      int tempCenter = centerAngle, tempCenter2 = centerAngle;
      int level = changedVal;

      for (int i = 0; i < term; i++) {
        
        rearRight.write(tempCenter += changedVal);
        delay(15);

        rearLeft.write(tempCenter2 -= changedVal);
        delay(15);

  //      Serial.print("rearRightAngle2 : ");
  //      Serial.print(i);
  //      Serial.print(" : ");
  //      Serial.println(rearRightAngle2);
        
        if (rearLeftAngle2 == secondHighAngle_FRRL) {
          level = changedVal * -1;
        }

        rearLeft2.write(rearLeftAngle2 += (2 * level));  
      }

      //Serial3.write("<- GO RIGHT START");
      Serial.write("<- GO RIGHT START");
    } else {
      int frontLeftAngle = frontLeft.read();
      int frontRightAngle = frontRight.read();
      int rearLeftAngle = rearLeft.read();
      int rearRightAngle = rearRight.read();

      int frontLeftAngle2 = secondStartAngle_FLRR;
      int frontRightAngle2 = secondStartAngle_FRRL;
      int rearLeftAngle2 = secondStartAngle_FRRL;
      int rearRightAngle2 = secondStartAngle_FLRR;

      int tempCenterFL = centerAngle;
      int tempCenterFR = centerAngle;
      int tempCenterRL = centerAngle;
      int tempCenterRR = centerAngle;

      int tempMin = minAngle;
      int tempMax = maxAngle;

//      Serial.print("frontLeftAngle : ");
//      Serial.println(frontLeftAngle);
        
      if (stepPhase) {
        stepPhase = false;
        int level = changedVal;

        for (int i = 0; i < term; i++) {
          //frontRight2 move  85 -> 39 -> 85
          if (frontRightAngle2 == secondHighAngle_FRRL) {
            level = changedVal * -1;
          }
    
          frontRight2.write(frontRightAngle2 += (secondChangeVal * level));  
          frontRight.write(tempMin += changedVal);
          delay(15);

          //rearLeft2 move  85 -> 39 -> 85
          rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));
          rearLeft.write(tempCenterRL -= changedVal);
          delay(15);
    
          frontLeft.write(tempMax -= changedVal);
          delay(15);

          rearRight.write(tempCenterRR += changedVal);
          delay(15);
        } 

        //Serial3.write("<- GO RIGHT PHASE 1");
        Serial3.write("<- GO RIGHT PHASE 1");
      } else {
        stepPhase = true;
        int level = -1 * changedVal;

        for (int i = 0; i < term; i++) {
          if (frontLeftAngle2 == secondLowAngle_FLRR) {
            level = changedVal;
          }
    
          frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));   
          frontLeft.write(tempCenterFL += changedVal);
          delay(15);

          rearRight2.write(rearRightAngle2 += (secondChangeVal * level)); 
          rearRight.write(tempMax -= changedVal);
          delay(15);
                      
          frontRight.write(tempCenterFR -= changedVal);
          delay(15);
    
          rearLeft.write(tempMin += changedVal);
          delay(15);
        }

        //Serial3.write("<- GO RIGHT PHASE 2");
        Serial.write("<- GO RIGHT PHASE 2");
      }
    }
  }

  void turnLeft() {
 //   Serial.println("turn left");

    int tempCenterFL = centerAngle;
    int tempCenterFR = centerAngle;
    int tempCenterRL = centerAngle;
    int tempCenterRR = centerAngle;

    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;
      
    int state = frontLeft.read();

    int level = changedVal;
    for (int i = 0; i < term; i++) {
      if (frontLeftAngle2 == secondHighAngle_FLRR) {
          level = -1 * changedVal;
      }

      frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
      frontLeft.write(tempCenterFL +=  changedVal);
      delay(15);
    } 
    delay(500);

    level = changedVal * -1;
    for (int i = 0; i < term; i++) {
      if (frontRightAngle2 == secondLowAngle_FRRL) {
        level = changedVal;
      }
      
      frontRight.write(tempCenterFR +=  changedVal);
      frontRight2.write(frontRightAngle2 += (secondChangeVal * level));   
      delay(15);
    }
    delay(500);

    level = changedVal;
    for (int i = 0; i < term; i++) {
      if (rearRightAngle2 == secondHighAngle_FLRR) {
        level = changedVal * -1;
      }
      
      rearRight.write(tempCenterRR +=  changedVal);
      rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
      delay(15);
    }
    delay(500);

    level = changedVal * -1;
    for (int i = 0; i < term; i++) {
      if (rearLeftAngle2 == secondLowAngle_FRRL) {
        level = changedVal;
      }
      
      rearLeft.write(tempCenterRL +=  changedVal);
      rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));  
      delay(15);
    }
    delay(500);

    for (int i = 0; i < term; i++) {
      frontLeft.write(tempCenterFL -=  changedVal);
      delay(15);
      frontRight.write(tempCenterFR -=  changedVal);
      delay(15);
      rearLeft.write(tempCenterRL -=  changedVal);
      delay(15);
      rearRight.write(tempCenterRR -=  changedVal);
    }
    delay(500);

    //Serial3.write("<- TURN LEFT");
    Serial.write("<- TURN LEFT");
  }

  void turnRight() {
 //   Serial.println("turn right");
    int tempCenterFL = centerAngle;
    int tempCenterFR = centerAngle;
    int tempCenterRL = centerAngle;
    int tempCenterRR = centerAngle;

    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;
      
    int state = frontLeft.read();

    int level = changedVal;
    for (int i = 0; i < term; i++) {
      if (frontLeftAngle2 == secondHighAngle_FLRR) {
        level = changedVal * -1;
      }
      
      frontLeft.write(tempCenterFL -=  changedVal);
      frontLeft2.write(frontLeftAngle2 += (secondChangeVal * level));
      delay(15);
    } 
    delay(500);

    level = changedVal * -1;
    for (int i = 0; i < term; i++) {
      if (frontRightAngle2 == secondLowAngle_FRRL) {
        level = changedVal;
      }
      
      frontRight.write(tempCenterFR -=  changedVal);
      frontRight2.write(frontRightAngle2 += (secondChangeVal * level));   
      delay(15);
    }
    delay(500);

    level = changedVal;
    for (int i = 0; i < term; i++) {
      if (rearRightAngle2 == secondHighAngle_FLRR) {
        level = changedVal * -1;
      }
      
      rearRight.write(tempCenterRR -=  changedVal);
      rearRight2.write(rearRightAngle2 += (secondChangeVal * level));
      delay(15);
    }
    delay(500);

    level = changedVal * -1;
    for (int i = 0; i < term; i++) {
      if (rearLeftAngle2 == secondLowAngle_FRRL) {
        level = changedVal;
      }
      
      rearLeft.write(tempCenterRL -=  changedVal);
      rearLeft2.write(rearLeftAngle2 += (secondChangeVal * level));  
      delay(15);
    }
    delay(500);

    
    for (int i = 0; i < term; i++) {
      frontLeft.write(tempCenterFL +=  changedVal);
      delay(15);
      frontRight.write(tempCenterFR +=  changedVal);
      delay(15);
      rearLeft.write(tempCenterRL +=  changedVal);
      delay(15);
      rearRight.write(tempCenterRR +=  changedVal);
    }
    delay(500);

    //Serial3.write("<- TURN RIGHT");
    Serial.write("<- TURN RIGHT");
  }

  void twistLeft() {
 //   Serial.println("twist left");
    int tempCenterFL = centerAngle;
    int tempCenterFR = centerAngle;
    int tempCenterRL = centerAngle;
    int tempCenterRR = centerAngle;
      
    int state = frontLeft.read();

    if (state <= minAngle) return;
    
    for (int i = 0; i < term; i++) {
      frontLeft.write(tempCenterFL -=  changedVal);
      frontRight.write(tempCenterFR -=  changedVal);
      rearLeft.write(tempCenterRL -=  changedVal);
      rearRight.write(tempCenterRR -=  changedVal);
    }

    //Serial3.write("<- TWIST LEFT");
    Serial3.write("<- TWIST LEFT");
  }

  void twistRight() {
    //Serial.println("twist right");
    int tempCenterFL = centerAngle;
    int tempCenterFR = centerAngle;
    int tempCenterRL = centerAngle;
    int tempCenterRR = centerAngle;
      
    int state = frontLeft.read();

    if (state >= maxAngle) return;
    
    for (int i = 0; i < term; i++) {
      frontLeft.write(tempCenterFL +=  changedVal);
      frontRight.write(tempCenterFR +=  changedVal);
      rearLeft.write(tempCenterRL +=  changedVal);
      rearRight.write(tempCenterRR +=  changedVal);
    }

    //Serial3.write("<- TWIST RIGHT");
    Serial.write("<- TWIST RIGHT");
  }

  void hTiltForward() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2--);
  //    delay(15);
      frontRight2.write(frontRightAngle2++);
  //    delay(15);
      rearLeft2.write(rearLeftAngle2--);
   //   delay(15);
      rearRight2.write(rearRightAngle2++);
    }
    
    //Serial3.write("<- HORIZONTAL TILT FORWARD");
    Serial.write("<- HORIZONTAL TILT FORWARD");
  }

  void hTiltBackward() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2++);
   //   delay(15);
      frontRight2.write(frontRightAngle2--);
   //   delay(15);
      rearLeft2.write(rearLeftAngle2++);
   //   delay(15);
      rearRight2.write(rearRightAngle2--);
    }

    //Serial3.write("<- HORIZONTAL TILT BACKWARD");
    Serial3.write("<- HORIZONTAL TILT BACKWARD");
  }

  void hTiltLeft() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2--);
  //    delay(15);
      rearLeft2.write(rearLeftAngle2++);
  //    delay(15);
      frontRight2.write(frontRightAngle2--);
  //    delay(15);
      rearRight2.write(rearRightAngle2++);
    }

    //Serial3.write("<- HORIZONTAL TILT LEFT");
    Serial.write("<- HORIZONTAL TILT LEFT");
  }

  void hTiltRight() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontRight2.write(frontRightAngle2++);
   //   delay(15);
      rearRight2.write(rearRightAngle2--);
  //    delay(15);
      frontLeft2.write(frontLeftAngle2++);
  //    delay(15);
      rearLeft2.write(rearLeftAngle2--);
    }

    //Serial3.write("<- HORIZONTAL TILT RIGHT");
    Serial.write("<- HORIZONTAL TILT RIGHT");
  }

  void tiltForward() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2--);
      frontRight2.write(frontRightAngle2++);
    }

    //Serial3.write("<- TILT FORWARD");
    Serial.write("<- TILT FORWARD");
  }

  void tiltBackward() {
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      rearLeft2.write(rearLeftAngle2++);
      rearRight2.write(rearRightAngle2--);
    }

    //Serial3.write("<- TILT BACKWARD");
    Serial.write("<- TILT BACKWARD");
  }

  void tiltLeft() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int rearLeftAngle2 = secondStartAngle_FRRL;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2--);
      rearLeft2.write(rearLeftAngle2++);
    }

    //Serial3.write("<- TILT LEFT");
    Serial.write("<- TILT LEFT");
  }

  void tiltRight() {
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontRight2.write(frontRightAngle2++);
      rearRight2.write(rearRightAngle2--);
    }

    //Serial3.write("<- TILT RIGHT");
    Serial.write("<- TILT RIGHT");
  }


  void up() {
  //  int frontLeftAngle2 = secondStartAngle;
  //  int frontRightAngle2 = secondStartAngle;
  //  int rearLeftAngle2 = secondStartAngle;
  //  int rearRightAngle2 = secondStartAngle;
  //
  //  for (int i = 0; i < tiltTerm; i++) {
  //    frontLeft2.write(frontLeftAngle2--);
  //    frontRight2.write(frontRightAngle2++);
  //    rearLeft2.write(frontLeftAngle2--);
  //    rearRight2.write(frontRightAngle2++);
  //  }

    //Serial3.write("<- UP");
    Serial.write("<- UP");
  }

  void down() {
    int frontLeftAngle2 = secondStartAngle_FLRR;
    int frontRightAngle2 = secondStartAngle_FRRL;
    int rearLeftAngle2 = secondStartAngle_FRRL;
    int rearRightAngle2 = secondStartAngle_FLRR;

    for (int i = 0; i < tiltTerm; i++) {
      frontLeft2.write(frontLeftAngle2++);
  //    delay(15);
      frontRight2.write(frontRightAngle2--);
  //    delay(15);
      rearLeft2.write(rearLeftAngle2--);
  //    delay(15);
      rearRight2.write(rearRightAngle2++);
    }

    //Serial3.write("<- DOWN");
    Serial.write("<- DOWN");
  }
