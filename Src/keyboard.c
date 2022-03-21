#include "main.h"

extern TIM_HandleTypeDef htim6;
extern uint8_t tim6_counter;

int delay_for_pressed_button = 250000;
char total_number [4] = {' ',' ',' ',' '};
int count = 0;
char comparsion1 = '0';
char comparison = ' ' ;
//total_number[0]=comparison;

__STATIC_INLINE void DelayMicro(uint32_t __IO micros)
{
  micros *=(SystemCoreClock/1000000)/8;
  while(micros--);
}

void CheckFirstButtonPressed ()
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_SET);
	
	DelayMicro(delay_for_pressed_button/2);
	
	while(1)
	{
		
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)==1 || HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)==1
			|| HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==1 || HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)==1)
		{

		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_RESET);
		break;
		}	
	}	
}

void Show_total_number (char *pressed_button) //*pressed_button - это ЗНАЧЕНИЕ цифры
{
	//устанавливаем шрифт и тд для ввода квартиры
	CS_ACTIVE();
	TFT9341_SetTextColor(TFT9341_WHITE); //возвращаем нормальный шрифт и тд для ввода ном вартиры 
	TFT9341_SetBackColor(TFT9341_BLACK);
	TFT9341_SetFont(&Font48);
	
	
	//исключаем, что значение счетчика случайно уйдет ниже нуля
	if (count < 0)
	{
		count = 0;
	}
	
	//исключаем момент с первой цифрой "0" в номере квартиры
	if (count == 0 && *pressed_button == comparsion1)
	{
		TFT9341_DrawChar(100,135,' ');
		count--; // делаем его нулем	
	}
	
	if (count < 4 )//&& count>=0)
	{
		total_number[count] = *pressed_button; //присваиваем элементу массива ЗНАЧЕНИЕ 
		
		for (int i=0;i<=count;i++)
		{
		//отправляем на экран именно символ, а не строку, чтобы не появился мусор
		TFT9341_DrawChar(100+i*30,135,*&total_number[i]); //отправляем функции ссылку, он именно ССЫЛКУ принимает выписываем набранные цифры со смещением	
		}
		
		for (int j=count+1;j<4;j++)//все, что идет за Count - пустота
		{
			TFT9341_String(100+j*30,135," ");
		}
	}
	else if(count >= 4)
	{
	count = 3;
	}	
	
	count++;
	//DelayMicro(delay_for_pressed_button);
}


void keyboard_default_waiting()
{
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4, GPIO_PIN_SET);
	
	//первый столбец (самый правый)
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)==1)   //112
	{
		Draw_c112call_page ();
		count=4; //чтобы нельзя выло лишнего выводить	
		DelayMicro(600000);     //Три точки возле "Вызов"
		
		CS_ACTIVE();
		
		TFT9341_SetTextColor(TFT9341_RED); //меняем шрифт и тд для страницы вызова
		TFT9341_SetBackColor(TFT9341_WHITE);
		TFT9341_SetFont(&Font24);
		
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_SET);//запитываем пин для контроля нажатия С
		while (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)!=1) 
			//пока не будет нажата кнопка "С"
		{
		TFT9341_String(220,137,".");		
		DelayMicro(600000);
		//TFT9341_String(220,137," ");
		TFT9341_String(227,137,".");
		DelayMicro(600000);	
		//TFT9341_String(227,137," ");
		TFT9341_String(234,137,".");
		DelayMicro(600000);
		TFT9341_String(220,137," ");
		TFT9341_String(227,137," ");
		TFT9341_String(234,137," ");
		DelayMicro(600000);
		}
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_RESET);
		
		DelayMicro(delay_for_pressed_button);		
	}
	
	if (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)==1)  //Вызов ДИСПЕТЧЕРА
	{
		Draw_concierge_page ();	
		count=4; //чтобы нельзя выло лишнего выводить		
		DelayMicro(delay_for_pressed_button);		
	}
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==1)   //Вызов КОНСЬЕРЖА
	{
		Draw_concierge_page ();		
		count=4; //чтобы нельзя выло лишнего выводить
		DelayMicro(delay_for_pressed_button);
	}
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)==1)  //Вызов ОДС
	{
		//Draw_callODS_page ();	
		Draw_concierge_page ();	
		count=4; //чтобы нельзя выло лишнего выводить
		DelayMicro(delay_for_pressed_button);
	}
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4, GPIO_PIN_RESET);
	
	//второй стоблец справа
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3, GPIO_PIN_SET);
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)==1)
	{
		Show_total_number ("3");	
		DelayMicro(delay_for_pressed_button);
	}
	if (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)==1)
	{
		Show_total_number ("6");		
		DelayMicro(delay_for_pressed_button);
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==1)
	{
		Show_total_number ("9");		
		DelayMicro(delay_for_pressed_button);
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)==1)  //   ВЫЗОВ квартиры
	{
		if (total_number[0]!=comparison)
		{
		Draw_call_page();
		CS_ACTIVE();
		
		TFT9341_SetTextColor(TFT9341_WHITE); //меняем шрифт и тд для страницы вызова
		TFT9341_SetBackColor(TFT9341_BLACK);
		TFT9341_SetFont(&Font24);
		
		for (int i=0;i<count;i++)
		{
		//отправляем на экран именно символ, а не строку, чтобы не появился мусор
		TFT9341_DrawChar(215+i*15,168,*&total_number[i]);//выводим номер квартиры
		}
		
		count = 4; //чвтобы нельзя было лишнего выводить
		//DelayMicro(delay_for_pressed_button);
		
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_SET);//запитываем пин для контроля нажатия С
		while (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)!=1) 
			//пока не будет нажата кнопка "С"
			{
		DelayMicro(600000);     //Три точки возле "Вызов"
		TFT9341_String(180,2,".");		
		DelayMicro(600000);
		TFT9341_String(180,2," ");
		TFT9341_String(187,2,".");
		DelayMicro(600000);	
		TFT9341_String(187,2," ");
		TFT9341_String(194,2,".");
		DelayMicro(600000);
		TFT9341_String(194,2," ");
			}
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_RESET);
		}
	}
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3, GPIO_PIN_RESET);
	
	
	//третий столбец справа
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)==1)
	{
		Show_total_number ("2");		
		DelayMicro(delay_for_pressed_button);
			
	}
	if (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)==1)
	{
		Show_total_number ("5");		
		DelayMicro(delay_for_pressed_button);
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==1)
	{
		Show_total_number ("8");		
		DelayMicro(delay_for_pressed_button);	
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)==1)
	{
		Show_total_number ("0");		
		DelayMicro(delay_for_pressed_button);
	}
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
	
	
	//четвертый стоблец (первый слева)
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_SET);
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0)==1)
	{
		Show_total_number ("1");		
		DelayMicro(delay_for_pressed_button);	
			
	}
	if (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)==1)
	{
		Show_total_number ("4");		
		DelayMicro(delay_for_pressed_button);
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==1)
	{
		Show_total_number ("7");		
		DelayMicro(delay_for_pressed_button);		
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)==1)  // КНОПКА   "C" 
	{	
		Draw_main_page();
		DelayMicro(delay_for_pressed_button);
		count=0;
		total_number [0]= ' ';
		total_number [1]= ' ';
		total_number [2]= ' ';
		total_number [3]= ' ';
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_RESET);
		keyboard_default_waiting();
		
		
	}
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1, GPIO_PIN_RESET);
	
}
