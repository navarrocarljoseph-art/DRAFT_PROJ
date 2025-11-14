#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

/* ---------- GLOBALS ---------- */
char lastN[50], firstN[100], middleN[50];
char birthD[20], sex[10];
char house[80], barangay[50], city[50], province[50];

/* -------------------------------------------------------------------
   Console helpers
   ------------------------------------------------------------------- */
void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void showCursor(int visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hConsole, &ci);
    ci.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(hConsole, &ci);
}

void clearBox(int x, int y, int width) {
    gotoxy(x, y);
    for (int i = 0; i < width; i++) putchar(' ');
    gotoxy(x, y);
}

/* -------------------------------------------------------------------
   Simple ASCII helpers (no <ctype.h>)
   ------------------------------------------------------------------- */
void toLowerStr(char *s) {
    for (int i = 0; s[i]; i++)
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
}

void capitalize(char *s) {
    if (!s || !s[0]) return;
    if (s[0] >= 'a' && s[0] <= 'z') s[0] -= 32;
    for (int i = 1; s[i]; i++)
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 32;
}

int isDigitChar(char c) {
    return (c >= '0' && c <= '9');
}

/* -------------------------------------------------------------------
   Loading screen
   ------------------------------------------------------------------- */
void loading_screen() {
    const int width = 40;
    char block = 178;
    srand((unsigned)time(NULL));

    system("cls");
    gotoxy(10, 6); printf("Loading...\n\n");

    gotoxy(10, 9);
    printf("[");
    for (int i = 0; i < width; i++) putchar(' ');
    printf("] 0%%");

    for (int i = 0; i <= width; i++) {
        gotoxy(11, 9);
        for (int j = 0; j < i; j++) putchar(block);
        for (int j = i; j < width; j++) putchar(' ');

        gotoxy(11 + width + 2, 9);
        printf("%3.0f%%", (i / (float)width) * 100);

        Sleep(25 + (rand() % 20));
    }

    gotoxy(10, 11);
    printf("Loading complete!");
    Sleep(500);
}

/* -------------------------------------------------------------------
   Input box function (typing inside ASCII box)
   ------------------------------------------------------------------- */
void inputBox(int x, int y, char *buf, int len) {
    int i = 0;
    int ch;
    memset(buf, 0, len);

    gotoxy(x, y);
    showCursor(1);

    while (1) {
        ch = getch();

        if (ch == 13) break;        // ENTER
        if (ch == 8) {              // BACKSPACE
            if (i > 0) {
                i--;
                buf[i] = 0;
                gotoxy(x + i, y);
                putchar(' ');
                gotoxy(x + i, y);
            }
        }
        else if (ch >= 32 && ch <= 126 && i < len - 1) {
            buf[i++] = ch;
            putchar(ch);
        }
    }

    buf[i] = '\0';
    showCursor(0);
}

/* -------------------------------------------------------------------
   Validation
   ------------------------------------------------------------------- */
int validateName(const char *s) {
    return s && strlen(s) >= 2;
}

int validateDate(int m, int d, int y) {
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    if (m == 2 && d > 29) return 0;
    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) return 0;
    return 1;
}

int validateSex(const char *s) {
    return (strcmp(s, "Male") == 0 || strcmp(s, "Female") == 0);
}

/* -------------------------------------------------------------------
   STEP 1 — Draw static form
   ------------------------------------------------------------------- */
void drawForm() {
    system("cls");
    printf("\n\n");
    printf("                             +--------------------------------------------------+\n");
    printf("                             |              STEP 1 - CREATE ACCOUNT             |\n");
    printf("                             +--------------------------------------------------+\n\n");

    printf("  Last Name                 First Name                         Middle Name\n");
    printf("  +---------------------+   +--------------------------------+   +---------------------+\n");
    printf("  |                     |   |                                |   |                     |\n");
    printf("  +---------------------+   +--------------------------------+   +---------------------+\n\n");

    printf("  Birth Date (MM / DD / YYYY)\n");
    printf("  +------+   +------+   +--------+\n");
    printf("  |      |   |      |   |        |\n");
    printf("  +------+   +------+   +--------+\n\n");

    printf("  Sex (Male/Female)\n");
    printf("  +---------------------------+\n");
    printf("  |                           |\n");
    printf("  +---------------------------+\n\n");

    printf("  Address Information\n");
    printf("  House & Street              Barangay               City/Municipality           Province\n");
    printf("  +----------------------+   +-------------------+   +----------------------+   +-------------------+\n");
    printf("  |                      |   |                   |   |                      |   |                   |\n");
    printf("  +----------------------+   +-------------------+   +----------------------+   +-------------------+\n");

    printf("\n  (Press ENTER to move between fields)\n");
}

/* -------------------------------------------------------------------
   STEP 1 – createAccount()
   ------------------------------------------------------------------- */
void account_info();  // forward declaration
void position_choices(); // stub

void createAccount() {

START_STEP1:

    drawForm();
    showCursor(1);

    /* ------------ NAME ------------ */
NAME_RETRY:

    clearBox(4, 8, 20);
    clearBox(30, 8, 31);
    clearBox(67, 8, 20);

    gotoxy(4, 8);  inputBox(4, 8, lastN, sizeof(lastN));
    gotoxy(30, 8); inputBox(30, 8, firstN, sizeof(firstN));
    gotoxy(67, 8); inputBox(67, 8, middleN, sizeof(middleN));

    if (!validateName(lastN) || !validateName(firstN) || !validateName(middleN)) {
        gotoxy(1, 27); printf("(System): Each name must be at least 2 characters.");
        getch();
        clearBox(1, 27, 90);
        goto NAME_RETRY;
    }

    /* ------------ DATE ------------ */
DATE_RETRY:;
    char mS[5], dS[5], yS[6];
    int m, d, y;

    clearBox(4, 13, 5);
    clearBox(15, 13, 5);
    clearBox(26, 13, 7);

    gotoxy(4, 13);  inputBox(4, 13, mS, sizeof(mS));
    gotoxy(15, 13); inputBox(15, 13, dS, sizeof(dS));
    gotoxy(26, 13); inputBox(26, 13, yS, sizeof(yS));

    m = atoi(mS); d = atoi(dS); y = atoi(yS);

    if (!validateDate(m, d, y)) {
        gotoxy(1, 27); printf("(System): Invalid date. Try again.");
        getch();
        clearBox(1, 27, 90);
        goto DATE_RETRY;
    }

    sprintf(birthD, "%02d/%02d/%04d", m, d, y);

    /* ------------ SEX ------------ */
SEX_RETRY:

    clearBox(4, 18, 26);
    gotoxy(4, 18); inputBox(4, 18, sex, sizeof(sex));

    capitalize(sex);

    if (!validateSex(sex)) {
        gotoxy(1, 27); printf("(System): Enter 'Male' or 'Female'.");
        getch();
        clearBox(1, 27, 90);
        goto SEX_RETRY;
    }

    /* ------------ ADDRESS ------------ */
    clearBox(4, 24, 21);
    clearBox(30, 24, 18);
    clearBox(54, 24, 22);
    clearBox(82, 24, 18);

    gotoxy(4, 24);  inputBox(4, 24, house, sizeof(house));
    gotoxy(30, 24); inputBox(30, 24, barangay, sizeof(barangay));
    gotoxy(54, 24); inputBox(54, 24, city, sizeof(city));
    gotoxy(82, 24); inputBox(82, 24, province, sizeof(province));

    /* ------------ NAVIGATION ------------ */
NAV1:
    gotoxy(1, 33);
    printf("  All inputs captured successfully!\n");

    printf("\n\n\t\t\t+-----------------+      +-----------------+\n");
    printf("\t\t\t|     [N] Next    |      |     [B] Back    |\n");
    printf("\t\t\t+-----------------+      +-----------------+\n");

    char c = getch();
    if (c >= 'A' && c <= 'Z') c += 32;

    if (c == 'n') { loading_screen(); account_info(); return; }
    if (c == 'b') goto START_STEP1;

    gotoxy(1, 40);
    printf("Invalid key. Press N or B.");
    Sleep(500);
    clearBox(1, 40, 90);
    goto NAV1;
}

/* -------------------------------------------------------------------
   STEP 2 – Account Information (from your working version)
   ------------------------------------------------------------------- */
void account_info() {

    char email[100];
    char contactN[20];
    char passW[50];
    char passConfirm[50];
    char ch;
    int i;

RELOAD_UI:
    system("cls");
    printf("\n\n");
    printf("                             +--------------------------------------------------+\n");
    printf("                             |           STEP 2 - ACCOUNT INFORMATION          |\n");
    printf("                             +--------------------------------------------------+\n\n");

    printf("  Email Address\n");
    printf("  +--------------------------------------------------+\n");
    printf("  |                                                  |\n");
    printf("  +--------------------------------------------------+\n\n");

    printf("  Contact Number (11 digits, starts with 09)\n");
    printf("  +---------------------+\n");
    printf("  |                     |\n");
    printf("  +---------------------+\n\n");

    printf("  Password (min 8 characters)\n");
    printf("  +---------------------+\n");
    printf("  |                     |\n");
    printf("  +---------------------+\n\n");

    printf("  Confirm Password\n");
    printf("  +---------------------+\n");
    printf("  |                     |\n");
    printf("  +---------------------+\n\n");

    /* ------------ EMAIL ------------ */
EMAIL:
    clearBox(4, 8, 49);
    gotoxy(4, 8);
    inputBox(4, 8, email, sizeof(email));

    if (!strchr(email, '@') || !strchr(email, '.')) {
        gotoxy(1, 25); printf("(System): Invalid email!");
        getch();
        clearBox(1, 25, 80);
        goto EMAIL;
    }

    /* simulate duplicate */
    if (strcmp(email, "test@example.com") == 0) {
        gotoxy(1, 25); printf("(System): Email already exists.");
        getch();
        clearBox(1, 25, 80);
        goto EMAIL;
    }

    /* ------------ CONTACT ------------ */
CONTACT:
    clearBox(4, 13,20);
    gotoxy(4,17);
    inputBox(4, 13, contactN, sizeof(contactN));

    if (strlen(contactN) != 11 || contactN[0] != '0' || contactN[1] != '9') {
        gotoxy(1, 25);
        printf("(System): Invalid phone number.");
        getch();
        clearBox(1, 25, 80);
        goto CONTACT;
    }

    /* ------------ PASSWORD ------------ */
PASSWORD:
    clearBox(4, 18, 20);
    clearBox(4, 18, 20);

    /* Password typing */
    gotoxy(4, 18);
    memset(passW, 0, sizeof(passW));
    i = 0;

    while ((ch = getch()) != '\r') {
        if (ch == 8 && i > 0) { i--; printf("\b \b"); }
        else if (ch >= 32 && ch <= 126 && i < sizeof(passW)-1) {
            passW[i++] = ch;
            printf("*");
        }
    }

    if (strlen(passW) < 8) {
        gotoxy(1, 30); printf("(System): Password too short.");
        getch();
        clearBox(1, 30, 80);
        goto PASSWORD;
    }

    /* Confirm */
    gotoxy(4, 23);
    memset(passConfirm, 0, sizeof(passConfirm));
    i = 0;

    while ((ch = getch()) != '\r') {
        if (ch == 8 && i > 0) { i--; printf("\b \b"); }
        else if (ch >= 32 && ch <= 126 && i < sizeof(passConfirm)-1) {
            passConfirm[i++] = ch;
            printf("*");
        }
    }

    if (strcmp(passW, passConfirm) != 0) {
        gotoxy(1, 25); printf("(System): Passwords do not match.");
        getch();
        clearBox(1, 25, 80);
        goto PASSWORD;
    }

    /* ------------ NAV ------------ */
NAV2:

    /* Move cursor to a clean area below the form */
    gotoxy(1, 23);
    clearBox(1, 23, 120);
    gotoxy(1, 24);
    clearBox(1, 24, 120);
    gotoxy(1, 25);
    clearBox(1, 25, 120);

    gotoxy(20, 23);
    printf("+-----------------+      +-----------------+");
    gotoxy(20, 24);
    printf("|     [N] Next    |      |     [B] Back    |");
    gotoxy(20, 25);
    printf("+-----------------+      +-----------------+");

    /* read key */
    ch = getch();
    if (ch >= 'A' && ch <= 'Z') ch += 32;

    if (ch == 'n') {
        loading_screen();
        position_choices();
        return;
    }
    if (ch == 'b') { 
        createAccount();
        return; 
    }

    gotoxy(1, 27);
    printf("Invalid key!");
    Sleep(500);
    clearBox(1, 27, 80);
    goto NAV2;
}

/* -------------------------------------------------------------------
   Placeholder STEP 3
   ------------------------------------------------------------------- */
void position_choices() {
    system("cls");
    printf("\n\nSTEP 3 SCREEN (placeholder)\n\n");
    printf("Press any key to exit...");
    getch();
}

/* -------------------------------------------------------------------
   MAIN
   ------------------------------------------------------------------- */
int main(void) {
    showCursor(1);
    createAccount();
    return 0;
}

