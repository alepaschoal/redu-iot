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
#include "SoftwareSerial.h"
#define MENSAGEM_PERGUNTA '1'

int estado = ESTADO_RECEBER_MENSAGEM;
#define ESTADO_RECEBER_MENSAGEM 0
#define ESTADO_LENDO_PERGUNTA 1
#define ESTADO_PROCESSANDO_LCD 2

SoftwareSerial minhaSoftwareSerial(2, 3);

//NexButton b0 = NexButton(0, 1, "b0");
NexText t0 = NexText(0, 8, "t0");
NexText g1 = NexText(0, 2, "g1");
NexText g2 = NexText(0, 3, "g2");
NexText g3 = NexText(0, 4, "g3");
NexText g4 = NexText(0, 5, "g4");
NexText g5 = NexText(0, 6, "g5");

NexText* textos = {t0, g1, g2, g3, g4, g5};

char buffer[300] = {
  0};
String texto = "teste 123";

NexTouch *nex_listen_list[] = 
{
  &t0,
  &g1,
  &g2,
  &g3,
  &g4,
  &g5,
  NULL
};

void b0PopCallback(void *ptr)
{
  uint16_t len;
  uint16_t number;
  String texto = "teste 123";

  NexText *btn = (NexText *)ptr;

  memset(buffer, 0, sizeof(buffer));
  btn->getText(buffer, sizeof(buffer));

  //
  texto = atoi(buffer);
  //number += 1;

  memset(buffer, 0, sizeof(buffer));
  //itoa(texto, buffer, 10);
  //sprintf(buffer,"%d",number);

  btn->setText(buffer);
}

void setup(void)
{
  Serial.begin(9600);
  nexInit();
  t0.attachPop(b0PopCallback, &t0);
  dbSerialPrintln("setup done");
}

int idxLeitura = 0;
void loop(void)
{
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
    char c = Serial.read();
    if(c != '\n') {
      buffer[idxLeitura++] = c;
    } else {
      escreverPergunta();
    }
    break;
  case ESTADO_PROCESSANDO_LCD:
    nexLoop(nex_listen_list);
    estado = ESTADO_RECEBER_MENSAGEM;
    break;
  }

}
void escreverPergunta() {
  int idxTexto = 0;
  int i = 0;
  
}






