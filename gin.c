#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {
  enableRawMode();

  while(1) {
    char c = '\0';

    if(read(STDIN_FILENO, &c, 1) == -1) die("read stage");

    if (iscntrl(c)) {
      printf("%d  %zu Bytes\r\n", c, sizeof(c));
    } else {
      printf("%d (' %c ')  %zu Bytes\r\n", c, c, sizeof(c));
    }

    if (c == CTRL_KEY('q')) break;
  }
  return 0;
}
