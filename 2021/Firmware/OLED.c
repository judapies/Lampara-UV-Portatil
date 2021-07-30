/* ======================================================================================================================================================
 *
 * Copyright JP Bioingenieria SAS, 2021
 * All Rights Reserved
 * BSD license, check license.txt for more information                          *
 * All text above, and the splash screen must be included in any redistribution
 *
 * Sistema de control de encendido y apagado de lu Ultravioleta (UV-C)
 * Tres modos de funcionamiento: Modo Push, Modo Hold y modo Timer
 * En el modo Push se debe mantener oprimido el boton para encender la luz, una vez se suelte la luz se apaga.
 * En el modo Hold, la luz cambia de estado cada vez que se presione el boton.
 * En el modo Timer, se configura el temporizador oprimiendo la tecla arriba durante mas de 2 segundos. La luz se enciende durante el tiempo programado.
 * El sistema utiliza un display OLED I2C SH1106 de 128x64 pixeles.
 * ======================================================================================================================================================
*/
#include <18F2550.h>
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR,NOUSBDIV,NOVREGEN,PLL5
#use delay(clock=20000000)
#use I2C(MASTER,FAST,sda=PIN_C1, scl=PIN_C0, stream = SSD1306_STREAM)
//#use I2C(MASTER,FAST,sda=PIN_B0, scl=PIN_B1, stream = SSD1306_STREAM)

#define ON  1
#define OFF 0
#define SSD1306_128_64
#include "imagenes.h"
#include "SSD1306OLED.c"

#define   UP               !input(PIN_A0)
#define   DOWN             !input(PIN_A1)
#define   RIGHT            !input(PIN_A2)
#define   LEFT             !input(PIN_A3)

#define   LuzUV_on         output_bit(PIN_C2,0)
#define   LuzUV_off        output_bit(PIN_C2,1)

typedef struct{
   char Letra[20];
}MENUU;

MENUU Menus;

short estado=0,flagModo=0,tiempoCumplido=0,seleccion=0,flaguv=0;
int8 i=0,j=0,menu=0,modo=0,opcion=0,minutos=0,segundos=0,minutosp=0,segundosp=0;
signed int8 k=0;
int16 p=0,tiempos=0,tup=0,tdown=0,tright=0,tleft=0;
char voltage[9];

void dibujaImagen(int8 imagen){
   if(imagen==1){
      for( i = 0 ; i < 64 ; i ++ ){ //Funcion para dibujar un .bmp en orientacion horizontal
         for( j = 0 ; j < 16 ; j ++){
            for(k=7;k>-1;k--){      
               if( bit_test(bombillo[i][j] ,7-k )) 
                  SSD1306_DrawPixel( j*8+k,i, ON );
            }  
         }
      }
   }
   
   SSD1306_Display();
}

#int_TIMER1
void temp1s(void){
   set_timer1(45536);//5ms
   tiempos++;
      if(UP){
         if(tup<1200)
            tup++;
      }else{
         tup=0;
      }
         
      if(DOWN){
         if(tdown<120)
            tdown++;
      }else{
         tdown=0;
      }
         
      if(RIGHT){
         if(tright<120)
            tright++;   
      }else{
         tright=0;
      }
      
      if(left){
         if(tleft<120)
            tleft++;
      }else{
         tleft=0;
      }
    
   if(tiempos>=200){//1s
      tiempos=0;
      if(!input_state(PIN_C2) && modo==2){
         if(segundos>0){
            segundos--;
         }else{
            if(minutos>0){
               minutos--;
               segundos=59;
            }else{
               tiempoCumplido=1;
               minutos=minutosp;
               segundos=segundosp;
            }
         }
      }
   }
}


void main()
{
   LuzUV_off;
   set_tris_a(0x0f);
   output_bit(PIN_A4,0);
   output_bit(PIN_A5,0);
   output_bit(PIN_C6,0);
   output_bit(PIN_C7,0);
   output_bit(PIN_B0,0);
   output_bit(PIN_B1,0);
   output_bit(PIN_B2,0);
   output_bit(PIN_B3,0);
   output_bit(PIN_B4,0);
   output_bit(PIN_B5,0);
   output_bit(PIN_B6,0);
   output_bit(PIN_B7,0);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);   
   enable_interrupts(INT_TIMER1);
   enable_interrupts(global);   
   segundosp=read_eeprom(0);
   delay_ms(5);
   minutosp=read_eeprom(1);
   delay_ms(5);
   if(segundosp>59){
      segundosp=0;
      write_eeprom(0,0);
      delay_ms(5);
   }
   if(minutosp>90){
      minutosp=5;
      write_eeprom(1,5);
      delay_ms(5);
   }
   segundos=segundosp;
   minutos=minutosp;
   SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
   strcpy(Menus.Letra,"Lampara UV");
   SSD1306_DrawText(20,40,Menus.Letra,1);
   strcpy(Menus.Letra,"Portatil");
   SSD1306_DrawText(30,50,Menus.Letra,1);
   SSD1306_Display();
   delay_ms(2000);
   SSD1306_ClearDisplay(); 
   SSD1306_Display();
   flagModo=ON;
   menu=0;
  
   while (1){
      SSD1306_Display();
      if(menu==0){// Menu de funcionamiento principal
         dibujaImagen(1);//Bombillo 
         if(!input_state(PIN_C2)){
            SSD1306_InvertDisplay(1);
            strcpy(Menus.Letra,"ON ");
            SSD1306_DrawText(0,50,Menus.Letra,2);
         }else{
            SSD1306_InvertDisplay(0);
            strcpy(Menus.Letra,"OFF");
            SSD1306_DrawText(0,50,Menus.Letra,2);
         }
         
         if(UP && DOWN){
            delay_ms(50);
            if(UP && DOWN){   
               modo++;
               flagModo=OFF;
            }
         }
         if(modo>2)
            modo=0;
         
         if(modo==0){//modo Push
            if(!flagModo){
               flagModo=ON;
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
               SSD1306_InvertDisplay(1);
               strcpy(Menus.Letra,"Modo");
               SSD1306_DrawText(20,0,Menus.Letra,3);
               strcpy(Menus.Letra,"Push");
               SSD1306_DrawText(30,30,Menus.Letra,3);
               SSD1306_Display();
               delay_ms(800);
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
            }
            if(tleft>=20){
               LuzUV_on;
               if(flaguv==0){
                  delay_ms(100);
                  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
                  flaguv=1;
               }
            }else{
               LuzUV_off;
               if(flaguv==1){
                  delay_ms(100);
                  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
                  flaguv=0;
               }
            }
         }else if(modo==1){//modo Hold-on
            if(!flagModo){
               flagModo=ON;
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
               SSD1306_InvertDisplay(1);
               strcpy(Menus.Letra,"Modo");
               SSD1306_DrawText(20,0,Menus.Letra,3);
               strcpy(Menus.Letra,"Hold");
               SSD1306_DrawText(30,30,Menus.Letra,3);
               SSD1306_Display();
               delay_ms(800);
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
            }
            if(tleft>=20){
               if(!estado){
                  output_toggle(PIN_C2);
                  delay_ms(100);
                  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
                  estado=ON;
               }
            }else{
               estado=OFF;
            }
         }else if(modo==2){//modo Temporizador
            if(!flagModo){
               flagModo=ON;
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
               SSD1306_InvertDisplay(1);
               strcpy(Menus.Letra,"Modo");
               SSD1306_DrawText(20,0,Menus.Letra,3);
               strcpy(Menus.Letra,"Timer");
               SSD1306_DrawText(30,30,Menus.Letra,3);
               SSD1306_Display();
               delay_ms(800);
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
            }
            
            sprintf(voltage, "%02i:", minutos);
            SSD1306_DrawText(92,55,voltage,1);
            sprintf(voltage, "%02i", segundos);
            SSD1306_DrawText(110,55,voltage,1);
            
            if(input_state(PIN_C2)){
               if(tup>=400){
                  menu=1;
                  SSD1306_ClearDisplay(); 
                  SSD1306_Display();
                  SSD1306_InvertDisplay(1);
                  SSD1306_Display();
                  delay_ms(800);
               }
            }
            
            if(tiempoCumplido)
               LuzUV_off;
               
            if(tleft>=20 && !tiempoCumplido){
               if(!estado){
                  output_toggle(PIN_C2);
                  delay_ms(100);
                  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
                  estado=ON;
               }
            }else if(tleft>=20 && tiempoCumplido){
               if(!estado){
                  tiempoCumplido=OFF;
                  LuzUV_on;
                  delay_ms(100);
                  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
                  estado=ON;
               }
            }else{
               estado=OFF; 
            }
         }
      }else if(menu==1){
         if(UP && DOWN){
            delay_ms(50);
            if(UP && DOWN){   
               menu=0;
               write_eeprom(0,segundosp);delay_ms(10);
               write_eeprom(1,minutosp);delay_ms(10);
               segundos=segundosp;
               minutos=minutosp;
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
               SSD1306_InvertDisplay(1);
               strcpy(Menus.Letra,"Valor");
               SSD1306_DrawText(20,0,Menus.Letra,2);
               strcpy(Menus.Letra,"Almacenado");
               SSD1306_DrawText(2,30,Menus.Letra,2);
               SSD1306_Display();
               delay_ms(800);
               SSD1306_ClearDisplay(); 
               SSD1306_Display();
            }
         }else{   
         
            if(tleft>=20){
               if(!estado){
                  seleccion=!seleccion;
                  SSD1306_ClearDisplay(); 
                  SSD1306_Display();
                  estado=ON;
               }
            }else{
               estado=OFF;
            }
            
            strcpy(Menus.Letra,"TIMER UV");
            SSD1306_DrawText(20,0,Menus.Letra,2);
            sprintf(voltage, "%02i:", minutosp);
            SSD1306_DrawText(22,25,voltage,2);
            sprintf(voltage, "%02i", segundosp);
            SSD1306_DrawText(60,25,voltage,2);
            
            if(seleccion){
               SSD1306_DrawRect(55,20,32,25);
               if(tup>=60 && segundosp<59){
                  segundosp++;
                  tdown=0;
               }
               
               if(tdown>=60 && segundosp>0){
                  segundosp--;
                  tdown=0;
               }
            }else{
               SSD1306_DrawRect(17,20,32,25);
               if(tup>=60 && minutosp<90){
                  minutosp++;
                  tdown=0;
               }
               
               if(tdown>=60 && minutosp>1){
                  minutosp--;
                  tdown=0;
               }
            }
                  
            if(minutosp>90)
               minutosp=1;
         }
      }
   }
}
