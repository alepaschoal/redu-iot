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

//open
//request.get()

parser.on('data', function (dadosLidos) {

    console.log(dadosLidos);

    // if (dadosLidos == "ready") {
    pegarPergunta()
});

var givenAnswer;
var rightAnswer;
var questionShow;
var alternativas;


var d;


function isRightAnswer() {

    var answer;
    switch (givenAnswer) {

        case alternativas[0]:
            answer = question.alternativas[0];
            break;

        case alternativas[1]:
            answer = question.alternativas[1];
            break;

        case alternativas[2]:
            answer = question.alternativas[2];
            break;
    };

    const data = {
        id: 4,
        resp: answer
    }

    console.log(data)

    request.put("http://localhost:8080/iot", {
        json: true,
        body: data
    });
}

function gravaPerguntaArduino (error, pergunta) {
    console.log("enviando pergunta");
    if (error) {
        console.log(error)
    };
    console.log("pós erro");
    serialPort.on("open", function () {
        console.log('open');
        serialPort.write(pergunta)
    });
    console.log("pergunta enviada");
    // });

}

function pegarPergunta() {
    console.log("Pegando pergunta");
    if (!perguntas) {
        request.get("http://localhost:8080/iot",
            {
                json: true
            },
            function (error, response, body) {
                if (error) {
                    console.log('error:', error); // Print the error if one occurred
                    console.log('statusCode:', response && response.statusCode);
                    return gravaPerguntaArduino(error, null);
                }
                perguntas = body;
                gravaPerguntaArduino(null, formatarPergunta(perguntas[perguntaAtual++]));
                console.log("pergunta formatada");
            });
    } else {
        if (perguntaAtual < perguntas.length) {
            gravaPerguntaArduino(null, perguntas[perguntaAtual++]);
        } else {
            // cabô!
        }
    }
}

function formatarPergunta(pergunta) {
    console.log("formando pergunta");
    var quase = ["1"].concat(perguntas.pergunta[perguntaAtual]).join("");
    var enviarArduino = [quase].concat(perguntas.alternativas).join(";");
    console.log(enviarArduino);
    return {
        enviarArduino
    }
}
// > [1,2,3].join(";")
// '1;2;3'
// > o = {alternativas:['uno', 'dos', 'tres']}
// { alternativas: [ 'uno', 'dos', 'tres' ] }
// > o.alternativas.join(";")
// > o.pergunta="oi?"
// 'oi?'
// > o
// { alternativas: [ 'uno', 'dos', 'tres' ], pergunta: 'oi?' }
// > [o.pergunta].concat(o.alternativas)
// [ 'oi?', 'uno', 'dos', 'tres' ]
// > [o.pergunta].concat(o.alternativas).join(";")
// 'oi?;uno;dos;tres'
// > [o.pergunta].concat(o.alternativas).join(";") + "\n"
// 'oi?;uno;dos;tres\n'