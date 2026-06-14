#include <SPI.h>
#include <MFRC522.h>
#include <DFRobotDFPlayerMini.h>

// =====================================================
// DEFINIÇÃO DOS PINOS
// =====================================================
#define SS_PIN 10
#define RST_PIN 9

const int BTN_FAZENDA   = 23;
const int BTN_ZOOLOGICO = 25;
const int BTN_AQUARIO   = 27;
const int BTN_PAUSE     = 29;
const int BTN_REBOBINAR = 31;
const int BTN_ON_OFF    = 33; // Novo botão de Ligar/Desligar
const int POT_VOLUME    = A0;

MFRC522 rfid(SS_PIN, RST_PIN);
DFRobotDFPlayerMini player;

struct Animal {
  const char* nome;
  byte uid[4];
  int audioNome;
  int audioSom;
};

// Variáveis de controle
bool sistemaLigado = true; // O sistema começa ligado por padrão
bool jogoAtivo = false;
bool pausado = false;
int volumeAtual = -1;
Animal animalAtual;

int indiceAnimalAtual = 0;
int ambienteAtivo = 0;

// =====================================================
// BANCO DE DADOS
// =====================================================

// FAZENDA
Animal fazenda[] = {
  {"Pato",     {0x05, 0x68, 0x8F, 0xBD}, 18, 19},
  {"Ovelha",   {0xE4, 0x03, 0x67, 0xC6}, 16, 17},
  {"Porco",    {0x55, 0x27, 0x1B, 0xC1}, 14, 15},
  {"Galinha",  {0x84, 0x1E, 0xF0, 0xC0}, 10, 11},
  {"Vaca",     {0x09, 0xE5, 0x61, 0xA4}, 12, 13}
};

// ZOOLÓGICO
Animal zoologico[] = {
  {"Elefante", {0xB4, 0x27, 0x68, 0xC0}, 26, 27},
  {"Girafa",   {0xC5, 0xFC, 0x3D, 0xBE}, 20, 21},
  {"Leao",     {0x99, 0x81, 0xE1, 0xB2}, 22, 23},
  {"Macaco",   {0x95, 0x6A, 0x7A, 0xBC}, 24, 25},
  {"Tucano",   {0xB9, 0x40, 0xC3, 0xAC}, 28, 29}
};

// AQUÁRIO
Animal aquario[] = {
  {"Baleia",    {0xD5, 0x2A, 0xAC, 0xBC}, 30, 31},
  {"Golfinho",  {0xD4, 0xC1, 0x3F, 0xC6}, 32, 33},
  {"Tartaruga", {0x1A, 0xAD, 0xBB, 0xC1}, 34, 35},
  {"Polvo",     {0xA5, 0x76, 0x03, 0xBC}, 36, 37},
  {"Foca",      {0x8A, 0xDA, 0x19, 0xCB}, 38, 39}
};

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  Serial3.begin(9600);

  if (!player.begin(Serial3)) {
    Serial.println("Erro no DFPlayer.");
    while (true);
  }

  pinMode(BTN_FAZENDA, INPUT_PULLUP);
  pinMode(BTN_ZOOLOGICO, INPUT_PULLUP);
  pinMode(BTN_AQUARIO, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BTN_REBOBINAR, INPUT_PULLUP);
  pinMode(BTN_ON_OFF, INPUT_PULLUP); // Configura o novo botão como entrada interna pull-up

  atualizarVolume();

  player.playMp3Folder(1); // 0001.mp3 - Áudio de introdução/boas-vindas
  Serial.println("Pronto!");
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  verificarBotaoOnOff(); // Sempre verifica o botão de ligar/desligar

  if (!sistemaLigado) {
    return; // Se o sistema estiver desligado, ignora o resto do loop
  }

  atualizarVolume();
  verificarBotoes();

  if (jogoAtivo && !pausado) {
    verificarNFC();
  }
}

// =====================================================
// FUNÇÕES
// =====================================================

void verificarBotaoOnOff() {
  if (digitalRead(BTN_ON_OFF) == LOW) {
    sistemaLigado = !sistemaLigado; // Inverte o estado atual do sistema

    if (!sistemaLigado) {
      // Ações ao DESLIGAR
      Serial.println("Sistema Desligado.");
      player.stop(); // Para qualquer música imediatamente
      
      // Reseta as variáveis do jogo
      jogoAtivo = false;
      pausado = false;
      ambienteAtivo = 0;
      indiceAnimalAtual = 0;
    } else {
      // Ações ao LIGAR novamente
      Serial.println("Sistema Ligado.");
      player.playMp3Folder(1); // Toca o áudio inicial novamente (opcional)
    }

    delay(500); // Debounce para evitar múltiplos cliques fantasmas
  }
}

void atualizarVolume() {
  int leitura = analogRead(POT_VOLUME);
  int novoVolume = map(leitura, 0, 1023, 0, 30);

  if (abs(novoVolume - volumeAtual) > 1) {
    volumeAtual = novoVolume;
    player.volume(volumeAtual);
  }
}

void verificarBotoes() {
  if (digitalRead(BTN_PAUSE) == LOW) {
    pausado = !pausado;

    if (pausado)
      player.pause();
    else
      player.start();

    delay(400);
  }

  if (digitalRead(BTN_REBOBINAR) == LOW) {
    if (jogoAtivo && !pausado)
      tocarAnimal();

    delay(400);
  }

  if (digitalRead(BTN_FAZENDA) == LOW) {
    iniciarNarrativa(1);
    delay(400);
  }

  else if (digitalRead(BTN_ZOOLOGICO) == LOW) {
    iniciarNarrativa(2);
    delay(400);
  }

  else if (digitalRead(BTN_AQUARIO) == LOW) {
    iniciarNarrativa(3);
    delay(400);
  }
}

void iniciarNarrativa(int ambiente) {
  ambienteAtivo = ambiente;
  indiceAnimalAtual = 0;
  carregarAnimalDaVez();
}

void carregarAnimalDaVez() {
  if (indiceAnimalAtual > 4) {
    if (ambienteAtivo == 1) player.playMp3Folder(4);
    else if (ambienteAtivo == 2) player.playMp3Folder(5);
    else if (ambienteAtivo == 3) player.playMp3Folder(6);

    jogoAtivo = false;
    ambienteAtivo = 0;
    return;
  }

  if (ambienteAtivo == 1)      animalAtual = fazenda[indiceAnimalAtual];
  else if (ambienteAtivo == 2) animalAtual = zoologico[indiceAnimalAtual];
  else if (ambienteAtivo == 3) animalAtual = aquario[indiceAnimalAtual];

  jogoAtivo = true;
  pausado = false;

  tocarAnimal();
}

void tocarAnimal() {
  player.playMp3Folder(animalAtual.audioSom);
}

void verificarNFC() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  bool correto = true;
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != animalAtual.uid[i]) {
      correto = false;
    }
  }

  if (correto) {
    player.playMp3Folder(2);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(4000);

    indiceAnimalAtual++;
    carregarAnimalDaVez();
    return;
  } else {
    player.playMp3Folder(3);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1500);
}