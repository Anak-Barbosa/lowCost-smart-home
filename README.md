# Sistema de Automação e Controle com VOSK e Raspberry Pi

Este projeto visa criar um sistema de automação residencial integrado, utilizando reconhecimento de voz para controlar dispositivos conectados. O sistema é baseado no processamento de comandos de voz através do Raspberry Pi, utilizando a biblioteca VOSK, e na execução desses comandos pelo ESP32, que controla dispositivos conectados às suas portas GPIO.

O objetivo principal deste projeto, que é o meu Trabalho de Conclusão de Curso (TCC), é desenvolver uma solução de automação de baixo custo, com foco em segurança, acessibilidade e sustentabilidade.

## Funcionalidades

- **Detecção de Palavras-chave (Wake Word)**: O sistema usa a biblioteca VOSK para identificar uma palavra-chave (wake word) que ativa o reconhecimento de comandos de voz subsequentes.
  
- **Transcrição e Processamento de Comandos**: Após a detecção da palavra-chave, o áudio capturado é transcrito para texto e os comandos de voz são processados para identificar ações que devem ser executadas. Comandos como "acender", "desligar", "abrir", entre outros, são interpretados para controlar dispositivos conectados.

- **Controle de Dispositivos GPIO via ESP32**: O ESP32 executa as ações físicas solicitadas nos comandos de voz, como o controle de luzes, motores, sistemas de alarme e outros dispositivos conectados às suas portas GPIO.

- **Sistema de Execução Offline**: O sistema não depende de uma conexão com a internet para funcionar. Todo o processamento de fala é feito localmente no Raspberry Pi, garantindo privacidade e segurança aos dados do usuário.

- **Comunicação entre Raspberry Pi e ESP32 via UART (RX/TX)**: O Raspberry Pi envia os comandos processados para o ESP32 via UART (RX/TX). O ESP32, por sua vez, executa as ações nos dispositivos físicos de acordo com os comandos recebidos.

## Estrutura do Repositório

Este repositório está organizado da seguinte forma:

- **/projetos_3d/**: Contém os principais arquivos de modelagem 3D desenvolvidos para a maquete de demonstração do projeto.

- **/codigo/**: Contém os códigos do sistema de automação e controle.
  - **Client.py**: Faz o reconhecimento de voz, monitora o estado dos botões de controle fixo e envia para o ESP32.
  - **Server.py**: Controla a fita de LED RGB. 
  - **Main.c**: Executa a lógica principal do programa.
  - **Functions_Init.c**: Contém as funções de inicialização do sistema. 
  - **Functions_Exec.c**: Contém as funções de task e execução dos sistema.
  - **Functions.h**: Declara os protótipos das funções, além de macros, variáveis externas e tipos utilizados no programa.

- **/documentação/**: Contém documentos e tutoriais sobre a instalação e configuração do ambiente para rodar o projeto, incluindo a configuração do Raspberry Pi, VOSK, e dependências.

## Requisitos

- **Raspberry Pi** com sistema operacional Raspbian ou equivalente.
- **Modelo VOSK** configurado para reconhecimento de fala em português.
- **ESP32** para controlar dispositivos conectados via GPIO.
- **Python 3.9+** com as dependências necessárias, incluindo a biblioteca VOSK.

## Contribuições

Se você possui sugestões ou melhorias para o projeto, por favor, abra um *pull request* ou envie uma *issue*.

## Licença

Este projeto é licenciado sob a [Licença MIT](LICENSE).
