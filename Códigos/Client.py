#!/usr/bin/env python3
import wave
import time
import json
import queue
import serial
import socket
import numpy as np
import threading
import sounddevice as sd
import RPi.GPIO as GPIO
from vosk import Model, KaldiRecognizer

MODEL_PATH       = "/home/pi/MODELO_VOSK/vosk-model-small-pt-0.3"
OUTPUT_WAV       = "recebido.wav"
CHANNELS         = 1
SAMPLE_RATE      = 16000
SAMPLE_WIDTH     = 2
WAKE_WORD        = "casa"
SILENCE_TIMEOUT  = 2
RMS_THRESHOLD    = 500
SERIAL_PORT      = "/dev/serial0"
SERIAL_BAUDRATE  = 115200

ser = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE)

def enviar_estado(estado):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(('localhost', 65432))
            s.sendall(str(estado).encode())
    except Exception as e:
        print(f"[Erro RGB] {e}")

def status_aguardando_wake_word():
    print("[+] Aguardando a wake word...")
    enviar_estado(1)  # Rosa

def status_wake_word_detectada(silence_timeout):
    print(f"[+] Wake word detectada, aguardando fala... Gravando até detectar silêncio por {silence_timeout}s")
    enviar_estado(2)  # Azul

def status_iniciando_transcricao():
    print("[+] Iniciando transcrição (partial) …")
    enviar_estado(3)  # Amarelo

q = queue.Queue()

def callback(indata, frames, time_info, status):
    if status:
        print(status)
    q.put(bytes(indata))

model = Model(MODEL_PATH)
COMMANDS = '["de", "do", "as", "olá", "casa", "acender", "ligar", "apagar", "desligar", "luz", "luzes", "todas", "portão", "abrir", "fechar", "garagem", "quarto", "casal", "dois", "sala", "estar", "quintal", "interno", "externo", "pedestre"]'

rec = KaldiRecognizer(model, SAMPLE_RATE, COMMANDS)

wake_word_detected = False
palavras = []

def rms(data_bytes):
    if len(data_bytes) < 2:
        return 0
    samples = np.frombuffer(data_bytes, dtype=np.int16)
    if samples.size == 0:
        return 0
    return np.sqrt(np.mean(samples.astype(np.float32)**2))

GPIO.setmode(GPIO.BCM)

pinos = {
    "QC": 21,
    "SE": 20,
    "GR": 16,
    "EX": 26,
    "Q2": 19,
    "QT": 13,
    "ST": 6,
    "IN": 5,
    "PE": 11,
    "AL": 9
}

for pin in pinos.values():
    GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

estado_anterior_alarme = GPIO.input(pinos["AL"])

def monitorar_gpio():
    global estado_anterior_alarme
    while True:
        if GPIO.input(pinos["QC"]) == GPIO.LOW:
            print("[GPIO21] → BUTTON:QC")
            ser.write(b"button:qc\n")
            time.sleep(0.3)

        if GPIO.input(pinos["SE"]) == GPIO.LOW:
            print("[GPIO20] → BUTTON:SE")
            ser.write(b"button:se\n")
            time.sleep(0.3)

        if GPIO.input(pinos["GR"]) == GPIO.LOW:
            print("[GPIO16] → BUTTON:GR")
            ser.write(b"button:gr\n")
            time.sleep(0.3)

        if GPIO.input(pinos["EX"]) == GPIO.LOW:
            print("[GPIO26] → GATE:EX")
            ser.write(b"gate:ex\n")
            time.sleep(0.3)

        if GPIO.input(pinos["Q2"]) == GPIO.LOW:
            print("[GPIO19] → BUTTON:Q2")
            ser.write(b"button:q2\n")
            time.sleep(0.3)

        if GPIO.input(pinos["QT"]) == GPIO.LOW:
            print("[GPIO13] → BUTTON:QT")
            ser.write(b"button:qt\n")
            time.sleep(0.3)

        if GPIO.input(pinos["ST"]) == GPIO.LOW:
            print("[GPIO6] → BUTTON:ST")
            ser.write(b"button:st\n")
            time.sleep(0.3)

        if GPIO.input(pinos["IN"]) == GPIO.LOW:
            print("[GPIO5] → GATE:IN")
            ser.write(b"gate:in\n")
            time.sleep(0.3)

        if GPIO.input(pinos["PE"]) == GPIO.LOW:
            print("[GPIO11] → GATE:PE")
            ser.write(b"gate:pe\n")
            time.sleep(0.3)

        estado_atual_alarme = GPIO.input(pinos["AL"])
        if estado_atual_alarme != estado_anterior_alarme:
            if estado_atual_alarme == GPIO.LOW:
                print("[GPIO9] → ALARM:L")
                ser.write(b"alarm:l\n")
            else:
                print("[GPIO9] → ALARM:D")
                ser.write(b"alarm:d\n")
            estado_anterior_alarme = estado_atual_alarme

        time.sleep(0.05)

status_aguardando_wake_word()
last_word_time = time.time()

threading.Thread(target=monitorar_gpio, daemon=True).start()

with sd.RawInputStream(samplerate=SAMPLE_RATE, blocksize=4096, dtype='int16',
                       channels=CHANNELS, callback=callback):
    while True:
        data = q.get()

        if rec.AcceptWaveform(data):
            result = json.loads(rec.Result())
            text = result.get("text", "").lower()
            if WAKE_WORD in text and not wake_word_detected:
                print(f"[+] Wake word detectada: {WAKE_WORD}")
                wake_word_detected = True
                palavras = []
                last_word_time = time.time()
        else:
            partial_result = rec.PartialResult()
            if partial_result and wake_word_detected:
                print(f"Parcial: {json.loads(partial_result).get('partial', '')}")
                last_word_time = time.time()

        if wake_word_detected:
            status_wake_word_detectada(SILENCE_TIMEOUT)

            wf = wave.open(OUTPUT_WAV, "wb")
            wf.setnchannels(CHANNELS)
            wf.setsampwidth(SAMPLE_WIDTH)
            wf.setframerate(SAMPLE_RATE)

            while True:
                try:
                    chunk = q.get(timeout=1)
                except queue.Empty:
                    continue

                wf.writeframes(chunk)

                if rms(chunk) < RMS_THRESHOLD:
                    if time.time() - last_word_time > SILENCE_TIMEOUT:
                        print("[+] Silêncio detectado. Parando a gravação.")
                        break
                else:
                    last_word_time = time.time()

            wf.close()

            status_iniciando_transcricao()

            wf = wave.open(OUTPUT_WAV, "rb")
            rec = KaldiRecognizer(model, wf.getframerate(), COMMANDS)

            palavras = []
            while True:
                data = wf.readframes(4000)
                if not data:
                    break
                if not rec.AcceptWaveform(data):
                    j = json.loads(rec.PartialResult())
                    texto = j.get("partial", "").strip()
                    if texto:
                        for w in texto.split():
                            if w not in palavras:
                                palavras.append(w)

            if palavras:
                comando = " ".join(palavras)
                print(f"[+] Enviando para o ESP: {comando}")
                ser.write((comando + "\n").encode())

            palavras = []
            wake_word_detected = False
            status_aguardando_wake_word()
