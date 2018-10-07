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
  RiseClockLCD(lcd);
  digitalWrite(lcd->DB7, (value >> 3) & 0x01);
  digitalWrite(lcd->DB6, (value >> 2) & 0x01);
  digitalWrite(lcd->DB5, (value >> 1) & 0x01);
  digitalWrite(lcd->DB4, value & 0x01);
  LowClockLCD(lcd);
}

void RiseClockLCD(const struct LCD *lcd)
{
  digitalWrite(lcd->EN, HIGH);
  delay(2);
}

void LowClockLCD(const struct LCD *lcd)
{
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

// Imprime uma string no display (deve terminar com caractere nulo)
void PrintLCD(const struct LCD *lcd, const char *str)
{
  SelectDataLCD(lcd);
  char c;
  while ((c = *str++))
    WriteByteLCD(lcd, c);
}


struct LCD lcd;
void setup() {
  Serial.begin(9600);
  InitLCD(&lcd, 8, 9, 7, 6, 5, 4);
  SetCursorLCD(&lcd, 0, 2);
  PrintLCD(&lcd, "Hello, world!");
}

void loop() {
  
}
