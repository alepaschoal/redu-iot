/**
 * @example CompButton.ino
 * 
 * @par How to Use
 * Show how to use API of class NexButton.  
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/7/10
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "Nextion.h"
#include "NextionText.h"
#include "SoftwareSerial.h"
#define MENSAGEM_PERGUNTA '1'

#define ESTADO_RECEBER_MENSAGEM 0
#define ESTADO_LENDO_PERGUNTA 1
#define ESTADO_PROCESSANDO_LCD 2
int estado = ESTADO_RECEBER_MENSAGEM;

SoftwareSerial serialLCD(2, 3);

Nextion lcd(serialLCD);

NextionText t0 = NextionText(lcd, 0, 8, "t0");
NextionText g1 = NextionText(lcd, 0, 2, "g1");
NextionText g2 = NextionText(lcd, 0, 3, "g2");
NextionText g3 = NextionText(lcd, 0, 4, "g3");
NextionText g4 = NextionText(lcd, 0, 5, "g4");
NextionText g5 = NextionText(lcd, 0, 6, "g5");

#define QUANT_ALTERNATIVAS 5
NextionText alternativas[QUANT_ALTERNATIVAS] = {g1, g2, g3, g4, g5};

char buffer[300] = {
  0};
String texto = "teste 123";

void alternativaCb(NextionEventType type, INextionTouchable *btn)
{
  if(type == NEX_EVENT_POP) {
    Serial.print("Alternativa pressionada:");
    Serial.println(btn->getComponentID());
  }
}

void setup(void)
{
  Serial.begin(9600);

  serialLCD.begin(9600);
  lcd.init();

  // cadastrar o callback em todos os textos de alternativas
  for(int i = 0; i < QUANT_ALTERNATIVAS; i++) {
    alternativas[i].attachCallback(&alternativaCb);
  } 
  Serial.println("setup done");
}

int idxLeitura = 0;
void loop(void)
{
  char c;
  switch(estado) {
  case ESTADO_RECEBER_MENSAGEM:
    if(Serial.available()) {
      switch(Serial.read()) {
      case MENSAGEM_PERGUNTA:
        memset(buffer, 0, sizeof(buffer));
        estado = ESTADO_LENDO_PERGUNTA;
        break;
      }
    } 
    else {
      estado = ESTADO_PROCESSANDO_LCD;
    }
    break;
  case ESTADO_LENDO_PERGUNTA:
    // '1Pergunta?;alt1;alt2;alt3;alt4;alt5;\n'
    c = Serial.read();
    if(c != '\n') {
      buffer[idxLeitura++] = c;
    } else {
      escreverPergunta();
    }
    break;
  case ESTADO_PROCESSANDO_LCD:
    lcd.poll();
    estado = ESTADO_RECEBER_MENSAGEM;
    break;
  }

}
void escreverPergunta() {
  int idxAlternativa = 0;

  // strtok() é uma função para quebrar uma string em determinado delimitador
  // a primeira chamada deve ter um ponteiro para a string a ser quebrada
  // http://www.cplusplus.com/reference/cstring/strtok/
  char *texto = strtok(buffer, ";");
  if(texto == NULL) {
    // nenhum delimitador encontrado, sair da função
    return;
  }

  // primeiro campo é o texto da pergunta 
  t0.setText(texto);

  // pegar texto da 1a alternativa
  // depois da primeira chamada, strtok deve ser chamada com NULL para 'continuar procurando'
  texto = strtok(NULL, ";");
  while (texto != NULL && idxAlternativa < QUANT_ALTERNATIVAS) {
    // chamamos setText() para cada NextionText em nossa array
    alternativas[idxAlternativa].setText(texto);
    idxAlternativa++;
    texto = strtok(NULL, ";");
  }
}
