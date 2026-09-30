#ifndef AI_CLIENT_H
#define AI_CLIENT_H

#define AI_RESP_MAX 1024

int ai_consultar(const char *pergunta, char *resposta, int max_len);

#endif