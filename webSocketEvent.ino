void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length){    //웹소켓에서 받는 신호
  if(type == WStype_TEXT){
    Serial.println(payload[1]);
    if(payload[0]=='#'){ //hex코드 일때(색)
       colorOption = false;
       option[0] = false;
       option[1] = false;
       option[2] = false;
       option[3] = false;
      if(payload[1]>=48&&payload[1]<=57) redValue[0]=(payload[1]-48)*16;
      else redValue[0]=(payload[1]-87)*16;
      
      if(payload[2]>=48&&payload[2]<=57) redValue[1]=payload[2]-48;
      else redValue[1]=(payload[2]-87);
      red = redValue[0]+redValue[1];

      if(payload[3]>=48&&payload[3]<=57) greenValue[0]=(payload[3]-48)*16;
      else greenValue[0]=(payload[3]-87)*16;
      
      if(payload[4]>=48&&payload[4]<=57) greenValue[1]=(payload[4]-48);
      else greenValue[1]=(payload[4]-87);
      green = greenValue[0]+greenValue[1];

      if(payload[5]>=48&&payload[5]<=57) blueValue[0]=(payload[5]-48)*16;
      else blueValue[0]=(payload[5]-87)*16;
      
      if(payload[6]>=48&&payload[6]<=57) blueValue[1]=payload[6]-48;
      else blueValue[1]=(payload[6]-87);
      blue = blueValue[0]+blueValue[1];
      
      strip.begin();
      for(int i=0; i<LED_COUNT; i++){
         strip.setPixelColor(i, red, green, blue);
      }
      strip.setBrightness(255);
      strip.show();
      LED_on = true;
      sensorCount = false;
    }
    else if(payload[0]=='@'){
      colorOption = true;
      if(payload[1]=='1'){    //옵션 1
        if(payload[2]=='1'){  //옵션 1 on
          option[0] = true;
          option[1] = false;
          option[2] = false;
          option[3] = false;
        }
        else{                //옵션 1 off
        option[0] = false;
        strip.setBrightness(0);
        strip.show();
        }
      }
      else if(payload[1]=='2'){    //옵션 2
        if(payload[2]=='1'){  //옵션 2 on
          option[0] = false;
          option[1] = true;
          option[2] = false;
          option[3] = false;
        }
        else{  //옵션 2 off
        option[1] = false;
        strip.setBrightness(0);
        strip.show();
        }
      }
      else if(payload[1]=='3'){    //옵션 3
        if(payload[2]=='1'){  //옵션 3 on
          option[0] = false;
          option[1] = false;
          option[2] = true;
          option[3] = false;
        }
        else{                 //옵션 3 off
        option[2] = false;
        strip.setBrightness(0);
        strip.show();
        }
      }
      else if(payload[1]=='4'){    //옵션 4
        if(payload[2]=='1'){  //옵션 4 on
          option[0] = false;
          option[1] = false;
          option[2] = false;
          option[3] = true;
        }
        else{                //옵션 4 off
        option[3] = false;
        strip.setBrightness(0);
        strip.show();
        }
      }
     }
    else if(payload[0]=='!'){
      if(payload[2]=='F'){
         sensorOn = false;
         sensorCount = false;
      }
      else if(payload[2]=='N'){
        sensorOn = true;
      }
     }
    else if(payload[0]=='$'){
     if(payload[1]=='1'){
      strip.setBrightness(255);
      strip.show();
     }
     else{
      strip.setBrightness(0);
      strip.show();
     }
   }
  }
}
