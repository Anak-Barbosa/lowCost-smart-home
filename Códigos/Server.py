#!/usr/bin/env python3
import socket
import time
import threading
from rpi_ws281x import PixelStrip, Color

LED_COUNT      = 132
LED_PIN        = 18
LED_FREQ_HZ    = 800000
LED_DMA        = 10
LED_BRIGHTNESS = 50
LED_INVERT     = False
LED_CHANNEL    = 0

strip = PixelStrip(LED_COUNT, LED_PIN, LED_FREQ_HZ, LED_DMA, LED_INVERT, LED_BRIGHTNESS, LED_CHANNEL)
strip.begin()

ROSA    = Color(255, 40, 20)
AMARELO = Color(255, 0, 255)
AZUL = Color(90, 255, 0)
APAGADO = Color(0, 0, 0)

def set_color(color):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, color)
    strip.show()

def efeito_onda(cor, delay=0.015):
    largura_onda = 20
    while efeito_ativo[0]:
        for pos in range(strip.numPixels() + largura_onda):
            if not efeito_ativo[0]:
                break
            for i in range(strip.numPixels()):
                dist = abs(i - pos)
                if dist < largura_onda:
                    brilho = 1 - (dist / largura_onda)
                    r = int(((cor >> 16) & 0xFF) * brilho)
                    g = int(((cor >> 8) & 0xFF) * brilho)
                    b = int((cor & 0xFF) * brilho)
                    strip.setPixelColor(i, Color(r, g, b))
                else:
                    strip.setPixelColor(i, APAGADO)
            strip.show()
            time.sleep(delay)

    set_color(APAGADO)

efeito_ativo = [False]
thread_efeito = None

def parar_efeito():
    global thread_efeito
    efeito_ativo[0] = False
    if thread_efeito is not None:
        thread_efeito.join()
        thread_efeito = None
    set_color(APAGADO)

def iniciar_servidor():
    global thread_efeito
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('localhost', 65432))
        s.listen()
        print("[LED] Servidor RGB ouvindo em localhost:65432")

        while True:
            conn, addr = s.accept()
            with conn:
                data = conn.recv(1024)
                if not data:
                    continue
                comando = data.decode().strip()
                print(f"[LED] Estado recebido: {comando}")

                parar_efeito()

                if comando == "1":
                    set_color(ROSA)
                elif comando == "2":
                    set_color(AZUL)
                elif comando == "3":
                    efeito_ativo[0] = True
                    thread_efeito = threading.Thread(target=efeito_onda, args=(AMARELO,))
                    thread_efeito.start()
                else:
                    print("[LED] Comando desconhecido")

if __name__ == "__main__":
    try:
        iniciar_servidor()
    except KeyboardInterrupt:
        parar_efeito()
        print("\n[LED] Servidor encerrado.")
