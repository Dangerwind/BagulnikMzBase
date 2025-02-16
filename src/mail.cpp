
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "NTPClient.h"

#include <WiFi.h>
#include <webhtml.h>

#include <Adafruit_SPIDevice.h>

#include <WebServer_ESP32_SC_W5500.h>
#include <FastBot.h>



// определение пинов --------
#define display_sda 2
#define display_sck 1

#define uart_tx 47  // 12
#define uart_rx 45  // 14
#define uart_rts 48 // 13

#define SPI_CS 18 //15 // 10
#define SPI_DI 40 //16 // 11
#define SPI_DO 17 // 17 // 12
#define SPI_SLK 39 //18 // 13

#define Rele 8
#define Led 3
#define zoomer 9

#define Button_minus 5
#define Button_menu 6
#define Button_plus 7

#define RS485_pauza 15 // пауза 15 бит
#define RS485_speed 38400

#define SizeOfFlash 16 // мегабит размер флэщки 

#define NO_ANSWER 5  // сколько окманд без ответа до потери связи
#define NO_ANSWER_SERCH 10  // сколько команд без ответа до потери связи

#define BOT_TOKEN "******"  // токен телеграм-бота

// w5500 -----------
//#define ETH_SPI_HOST        SPI3_HOST
//#define SPI_CLOCK_MHZ       25
// Must connect INT to GPIOxx or not working
// #define INT_GPIO            39

//#define MISO_GPIO           40
//#define MOSI_GPIO           36
//#define SCK_GPIO            37
//#define CS_GPIO             38
#define w5500_reset         41

//#define ETH_SPI_HOST        SPI3_HOST
//#define SPI_CLOCK_MHZ       25

// Must connect INT to GPIOxx or not working

#define INT_GPIO            14
#define MISO_GPIO           13
#define MOSI_GPIO           11
#define SCK_GPIO            12
#define CS_GPIO             10

#define ID_CHAT "*****"  // список ID юзеров с кем будет работаьь бот

FastBot bot(BOT_TOKEN);

uint16_t Speed_ans, Speed_err;

uint8_t coconut1, coconut2, coconut3, coconut0;     

String BotAnswer =  "";
 
Adafruit_SPIDevice spi_dev = Adafruit_SPIDevice(SPI_CS, SPI_SLK, SPI_DO, SPI_DI, 10000000, SPI_BITORDER_MSBFIRST, SPI_MODE0);

byte DatchikBase[256];  // состояние всех датчиков
String ArhivDatas = ""; // архив куда будет записана вся база тревог
uint32_t RecNumber = 0; // порядковый номер

// текущее время и дата
String Time_Time;
String Time_Date;

WiFiUDP ntpUDP;                                                   // Объект ntp
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000); // Так-же можно более детально настроить пул и задержку передачи.
uint8_t TimeZone = 3;

char *ssid = "******                   ";              //  SSID     вети wifi
char *password = "***************          "; //  Password 

//Настройки IP адреса 
uint8_t num_ip[4]={192,168,0,25};
uint8_t num_geteway[4]={192,168,0,1};
uint8_t num_subnet[4]={255,255,255,0};
uint8_t num_primaryDNS[4]={8,8,8,8};
uint8_t num_secondaryDNS[4]={8,8,4,4};
uint32_t num_server=80;
uint8_t Type_connection=1;  // 0 - mqtt 1 - web 2 - telegram bot
uint8_t NET_use=1;   // 1 - WiFi     2 - w5500
uint8_t Datchik_nums=0; // количество блоков на линии

String header;

  // тут надо сичтать начальные данные 
uint8_t Setup_enter=0;

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};



// 
 U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE, display_sck, display_sda); // ESP32 Thing, pure SW emulated I2C

QueueHandle_t RS485_in_stack; // сткж для отправки команд
QueueHandle_t RS485_stack; // сткж принятых команд
QueueHandle_t RS485_flags;
QueueHandle_t Button_stack;
QueueHandle_t Music_stack;

QueueHandle_t Flash_input;
QueueHandle_t Flash_output;

struct Button_struct
{
  uint8_t Button_Pressed[3]; // 0-не нажато 1-нажато 2-долго нажато 3- очень долго
};

struct RS485_in_out
{
  uint16_t pak;
  uint16_t pak_err;
  uint16_t pak_null;
  unsigned long time;
  uint16_t action;
  uint8_t Nums_of_datchik;
};

struct ArhivBase
{
  uint32_t nomer;  // 4 байтный номер записи
  uint8_t datchik;
  uint8_t time[3];
  uint8_t date[3];
  uint8_t message;
  uint8_t command_to_flash; // 1 - записать что есть 2 - стереть всю флэшку
};

// CRC для протокола RS485
uint8_t crc_table[256] = { // CRC для протокола RS485
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

// --------------------------------------------

hw_timer_t *uart_timer = NULL;

unsigned long old_time;

// расчет crc для отсылки
uint8_t crc_make(volatile uint8_t *dats)
{
  uint8_t crc = 0;
  for (uint8_t ii = 0; ii < 7; ii++) // посчитать CRC
  {
    crc = crc_table[crc ^ dats[ii]]; // расчет CRC
  }
  return (crc);
}

void Sound_Task(void *pvParameters)
{
  int music;
  while (1)
  {
    if (xQueueReceive(Music_stack, &music, 0) == pdPASS) // portMAX_DELAY
    {
      switch (music)
      {
        case 1:
        {
          ledcWriteNote(0, NOTE_G, 4);
          vTaskDelay(5 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_C, 4);
          vTaskDelay(5 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_G, 4);
          vTaskDelay(5 / portTICK_PERIOD_MS);
          ledcWrite(0, 0);
        } break;
        case 2:
        {
          ledcWriteNote(0, NOTE_G, 4);
          vTaskDelay(50 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_C, 4);
          vTaskDelay(100 / portTICK_PERIOD_MS);
          ledcWrite(0, 0);
        } break;
        case 3:
        {
          ledcWriteNote(0, NOTE_C, 5);
          vTaskDelay(80 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_E, 5);
          vTaskDelay(80 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_G, 5);
          vTaskDelay(160 / portTICK_PERIOD_MS);
          ledcWrite(0, 0);
        } break;
        case 4:
        {
          ledcWriteNote(0, NOTE_C, 4);
          vTaskDelay(100 / portTICK_PERIOD_MS);
          ledcWriteNote(0, NOTE_C, 4);
          vTaskDelay(100 / portTICK_PERIOD_MS);
          ledcWrite(0, 0);
        } break;
        default: break;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    else
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
  vTaskDelete(NULL);
}

void sound(int music)
{
  xQueueOverwrite(Music_stack, &music);
}

void RS485_Task(void *pvParameters)
{
  byte buff_send[8] = {0x01, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  byte buff_get[8];
  byte buff_get_trash[256]; // считает сколько было запросов без ответов если периметр порвется - то выдать тревогу

  uint8_t flag0;

  RS485_in_out rs485_status;
  unsigned int pak, pak_err, pak_null;
  unsigned long old_time;
  uint8_t ActionIs;

  uint8_t Sender_flag=0;  // 0 - просто отсылать запросы 1 - отсылать только то, что получено от основной программы и не слать больше автоматом
  int Nums_of_datchik=0;  // количество датчико на линии всего

  int Datchik_counter=1;   // счетчик датчиков для цикла
  uint8_t block_offline[256];

  uint8_t crc = 0;

  int autodetect_etap=0;  // для обнаружения нового периметра
  int autodetect_repeat=0;
  int autodetect_block=1;

  int autoup_etap=0;  // если была потеря связи чтобы восстановить ее 
  int autoup_repeat=0;

  uint8_t autodetect_lognum=1;
  uint8_t autodetect_repeater=0;

  uint8_t led_stat=0;
  uint8_t led_count=0;

  while (1)
  {
    buff_send[0]=Datchik_counter;
    buff_send[1]=0x83; // запрос состояни
    for (uint8_t i=2; i<=7; i++) { buff_send[i]=0x00; }   

    // если есть что получить - получить из буфера команду из гоавной программы
    if (xQueueReceive(RS485_in_stack, &buff_get, 0) == pdPASS) // portMAX_DELAY
    {
        if ( (buff_get[0]==0x00)&(buff_get[1]==0x81) )
        {
           if (buff_get[2]==0x01) { Sender_flag=1; autodetect_etap=0; } // отсылать только принятые от основной прогрпаммы
           if (buff_get[2]==0x02) { Sender_flag=0; autodetect_etap=0; }// отсылать все

           if (buff_get[2]==0x03) 
           { Nums_of_datchik=buff_get[3]; 
                // Serial.println("RS -> Nums_of_datchik=" + String(Nums_of_datchik));
           }  //   измеиить количество датчиков на линии

           if (buff_get[2]==0x04) // отослать клличпество блоков которые есть на линии
           {
                buff_send[0]=0x01;
                buff_send[1]=0x87;
                buff_send[2]=0x00;
                buff_send[3]=Nums_of_datchik;            
           }

           if (buff_get[2]==0x05) // запуск алгорится поиска блоков на линии
           { 
              autodetect_etap=1;  // первый этап, опрос кто на линии
              Sender_flag=0;    // отсылать постоянно
              autodetect_repeater=0;  // пока 0 неотвеченных команд
              autodetect_block=1;  // номерация с 1 блока
              autoup_etap=0;

             // Nums_of_datchik=0;
             //  Serial.println("AUTODETECT");
           } 
        }
        else  // если была какая то команда
        {
              if (Sender_flag>1) Sender_flag=1; // если надо отослать 1 раз и все
              for (uint8_t i=0; i<=7; i++)
              {
                buff_send[i]=buff_get[i];
              }
        }
    }

    if (autodetect_etap>0)   // проводить запросы и автовключенние блоков на линии
    {
        switch (autodetect_etap)
        {
          case 1:  // запрос кто на линии
          {
             buff_send[0]=0;
             buff_send[1]=0x82;
             autodetect_repeater++; // добавить и если имного без ответа - то прервать автоопределение 
          } break;

          case 2:  // назначить номер
          {
             buff_send[0]=autodetect_lognum;
             buff_send[1]=0x88;
             buff_send[2]=autodetect_block;
             autodetect_repeater++;
          } break;

          case 3:  // не отвечать на поиски
          {   
             buff_send[0]=autodetect_block;
             buff_send[1]=0x87;
             buff_send[2]=0;
             autodetect_repeater++;
          } break;          
          case 4:  // вклюдчить питание следуюзего
          {
             buff_send[0]=autodetect_block;
             buff_send[1]=0x86;
             buff_send[2]=0xFF;  // включить питание
             autodetect_repeater++;
          } break;
          default: break;
        }

        if (autodetect_repeater>10) 
        {
          autodetect_etap=0;
          autodetect_repeater=0;
          if (autodetect_block>1) Nums_of_datchik=(autodetect_block-1);
          else Nums_of_datchik=1;
        }
    }

    if (autoup_etap>0)   // проводить запросы и автовключенние блоков на линии
    {
        switch (autoup_etap)
        {
          case 1:  // запрос кто на линии
          {
             buff_send[0]=0;
             buff_send[1]=0x82;
             autoup_repeat++; // добавить и если имного без ответа - то прервать автоопределение 
          } break;

          case 2:  // не отвечать на поиски
          {
             buff_send[0]=autodetect_lognum;
             buff_send[1]=0x87;
             buff_send[2]=0;
             autoup_repeat++;
          } break;          
          case 3:  // вклюдчить питание следуюзего
          {
             buff_send[0]=autodetect_lognum;
             buff_send[1]=0x86;
             buff_send[2]=0xFF;  // включить питание
             autoup_repeat++;
          } break;
          default: break;
        }

        if (autoup_repeat>10) 
        {
            autoup_etap=0;
            autoup_repeat=0;
            if (Nums_of_datchik<autodetect_lognum)
            {
                Nums_of_datchik=autodetect_lognum;
            }
        }
    }

    if (Sender_flag<=1)
    {
          digitalWrite(uart_rts, HIGH);
          delayMicroseconds(RS485_pauza * 1000000 / RS485_speed);
          buff_send[7] = 0;
          for (uint8_t ii = 0; ii < 7; ii++) { buff_send[7] = crc_table[buff_send[7] ^ buff_send[ii]]; } 
          Serial1.write((byte *)&buff_send, 8);
          vTaskDelay(2 / portTICK_PERIOD_MS);
          Serial1.flush();

          if (Serial1.available()) { Serial1.read(buff_get_trash, 256); }
          digitalWrite(uart_rts, LOW);

          vTaskDelay(4 / portTICK_PERIOD_MS); 

          if (Serial1.available() >= 7)
          {
            Serial1.read(buff_get, 8);
            uint8_t crc = 0;
            for (uint8_t ii = 0; ii < 7; ii++) { crc = crc_table[crc ^ buff_get[ii]]; } // посчитать CRC

            if (crc == buff_get[7])
            { 
              pak += 1; 

              if (autodetect_etap>0)  // если автоопределения блоков
              {                
                switch (autodetect_etap)
                {
                  case 1:
                  {
                          if ((buff_get[1]==0x02))  // если был ответ на "кто на линии"
                          {
                              autodetect_repeater=0;
                              autodetect_lognum=buff_get[0];
                              autodetect_etap=2;
                          }
                          
                  } break;
                  case 2:
                  {
                          if ((buff_get[0]==autodetect_lognum)&(buff_get[1]==0x08))  // назначить новый номер
                          {
                              autodetect_repeater=0;
                              //autodetect_block++;
                              autodetect_etap=3;
                          }
                  } break;
                  case 3:
                  {
                          if ((buff_get[0]==autodetect_block)&(buff_get[1]==0x07))  // не отвечать на общие запросы
                          {
                              autodetect_repeater=0;
                              //autodetect_block++;
                              autodetect_etap=4;
                          }
                  } break;
                  case 4:
                  {
                          if ((buff_get[0]==autodetect_block)&(buff_get[1]==0x06))  // включить следующий
                          {
                           //  digitalWrite(Rele, LOW);
                              autodetect_repeater=0;
                              autodetect_block++;
                              autodetect_etap=1;
                          }
                  } break;
                  default: break;
                }
              }

              if (autoup_etap>0)  // если потерялся блок надо его восстановить
              {                
                switch (autoup_etap)
                {
                  case 1:
                  {
                          if ((buff_get[1]==0x02))  // если был ответ на "кто на линии"
                          {
                              autoup_repeat=0;
                              autodetect_lognum=buff_get[0];
                              autoup_etap=2;
                          }
                  } break;
                  case 2:
                  {
                          if ((buff_get[0]==autodetect_lognum)&(buff_get[1]==0x07))  // не отвечать на общие запросы
                          {
                              autoup_repeat=0;
                              //autodetect_block++;
                              autoup_etap=3;
                          }
                  } break;
                  case 3:
                  {
                          if ((buff_get[0]==autodetect_lognum)&(buff_get[1]==0x06))  // включить следующий
                          {
                          //  digitalWrite(Rele, LOW);
                              autoup_repeat=0;
                              autoup_etap=1;
                          }
                  } break;
                  default: break;
                }
              }

              if ((autoup_etap==0)&&(autodetect_etap==0))  // если нет автоопределения и восстановления
              {
                  xQueueSend(RS485_stack, &buff_get, 0); // portMAX_DELAY
                  block_offline[Datchik_counter]=0; // обнулить счеткий неответа блока
              }

            }
            else
            {
              pak_err += 1;
              if (block_offline[Datchik_counter]<NO_ANSWER_SERCH) block_offline[Datchik_counter]++; // нет ответа или приняли с ошибкой от блока, добавить счетчик
              if (block_offline[Datchik_counter]==NO_ANSWER) // а если небыло ответа 5 раз то отослать команду потери связи вместо ответа 
              {
                buff_send[0]=Datchik_counter;
                buff_send[1]=0x03;
                buff_send[2]=0x0F; // это и есть ответ что небыло ответа
                buff_send[7] = 0;
                for (uint8_t ii = 0; ii < 7; ii++) { buff_send[7] = crc_table[buff_send[7] ^ buff_send[ii]]; } 
                xQueueSend(RS485_stack, &buff_send, 0); // portMAX_DELAY
              }

            }
          }
          else
          {
            Serial1.read(buff_get_trash, 256);
            pak_null += 1;

            if (block_offline[Datchik_counter]<NO_ANSWER_SERCH) block_offline[Datchik_counter]++; // нет ответа от блока, добавить счетчик
            if (block_offline[Datchik_counter]==NO_ANSWER) // а если небыло ответа 5 раз то отослать команду потери связи вместо ответа 
            {
                buff_send[0]=Datchik_counter;
                buff_send[1]=0x03;
                buff_send[2]=0x0F;
                buff_send[7] = 0;
                for (uint8_t ii = 0; ii < 7; ii++) { buff_send[7] = crc_table[buff_send[7] ^ buff_send[ii]]; } 
                xQueueSend(RS485_stack, &buff_send, 0); // portMAX_DELAY
            }

            if ((block_offline[Datchik_counter]>=NO_ANSWER_SERCH)&(autodetect_etap==0)&(autoup_etap==0)) // а если небыло ответа и надо начать искать датчики
            {
                for(uint16_t i=0; i<255; i++)  // 
                {
                   if (block_offline[i]>NO_ANSWER) block_offline[i]=NO_ANSWER+1;
                }
                autoup_etap=1; // начать поиск откл датчиков
            }

          }

          if (Sender_flag==1) Sender_flag=2;
    }
    else
    {
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    unsigned long now_time = esp_timer_get_time();
    if ((now_time - old_time) >= 1000000)
    {
      rs485_status.pak = pak;
      rs485_status.pak_err = pak_err;
      rs485_status.pak_null = pak_null;
      rs485_status.time = (now_time - old_time);
      rs485_status.Nums_of_datchik=Nums_of_datchik;

      old_time = now_time;
      pak = 0;
      pak_err = 0;
      pak_null = 0;
    }

    xQueueOverwrite(RS485_flags, &rs485_status);
    // vTaskDelay(2 / portTICK_PERIOD_MS);
    // считаем по очереди датчики
    Datchik_counter++;
    if (Datchik_counter>(Nums_of_datchik)) Datchik_counter=1; 
  }
  vTaskDelete(NULL);
}

void Button_Task(void *pvParameters)
{
  Button_struct button_status, old_button_status;
  uint32_t button_timer[3];

  for (uint8_t i = 0; i <= 2; i++)
  {
    button_status.Button_Pressed[i] = 0;
  }
  while (1)
  {
    if (digitalRead(Button_minus) == 0)
      button_timer[0] += 1;
    else
    {
      button_timer[0] = 0;
      button_status.Button_Pressed[0] = 0;
    }

    if (digitalRead(Button_menu) == 0)
      button_timer[1] += 1;
    else
    {
      button_timer[1] = 0;
      button_status.Button_Pressed[1] = 0;
    }

    if (digitalRead(Button_plus) == 0)
      button_timer[2] += 1;
    else
    {
      button_timer[2] = 0;
      button_status.Button_Pressed[2] = 0;
    }

    for (uint8_t i = 0; i <= 2; i++)
    {
      if (button_timer[i] == 2)
        button_status.Button_Pressed[i] = 1;
      if (button_timer[i] == 99)
        button_status.Button_Pressed[i] = 0;
      if (button_timer[i] >= 100)
      {
        button_status.Button_Pressed[i] = 2;
        button_timer[i] = 98;
      }
    }

    if ((button_status.Button_Pressed[0] != old_button_status.Button_Pressed[0]) |
        (button_status.Button_Pressed[1] != old_button_status.Button_Pressed[1]) |
        (button_status.Button_Pressed[2] != old_button_status.Button_Pressed[2]))
    {
      old_button_status.Button_Pressed[0] = button_status.Button_Pressed[0];
      old_button_status.Button_Pressed[1] = button_status.Button_Pressed[1];
      old_button_status.Button_Pressed[2] = button_status.Button_Pressed[2];

      if (uxQueueMessagesWaiting(Button_stack) < 5)
        xQueueSend(Button_stack, &button_status, portMAX_DELAY); // portMAX_DELAY
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void Flash_Task(void *pvParameters)
{
  byte SPI_buffer[255];
  byte SPI_buffer2[255];
  byte SPI_buffer_out[9];
  byte oper = 0;

  ArhivBase Input;
  ArhivBase Output;

  while (1)
  {
    if (xQueueReceive(Flash_input, &Input, 0) == pdPASS) // portMAX_DELAY
    {
      oper = Input.command_to_flash;

      if (oper == 1) // Сброс чипа
      {
        SPI_buffer[0] = 0x66;
        spi_dev.write(SPI_buffer, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        SPI_buffer[0] = 0x99;
        spi_dev.write(SPI_buffer, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }

      if (oper == 2) // Стереть всю флжшку
      {
        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x06;
        spi_dev.write(SPI_buffer, 1);

        SPI_buffer[0] = 0xC7;
        spi_dev.write(SPI_buffer, 1);

        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x04;
        spi_dev.write(SPI_buffer, 1);
      }

      if (oper == 3) // стереть начальную зону
      {
        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x06;
        spi_dev.write(SPI_buffer, 1);

        SPI_buffer[0] = 0x20;
        SPI_buffer[1] = 0x0;
        SPI_buffer[2] = 0x0;
        SPI_buffer[3] = 0x0;
        spi_dev.write(SPI_buffer, 4);

        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x04;
        spi_dev.write(SPI_buffer, 1);
      }

      if (oper == 4) // записать по номеру
      {
        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x06;
        spi_dev.write(SPI_buffer, 1);

        uint32_t addres;
        addres = 4096 + (Input.nomer - 1) * 11; // адрес куда записывать

        SPI_buffer[0] = 0x02;
        SPI_buffer[1] = (addres >> 16) & 0xFF;
        SPI_buffer[2] = (addres >> 8) & 0xFF;
        SPI_buffer[3] = (addres) & 0xFF;

        SPI_buffer[4] = (Input.nomer>>16) & 0xFF;  //1
        SPI_buffer[5] = (Input.nomer>>8) & 0xFF;
        SPI_buffer[6] = (Input.nomer) & 0xFF;       

        SPI_buffer[7] = Input.datchik;

        SPI_buffer[8] = Input.date[0];
        SPI_buffer[9] = Input.date[1];
        SPI_buffer[10] = Input.date[2];

        SPI_buffer[11] = Input.time[0];
        SPI_buffer[12] = Input.time[1];
        SPI_buffer[13] = Input.time[2];

        SPI_buffer[14] = Input.message;   // 11 байт

        spi_dev.write(SPI_buffer, 15);

        do
        {
          vTaskDelay(5 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        SPI_buffer[0] = 0x04;
        spi_dev.write(SPI_buffer, 1);
      }

      if (oper == 5) // чтение по номеру
      {
        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        uint32_t addres;
        addres = 4096 + (Input.nomer - 1) * 11; // адрес куда записывать

        SPI_buffer[0] = 0x03;
        SPI_buffer[1] = (addres >> 16) & 0xFF;
        SPI_buffer[2] = (addres >> 8) & 0xFF;
        SPI_buffer[3] = (addres) & 0xFF;

        spi_dev.write_then_read(SPI_buffer, 4, SPI_buffer_out, 11);

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_buffer, 1, SPI_buffer2, 1);
        } while (SPI_buffer2[0] != 0);

        Output.nomer = (((uint32_t)SPI_buffer_out[0])>>16) | (((uint32_t)SPI_buffer_out[1])>>8) | (((uint32_t)SPI_buffer_out[2]));

        Output.datchik = SPI_buffer_out[3];

        Output.date[0] = SPI_buffer_out[4];
        Output.date[1] = SPI_buffer_out[5];
        Output.date[2] = SPI_buffer_out[6];

        Output.time[0] = SPI_buffer_out[7];
        Output.time[1] = SPI_buffer_out[8];
        Output.time[2] = SPI_buffer_out[9];
        Output.message = SPI_buffer_out[10];

        xQueueSend(Flash_output, &Output, portMAX_DELAY);
      }
    }
    else
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
  vTaskDelete(NULL);
}

// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

float bar_x, bar_delta;

struct ArhivBase Arhiv[21]; // архив который надо показать на экране
struct ArhivBase Arhiv_now; // содытие которые сейчас
uint8_t Arhiv_last = 0;     // куда пишется запись 0 - 19

Button_struct button_status;
int key_count = 0;

char lcd_buf[25];
char lcd_names[25];

int flipFlag=0;

void ip_num_edit(uint8_t *num_num, String WWr, String WWnext)
{
          u8g2.clearBuffer();
          u8g2.setCursor(0,13);
          u8g2.print("Accepted!"); //"Enter device IP:");
          u8g2.sendBuffer();

          uint8_t num_of_ip=0;

          while(num_of_ip==0)
          {
                num_of_ip=1;
                if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
                {
                    num_of_ip=0;
                }

                vTaskDelay(10);
          }
          num_of_ip=0;  

          u8g2.clearBuffer();

          u8g2.setDrawColor(1);
          u8g2.setFont(u8g2_font_5x7_mr);
          u8g2.setCursor(0,21);
          u8g2.print(WWnext);
          u8g2.setFont(u8g2_font_8x13_mr);
          u8g2.setCursor(0,13);
          u8g2.print(WWr); //"Enter device IP:");
          u8g2.setCursor(0, 39);
          
          for (int xx=0; xx<4; xx++)
          {
            if (num_num[xx]<100) u8g2.print("0");
            if (num_num[xx]<10) u8g2.print("0");
            u8g2.print(num_num[xx]);  
            if (xx<3) u8g2.print(".");
          }
          // опустощить буффер кнопок
          for (int i=0; i<=20; i++)
          {  
            xQueueReceive(Button_stack, &button_status, 0);
          }
          int Exit1_flag=0;
          while(Exit1_flag==0)
          {
          // чтение кнопок
            button_status.Button_Pressed[0] = 0;
            button_status.Button_Pressed[1] = 0;
            button_status.Button_Pressed[2] = 0;

            if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
            {
              if (button_status.Button_Pressed[1] == 1) 
              {
                  if (num_of_ip<4) num_of_ip++;
                  if (num_of_ip>=4) num_of_ip=0;
                  sound(1);
              }
              if (button_status.Button_Pressed[1] == 2) 
              {
                  sound(3);
                  Exit1_flag=1;
              }

              if (button_status.Button_Pressed[2] == 1)
                if (num_num[num_of_ip]<255) { num_num[num_of_ip]++; sound(1); }
              if (button_status.Button_Pressed[0] == 1)
                if (num_num[num_of_ip]>0) { num_num[num_of_ip]--; sound(1); }

              if (button_status.Button_Pressed[2] == 2)
                if (num_num[num_of_ip]<252) { num_num[num_of_ip]+=3; sound(1); }
              if (button_status.Button_Pressed[0] == 2)
                if (num_num[num_of_ip]>3) { num_num[num_of_ip]-=3;; sound(1); }
            };

            u8g2.setCursor(0, 39);
            
            for (uint8_t xx=0; xx<4; xx++)
            {
              if (num_num[xx]< 100) u8g2.print("0");
              if (num_num[xx]<10) u8g2.print("0");
              u8g2.print(num_num[xx]);  
              if (xx<3) u8g2.print(".");
            }

            u8g2.setDrawColor(0);
            u8g2.drawHLine(0,41,127);
            u8g2.setDrawColor(1);
            u8g2.drawHLine(num_of_ip*32,41,24);

            u8g2.sendBuffer();
          }
}
     
void server_num_edit(uint32_t &num_num, String WWr)
{
          u8g2.clearBuffer();
          u8g2.setCursor(0,13);
          u8g2.print("Accepted!"); //"Enter device IP:");
          u8g2.sendBuffer();
key_count=10;
uint8_t Blinker_k=0;

          uint8_t num_of_ip=0;
          while(num_of_ip==0)
          {
                num_of_ip=1;
                if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
                {
                    num_of_ip=0;
                }
                vTaskDelay(10);
          }
          num_of_ip=0;  


          u8g2.clearBuffer();

          u8g2.setDrawColor(1);
          u8g2.setFont(u8g2_font_8x13_mr);
          u8g2.setCursor(0,13);
          u8g2.print(WWr); //"Enter device IP:");
          u8g2.setCursor(43, 39);

          num_num=num_num+100000;
          itoa(num_num, lcd_buf, 10);
          for(uint8_t i=0; i<7; i++)
          {
            lcd_buf[i]=lcd_buf[i+1];
          }

          u8g2.drawStr(63-20, 30, lcd_buf);
      
          int ssid_position=0;

          // опустощить буффер кнопок
          for (int i=0; i<=20; i++)
          {  
            xQueueReceive(Button_stack, &button_status, 0);
          }
          int Exit1_flag=0;
          while(Exit1_flag==0)
          {
          // чтение кнопок
            button_status.Button_Pressed[0] = 0;
            button_status.Button_Pressed[1] = 0;
            button_status.Button_Pressed[2] = 0;

            if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
            {
              if (button_status.Button_Pressed[1] == 1) 
              {
                if (key_count==10)
                {
                  if (atoi(lcd_buf)<=65535) 
                  {
                    sound(3);
                    Exit1_flag=1;
                  }
                  else sound(2);
                }
                else
                {
                    sound(1);
                    lcd_buf[ssid_position]=key_count+48;
                    ssid_position++;
                    if (ssid_position>=5) ssid_position=0;
                }
              }
              if (button_status.Button_Pressed[2] >= 1)
                if (key_count<10) { key_count++; sound(1); }
              if (button_status.Button_Pressed[0] >= 1)
                if (key_count>0) { key_count--; sound(1); }

            };

            u8g2.drawStr(44 ,30, lcd_buf);

            u8g2.setDrawColor(0);
            u8g2.drawHLine(0,31,127);
            u8g2.setDrawColor(1);
            if (Blinker_k>6) u8g2.drawHLine(ssid_position*8+44,31,8);

            if (key_count==9) 
            {
              u8g2.setDrawColor(0);
              u8g2.drawFrame(66,42,35,15);             
              u8g2.setDrawColor(1);
            }

            for (int ii=0; ii<15; ii++)
            {
              int CharNomerok=0;
              CharNomerok=key_count+ii+48-7;
              if (((key_count+ii)<7) | ((key_count+ii)>16) )CharNomerok=32;

              if (ii<7)  u8g2.drawGlyph(ii*8 , 54, CharNomerok);
              if (ii==7) u8g2.drawGlyph(ii*8+2 , 54, CharNomerok);
              if (ii>7) u8g2.drawGlyph(ii*8+4, 54, CharNomerok);
            }
            Blinker_k++;
            if (Blinker_k>12) Blinker_k=0;
              u8g2.setDrawColor(1);
              u8g2.drawFrame(57,42,11,15);

            if (key_count==10) 
            {
              u8g2.drawStr(68 , 54, "Next");
              u8g2.setDrawColor(0);
              u8g2.drawFrame(57,42,11,15);
              u8g2.setDrawColor(1);
              u8g2.drawFrame(66,42,35,15);
            }
            u8g2.sendBuffer();
          }
          num_num=atoi(lcd_buf);
}

void Names_edit(char *num_num, String WWr)
{
          u8g2.setFont(u8g2_font_8x13_mr);  
          u8g2.clearBuffer();
          u8g2.setDrawColor(1);
        
          u8g2.setCursor(0,13);
          u8g2.print(WWr);

          key_count=94;

          int ssid_position=0;
          for (int i=0; i<25; i++)
          {
            if ((num_num[i]!=0)&(num_num[i]!=32)) 
            {
                lcd_names[i] = num_num[i];
                ssid_position++;
            }
            else lcd_names[i] = 0;
          }

          // опустощить буффер кнопок
          for (int i=0; i<=20; i++)
          {  
            xQueueReceive(Button_stack, &button_status, 0);
          }
          int Exit1_flag=0;
          while(Exit1_flag==0)
          {
          // чтение кнопок
            button_status.Button_Pressed[0] = 0;
            button_status.Button_Pressed[1] = 0;
            button_status.Button_Pressed[2] = 0;

            if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
            {
              if (button_status.Button_Pressed[1] == 1) 
              {
                if ( ((ssid_position>=25)&(key_count!=0))| ((ssid_position<=0)&(key_count==0))  ) sound(2);
                else sound(1);

                if ( (key_count<94)&(key_count>0)&(ssid_position<25) )
                {
                    lcd_names[ssid_position]=key_count+32;
                    ssid_position++;
                }

                if  ((key_count==0)&(ssid_position>0)) 
                {
                    ssid_position--;
                    lcd_names[ssid_position]=0;   
                }
                if  ((key_count==94)&(ssid_position>0)) 
                {
                    Exit1_flag=1;  
                }
              }

              if (button_status.Button_Pressed[2] == 1)
                if (key_count<94) { key_count++; sound(1); }
              if (button_status.Button_Pressed[0] == 1)
                if (key_count>0) { key_count--; sound(1); }

              if (button_status.Button_Pressed[2] == 2)
                if (key_count<=91) { key_count+=3; sound(1); }
                else key_count=94;
              if (button_status.Button_Pressed[0] == 2)
                if (key_count>3) { key_count-=3; sound(1); }
                else key_count=0;
            };

            u8g2.setFont(u8g2_font_5x7_mr);
            u8g2.drawStr(0 ,26, "                         ");
            u8g2.drawStr(0 ,26, lcd_names);

            if (ssid_position<=24)
            {
              flipFlag++;
              if (flipFlag>10)  u8g2.drawStr(ssid_position*5 ,26, "_"); 
              if (flipFlag>20) flipFlag=0;
            }

            u8g2.setFont(u8g2_font_8x13_mr);

            if (key_count==1) 
            {
              u8g2.setDrawColor(0);
              u8g2.drawFrame(6,36,51,15);
              u8g2.setDrawColor(1);
              u8g2.drawFrame(57,36,11,15);
            }
            if (key_count==93) 
            {
              u8g2.setDrawColor(0);
              u8g2.drawFrame(66,36,35,15);
              u8g2.setDrawColor(1);
              u8g2.drawFrame(57,36,11,15);
            }

            for (int ii=0; ii<15; ii++)
            {
              int CharNomerok=0;
              CharNomerok=key_count+ii+32-7;
              if (((key_count+ii)<7) | ((key_count+ii)>100) )CharNomerok=32;

              if (ii<7) u8g2.drawGlyph(ii*8 , 48, CharNomerok);
              if (ii==7) u8g2.drawGlyph(ii*8+2 , 48, CharNomerok);
              if (ii>7) u8g2.drawGlyph(ii*8+4, 48, CharNomerok);
            }

            if (key_count==0) 
            {
              u8g2.drawStr(8, 48, "Delete");
              u8g2.setDrawColor(0);
              u8g2.drawFrame(57,36,11,15);
              u8g2.setDrawColor(1);
              u8g2.drawFrame(6,36,51,15);
            }

            if (key_count==94) 
            {
              u8g2.drawStr(68 , 48, "Next");
              u8g2.setDrawColor(0);
              u8g2.drawFrame(57,36,11,15);
              u8g2.setDrawColor(1);
              u8g2.drawFrame(66,36,35,15);
            }
            u8g2.sendBuffer();
          }

          // int ssid_position=0;
          for (int i=0; i<25; i++)
          {
            if ((lcd_names[i]!=0)&(lcd_names[i]!=32)) 
            {
                num_num[i]=lcd_names[i];
               // ssid_position++;
            }
            else num_num[i] = 0;
          }
}

void Net_edit(uint8_t &type_con)
{
          u8g2.setFont(u8g2_font_8x13_mr);
          key_count=1;

          // опустощить буффер кнопок
          for (int i=0; i<=20; i++)
          {  
            xQueueReceive(Button_stack, &button_status, 0);
          }

          int Exit1_flag=0;
          while(Exit1_flag==0)
          {

          u8g2.clearBuffer();
          u8g2.setDrawColor(1);
          u8g2.setCursor(0,13);
          u8g2.print("What do you use?");

          u8g2.setCursor(0,37);
          u8g2.print(" None WiFi LAN  ");      

          // чтение кнопок
            button_status.Button_Pressed[0] = 0;
            button_status.Button_Pressed[1] = 0;
            button_status.Button_Pressed[2] = 0;

            if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
            {
              if (button_status.Button_Pressed[1] == 1) 
              {
                    Exit1_flag=1; 
                    sound(3);
              }

              if (button_status.Button_Pressed[2] == 1)
                if (key_count<2) { key_count++; sound(1); }
              if (button_status.Button_Pressed[0] == 1)
                if (key_count>0) { key_count--; sound(1); }
            };

            if (key_count==0) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(4,25,40,15);
                u8g2.setCursor(0,53);
                u8g2.print("No connected");
            }
            if (key_count==1) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(44,25,40,15);
                u8g2.setCursor(0,53);
                u8g2.print("Conn. via WiFi");
            }

            if (key_count==2) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(84,25,40,15);
                u8g2.setCursor(0,53);
                u8g2.print("Conn. via LAN");
            }
            u8g2.sendBuffer();
          }

          type_con=key_count; // Type_connection=
}

void Type_edit(uint8_t &type_con)
{
          u8g2.setFont(u8g2_font_8x13_mr);
          key_count=1;

          // опустощить буффер кнопок
          for (int i=0; i<=20; i++)
          {  
            xQueueReceive(Button_stack, &button_status, 0);
          }

          int Exit1_flag=0;
          while(Exit1_flag==0)
          {

          u8g2.clearBuffer();
          u8g2.setDrawColor(1);
          u8g2.setCursor(0,13);
          u8g2.print("What do you use?");

          u8g2.setCursor(0,37);
          u8g2.print(" MQTT Web T.Bot ");      

          // чтение кнопок
            button_status.Button_Pressed[0] = 0;
            button_status.Button_Pressed[1] = 0;
            button_status.Button_Pressed[2] = 0;

            if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
            {
              if (button_status.Button_Pressed[1] == 1) 
              {
                    Exit1_flag=1; 
                    sound(3);
              }

              if (button_status.Button_Pressed[2] == 1)
                if (key_count<2) { key_count++; sound(1); }
              if (button_status.Button_Pressed[0] == 1)
                if (key_count>0) { key_count--; sound(1); }
            };

            if (key_count==0) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(4,25,40,15);
                u8g2.setCursor(0,53);
                u8g2.print("MQTT publisher");
            }
            if (key_count==1) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(44,25,32,15);
                u8g2.setCursor(0,53);
                u8g2.print("Web Server");
            }

            if (key_count==2) 
            {
                u8g2.setDrawColor(1);
                u8g2.drawFrame(76,25,48,15);
                u8g2.setCursor(0,53);
                u8g2.print("Telegram Bot");
            }
            u8g2.sendBuffer();
          }

          type_con=key_count; 

}


void ReadFlashMain()
{
    byte SPI_main_buffer[80];
    byte SPI_buffer_tresh[255];
    byte SPI_wr_buffer[255];
/*
25 char *ssid = "                         ";       SSID 
25 char *password = "                       ";  Password
4 uint8_t num_ip[4]={192,168,0,25};
4 uint8_t num_geteway[4]={192,168,0,1};
4 uint8_t num_subnet[4]={255,255,255,0};
4 uint8_t num_primaryDNS[4]={8,8,8,8};
4 uint8_t num_secondaryDNS[4]={8,8,4,4};
2 uint32_t num_server=80;
1 uint8_t Type_connection=1;  // 0 - mqtt 1 - web 2 - telegram bot
1 NET_use=1; // 1 - wifi 2 - lan 
*/

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0] != 0);

        uint32_t addres;
        addres = 0; // адрес откуда читать

        SPI_wr_buffer[0] = 0x03;
        SPI_wr_buffer[1] = (addres >> 16) & 0xFF;
        SPI_wr_buffer[2] = (addres >> 8) & 0xFF;
        SPI_wr_buffer[3] = (addres) & 0xFF;

        spi_dev.write_then_read(SPI_wr_buffer, 4, SPI_main_buffer, 75);

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0] != 0);

        for (uint8_t i=0; i<25; i++)
        {
          ssid[i]=SPI_main_buffer[i];
          password[i]=SPI_main_buffer[i+25];
        }

        for (uint8_t i=0; i<4; i++)
        {
            num_ip[i]=SPI_main_buffer[i+50];
            
            num_geteway[i]=SPI_main_buffer[i+54];
            num_subnet[i]=SPI_main_buffer[i+58];
            num_primaryDNS[i]=SPI_main_buffer[i+62];
            num_secondaryDNS[i]=SPI_main_buffer[i+66];
        }
        num_server=SPI_main_buffer[71];
        num_server=(num_server<<8) | (SPI_main_buffer[70]);

        Type_connection=SPI_main_buffer[72];
        NET_use=SPI_main_buffer[73];
        Datchik_nums=SPI_main_buffer[74];

}

void SaveFlashMain()
{
byte SPI_main_buffer[80];
byte SPI_buffer_tresh[255];
byte SPI_wr_buffer[255];

// стереть часть флэшки
        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0]!=0);

        SPI_main_buffer[0] = 0x06;
        spi_dev.write(SPI_main_buffer, 1);

        SPI_main_buffer[0] = 0x20;
        SPI_main_buffer[1] = 0x0;
        SPI_main_buffer[2] = 0x0;
        SPI_main_buffer[3] = 0x0;
        spi_dev.write(SPI_main_buffer, 4);

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0]!=0);

        SPI_main_buffer[0] = 0x04;
        spi_dev.write(SPI_main_buffer, 1);

// запись флжшки
        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0]!=0);

        SPI_main_buffer[0]=0x06;

        spi_dev.write(SPI_main_buffer, 1);

        uint32_t addres;
        addres = 0; // адрес куда записывать

        SPI_main_buffer[0] = 0x02;
        SPI_main_buffer[1] = (addres >> 16) & 0xFF;
        SPI_main_buffer[2] = (addres >> 8) & 0xFF;
        SPI_main_buffer[3] = (addres) & 0xFF;

        for (uint8_t i=0; i<25; i++)
        {
          SPI_main_buffer[i+4]= ssid[i];
          SPI_main_buffer[i+25+4]=password[i];
        }

        for (uint8_t i=0; i<4; i++)
        {
            SPI_main_buffer[i+50+4]=num_ip[i];
            SPI_main_buffer[i+54+4]=num_geteway[i];
            SPI_main_buffer[i+58+4]=num_subnet[i];
            SPI_main_buffer[i+62+4]=num_primaryDNS[i];
            SPI_main_buffer[i+66+4]=num_secondaryDNS[i];
        }
        SPI_main_buffer[74]=num_server&0xFF;
        SPI_main_buffer[75]=(num_server&0xFF00)>>8;

        SPI_main_buffer[76]=Type_connection;
        SPI_main_buffer[77]=NET_use;
        SPI_main_buffer[78]=Datchik_nums;

        spi_dev.write(SPI_main_buffer, 79);

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0] != 0);

        SPI_wr_buffer[0] = 0x04;
        spi_dev.write(SPI_wr_buffer, 1);
}

void EraseFlashMain()
{
        byte SPI_main_buffer[80];
        byte SPI_buffer_tresh[255];
        byte SPI_wr_buffer[255];

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0] != 0);

        SPI_main_buffer[0] = 0x06;
        spi_dev.write(SPI_main_buffer, 1);

        SPI_main_buffer[0] = 0xC7;
        spi_dev.write(SPI_main_buffer, 1);

        do
        {
          vTaskDelay(1 / portTICK_PERIOD_MS);
          SPI_wr_buffer[0] = 0x05;
          spi_dev.write_then_read(SPI_wr_buffer, 1, SPI_buffer_tresh, 1);
        } while (SPI_buffer_tresh[0] != 0);

        SPI_main_buffer[0] = 0x04;
        spi_dev.write(SPI_main_buffer, 1);

        SaveFlashMain(); // вкрнуть начальный заголовок
      
}

// обработчик сообщений
void newMsg(FB_msg& msg) {
  // выводим ID чата, имя юзера и текст сообщения
  
  if ((msg.text=="/help") || (msg.text=="/menu"))
  {
      String Otvet = "Привет "+ msg.username+"!";
      bot.sendMessage(Otvet, msg.chatID);  
     // Otvet = "/arhiv - для вывода архива";
     // bot.sendMessage(Otvet, msg.chatID);  
      Otvet = "/consist - состояние всего";
      bot.sendMessage(Otvet, msg.chatID);  
      Otvet = "/releon - для включения реле";
      bot.sendMessage(Otvet, msg.chatID);  
      Otvet = "/releoff - для выключения реле";
      bot.sendMessage(Otvet, msg.chatID); 
      Otvet = "/test - проверка связи";
      bot.sendMessage(Otvet, msg.chatID); 
  }

  if (msg.text=="/test")
  {
    sound(3);
    BotAnswer="Все хорошо, на линии "+(String)Datchik_nums+" датчиков, скорость обмена "+ (String) Speed_ans + " пак/с, ошибок "+ String(Speed_err)+" пак/с";
    bot.sendMessage(BotAnswer, ID_CHAT);  
  }

  if (msg.text=="/consist")
  {

    BotAnswer="";
    for (uint8_t izz=1; izz<=Datchik_nums; izz++)
    {
        BotAnswer=BotAnswer+"Блок: "+(String)izz;
        BotAnswer=BotAnswer+" -> ";
        switch (DatchikBase[izz])
        {
        case 0 :
          {
            BotAnswer=BotAnswer+"норма ";
          } break;
        case 1 :
          {
            BotAnswer=BotAnswer+"тревога ";
          } break;   
        case 2 :
          {
            BotAnswer=BotAnswer+"перекус ";
          } break;    
        case 3 :
          {
            BotAnswer=BotAnswer+"переполнение ";
          } break; 
        case 4 :
          {
            BotAnswer=BotAnswer+"открыта крышка ";
          } break; 
        case 5 :
          {
            BotAnswer=BotAnswer+"автонастройка ";
          } break;  
        case 0x0F :
          {
            BotAnswer=BotAnswer+"потеря связи ";
          } break;  
        default: 
          { 
            BotAnswer=BotAnswer+"неизвестно ";
          }
          break;
        }
        BotAnswer=BotAnswer+"\n";
    }
    BotAnswer=BotAnswer+"Cкорость обмена "+ (String) Speed_ans + " пак/с, ошибок "+ String(Speed_err)+" пак/с";
    bot.sendMessage(BotAnswer, ID_CHAT);  
  }
  if (msg.text=="/arhiv")
  {
     BotAnswer="Архив пока не выводится";
     bot.sendMessage(BotAnswer, ID_CHAT);  
  }
}

uint8_t buff_get[8];
byte buff_in[8]; // посылать в таск команды

void setup()
{
  pinMode(Led, OUTPUT);
  pinMode(Rele, OUTPUT);

  ledcAttachPin(zoomer, 0);

  pinMode(uart_rts, OUTPUT);

  pinMode(Button_menu, INPUT_PULLUP);
  pinMode(Button_minus, INPUT_PULLUP);
  pinMode(Button_plus, INPUT_PULLUP);

  Serial1.begin(RS485_speed, SERIAL_8N1, uart_rx, uart_tx, 1, 8);

  Serial.begin(921600);  // для отладки

  spi_dev.begin();

  // =--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-==-=--=-=-=

  RS485_in_stack = xQueueCreate(10, sizeof(uint8_t[8]));  // чтобы запихнуть команду в таск
  RS485_stack = xQueueCreate(400, sizeof(uint8_t[8]));   // полученный ответ от таска
  RS485_flags = xQueueCreate(1, sizeof(RS485_in_out)); // скорость и тд
  xTaskCreate(RS485_Task, "RS-485", 10000, NULL, 1, NULL);

  Music_stack = xQueueCreate(1, sizeof(int));
  xTaskCreate(Sound_Task, "Sound", 2000, NULL, 2, NULL);

  Button_stack = xQueueCreate(10, sizeof(Button_struct));
  xTaskCreate(Button_Task, "Buttons", 1000, NULL, 3, NULL);

    Flash_input = xQueueCreate(10, sizeof(ArhivBase));  // флэщка получает данные
    Flash_output = xQueueCreate(10, sizeof(ArhivBase)); // фдэшка отдает данные
    xTaskCreate(Flash_Task, "Flash", 5000, NULL, 4, NULL);
  
  sound(3);

  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13_mr);

  bar_x = 63;
  bar_delta = 1;
  u8g2.clearBuffer();
 
ReadFlashMain();


for (uint8_t i=0; i<255; i++)
{
  DatchikBase[i]=0;
}

vTaskDelay(1000);
        // изменить колличество блоков на линии
              buff_in[0]=0x00;
              buff_in[1]=0x81;
              buff_in[2]=0x03;  // передать количество блоков
              buff_in[3]=Datchik_nums;  // ---
              xQueueSend(RS485_in_stack, &buff_in, 0);

              

// меню выбора WAN LAN
 if (digitalRead(Button_menu) == 0)
 {
      u8g2.drawStr(44, 13, "Setup"); 
      u8g2.sendBuffer();
      Setup_enter=1;
 
      u8g2.clearBuffer();

      // ждем когда кнопки отпустят
      uint8_t exit_flag=1;
      while(exit_flag==1)
      {
          exit_flag=0;
          if ( (digitalRead(Button_menu) == 0) | (digitalRead(Button_minus) == 0) | (digitalRead(Button_plus) == 0) ) exit_flag=1;
      }
      exit_flag=0;

      Net_edit(NET_use);

// --------------------------------------
      u8g2.clearBuffer();

      if (NET_use>=1) // настраиваем
      {          
        if (NET_use==1) // настраиваем Имя сети и Пароль
        {
          u8g2.clearBuffer();
          Names_edit(ssid,"Enter SSID:");
          Names_edit(password,"Enter password:");         
        }
        u8g2.clearBuffer();
        // растраиваем IP и так далее   ---------
        ip_num_edit(num_ip,           "Enter device IP:", "Long press M for gateway");
        ip_num_edit(num_geteway,      "Enter gateaway:",  "Long press M for subnet");
        ip_num_edit(num_subnet,       "Enter subnet:",    "Long press M for pr.DNS");
        ip_num_edit(num_primaryDNS,   "Enter pr.DNS:",    "Long press M for sec.DNS");
        ip_num_edit(num_secondaryDNS, "Enter sec.DNS:",   "Long press M for port");
        server_num_edit(num_server,   "Enter port num.:" );
        Type_edit(Type_connection);
      }
  // MQTT/Server/Telegram
 // NET_use=2;

     SaveFlashMain();
  }  
          // int ssid_position=0;
          for (int i=0; i<25; i++)
          {
            if (ssid[i]==32) ssid[i]=0; 
            if (password[i]==32) password[i]=0; 

          }
  // ---------- SERVER --------------------------------------

  u8g2.clearBuffer();
  u8g2.drawStr(0, 13,  "Please wait."); 
  u8g2.sendBuffer();

  for (uint8_t i = 0; i < 20; i++) // очистить весь архив
  {
    Arhiv[i].datchik = 0; // и номер блоков 0 - нет записи
    Arhiv[i].date[0] = 0; // если год равен 0 - значит записей больше нет
  }

 
    uint32_t nomz=0;

      u8g2.setCursor(0,26);
      u8g2.print("Database reading:");
      u8g2.sendBuffer();

    while(1)
    {
 break;
 
      u8g2.setCursor(0,39);
      u8g2.print(nomz);
      u8g2.sendBuffer();


      Arhiv_now.nomer=nomz+1;
      Arhiv_now.command_to_flash=5;
      xQueueSend(Flash_input, &Arhiv_now, portMAX_DELAY);
      //vTaskDelay(100);
      while(uxQueueMessagesWaiting(Flash_output)>0)
      {
        vTaskDelay(1);
      }

      xQueueReceive(Flash_output, &Arhiv_now, portMAX_DELAY);

      if (Arhiv_now.date[0]==0xFF) break;


      for (int iiiooo=18; iiiooo>=0; iiiooo--)
      {
          Arhiv[iiiooo+1]=Arhiv[iiiooo];
      }
      Arhiv[0]=Arhiv_now;

      nomz++;

    }
    RecNumber=nomz+1;


  u8g2.clearBuffer();
  u8g2.sendBuffer();

}

char Time_str[10];


uint8_t HTML_Flag = 0;

int Years;

// 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
   
      
uint8_t first_time_setup=0;
int64_t time_exec;
int64_t old_time_exec;
uint8_t work_flag;

void loop()
{
          IPAddress local_ip(num_ip[0], num_ip[1], num_ip[2], num_ip[3]);
          IPAddress gateway(num_geteway[0], num_geteway[1], num_geteway[2], num_geteway[3]);
          IPAddress subnet(num_subnet[0], num_subnet[1], num_subnet[2], num_subnet[3]);
          IPAddress primaryDNS(num_primaryDNS[0], num_primaryDNS[1], num_primaryDNS[2], num_primaryDNS[3]);   // опционально
          IPAddress secondaryDNS(num_secondaryDNS[0], num_secondaryDNS[1], num_secondaryDNS[2], num_secondaryDNS[3]); // опционально
          WiFiServer server(num_server);

          u8g2.setFont(u8g2_font_5x7_mr);
 
 // Ксли без подключения к интернету
        if (NET_use==0)
        {
// !!!!
        }

// Если WiFi -------
        if (NET_use==1)
        {

            u8g2.drawStr(0, 7,"SSID: ");
            u8g2.drawStr(0, 14,ssid);
        
            u8g2.drawStr(0, 21,"Password: ");
            u8g2.drawStr(0, 28,password);

            // Настраиваем статический IP-адрес:
            if (!WiFi.config(local_ip, gateway, subnet, primaryDNS, secondaryDNS))
            {
              u8g2.drawStr(0, 35, "WiFi: STA Faild."); //  "Не удалось задать статический IP-адрес"
              u8g2.sendBuffer();
            }
            else
            {
              u8g2.drawStr(0, 35, "WiFi: STA Ok."); //  "Не удалось задать статический IP-адрес"
              u8g2.sendBuffer();
            }

            u8g2.drawStr(0, 42, "WiFi: Try connect"); //  "
            u8g2.sendBuffer();
            uint8_t TryCon = 0;

            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED)
            {
              u8g2.drawStr(TryCon * 5, 49, "*");
              u8g2.sendBuffer();
              TryCon++;
              if (TryCon>=16) 
              {
                u8g2.drawStr(0, 49, "                         ");
                TryCon=0;
              }
              delay(500);
            }

            u8g2.drawStr(0, 49, "WiFi connected."); //  "Не удалось задать статический IP-адрес"
            u8g2.sendBuffer();

        }
// Если LAN  -------
        if (NET_use==2)
        {
                u8g2.drawStr(0, 7, "Connecting to the network");  
                u8g2.sendBuffer();
              ESP32_W5500_onEvent();
            ETH.begin(MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, W5500_Default_Mac );

            bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress primaryDNS, IPAddress secondaryDNS);
            ETH.config(local_ip, gateway, subnet, primaryDNS);

          // ESP32_W5500_waitForConnect();
            uint8_t xxx=0;
            while (!ESP32_W5500_eth_connected)
            {
              delay(100);
                u8g2.drawStr(xxx*5, 21, "."); 
                xxx++;
                u8g2.sendBuffer();
                if (xxx>25)
                {
                  u8g2.clearBuffer();
                  u8g2.drawStr(0, 7, "Connecting to the network");   
                  xxx=0;
                }
            }
                u8g2.drawStr(0, 35, "Connected !");  
                u8g2.sendBuffer();
        }

        if  (Type_connection==2) // если телеграм
        {
            bot.setChatID(ID_CHAT);
            bot.attach(newMsg);
        }

        if  (Type_connection==1) // если WEB
        {
           server.begin();
        }
        timeClient.begin();                           // Запускаем клиент времени
        timeClient.setTimeOffset(60 * 60 * TimeZone); // Указываем смещение по времени от Гринвича. Москва GMT+3 => 60*60*3 = 10800

        uint8_t reley_status=0;  // реле выключено
        uint8_t comm_485_status=1;  // обмен 485 включен

        key_count=0;
 
uint8_t MenuLevel=1;  // страница меню

BotAnswer="Блок ключен !";
bot.sendMessage(BotAnswer, ID_CHAT);  

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  while(1)
  {  

    uint8_t ahtung_flag=0;
    ahtung_flag=0;
    for(uint8_t ddd=0; ddd<255; ddd++)
    {
      if ((DatchikBase[ddd]==1)|(DatchikBase[ddd]==3)|(DatchikBase[ddd]==4)|(DatchikBase[ddd]==0x0F)) ahtung_flag=1;
    }

    if (ahtung_flag==1) { digitalWrite(Rele, HIGH); digitalWrite(Led, HIGH); }
    else  { digitalWrite(Rele, LOW); digitalWrite(Led, LOW); }

      old_time_exec=time_exec;
      time_exec=esp_timer_get_time();

      u8g2.clearBuffer();

      if (MenuLevel==1)
      {
                                  u8g2.drawStr(2,34, "1) Search for sensors ");

      if (comm_485_status==1)     u8g2.drawStr(2,43, "2) Stop communication ");
      else                        u8g2.drawStr(2,43, "2) Start communication");

      if (reley_status==0)        u8g2.drawStr(2,52, "3) Turn on the relay  ");
      else if (reley_status==1)   u8g2.drawStr(2,52, "3) Turn off the relay ");
      else if (reley_status==2)   u8g2.drawStr(2,52, "3) On during alarm    ");
      else if (reley_status==3)   u8g2.drawStr(2,52, "3) Off during alarm   ");

                                  u8g2.drawStr(2,61, "4) Next menu          ");

                              
      }
      if (MenuLevel==2)
      {
                                  u8g2.drawStr(2,34, "1) Erase data memory  ");
                                  u8g2.drawStr(2,43, "2) Erase sensors base ");
                                  u8g2.drawStr(2,52, "3)                    ");

                                  u8g2.drawStr(2,61, "4) Main menu          ");                        
      }

      u8g2.drawBox(0,27+key_count*9,126,8);
      RS485_in_out status;

      Time_Time = timeClient.getFormattedTime();
      Time_Date = timeClient.getFormattedDate().substring(0, 10);
      // Считать скорость и количество ошибок RS485
      if (uxQueueMessagesWaiting(RS485_flags))
      {
        xQueueReceive(RS485_flags, &status, 0);
      } // portMAX_DELAY

      // вычитываем весь буффер принятого
      uint16_t forka = uxQueueMessagesWaiting(RS485_stack);
      for (uint16_t iii = 0; iii <= forka; iii++)
      {
        if (xQueueReceive(RS485_stack, &buff_get, 0) == pdPASS) // portMAX_DELAY
        {
          // тут разбор что приняли и что надо занести в базу  -----------------------------------------
          if (buff_get[1] == 0x03) // если ответ на состояние
          {
            if (DatchikBase[buff_get[0]] != buff_get[2])
            {
              //if ((DatchikBase[buff_get[0]]==0x0F)&(buff_get[2]==0x00))  // чтбы иписало что связь восстановлена
              DatchikBase[buff_get[0]] = buff_get[2];
              RecNumber += 1;
              // подготовить строку сообщений о тревоге или не тревоге
              Arhiv_now.nomer = RecNumber-1;
              Arhiv_now.datchik = buff_get[0];

              Arhiv_now.date[0] = (Time_Date.substring(0, 4)).toInt() - 2000;
              Arhiv_now.date[1] = (Time_Date.substring(5, 7)).toInt();
              Arhiv_now.date[2] = (Time_Date.substring(8, 10)).toInt();

              Arhiv_now.time[0] = (Time_Time.substring(0, 2)).toInt();
              Arhiv_now.time[1] = (Time_Time.substring(3, 5)).toInt();
              Arhiv_now.time[2] = (Time_Time.substring(6, 8)).toInt();
              Arhiv_now.message = buff_get[2];

              Arhiv_now.command_to_flash=4; // записать

              xQueueSend(Flash_input, &Arhiv_now, portMAX_DELAY); // записать во flash

              if (Arhiv_last > 0)
                Arhiv_last--;
              else
                Arhiv_last = 19; // записываем циклично

              Arhiv[Arhiv_last] = Arhiv_now;

              if (Type_connection==1)
              {
                  uint8_t display_counter = 0;
                  uint8_t display_pointer = Arhiv_last;
                  String ArhivDatas_new = "";
                  ArhivDatas = "";
                  while (display_counter <= 19)
                  {
                    if (Arhiv[display_pointer].date[0] > 0)
                    {

                      ArhivDatas_new = "<span style=\"color: black;\"> №" + (String)Arhiv[display_pointer].nomer + " (Блок " + (String)Arhiv[display_pointer].datchik + ") [20" +
                                      (String)Arhiv[display_pointer].date[0] + "/" + (String)Arhiv[display_pointer].date[1] + "/" + (String)Arhiv[display_pointer].date[2] + " " +
                                      (String)Arhiv[display_pointer].time[0] + ":" + (String)Arhiv[display_pointer].time[1] + ":" + (String)Arhiv[display_pointer].time[2] + "] ";
                      switch (Arhiv[display_pointer].message)
                      {
                      case 0:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(0, 0, 0);\"> Норма <br>";
                      }
                      break;
                      case 1:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(255, 0, 0);\"> Тревога удар<br>";
                      }
                      break;
                      case 2:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(100, 0, 0);\"> Внимание, пререкус<br>";
                      }
                      break;
                      case 3:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(255, 0, 0);\">Переполнение<br>";
                      }
                      break;
                      case 4:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(255, 127, 0);\">Крышка открыта<br>";
                      }
                      break;
                      case 5:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(127, 127, 127);\">Автонастройка <br>";
                      }
                      break;

                      case 0x0F:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(0, 255, 0);\">Потеря связи<br>";
                      }
                      break;
                      default:
                      {
                        ArhivDatas_new = ArhivDatas_new + "<span style=\"color: rgb(127, 0, 127);\">Неизвестная команда " + (String)Arhiv[display_pointer].message + "<br>";
                      }
                      break;
                      }
                      ArhivDatas = ArhivDatas + ArhivDatas_new;
                    }
                    else
                      break;

                    display_pointer++;
                    if (display_pointer > 19)
                      display_pointer = 0;
                    display_counter++;
                  }
              }
              //   отослать в чатбот если это телеграм
              if (Type_connection==2)
              {
                BotAnswer="";
                      switch (Arhiv_now.message)
                      {
                            case 0:
                            {
                              BotAnswer = BotAnswer + "Норма";
                            }
                            break;
                            case 1:
                            {
                              BotAnswer = BotAnswer + "Тревога удар";
                            }
                            break;
                            case 2:
                            {
                              BotAnswer = BotAnswer + "Внимание, пререкус";
                            }
                            break;
                            case 3:
                            {
                              BotAnswer = BotAnswer + "Переполнение";
                            }
                            break;
                            case 4:
                            {
                              BotAnswer = BotAnswer + "Крышка открыта";
                            }
                            break;
                            case 5:
                            {
                              BotAnswer = BotAnswer + "Автонастройка";
                            }
                            break;

                            case 0x0F:
                            {
                              BotAnswer = BotAnswer + "Потеря связи";
                            }
                            break;
                            default:
                            {
                              BotAnswer = BotAnswer + "Неизвестная команда " + (String)Arhiv_now.message ;
                            }
                            break;
                      }
                    BotAnswer  =  BotAnswer+ " (Блок " + (String)Arhiv_now.datchik + ") [20" +
                                      (String)Arhiv_now.date[0] + "/" + (String)Arhiv_now.date[1] + "/" + (String)Arhiv_now.date[2] + " " +
                                      (String)Arhiv_now.time[0] + ":" + (String)Arhiv_now.time[1] + ":" + (String)Arhiv_now.time[2] + "] " + "№" + (String)Arhiv_now.nomer;
                  u8g2.drawStr(70, 7, "Snd"); 
                  bot.sendMessage(BotAnswer, ID_CHAT);  
              }

            }
          }
        }
      }


      // чтение кнопок
      button_status.Button_Pressed[0] = 0;
      button_status.Button_Pressed[1] = 0;
      button_status.Button_Pressed[2] = 0;

      if (xQueueReceive(Button_stack, &button_status, 0) == pdPASS)
      {

        if (button_status.Button_Pressed[1] == 1)
        {
          if (key_count==0) // актопоиск блоков
          {
            if (MenuLevel==1)  // автопоиск
            {
              Datchik_nums=0;
              sound(3);
              buff_in[0]=0x00;
              buff_in[1]=0x81;
              buff_in[2]=0x05;  // павтопоиск блоков на линии
              xQueueSend(RS485_in_stack, &buff_in, 0);
            }
            if (MenuLevel==2)  // удаление всего архива
            { 
                sound(3);

                  u8g2.clearBuffer();
                  u8g2.drawStr(0, 32, " Erasing archiv "); 
                  u8g2.sendBuffer();
                
                  EraseFlashMain();

                  RecNumber=1;
                  for (uint8_t i = 0; i < 20; i++) // очистить весь архив
                  {
                    Arhiv[i].datchik = 0; // и номер блоков 0 - нет записи
                    Arhiv[i].date[0] = 0; // если год равен 0 - значит записей больше нет
                  }

                  u8g2.clearBuffer();
                  u8g2.sendBuffer();

                sound(3);
            }
          }

          if (key_count==1) // старт и стом актопосылок
          {
            if (MenuLevel==1)  // 
            {
              sound(3);
              buff_in[0]=0x00;
              buff_in[1]=0x81;
              if (comm_485_status==1) {buff_in[2]=0x01; comm_485_status=0; } // остановить посылки
              else {buff_in[2]=0x02; comm_485_status=1; } // остановить посылки
              xQueueSend(RS485_in_stack, &buff_in, 0);
            }

            if (MenuLevel==2)  // Обнуление количества блоков на линии
            { 
              sound(3);
              Datchik_nums=0;

               // изменить колличество блоков на линии
              buff_in[0]=0x00;
              buff_in[1]=0x81;
              buff_in[2]=0x03;  // передать количество блоков
              buff_in[3]=Datchik_nums;  // ---
              xQueueSend(RS485_in_stack, &buff_in, 0);

              SaveFlashMain();
            }
          }

          if (key_count==2)  // про реле вкл выкл или усправление 
          {
              if (MenuLevel==1)
              {
                  sound(3); 
                  reley_status++;
                  if (reley_status>3) reley_status=0;
              }
              if (MenuLevel==2)
              {
                    sound(3); 
              }

          }

          if (key_count==3)  // про настройки блоков
          {
              sound(3); 
              if (MenuLevel==1) MenuLevel=2;
              else MenuLevel=1;
              key_count=0;
          }

        }

        if (button_status.Button_Pressed[2] == 1)
        {
          if (key_count<3) { key_count++; sound(1); }
          else sound(2);
        }
        if (button_status.Button_Pressed[0] == 1)
        {
          if (key_count>0) { key_count--; sound(1);}
          else sound(2);
        }
      }; // portMAX_DELAY

      u8g2.setFont(u8g2_font_5x7_mr);
      u8g2.setDrawColor(2);
      u8g2.drawHLine(0, 15, 127);

      timeClient.update(); // Обновляем дату
                          // выводим время и дату

      strcpy(lcd_buf, Time_Time.c_str());
      u8g2.drawStr(87, 7, lcd_buf);

      strcpy(lcd_buf, Time_Date.c_str());
      u8g2.drawStr(77, 14, lcd_buf);

      // выводим скорость обмена
      itoa(status.pak, lcd_buf, 10);
      u8g2.drawStr(0, 7, "[       ]");
      if ((work_flag&0x3)==0) u8g2.drawStr(20, 7, "/");
      if ((work_flag&0x3)==1) u8g2.drawStr(20, 7, "-");
      if ((work_flag&0x3)==2) u8g2.drawStr(20, 7, "\\");
      if ((work_flag&0x3)==3) u8g2.drawStr(20, 7, "|");
      work_flag++;

      if (Type_connection==2)
      {
        u8g2.drawStr(50, 7, "Bot:   "); 
      }

      u8g2.drawStr(5, 7, lcd_buf);
      itoa(status.pak_err + status.pak_null, lcd_buf, 10);
      u8g2.drawStr(25, 7, lcd_buf);

      Speed_err=status.pak_err + status.pak_null;
      Speed_ans=status.pak;

      if (status.Nums_of_datchik!=Datchik_nums)  // если количество датчиков стало другим
      {   
          Datchik_nums=status.Nums_of_datchik;
          SaveFlashMain();
      }
      if (NET_use==0)
      {
          u8g2.drawStr(0, 14, "No connections");
      }

      if (NET_use==1)
      {
          u8g2.drawStr(0, 14, "WiFi:");
          switch (WiFi.status())
          {
            case 0: { u8g2.drawStr(30, 17, "none     "); } break;
            case 1: { u8g2.drawStr(30, 14, "no SSID  "); } break;
            case 2: { u8g2.drawStr(30, 14, "Scan...  "); } break;
            case 3: { u8g2.drawStr(30, 14, "Connected"); } break;
            case 4: { u8g2.drawStr(30, 14, "faild    "); } break;
            case 5: { u8g2.drawStr(30, 14, "lost     "); } break;
            case 6: { u8g2.drawStr(30, 14, "Discon.  "); } break;
            default: break;
          }
      }
      if (NET_use==2)
      {
        u8g2.drawStr(0, 14, "Eth.:");
        if (ESP32_W5500_eth_connected)  u8g2.drawStr(25, 14, "Connected");
        else u8g2.drawStr(25, 14, "Discon.  ");
      }

      u8g2.setCursor(0,23);
      u8g2.print("Speed: ");
      u8g2.print(float(1000000)/(float(time_exec-old_time_exec)) );
      u8g2.print("c/sec B:");
      u8g2.print(Datchik_nums);

    char ender_buffer[4];


// Если теоеграм бот
    if (Type_connection==2)
    { bot.tick(); }

// Если WEB
    if (Type_connection==1)
    {
      WiFiClient client = server.available(); // Запускаем прослушку

      if (client) // Если подключился новый клиент,
      {     
        u8g2.drawStr(64, 7, "Cl  "); 
        u8g2.sendBuffer();

        String Xline = "";
        String POST_number = "";
        uint8_t POST_flag = 0;
        uint8_t POST_n = 0;
        String POST_data = "";

        // для хранения данных,
        // пришедших от клиента.

        long x_count=0;
        long y_count=0;

        while (client.connected())
        {
          x_count++;
          if (client.available())
          {
            y_count++;
            char ccc = client.read(); // читаем байт, затем
            header += ccc;

            ender_buffer[0] = ender_buffer[1];
            ender_buffer[1] = ender_buffer[2];
            ender_buffer[2] = ender_buffer[3];
            ender_buffer[3] = ccc;

            Xline += ccc;

            if (POST_flag == 1)
            {
              POST_number += ccc;
            }

            if (Xline == "Content-Length: ")
            {
              POST_flag = 1;
            }

            if ( ((ender_buffer[2] == '\r') & (ender_buffer[3] == '\n')) | ((ender_buffer[3] == '\r') & (ender_buffer[2] == '\n')) )
            {
              // ArhivDatas += "_r_<br>";
              Xline = "";

              if (POST_flag == 1) // если получили сколько еще надо считать данных при POST
              {
                POST_n = atoi(POST_number.c_str());
                POST_flag = 2;
              }
            }

            if ((POST_flag == 3) & (POST_n > 0))
            {
              POST_n--;
              POST_data += ccc;
            }

            if ((ender_buffer[0] == '\r') & (ender_buffer[1] == '\n') & (ender_buffer[2] == '\r') & (ender_buffer[3] == '\n'))
            {
              POST_flag = 3;
            }

            if ((POST_flag == 3) & (POST_n == 0))
            {

              // Проанализировать что было в POST или GET
              if (POST_data == "pressed=CLEAR_HISTORY")
              {
                  sound(4);
                 EraseFlashMain();
                    sound(1);


                  RecNumber=1;
                  for (uint8_t i = 0; i < 20; i++) // очистить весь архив
                  {
                    Arhiv[i].datchik = 0; // и номер блоков 0 - нет записи
                    Arhiv[i].date[0] = 0; // если год равен 0 - значит записей больше нет
                  }
              }
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              String soob = "В архиве: " + (String)(RecNumber-1)+" сообщений. Осталось памяти для " + (String) (((SizeOfFlash/8)*1024*1024-4096)/9-RecNumber) + " сообщений. Блоков на линии:" + (String)Datchik_nums+ " <br>" ;

              client.println(main_page_13 +soob+ main_page_23 + ArhivDatas + main_page_33);

              POST_number = "";
              POST_flag = 0;
              POST_data = "";
              client.println();
              break;
            }
          }
        }
        
        // Очистим переменную
        header = "";
        ender_buffer[0] = 0;
        ender_buffer[1] = 0;
        ender_buffer[2] = 0;
        ender_buffer[3] = 0;
        // Закрываем соединение
        client.stop();

        u8g2.drawStr(64, 7, "  Dc"); 
        u8g2.sendBuffer();
        //  "Клиент отключился."
      }
    }
      u8g2.sendBuffer();
  }
}
