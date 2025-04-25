#!/bin/bash

# IP do Raspberry Pi (altere conforme necessário)
RASPBERRY_IP="000.000.000.000"
PORTA=1234

ffmpeg -f alsa -i default -ac 1 -ar 16000 -acodec pcm_s16le -f wav tcp://$RASPBERRY_IP:$PORTA
