# Sistema de Automação e Controle com VOSK e Raspberry Pi

Este projeto está em fase de desenvolvimento e visa criar um sistema de automação residencial integrado, utilizando reconhecimento de voz para controlar dispositivos conectados. O sistema é baseado no processamento de comandos de voz através do Raspberry Pi, utilizando a biblioteca VOSK, e na execução desses comandos pelo ESP32, que controla dispositivos conectados às suas portas GPIO.

O objetivo principal deste projeto, que é o meu Trabalho de Conclusão de Curso (TCC), é desenvolver uma solução de automação de baixo custo, com foco em segurança, usabilidade, acessibilidade, sustentabilidade e personalização.

## Funcionalidades

- **Detecção de Palavras-chave (Wake Word)**: O sistema usa a biblioteca VOSK para identificar uma palavra-chave (wake word) que ativa o reconhecimento de comandos de voz subsequentes.
  
- **Transcrição e Processamento de Comandos**: Após a detecção da palavra-chave, o áudio capturado é transcrito para texto e os comandos de voz são processados para identificar ações que devem ser executadas. Comandos como "acender", "desligar", "mudar velocidade", entre outros, são interpretados para controlar dispositivos conectados.

- **Controle de Dispositivos GPIO via ESP32**: O ESP32 executa as ações físicas solicitadas nos comandos de voz, como o controle de luzes, motores, sistemas de alarme e outros dispositivos conectados às suas portas GPIO.

- **Sistema de Execução Offline**: O sistema não depende de uma conexão com a internet para funcionar. Todo o processamento de fala é feito localmente no Raspberry Pi, garantindo uma operação mais segura e confiável em ambientes fechados e isolados.

- **Comunicação entre Raspberry Pi e ESP32 via UART (RX/TX)**: O Raspberry Pi envia os comandos processados para o ESP32 via UART (RX/TX). O ESP32, por sua vez, executa as ações nos dispositivos físicos de acordo com os comandos recebidos.

- **Captura de Áudio via Microfone**: Embora o sistema esteja em fase de testes, atualmente ele utiliza o microfone do PC para captar áudio e transmite esse áudio via TCP para o Raspberry Pi. Esse processo é temporário até que o sistema de captura de áudio seja integrado de maneira mais eficiente.

## Estrutura do Repositório

Este repositório está organizado da seguinte forma:

- **/projetos_3d/**: Contém os principais arquivos de modelagem 3D desenvolvidos para a maquete de demonstração do projeto.

- **/codigo/**: Contém os códigos do sistema de automação e controle.
  - **control_unit.c**: Código responsável pela execução de comandos no ESP32, controlando dispositivos conectados às portas GPIO com base nos comandos recebidos do Raspberry Pi.
  - **voice_recognition.py**: Script Python que gerencia a detecção de palavras-chave, gravação de áudio e transcrição usando a biblioteca VOSK, além de enviar os comandos para o ESP32.
  - **Send_audio.sh**: Script shell que deve ser executado no PC para estabelecer a comunicação TCP e enviar o áudio capturado para o Raspberry Pi.

- **/documentação/**: Contém documentos e tutoriais sobre a instalação e configuração do ambiente para rodar o projeto, incluindo a configuração do Raspberry Pi, VOSK, e dependências.

## Requisitos

- **Raspberry Pi** com sistema operacional Raspbian ou equivalente.
- **Modelo VOSK** configurado para reconhecimento de fala em português.
- **FFmpeg** instalado para captura e transmissão de áudio.
- **ESP32** para controlar dispositivos conectados via GPIO.
- **Python 3.9+** com as dependências necessárias, incluindo a biblioteca VOSK.

## Como Usar

1. **Configuração do Raspberry Pi**: Siga as instruções de configuração na documentação para preparar o Raspberry Pi, incluindo a instalação do modelo VOSK e suas dependências.
2. **Configuração do VOSK**: Instale o modelo VOSK adequado para reconhecimento de voz em português.
3. **Execução do Código**: Execute os scripts Python para iniciar a detecção de palavras-chave e o processamento de comandos de voz. Compile e carregue o código `control_unit.c` no ESP32 para gerenciar a execução das ações de controle.

## Contribuições

Se você possui sugestões ou melhorias para o projeto, por favor, abra um *pull request* ou envie uma *issue*.

## Licença

Este projeto é licenciado sob a [Licença MIT](LICENSE).
