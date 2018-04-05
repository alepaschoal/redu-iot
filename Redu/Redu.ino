#include <Nextion.h>
#include <SoftwareSerial.h>

SoftwareSerial nextion(2, 3);
Nextion myNextion (nextion, 9600);

#define MENSAGEM_PERGUNTA '1'
#define MENSAGEM_PONTUACAO '2'

#define INTERVALO_EXIBIR_PONTUACAO 2000

#define ESTADO_INICIO 0
#define ESTADO_RECEBER_MENSAGEM 1
#define ESTADO_LENDO_PERGUNTA 2
#define ESTADO_PROCESSANDO_LCD 3
#define ESTADO_LENDO_PONTUACAO 4
#define ESTADO_EXIBINDO_PONTUACAO 5
#define QUANT_ALTERNATIVAS 5
int estado = ESTADO_RECEBER_MENSAGEM;
int comecoDisplayPontuacao = 0;
const int pinoReset = 12;

char* alternativas[5] = { "t1", "t2", "t3", "t4", "t5"};


char buffer[300] = {
  0
};

void (* resetFunc)(void) = 0;

void setup() {
  Serial.begin(9600);
  pinMode (pinoReset, OUTPUT);
  myNextion.init();
}
int idxLeitura = 0;

void loop() {
  char c;
  switch (estado) {
    case ESTADO_INICIO:
      lerResposta();
    case ESTADO_RECEBER_MENSAGEM:
      if (Serial.available()) {
        switch (Serial.read()) {
          case MENSAGEM_PERGUNTA:
            memset(buffer, 0, sizeof(buffer));
            estado = ESTADO_LENDO_PERGUNTA;
            break;
          case MENSAGEM_PONTUACAO:
            memset(buffer, 0, sizeof(buffer));
            estado = ESTADO_LENDO_PONTUACAO;
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
      if (c != '\n') {
        buffer[idxLeitura++] = c;
        estado = ESTADO_LENDO_PERGUNTA;
      } else {
        //Serial.print("Lida");
        escreverPergunta();
        idxLeitura = 0;
        estado = ESTADO_PROCESSANDO_LCD;
      }
      break;
    case ESTADO_LENDO_PONTUACAO:
      // '210\n'
      c = Serial.read();
      if (c != '\n') {
        buffer[idxLeitura++] = c;
        estado = ESTADO_LENDO_PONTUACAO;
      } else {
        escreverPontuacao();
        comecoDisplayPontuacao = millis();
        idxLeitura = 0;
        estado = ESTADO_EXIBINDO_PONTUACAO;
      }
      break;
    case ESTADO_EXIBINDO_PONTUACAO:
      if (millis() - comecoDisplayPontuacao > INTERVALO_EXIBIR_PONTUACAO) {
        Serial.println("reset");
        Serial.flush();
        resetFunc();
      }
      break;
    case ESTADO_PROCESSANDO_LCD:
      lerResposta();

      estado = ESTADO_RECEBER_MENSAGEM;
      break;
  }
  // Serial.print("Estado: "); Serial.println(estado);
}

void lerResposta() {
  String message = myNextion.listen();
  if (message != " ") {
    if (message == "65 0 1 0 ffff ffff ffff") {
      Serial.println("ready");
      estado = ESTADO_RECEBER_MENSAGEM;
    }
    if (message == "65 1 3 0 ffff ffff ffff") {
      Serial.println("Alternativa A");
    }
    else if ( message == "65 1 4 0 ffff ffff ffff") {
      Serial.println("Alternativa B");
    }

    else if ( message == "65 1 5 0 ffff ffff ffff") {
      Serial.println("Alternativa C");
    }
    else if ( message == "65 1 6 0 ffff ffff ffff") {
      Serial.println("Alternativa D");
    }

    else if (message == "65 1 7 0 ffff ffff ffff") {
      Serial.println("Alternativa E");
    }
  }
  delay(500);
}

void escreverPergunta() {
  //Serial.println("Inicio escreverPergunta");
  int idxAlternativa = 0;
  //Serial.println(idxAlternativa);
  //Serial.println(buffer);

  // strtok() é uma função para quebrar uma string em determinado delimitador
  // a primeira chamada deve ter um ponteiro para a string a ser quebrada
  // http://www.cplusplus.com/reference/cstring/strtok/
  char *texto = strtok(buffer, ";");
  //Serial.println(texto);
  if (texto == NULL) {
    // nenhum delimitador encontrado, sair da função
    return;
  }

  // primeiro campo é o texto da pergunta
  myNextion.setComponentText("t0", texto);

  // pegar texto da 1a alternativa
  // depois da primeira chamada, strtok deve ser chamada com NULL para 'continuar procurando'
  texto = strtok(NULL, ";");
  while (texto != NULL && idxAlternativa < QUANT_ALTERNATIVAS) {
    // chamamos setText() para cada NextionText em nossa array
    myNextion.setComponentText(String(alternativas[idxAlternativa]), texto);
    //Serial.println(alternativas[idxAlternativa]);
    idxAlternativa++;
    texto = strtok(NULL, ";");
  }
  //Serial.println("Final escreverPergunta");
}

void escreverPontuacao() {

  myNextion.sendCommand("page 2");
  myNextion.setComponentText("t1", buffer);

  //Serial.println("Final escreverPontuacao");
}
