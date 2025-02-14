//
// Created by Zlatan Ljutika on 14. 2. 2025..
//
/*
Napisati funkciju char* sf(char *s, ...) koja prima string i na mjestima gdje se
nalazi modifikator %, mjenja taj dio stringa na adekvatan nacin.
Moguci modifikatori su:
%d - int
%s - string
%c - char
%% - sam %
funkcija vraca pokazivac na izmjenjeni string.

Napomena: Ne smije se kreirati pomocni niz ili string
*/


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define memDump                                                                \
  for (int k = 0; k < 100; k++)                                                \
    printf("%c", s[k]);                                                        \
  printf("\n");

int prebrojmjestacifri(int a) {
  int br = 0;
  if (a < 0)
    br++;
  if (a >= 0 && a < 10)
    br++;
  else {
    while (a != 0) {
      br++;
      a /= 10;
    }
  }
  return br;
}

char *sf(char *s, ...) {
  va_list args;
  va_start(args, s);

  char *next = s;
  while (*next != 0) {
    if (*next == '%') {
      if (*++next != 0) {
        if (*next == '%') {
          /* Izbaci trenutni element */
          char *tmp = next;
          while (*tmp != 0) {
            tmp++;
            *(tmp - 1) = *tmp;
          }
        } else if (*next == 'd') {
          int a = va_arg(args, int);
          int br_mjesta = prebrojmjestacifri(a);

          /* 1 cifra */
          if (br_mjesta == 1) {
            next--;
            *next++ = a + '0';
            char *tmp = next;
            while (*tmp != 0) {
              tmp++;
              *(tmp - 1) = *tmp;
            }
            next--;
          } else if (br_mjesta == 2) {
            if (a < 0) {
              *--next = '-';
              *++next = (-a) + '0';
            } else {
              *next-- = (a % 10) + '0';
              *next = (a / 10) + '0';
            }

          } else {
            --next;
            int delta = br_mjesta - 2;
            if (a < 0) {
              *next++ = '-';
              br_mjesta--;
              a *= -1;
              delta = br_mjesta - 1;
            }
            char *tmp = next;
            tmp += br_mjesta - 1;
            char *end = next;
            while (*end != 0) {
              end++;
            }

            while (end + delta >= next) {
              *(end + delta) = *end;
              end--;
            }
            while (a != 0) {

              *tmp = (a % 10) + '0';
              a /= 10;
              tmp--;
            }
          }

        } else if (*next == 's') {
          char *s1 = va_arg(args, char *);
          int l1 = strlen(s1);
          if (l1 == 0) {
            /* samo shift sve u lijevo */
            char *tmp = next;
            --next;
            while (*tmp != 0) {
              tmp++;
              *(tmp - 2) = *tmp;
            }

          } else if (l1 == 2) {
            *--next = *s1++;
            *++next = *s1;
          } else if (l1 == 1) {
            char *tmp = next;
            while (*tmp != 0) {
              tmp++;
              *(tmp - 1) = *tmp;
            }
            *--next = *s1;
          } else {
            /* Ako je više od 2 slova, shift ostatak rijeci u desno za l1-2 */
            next--;
            char *end = next;
            int delta = l1 - 2;
            while (*end != '\0') {
              end++;
            }

            while (end + delta >= next) {
              *(end + delta) = *end;
              end--;
            }
            while (*s1 != 0) {
              *next++ = *s1++;
            }
          }
        }
        next++;
        continue;
      }
    }
    next++;
  }
  /* memDump; */
  va_end(args);
  return s;
}

/*remove 'dont' to compile main()*/
#define dontRUN

#ifdef RUN
int _main() {
  char text[250] = "Moje ime je %s, imam %d godina. Grupa sam %s-%d%s, volim "
                   "znak %%, %si jos %d stvari (mozda i %d).";
  printf("%s\n", sf(text, "Zlatan", 20, "RI", 2, "b", "", 123, -44123));
  return 0;
}
#endif
#undef RUN