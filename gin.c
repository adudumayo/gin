// INCLUDES
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

// DEFINES
#define CTRL_KEY(k) ((k) & 0x1f)

// DATA SEGMENT
struct termios orig_termios;

// TERMINAL
void die(const char *s) {
  write(STDIN_FILENO, "\x1b[2J", 4);
  write(STDIN_FILENO, "\x1b[H", 3);
  perror(s);
  exit(1);
} // Kill the program on error

void disableRawMode() {
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
} // Set the terminal back to original attribute values

void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
} // Disable some terminal attributes to build raw mode.

char editorReadKey() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
} // read keys from the editor 

// OUTPUT
void editorDrawRows() {
  int y;
  for(y = 0; y <= 10; y++) {
    write(STDIN_FILENO, "~\r\n", 3);
  }
}

void editorRefreshScreen() {
  write(STDIN_FILENO, "\x1b[2J", 4);
  write(STDIN_FILENO, "\x1b[H", 3);

  editorDrawRows();
  write(STDIN_FILENO, "\x1b[H", 3);
} // Clear the whole screen and reposition the cursor at the top left

// INPUT
void editorProcessKeypress() {
  char c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):
      write(STDIN_FILENO, "\x1b[2J", 4);
      write(STDIN_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
} // Main logic of what happens when a certain key is pressed

// INIT
int main() {
  enableRawMode(); 

  while(1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
}
