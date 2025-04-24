#!/usr/bin/env python3
import socket
import wave
import time
import json
import numpy as np
from vosk import Model, KaldiRecognizer

MODEL_PATH = "/home/pi/MODELO_VOSK/vosk-model-pt-fb-v0.1.1-20220516_2113"
IP = "0.0.0.0"
PORT = 1234
OUTPUT_WAV = "recebido.wav"
CHANNELS = 1
SAMPLE_RATE = 16000
SAMPLE_WIDTH = 2
BUFFER_SIZE = 4096
WAKE_WORD = "casa"
SILENCE_TIMEOUT = 3.5
RMS_THRESHOLD = 500

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((IP, PORT))
sock.listen(1)
print(f"[+] Aguardando conexão em {IP}:{PORT} …")
conn, addr = sock.accept()
print(f"[+] Conectado por {addr}, aguardando wake word...")

model = Model(MODEL_PATH)
rec = KaldiRecognizer(model, SAMPLE_RATE)

wake_word_detected = False
palavras = []

def rms(data_bytes):
    if len(data_bytes) < 2:
        return 0
    samples = np.frombuffer(data_bytes, dtype=np.int16)
    if samples.size == 0:
        return 0
    return np.sqrt(np.mean(samples.astype(np.float32)**2))

last_word_time = time.time()

while True:
    data = conn.recv(BUFFER_SIZE)
    if not data:
        break

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
        print(f"[+] Wake word detectada, aguardando fala... Gravando até detectar silêncio por {SILENCE_TIMEOUT}s")
        wf = wave.open(OUTPUT_WAV, "wb")
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(SAMPLE_WIDTH)
        wf.setframerate(SAMPLE_RATE)

        while True:
            chunk = conn.recv(BUFFER_SIZE)
            if not chunk:
                break
            wf.writeframes(chunk)

            if time.time() - last_word_time > SILENCE_TIMEOUT:
                print("[+] Silêncio detectado. Parando a gravação.")
                break

        wf.close()

        print("[+] Iniciando transcrição (partial) …")
        wf = wave.open(OUTPUT_WAV, "rb")
        rec = KaldiRecognizer(model, wf.getframerate())

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

        print(f"[+] Palavras extraídas: {palavras}")
        palavras = []
        print(f"[+] Aguardando a wake word novamente...")
        wake_word_detected = False

conn.close()
sock.close()
