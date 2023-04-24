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


<MUDAR>

### Signal-Receiver
