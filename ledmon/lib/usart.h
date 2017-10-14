/*
 * usart.h
 *
 * Description: USART header file
 */ 

/******************************************************************************
  Defines
******************************************************************************/
#define CR        ('\r')
#define LF        ('\n')
#define BACKSPACE ('\b')
#define SPACE     (' ')
#define ESC       (0x1B)

/******************************************************************************
  Function prototypes
******************************************************************************/
void          USART_init   (unsigned long int baud);
int           USART_putch  (char c);
unsigned char USART_getch  (void);
void          USART_putstr (char *p);
void          USART_getstr (char *p);
