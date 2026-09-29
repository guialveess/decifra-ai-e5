#!/usr/bin/env python3
"""
Servidor de inferência local do modelo LOGI.

Usa llama-cpp-python para carregar o modelo GGUF (guiiwfz/logi-1.5b-gguf)
e responde requisições POST /ask com dicas didáticas em português.

Uso:
    python3 scripts/local_server.py

O servidor sobe na porta 8787. Sem ele, o jogo usa dicas estáticas como fallback.
"""

import json
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer

from llama_cpp import Llama

MODEL_REPO   = "guiiwfz/logi-1.5b-gguf"
MODEL_FILE   = "*Q4_K_M.gguf"   # menor e mais rápido; use *Q6_K.gguf para mais qualidade
PORT         = 8787
CONTEXT_SIZE = 1024
TEMPERATURE  = 0.2

SYSTEM_PROMPT = (
    "Voce e LOGI, tutor do jogo educativo Decifra.IA. "
    "Seu escopo inclui: "
    "logica proposicional: AND, OR, NOT, IMPLICA, BICONDICIONAL, tabelas-verdade, "
    "equivalencias, falacias e regras de inferencia; "
    "alfabetizacao em IA: modelos, dados, treino, avaliacao, vies, alucinacao, RAG, "
    "privacidade, etica e uso responsavel; "
    "seguranca digital relacionada ao uso de IA: phishing, deepfakes, engenharia social "
    "e protecao de dados. "
    "Responda em portugues, com linguagem simples e correta. "
    "Em calculos de logica, mostre os passos. "
    "Nas outras perguntas, responda de forma direta e didatica. "
    "Nao invente fontes, leis, numeros ou capacidades. "
    "Quando o tema estiver fora do escopo, diga isso brevemente e redirecione "
    "para logica, IA ou seguranca no uso de IA. "
    "Voce e um modelo local e nao acessa a internet em tempo real."
)

print("Carregando modelo LOGI (GGUF)...", flush=True)

llm = Llama.from_pretrained(
    repo_id=MODEL_REPO,
    filename=MODEL_FILE,
    n_ctx=CONTEXT_SIZE,
    n_gpu_layers=-1,   # usa GPU/Metal se disponível; 0 para forçar CPU
    verbose=False,
)

print(f"Modelo carregado. Servidor pronto na porta {PORT}.", flush=True)

lock = threading.Lock()


def gerar(pergunta: str) -> str:
    """Gera uma dica didática para o enunciado do desafio."""
    with lock:
        resultado = llm.create_chat_completion(
            messages=[
                {"role": "system", "content": SYSTEM_PROMPT},
                {"role": "user",   "content": pergunta},
            ],
            temperature=TEMPERATURE,
            max_tokens=300,
        )
    return resultado["choices"][0]["message"]["content"]


class Handler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass  # silencia logs de acesso no terminal

    def do_POST(self):
        if self.path != "/ask":
            self.send_response(404)
            self.end_headers()
            return

        length   = int(self.headers.get("Content-Length", 0))
        body     = json.loads(self.rfile.read(length))
        pergunta = body.get("pergunta", "")

        try:
            resposta = gerar(pergunta)
        except Exception as e:
            resposta = f"Erro na geracao: {e}"

        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps({"resposta": resposta}).encode())


if __name__ == "__main__":
    server = HTTPServer(("127.0.0.1", PORT), Handler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServidor encerrado.")
