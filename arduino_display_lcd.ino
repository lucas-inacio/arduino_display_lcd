struct LCD
{
  int RS, EN;
  int DB7, DB6, DB5, DB4;  
};

// Envia o nibble alto seguido no nibble baixo para o display
void WriteByteLCD(const struct LCD *lcd, byte value)
{
  WriteNibbleLCD(lcd, (value >> 4) & 0x0F);
  WriteNibbleLCD(lcd, value & 0x0F);
}

// Envia um nibble para o display
void WriteNibbleLCD(const struct LCD *lcd, byte value)
{
  digitalWrite(lcd->DB7, (value >> 3) & 0x01);
  digitalWrite(lcd->DB6, (value >> 2) & 0x01);
  digitalWrite(lcd->DB5, (value >> 1) & 0x01);
  digitalWrite(lcd->DB4, value & 0x01);
  ClockLCD(lcd);
}

void ClockLCD(const struct LCD *lcd)
{
  delay(2);
  digitalWrite(lcd->EN, HIGH);
  delay(2);
  digitalWrite(lcd->EN, LOW);
  delay(2);
}

// Seleciona o registrador de dados
void SelectDataLCD(const struct LCD *lcd)
{
  digitalWrite(lcd->RS, HIGH);
}

// Seleciona o registrador de instruções
void SelectInstructionLCD(const struct LCD *lcd)
{
  digitalWrite(lcd->RS, LOW);
}

// Configura o display LCD
void InitLCD(struct LCD *lcd, int RS, int EN, int DB7, int DB6, int DB5, int DB4)
{
  lcd->RS = RS;
  lcd->EN = EN;
  lcd->DB7 = DB7;
  lcd->DB6 = DB6;
  lcd->DB5 = DB5;
  lcd->DB4 = DB4;

  pinMode(lcd->RS, OUTPUT);
  pinMode(lcd->EN, OUTPUT);
  pinMode(lcd->DB7, OUTPUT);
  pinMode(lcd->DB6, OUTPUT);
  pinMode(lcd->DB5, OUTPUT);
  pinMode(lcd->DB4, OUTPUT);

  delay(50);

  // Configura display para trabalhar com 4 vias de dados
  SelectInstructionLCD(lcd);
  WriteNibbleLCD(lcd, 0x02); // Ativa o display para o modo de 4 bits
  WriteByteLCD(lcd, 0x28);   // quatro bits; duas linhas
  WriteByteLCD(lcd, 0x0F);   // Liga display, cursor piscante
  WriteByteLCD(lcd, 0x06);   // Deslocamento automático do cursor da direita para a esquerda. Scroll ligado.
  WriteByteLCD(lcd, 0x01);   // Limpa o display.
  WriteByteLCD(lcd, 0x80);   // Coloca o cursor no início da primeira linha
}

// Assume o uso de um display de 2 linhas e 16 colunas
void SetCursorLCD(const struct LCD *lcd, byte line, byte col)
{
  byte comando = (line > 0) ? 0xC0 : 0x80;
  comando |= col;
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, comando);
}

// Desloca o display. RL = 1 para direita ou 0 para esquerda
void ShiftDisplayLCD(const struct LCD *lcd, int RL)
{
  byte comando = (RL > 0) ? 0x1C : 0x18;
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, comando);
}

void ClearLCD(const struct LCD *lcd)
{
  SelectInstructionLCD(lcd);
  WriteByteLCD(lcd, 0x01);
}

// Imprime uma string no display (deve terminar com caractere nulo)
void PrintLCD(const struct LCD *lcd, const char *str)
{
  SelectDataLCD(lcd);
  char c;
  while ((c = *str++))
    WriteByteLCD(lcd, c);
}

// Código para leitura dos botões do shield LCD para arduino
const int RIGHT  = 0;
const int UP     = 1;
const int DOWN   = 2;
const int LEFT   = 3;
const int SELECT = 4;
const char *FRASES[] = {
    "RIGHT",
    "UP",
    "DOWN",
    "LEFT",
    "SELECT"
  };

int detectaBotao(int pino)
{
  int valor = analogRead(pino);
  if (valor <= 10) // Margem de erro para zero
    return 0;
  else if (valor <= 155)
    return 1;
  else if (valor <= 335)
    return 2;
  else if (valor <= 515)
    return 3;
  else if (valor <= 750)
    return 4;
  else return -1;
}

// Configuração inicial
struct LCD lcd;
void setup() {
  pinMode(A0, INPUT);
  InitLCD(&lcd, 8, 9, 7, 6, 5, 4);
  PrintLCD(&lcd, "Texto maroto");
}

// Loop principal
int textoAtual = -1;
void loop() {
  int novoTexto = detectaBotao(A0);
  Serial.println(novoTexto);
  if (textoAtual != novoTexto && novoTexto >= 0)
  {
    SelectInstructionLCD(&lcd);
    ClearLCD(&lcd);
    SetCursorLCD(&lcd, 0, 0);
    PrintLCD(&lcd, FRASES[novoTexto]);
    textoAtual = novoTexto;
  }
  delay(100);
}
