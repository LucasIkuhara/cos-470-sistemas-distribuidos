# *Sistemas Distribuídos - COS470*
## TP 1
#### Lucas Ribeiro Ikuhara, DRE 119019172
#### lucasikuhara@poli.ufrj.br
---

# Introdução

O objetivo do trabalho é experimentar com as diversas formas de comunicação entre processos (IPC) existentes. Para isso, foram desenvolvidos diversos programas para testar as funcionalidades de signals, pipes e sockets em diferentes linguagens. O código fonte para todos os programas pode ser encontrado [neste repositório](https://github.com/LucasIkuhara/cos-470-sistemas-distribuidos) do Github.

## Sinais

Para realizar os testes com signals, foram desenvolvidos dois programas separados, um que apenas envia sinais, chamado signaler, e um que apenas recebe sinais, chamado [signal-receiver](https://github.com/LucasIkuhara/cos-470-sistemas-distribuidos/tree/master/signals/signal-receiver).

### Signaler

Este programa é responsável pelo envio de sinais para outros programas, e foi o projetado para funcionar como um CLI. Por isso, os parâmetros de funcionamento devem se passados no momento em que o programa é invocado.

#### Modo de uso

Para compilar e executar o programa de maneira independente dos outros presentes neste trabalho, as instruções se encontram [aqui](https://github.com/LucasIkuhara/cos-470-sistemas-distribuidos/tree/master/signals/signaler). No entanto, para facilitar a demonstração, é recomendado que seja usado o script que compila todos os projetos.

Uma vez compilado, o programa deverá ser chamado da seguinte maneira: 
```bash 
./signaler/target/release/signaler 123 SIGUSR1
```
Sendo 123 o PID escolhido no exemplo, e SIGUSR1 um signal arbitrário.

#### Explicando o código-fonte

Logo no início da função main, nos deparamos com o seguinte trecho de código:
```rust 
// Get all args from argv
let mut args =  env::args_os();

// Discard first argument, which is the executable path
args.next().unwrap();

// Parse PID and SIGNAL
let pid = args.next()
    .expect("Missing PID argument. (run as ./signaler <pid> <signal>)")
    .into_string().unwrap();

let signal = args.next()
    .expect("Missing SIGNAL argument. (run as ./signaler <pid> <signal>)")
    .into_string().unwrap();

println!("PID: {}", pid);
println!("SIGNAL: {}", signal);
```

Esse trecho é responsável por ler os argumentos usados para executar o programa. A função chamada para acessar esses valores é a `std::env::args_os`, que retorna um iterador. O primeiro argumento é ignorado, pois trata-se do path do executável, e os dois valores seguintes devem ser o PID e o SIGNAL, respectivamente.

Em seguida, este trecho inicializa um struct usado como interface com o sistema operacional, e checa se existe um processo com o PID recebido do usuário, após fazer as transformações de tipagem necessárias: 
```rust
// Get and update system info
let mut system = sysinfo::System::new_all();
system.refresh_all();

// Get PID as a PID Struct
let pid_number: usize = pid.parse().expect("The received value for PID cannot be cast to int. Please use numbers only.");
let pid_struct: Pid = Pid::from(pid_number);

// Confirm the PID exists
let proc = system.process(pid_struct).expect("Chosen PID does not exist.");
println!("PID {} exists and is {}", pid, proc.status());
```
Caso um processo com o PID exista, seu status é imprimido na tela. Caso contrário, o erro `Chosen PID does not exist.` ocorre, e a execução do programa é abortada.


## Pipes

### Explicando o código-fonte


```C
// Check if an argument is present for busy vs blocking waiting
if (argc < 2) {
    printf("Missing argument N. A number of iterations should be given as an argument. Eg. './pipes.out 100'\nAborting.\n");
    exit(1);
}

// Parse argv to int
char *p;
int n = (int) strtol( argv[1],&p, 10);
```

Este trecho é responsável por ler os argumentos utilizados para executar o programa (ex: "./pipes.out 100"). O argumento esperado é o valor de N, que indica a quantidade de números que deverão ser gerados pelo produtor. A primeira condicional do código testa se foram recebidos argumentos suficientes, ou seja, se o valor de N foi inserido. Em seguida, é executada a conversão do argumento lido como texto (*char) para um inteiro, que será usado para controlar as iterações do produtor.

Em seguida, é criado um array que guardará os descritores de pipe, e é invocada a função que os cria:
```C
// Creates a new pipe using this int[] as a descriptor
int pipeDesc[2];
createPipe(pipeDesc);
```

Sendo as funções de criação e desalocação de pipes definidas como:
```C
// Creates a new pipe and save descriptors to the input array
void createPipe(int descriptor[]) {

    // Creates a new pipe
    if(pipe(descriptor) < 0) {

        // Error handling
        printf("Failed to create a pipe. Aborting.\n");
        exit(1);
    }
}

// Closes an open pipe given its descriptors
void closePipe(int* descriptor) {

    close (descriptor[0]);
    close (descriptor[1]);
}
```

Ao invocar a função pipe, checamos se o retorno é negativo, o que indica um erro na criação do pipe, para que o usuário possa ser notificado.
A função de desalocação simplesmente fecha individualmente as duas pontas do pipe.

Para a criação de um processo separado para o consumidor, é utilizado o comando `fork()`.

```C
// Fork to create a new process
    int pid = fork ();
    if (pid < 0) {

        // Error handling for fork command
        printf("Failed to perform fork. Aborting.\n");
        exit(1); // Finaliza o programa com código 1
    }

    // Producer code
    if (pid > 0) {
        producer(pipeDesc, n);
    }

    // Consumer code
    else {
        consumer(pipeDesc);
    }
```

Caso a chamada do `fork()` retorne um valor negativo, sua execução falhou, e portanto, o erro é tratado.
Se a chamada for executada de maneira bem-sucedida, ela retornará o PID do novo processo criado. 

Note que no processo pai, a variável PID terá guardado da chamada `fork()`, enquanto no processo filho, não. Essa será a base do mecanismo que diferenciará o comportamento dos processos. Desta forma, o processo pai executará o código do produtor, segregado para a função `producer`; e o processo filho, executará o código da função `consumer`.

#### Producer

A função do produtor recebe apenas dois argumentos: o array de descritores de pipe, para que possa escrever, e o valor de N, que dermina quantos valores serão produzidos.

```C
/*
Producer source-code.
Responsible for generating numbers.
*/
void producer(int* descriptor, int n) {

    printf("Started producer.\n");
    int value = 1;

    // Use time as Seed for RNG
    srand(time(NULL));

    for (int i = 0; i < n; i++) {

        // Random value generation from 1 to 100
        int delta = (rand() % 100) + 1;
        value = value + delta;

        // Send value N by writing to the pipe
        write (descriptor[1], &value, sizeof(int));
    }

    // Send stop command to consumer
    int stop = 0;
    write (descriptor[1], &stop, sizeof(int));
}
```

O trecho `srand(time(NULL))` é usado para gerar um seed novo para a função geradora de números aleatórios baseado no tempo atual, para que sejam diferentes toda vez que o programa é executado.

Iteramos N vezes, para produzir e enviar N números. O trecho `(rand() % 100) + 1` gera valores de int aleatórios, e garante que estão no intervalo [1, 100] usando a operação `mod`. A adição de `1` garante que `0` não serão enviados, pois interrompem a execução do programa.

É usada a chamada `write()` para escrever no pipe, usando o tamanho fixo de `sizeof(int)`, tanto para escrita, tanto pra leitura no consumidor.

Ao acabar as N iterações, é feito um último `write()`, que sinaliza a condição de parada do consumer.

#### Consumer
```C
/*
Consumer source-code.
Responsible for reading numbers.
*/
void consumer(int* descriptor) {

    printf("Started consumer.\n");
    int lastNum = 1;

    // Create reading loop
    while(lastNum != 0) {

        // Read pipe output and store at lastNum
        read(descriptor[0], &lastNum, sizeof(int));
        printf("Consumer: %d\n", lastNum);
    }

}
```
