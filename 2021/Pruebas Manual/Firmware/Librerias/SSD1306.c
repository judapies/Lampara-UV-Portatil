#include "fonts.c"
#include "SSD1306.h"



void OLED_init()
{
   
   #ifdef   reset
   RST_direction=0; // salida pin de reset;
#endif



    I2C__Init();
    Delay_ms(100);

    OLED_clear_buffer();
    OLED_reset_sequence();
     //OLED_write(0xAF, CMD);
       OLED_write(0X3C, CMD);


    OLED_write((Set_Display_ON_or_OFF_CMD + Display_OFF) , CMD);

    OLED_write(Set_Display_Clock_CMD, CMD);
    OLED_write(0x80, CMD);

    OLED_write(Set_Multiplex_Ratio_CMD, CMD);
    OLED_write(0x3F, CMD);

    OLED_write(Set_Display_Offset_CMD, CMD);
    OLED_write(0x00, CMD);

    OLED_write((Set_Display_Start_Line_CMD | 0x00), CMD);

    OLED_write(Set_Charge_Pump_CMD, CMD);
    OLED_write((Set_Higher_Column_Start_Address_CMD | Enable_Charge_Pump), CMD);

    OLED_write(Set_Memory_Addressing_Mode_CMD, CMD);
    OLED_write(Page_Addressing_Mode, CMD);

    OLED_write((Set_Segment_Remap_CMD | Column_Address_0_Mapped_to_SEG127), CMD);

    OLED_write((Set_COM_Output_Scan_Direction_CMD | Scan_from_COM63_to_0), CMD);

    OLED_write(Set_Common_HW_Config_CMD, CMD);
    OLED_write(0x12, CMD);

    OLED_write(Set_Contrast_Control_CMD, CMD);
    OLED_write(0xCF, CMD);

    OLED_write(Set_Pre_charge_Period_CMD, CMD);
    OLED_write(0xF1, CMD);

    OLED_write(Set_VCOMH_Level_CMD, CMD);
    OLED_write(0x40, CMD);

    OLED_write((Set_Entire_Display_ON_CMD | Normal_Display), CMD);

    OLED_write((Set_Normal_or_Inverse_Display_CMD | Non_Inverted_Display), CMD);

    OLED_write((Set_Display_ON_or_OFF_CMD + Display_ON) , CMD);

    OLED_gotoxy(0, 0);

    OLED_clear_screen();
}


void OLED_reset_sequence()
{
   #ifdef   reset
       RST = 0;
    delay_us(40);
    RST = 1;
   delay_us(4);

#endif

}

void OLED_write(unsigned char value, unsigned char control_byte)
{    
     I2C__Start();
     I2C__Write(SSD1306_I2C_Address);
    I2C__Write(control_byte);
    I2C__Write(value);
    I2C__Stop();
}


void OLED_gotoxy(unsigned char x_pos, unsigned char y_pos)
{
     OLED_write((Set_Page_Start_Address_CMD + y_pos), CMD);
     OLED_write(((x_pos & 0x0F) | Set_Lower_Column_Start_Address_CMD), CMD);
     OLED_write((((x_pos & 0xF0) >> 0x04) | Set_Higher_Column_Start_Address_CMD), CMD);
}


void OLED_fill(unsigned char bmp_data)
{
    unsigned char x_pos = 0x00;
    unsigned char page = 0x00;

    for(page = 0; page < y_max; page++)
    {
        OLED_gotoxy(x_min, page);

       I2C__Start();
       I2C__Write(SSD1306_I2C_Address);
       I2C__Write(DAT);

        for(x_pos = x_min; x_pos < x_max; x_pos++)
        {
          I2C__Write(bmp_data);
        }

       I2C__Stop();
    }
}


void OLED_print_Image(const unsigned char *bmp, unsigned char pixel)
{
    unsigned char x_pos = 0;
    unsigned char page = 0;

    if(pixel != OFF)
    {
        pixel = 0xFF;
    }
    else
    {
        pixel = 0x00;
    }

    for(page = 0; page < y_max; page++)
    {
         OLED_gotoxy(x_min, page);

        I2C__Start();
        I2C__Write(SSD1306_I2C_Address);
        I2C__Write(DAT);

         for(x_pos = x_min; x_pos < x_max; x_pos++)
         {
           I2C__Write((*bmp++ ^ pixel));
         }

        I2C__Stop();
     }
}


void OLED_clear_screen()
{
    OLED_fill(0x00);
}


void OLED_clear_buffer()
{
     unsigned int s = 0x0000;

     for(s = 0; s < buffer_size; s++)
     {
          buffer[s] = 0x00;
     }
}


void OLED_cursor(unsigned char x_pos, unsigned char y_pos)
{
    unsigned char s = 0x00;

    if(y_pos != 0x00)
    {
        if(x_pos == 1)
        {
            OLED_gotoxy(0x00, (y_pos + 0x02));
        }
        else
        {
            OLED_gotoxy((0x50 + ((x_pos - 0x02) * 0x06)), (y_pos + 0x02));
        }

        for(s = 0x00; s < 0x06; s++)
        {
            OLED_write(0xFF, DAT);
        }
    }
}


void OLED_draw_bitmap(unsigned char xb, unsigned char yb, unsigned char xe, unsigned char ye, const unsigned char *bmp_img)
{
    unsigned int s = 0x0000;
    unsigned char x_pos = 0x00;
    unsigned char y_pos = 0x00;

    for(y_pos = yb; y_pos <= ye; y_pos++)
    {
        OLED_gotoxy(xb, y_pos);
        for(x_pos = xb; x_pos < xe; x_pos++)
        {
     OLED_write(bmp_img[s++], DAT);
        }
    }
}


void OLED_print_char(unsigned char x_pos, unsigned char y_pos, unsigned char ch)
{
    unsigned char chr = 0x00;
    unsigned char s = 0x00;

    chr = (ch - 0x20);

    if(x_pos > (x_max - 0x06))
    {
        x_pos = 0x00;
        y_pos++;
    }
    OLED_gotoxy(x_pos, y_pos);

    for(s = 0x00; s < 0x06; s++)
    {
        OLED_write(font_regular[chr][s], DAT);
    }
}


void OLED_print_string(unsigned char x_pos, unsigned char y_pos, unsigned char *ch)
{
    unsigned char s = 0x00;

    do
    {
        OLED_print_char(x_pos, y_pos, ch[s++]);
        x_pos += 0x06;
     }while((ch[s] >= 0x20) && (ch[s] <= 0x7F));
}


void OLED_print_chr(unsigned char x_pos, unsigned char y_pos, signed int value)
{
    unsigned char ch = 0x00;

    if(value < 0x00)
    {
        OLED_print_char(x_pos, y_pos, '-');
        value = -value;
    }
    else
    {
        OLED_print_char(x_pos, y_pos,' ');
    }

     if((value > 99) && (value <= 999))
     {
         ch = (value / 100);
         OLED_print_char((x_pos + 6), y_pos , (48 + ch));
         ch = ((value % 100) / 10);
         OLED_print_char((x_pos + 12), y_pos , (48 + ch));
         ch = (value % 10);
         OLED_print_char((x_pos + 18), y_pos , (48 + ch));
     }
     else if((value > 9) && (value <= 99))
     {
         ch = ((value % 100) / 10);
         OLED_print_char((x_pos + 6), y_pos , (48 + ch));
         ch = (value % 10);
         OLED_print_char((x_pos + 12), y_pos , (48 + ch));
         OLED_print_char((x_pos + 18), y_pos , 32);
     }
     else if((value >= 0) && (value <= 9))
     {
         ch = (value % 10);
         OLED_print_char((x_pos + 6), y_pos , (48 + ch));
         OLED_print_char((x_pos + 12), y_pos , 32);
         OLED_print_char((x_pos + 18), y_pos , 32);
     }
}


void OLED_print_int(unsigned char x_pos, unsigned char y_pos, signed long value)
{
    unsigned char ch = 0x00;

    if(value < 0)
    {
        OLED_print_char(x_pos, y_pos, '-');
        value = -value;
    }
    else
    {
        OLED_print_char(x_pos, y_pos,' ');
    }

    if(value > 9999)
    {
        ch = (value / 10000);
        OLED_print_char((x_pos + 6), y_pos , (48 + ch));

        ch = ((value % 10000)/ 1000);
        OLED_print_char((x_pos + 12), y_pos , (48 + ch));

        ch = ((value % 1000) / 100);
        OLED_print_char((x_pos + 18), y_pos , (48 + ch));

        ch = ((value % 100) / 10);
        OLED_print_char((x_pos + 24), y_pos , (48 + ch));

        ch = (value % 10);
        OLED_print_char((x_pos + 30), y_pos , (48 + ch));
    }

    else if((value > 999) && (value <= 9999))
    {
        ch = ((value % 10000)/ 1000);
        OLED_print_char((x_pos + 6), y_pos , (48 + ch));

        ch = ((value % 1000) / 100);
        OLED_print_char((x_pos + 12), y_pos , (48 + ch));

        ch = ((value % 100) / 10);
        OLED_print_char((x_pos + 18), y_pos , (48 + ch));

        ch = (value % 10);
        OLED_print_char((x_pos + 24), y_pos , (48 + ch));
        OLED_print_char((x_pos + 30), y_pos , 32);
    }
    else if((value > 99) && (value <= 999))
    {
        ch = ((value % 1000) / 100);
        OLED_print_char((x_pos + 6), y_pos , (48 + ch));

        ch = ((value % 100) / 10);
        OLED_print_char((x_pos + 12), y_pos , (48 + ch));

        ch = (value % 10);
        OLED_print_char((x_pos + 18), y_pos , (48 + ch));
        OLED_print_char((x_pos + 24), y_pos , 32);
        OLED_print_char((x_pos + 30), y_pos , 32);
    }
    else if((value > 9) && (value <= 99))
    {
        ch = ((value % 100) / 10);
        OLED_print_char((x_pos + 6), y_pos , (48 + ch));

        ch = (value % 10);
        OLED_print_char((x_pos + 12), y_pos , (48 + ch));

        OLED_print_char((x_pos + 18), y_pos , 32);
        OLED_print_char((x_pos + 24), y_pos , 32);
        OLED_print_char((x_pos + 30), y_pos , 32);
    }
    else
    {
        ch = (value % 10);
        OLED_print_char((x_pos + 6), y_pos , (48 + ch));
        OLED_print_char((x_pos + 12), y_pos , 32);
        OLED_print_char((x_pos + 18), y_pos , 32);
        OLED_print_char((x_pos + 24), y_pos , 32);
        OLED_print_char((x_pos + 30), y_pos , 32);
    }
}


void OLED_print_decimal(unsigned char x_pos, unsigned char y_pos, unsigned int value, unsigned char points)
{
    unsigned char ch = 0x00;

    OLED_print_char(x_pos, y_pos, '.');

    ch = (value / 1000);
    OLED_print_char((x_pos + 6), y_pos , (48 + ch));

    if(points > 1)
    {
        ch = ((value % 1000) / 100);
        OLED_print_char((x_pos + 12), y_pos , (48 + ch));


        if(points > 2)
        {
            ch = ((value % 100) / 10);
            OLED_print_char((x_pos + 18), y_pos , (48 + ch));

            if(points > 3)
            {
                ch = (value % 10);
                OLED_print_char((x_pos + 24), y_pos , (48 + ch));
            }
        }
    }
}


void OLED_print_float(unsigned char x_pos, unsigned char y_pos, float value, unsigned char points)
{
    signed long tmp = 0x00;

    tmp = value;
    OLED_print_int(x_pos, y_pos, tmp);
    tmp = ((value - tmp) * 10000);

    if(tmp < 0)
    {
       tmp = -tmp;
    }

    if((value >= 9999) && (value < 99999))
    {
        OLED_print_decimal((x_pos + 36), y_pos, tmp, points);
    }
    else if((value >= 999) && (value < 9999))
    {
        OLED_print_decimal((x_pos + 30), y_pos, tmp, points);
    }
    else if((value >= 99) && (value < 999))
    {
        OLED_print_decimal((x_pos + 24), y_pos, tmp, points);
    }
    else if((value >= 9) && (value < 99))
    {
        OLED_print_decimal((x_pos + 18), y_pos, tmp, points);
    }
    else if(value < 9)
    {
        OLED_print_decimal((x_pos + 12), y_pos, tmp, points);
        if((value) < 0)
        {
            OLED_print_char(x_pos, y_pos, '-');
        }
        else
        {
            OLED_print_char(x_pos, y_pos, ' ');
        }
    }
}


void Draw_Pixel(unsigned char x_pos, unsigned char y_pos, unsigned char colour)
{
    unsigned char value = 0x00;
    unsigned char page = 0x00;
    unsigned char bit_pos = 0x00;

    page = (y_pos / y_max);
    bit_pos = (y_pos - (page * y_max));
    value = buffer[((page * x_max) + x_pos)];

    if((colour & YES) != NO)
    {
        value |= (1 << bit_pos);
    }
    else
    {
        value &= (~(1 << bit_pos));
    }

    buffer[((page * x_max) + x_pos)] = value;
    OLED_gotoxy(x_pos, page);
    OLED_write(value, DAT);
}


void Draw_Line(signed int x1, signed int y1, signed int x2, signed int y2, unsigned char colour)
{
    signed int dx = 0x0000;
    signed int dy = 0x0000;
    signed int stepx = 0x0000;
    signed int stepy = 0x0000;
    signed int fraction = 0x0000;

    dy = (y2 - y1);
    dx = (x2 - x1);

    if (dy < 0)
    {
        dy = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if (dx < 0)
    {
        dx = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    Draw_Pixel(x1, y1, colour);

    if(dx > dy)
    {
        fraction = (dy - (dx >> 1));
        while (x1 != x2)
        {
            if(fraction >= 0)
            {
                y1 += stepy;
                fraction -= dx;
            }

            x1 += stepx;
            fraction += dy;

            Draw_Pixel(x1, y1, colour);
        }
    }
    else
    {
        fraction = (dx - (dy >> 1));
        while (y1 != y2)
        {
            if (fraction >= 0)
            {
                x1 += stepx;
                fraction -= dy;
            }

            y1 += stepy;
            fraction += dx;

            Draw_Pixel(x1, y1, colour);
        }
    }
}


void Draw_Rectangle(signed int x1, signed int y1, signed int x2, signed int y2, unsigned char fill, unsigned char colour, unsigned char type)
{
     unsigned short i = 0x00;
     unsigned short xmin = 0x00;
     unsigned short xmax = 0x00;
     unsigned short ymin = 0x00;
     unsigned short ymax = 0x00;

     if(fill != NO)
     {
        if(x1 < x2)
        {
           xmin = x1;
           xmax = x2;
        }
        else
        {
           xmin = x2;
           xmax = x1;
        }

        if(y1 < y2)
        {
           ymin = y1;
           ymax = y2;
        }
        else
        {
           ymin = y2;
           ymax = y1;
        }

        for(; xmin <= xmax; ++xmin)
        {
           for(i = ymin; i <= ymax; ++i)
           {
               Draw_Pixel(xmin, i, colour);
           }
         }
     }

     else
     {
        Draw_Line(x1, y1, x2, y1, colour);
        Draw_Line(x1, y2, x2, y2, colour);
        Draw_Line(x1, y1, x1, y2, colour);
        Draw_Line(x2, y1, x2, y2, colour);
     }

     if(type != SQUARE)
     {
         Draw_Pixel(x1, y1, ~colour);
         Draw_Pixel(x1, y2, ~colour);
         Draw_Pixel(x2, y1, ~colour);
         Draw_Pixel(x2, y2, ~colour);
     }
}


void Draw_Circle(signed int xc, signed int yc, signed int radius, unsigned char fill, unsigned char colour)
{
   signed int a = 0x0000;
   signed int b = 0x0000;
   signed int P = 0x0000;

   b = radius;
   P = (1 - b);

   do
   {
        if(fill != NO)
        {
           Draw_Line((xc - a), (yc + b), (xc + a), (yc + b), colour);
           Draw_Line((xc - a), (yc - b), (xc + a), (yc - b), colour);
           Draw_Line((xc - b), (yc + a), (xc + b), (yc + a), colour);
           Draw_Line((xc - b), (yc - a), (xc + b), (yc - a), colour);
        }
        else
        {
           Draw_Pixel((xc + a), (yc + b), colour);
           Draw_Pixel((xc + b), (yc + a), colour);
           Draw_Pixel((xc - a), (yc + b), colour);
           Draw_Pixel((xc - b), (yc + a), colour);
           Draw_Pixel((xc + b), (yc - a), colour);
           Draw_Pixel((xc + a), (yc - b), colour);
           Draw_Pixel((xc - a), (yc - b), colour);
           Draw_Pixel((xc - b), (yc - a), colour);
        }

        if(P < 0)
        {
           P += (3 + (2 * a++));
        }
        else
        {
           P += (5 + (2 * ((a++) - (b--))));
        }
    }while(a <= b);
}



