var five = require("johnny-five");
const _ = require("lodash");
const request = require("request");
var lcd;

var SerialPort = require("serialport");
var serialPort = new SerialPort("COM4", {
    baudRate: 9600
});

const Readline = require('parser-readline');
const parser = serialPort.pipe(new Readline({ delimiter: '\n' }));
let perguntas = null;
let perguntaAtual = 0;
var portaAberta = false;
var pontuacao = 0;

parser.on('data', function (dadosLidos) {
    if (/^alternativa/.test(dadosLidos.toLocaleLowerCase())) {
        console.log(dadosLidos[12]);
        // var letra = dadosLidos.substr(dadosLidos.length - 1, 1);
        // console.log("letra:"+ letra);
        if(dadosLidos[12].toLocaleUpperCase() == respostaCerta){
            pontuacao++;
            console.log(pontuacao);
        }
        // guardar pontuação
    }
    if (/^ready/.test(dadosLidos) || /^alternativa/.test(dadosLidos.toLocaleLowerCase())) {
        pegarPergunta();
    }
});

serialPort.on("open", function () {
    portaAberta = true;
});

var respostaCerta;
var d;

function pegarPergunta() {
    console.log("Pegando pergunta");
    console.log("perguntaAtual antes: " + perguntaAtual);
    if (!perguntas) {
        request.get("http://backredu-001-site1.btempurl.com/api/pergunta/listar",
            { json: true },
            function (error, response, body) {
                if (error) {
                    console.log('error:', error); // Print the error if one occurred
                    console.log('statusCode:', response && response.statusCode);
                    return;
                }
                perguntas = body;
                respostaCerta = perguntas[perguntaAtual].opcaoCorreta;
                gravaPerguntaArduino(formatarPergunta(perguntas[perguntaAtual++]));
                console.log("respostaCerta " + respostaCerta);
            });
    } else {
        if (perguntaAtual < perguntas.length) {
            respostaCerta = perguntas[perguntaAtual].opcaoCorreta;
            gravaPerguntaArduino(formatarPergunta(perguntas[perguntaAtual++]));
            console.log("respostaCerta " + respostaCerta);
        } else {
            // cabô!
        }
    }
    console.log("perguntaAtual depois: " + perguntaAtual);
}

function formatarPergunta(pergunta) {
    var mensagem = ["1" + pergunta.titulo];
    var alternativas = ["A", "B", "C", "D", "E"];

    for (let index = 0; index < alternativas.length; index++) {
        mensagem.push(pergunta["opcao" + alternativas[index]])
    }

    var enviarArduino = mensagem.join(";") + "\n";

    return enviarArduino;
}

function gravaPerguntaArduino(pergunta) {
    if (portaAberta) {
        //console.log(pergunta);
        serialPort.write(RemoveAccents(pergunta));
        console.log("pergunta enviada");
    } else {
        port.open(function (err) {
            if (err) {
                return console.log('Error opening port: ', err.message);
            }
            serialPort.write(RemoveAccents(pergunta));
        });
    }

}

function RemoveAccents(str) {
    var accents = 'ÀÁÂÃÄÅàáâãäåÒÓÔÕÕÖØòóôõöøÈÉÊËèéêëðÇçÐÌÍÎÏìíîïÙÚÛÜùúûüÑñŠšŸÿýŽž';
    var accentsOut = "AAAAAAaaaaaaOOOOOOOooooooEEEEeeeeeCcDIIIIiiiiUUUUuuuuNnSsYyyZz";
    str = str.split('');
    var strLen = str.length;
    var i, x;
    for (i = 0; i < strLen; i++) {
        if ((x = accents.indexOf(str[i])) != -1) {
            str[i] = accentsOut[x];
        }
    }
    return str.join('');
}