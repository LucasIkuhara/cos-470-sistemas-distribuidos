# Signal Receiver

Este é o módulo responsável por receber a comunicação via signais. Ele é capaz de receber e tratar os sinais SIGUSR1, SIGUSR2 e SIGINT. No caso dos dois primeiros, o programa simplesmente sinaliza ao usuário que o sinal foi recebido. No caso do SIGINT, o programa sinaliza o recebimento do sinal e interrompe sua execução. 

O programa oferece duas possíveis maneiras de esperar por sinais, via busy-wait ou blocking wait. Uma das duas opções deve ser escolhida no momento da inicialização.

## Build

O programa deve ser compilado preferencialmente usando o GCC. Para fazê-lo, execute `gcc main.c -o receiver.out` (a partir da pasta onde se encontra o readme).

## Run

Para executar o programa, utilize o comando `./receiver.out <MODO>`, onde <MODO> deverá ser substituído por "block" ou "busy", para declarar o modo de espera que deverá ser utilizado. Caso outra opção seja usada, busy-wait será utilizado.