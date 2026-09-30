#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "ai_client.h"

#define PYTHON        "python3"
#define SCRIPT        "scripts/ai_client.py"
#define TIMEOUT_TICKS 75

static void escape_sq(const char *src, char *dst, int max) {
    int j = 0;
    for (int i = 0; src[i] && j < max - 4; i++) {
        if (src[i] == '\'') {
            dst[j++] = '\''; dst[j++] = '\\';
            dst[j++] = '\''; dst[j++] = '\'';
        } else {
            dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

int ai_consultar(const char *pergunta, char *resposta, int max_len) {
    resposta[0] = '\0';

    if (access(SCRIPT, F_OK) != 0)
        return -1;

    char escaped[1024];
    escape_sq(pergunta, escaped, sizeof(escaped));

    char cmd[2048];
    snprintf(cmd, sizeof(cmd), PYTHON " " SCRIPT " '%s' 2>/dev/null", escaped);

    FILE *f = popen(cmd, "r");
    if (!f) return -1;

    int fd = fileno(f);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    int total = 0, ticks = 0;
    while (total < max_len - 1 && ticks < TIMEOUT_TICKS) {
        char tmp[256];
        int n = (int)fread(tmp, 1, sizeof(tmp), f);
        if (n > 0) {
            if (total + n > max_len - 1) n = max_len - 1 - total;
            memcpy(resposta + total, tmp, n);
            total += n;
        }
        if (feof(f)) break;
        if (ferror(f) && errno != EAGAIN) break;
        usleep(80 * 1000);
        ticks++;
    }

    resposta[total] = '\0';
    while (total > 0 && (resposta[total - 1] == '\n' || resposta[total - 1] == '\r'))
        resposta[--total] = '\0';

    pclose(f);
    return (total > 0) ? 0 : -1;
}