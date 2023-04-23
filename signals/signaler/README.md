# Signaler

Este é o módulo responsável por enviar sinais arbitrários para um determinado PID.

## Build

Para compilar o código, é necessário que as dependências "rustc" e "cargo" estejam instaladas. Ambos são instalados automaticamente no processo de instalação da linguagem [rust](https://www.rust-lang.org/).

Com as dependências já instaladas, utilize o comando `cargo build --release` para compilar o código (a partir do diretório e que esse README se encontra).

## Run

Para executar o código, utilize `./signaler/target/release/signaler <PID> <SIGNAL>` (a partir do diretório e que esse README se encontra). O valor de <PID> deve, necessariamente, ser um número; enquanto o valor de <SIGNAL> pode ser um número ou um nome de signal.

Ex: `./signaler/target/release/signaler 123 SIGUSR1 `
