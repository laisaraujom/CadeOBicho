# 🐾 Cadê o Bicho?

Protótipo educativo baseado em Arduino que utiliza tags **RFID** e um módulo de áudio **DFPlayer Mini** para criar um jogo sonoro interativo: a criança escolhe um ambiente (Fazenda, Zoológico ou Aquário) e precisa encontrar, entre as figuras de animais, aquela que corresponde ao som tocado, aproximando-a do leitor RFID.

> 📁 Este repositório contém: imagens do protótipo (`/imagens`), o código-fonte (`/codigo`), o esquema de ligações (`/esquematico`), este README e o [Artigo Tutorial](ARTIGO_TUTORIAL.md) com o passo a passo completo de reprodução.

---

## Sumário

- [Visão geral](#visão-geral)
- [Materiais necessários](#materiais-necessários)
- [Esquema de ligações (pinos)](#esquema-de-ligações-pinos)
- [Bibliotecas necessárias](#bibliotecas-necessárias)
- [Preparando o cartão microSD (áudios)](#preparando-o-cartão-microsd-áudios)
- [Identificando as tags RFID dos animais](#identificando-as-tags-rfid-dos-animais)
- [Carregando o código no Arduino](#carregando-o-código-no-arduino)
- [Como usar (controles)](#como-usar-controles)
- [Estrutura do repositório](#estrutura-do-repositório)
- [Possíveis melhorias futuras](#possíveis-melhorias-futuras)
- [Licença](#licença)

---

## Visão geral

- Um **leitor RFID (MFRC522)** identifica tags coladas em figuras de animais, impressos em 3D e texturizados pela equipe.
- Um **DFPlayer Mini** toca arquivos MP3 de um cartão microSD: nome/som de cada animal, mensagens de acerto/erro e finalização de cada ambiente.
- **3 botões** selecionam o ambiente (Fazenda, Zoológico, Aquário).
- **1 botão** pausa/retoma o áudio, **1 botão** repete o som do animal atual e **1 botão** liga/desliga todo o sistema.
- Um **potenciômetro** controla o volume em tempo real.

Cada ambiente tem 5 animais. Ao acertar o animal correto, o jogo avança automaticamente para o próximo; ao completar os 5, toca um áudio de finalização daquele ambiente.

---

## Materiais necessários

| Item | Observação |
|---|---|
| Arduino Mega 2560 | 
| Módulo leitor RFID MFRC522 | Opera em 3.3V — **não ligar em 5V** |
| Tags/cartões RFID (mínimo 15) | Uma para cada animal (5 por ambiente x 3 ambientes) |
| Módulo DFPlayer Mini | + cartão microSD (até 32GB, formatado em FAT16/FAT32) |
| Alto-falante 3–8 Ω, até 3W | Conectado ao DFPlayer |
| 6x botões (push button) | Seleção de ambiente (3x), pause, rebobinar e liga/desliga |
| 1x potenciômetro 10kΩ | Controle de volume |
| Resistor 1kΩ | Recomendado na linha RX do DFPlayer (protege o pino do Arduino) |
| Protoboard e jumpers | Para montagem |
| Fonte de alimentação 5V | USB ou fonte externa, dependendo do consumo do alto-falante |

---

## Esquema de ligações (pinos)

> Veja também o diagrama em `/esquematico/esquema_ligacoes.svg`.

### Leitor RFID MFRC522 (SPI)

| Pino do MFRC522 | Pino no Arduino Mega |
|---|---|
| SDA (SS) | 10 |
| SCK | 52 |
| MOSI | 51 |
| MISO | 50 |
| RST | 9 |
| 3.3V | 3.3V |
| GND | GND |
| IRQ | não conectado |

### DFPlayer Mini

| Pino do DFPlayer | Pino no Arduino Mega |
|---|---|
| VCC | 5V |
| GND | GND |
| RX | TX3 (pino 14) — com resistor de 1kΩ em série |
| TX | RX3 (pino 15) |
| SPK_1 / SPK_2 | Alto-falante |

### Botões (todos com `INPUT_PULLUP` — uma perna no pino, outra no GND)

| Botão | Pino |
|---|---|
| Fazenda | 23 |
| Zoológico | 25 |
| Aquário | 27 |
| Pause/Play | 29 |
| Rebobinar (repetir som) | 31 |
| Ligar/Desligar | 33 |

### Potenciômetro de volume

| Terminal | Conexão |
|---|---|
| Pino 1 | 5V |
| Pino 2 (cursor/wiper) | A0 |
| Pino 3 | GND |

---

## Bibliotecas necessárias

Instale pela IDE do Arduino em **Sketch → Incluir Biblioteca → Gerenciar Bibliotecas**:

- `MFRC522` (by GithubCommunity / miguelbalboa)
- `DFRobotDFPlayerMini` (by DFRobot)

A biblioteca `SPI` já vem incluída na IDE do Arduino.

---

## Preparando o cartão microSD (áudios)

O DFPlayer Mini, ao usar `playMp3Folder(N)`, busca o arquivo dentro de uma pasta chamada **`mp3`** (minúsculo) na raiz do cartão, com nomes no formato `0001.mp3`, `0002.mp3`, etc.

Estrutura esperada do cartão:

```
SD_CARD/
└── mp3/
    ├── 0001.mp3   → Introdução / boas-vindas
    ├── 0002.mp3   → Som de "acerto"
    ├── 0003.mp3   → Som de "erro / tente novamente"
    ├── 0004.mp3   → Finalização do ambiente Fazenda
    ├── 0005.mp3   → Finalização do ambiente Zoológico
    ├── 0006.mp3   → Finalização do ambiente Aquário
    ├── 0010.mp3 / 0011.mp3 → Galinha (nome / som)
    ├── 0012.mp3 / 0013.mp3 → Vaca (nome / som)
    ├── 0014.mp3 / 0015.mp3 → Porco (nome / som)
    ├── 0016.mp3 / 0017.mp3 → Ovelha (nome / som)
    ├── 0018.mp3 / 0019.mp3 → Pato (nome / som)
    ├── 0020.mp3 / 0021.mp3 → Girafa (nome / som)
    ├── 0022.mp3 / 0023.mp3 → Leão (nome / som)
    ├── 0024.mp3 / 0025.mp3 → Macaco (nome / som)
    ├── 0026.mp3 / 0027.mp3 → Elefante (nome / som)
    ├── 0028.mp3 / 0029.mp3 → Tucano (nome / som)
    ├── 0030.mp3 / 0031.mp3 → Baleia (nome / som)
    ├── 0032.mp3 / 0033.mp3 → Golfinho (nome / som)
    ├── 0034.mp3 / 0035.mp3 → Tartaruga (nome / som)
    ├── 0036.mp3 / 0037.mp3 → Polvo (nome / som)
    └── 0038.mp3 / 0039.mp3 → Foca (nome / som)
```

## Identificando as tags RFID dos animais

Cada tag precisa ter seu UID (4 bytes) cadastrado no código, no array correspondente (`fazenda[]`, `zoologico[]` ou `aquario[]`).

Para descobrir o UID de cada tag:

1. Abra na IDE do Arduino o exemplo `DumpInfo`, disponível em **Arquivo → Exemplos → MFRC522**.
2. Carregue o exemplo no Arduino (com o RFID já ligado conforme a tabela de pinos acima).
3. Abra o Monitor Serial (9600 bps) e aproxime cada tag do leitor.
4. Anote o UID exibido (ex.: `05 68 8F BD`) e associe-o ao animal correspondente no código.

---

## Carregando o código no Arduino

1. Abra `codigo/jogo_animais_comentado.ino` na IDE do Arduino.
2. Selecione a placa **Arduino Mega 2560** em **Ferramentas → Placa**.
3. Selecione a porta serial correta em **Ferramentas → Porta**.
4. Confirme que as bibliotecas `MFRC522` e `DFRobotDFPlayerMini` estão instaladas.
5. Clique em **Carregar**.
6. Abra o Monitor Serial (9600 bps) para acompanhar as mensagens de inicialização (deve aparecer `Pronto!`).

---

## Como usar (controles)

| Ação | O que acontece |
|---|---|
| Pressionar **Ligar/Desligar** | Liga ou desliga todo o sistema. Ao desligar, o áudio para e o jogo é reiniciado. |
| Pressionar **Fazenda / Zoológico / Aquário** | Inicia (ou reinicia) a narrativa daquele ambiente, começando pelo primeiro dos 5 animais |
| Aproximar a tag correta do leitor | Toca o som de "acerto" e avança para o próximo animal |
| Aproximar uma tag errada | Toca o som de "erro" e mantém o mesmo desafio |
| Pressionar **Pause/Play** | Pausa ou retoma a reprodução do áudio atual |
| Pressionar **Rebobinar** | Repete o som do animal atual (se o jogo estiver ativo e não pausado) |
| Girar o **potenciômetro** | Ajusta o volume em tempo real (0 a 30) |

Ao identificar corretamente os 5 animais de um ambiente, toca-se um áudio de finalização e o jogo aguarda a escolha de um novo ambiente.

---

## Estrutura do repositório

```
.
├── README.md
├── ARTIGO_TUTORIAL.md
├── codigo/
│   └── jogo_animais_comentado.ino
├── esquematico/
│   └── esquema_ligacoes.svg
└── imagens/
    └── (fotos do protótipo montado, vídeo/gif de demonstração, etc.)
```

> 📸 Fotos do produto final

<img width="480" height="853" alt="1781183445276" src="https://github.com/user-attachments/assets/9796492e-c7f7-4419-83cd-497cd2d2e1e8" />
<img width="480" height="1040" alt="1781183445038" src="https://github.com/user-attachments/assets/269771c4-d38c-455d-8821-b556a015c479" />
<img width="480" height="1039" alt="jogo" src="https://github.com/user-attachments/assets/75b5de73-3e32-4bb5-8af8-5753024b590d" />
<img width="739" height="1600" alt="WhatsApp Image 2026-06-14 at 20 21 44" src="https://github.com/user-attachments/assets/8f210da7-4de8-47f2-9d7c-4caf5604dfd0" />
<img width="739" height="1600" alt="WhatsApp Image 2026-06-14 at 20 21 43" src="https://github.com/user-attachments/assets/f53990d3-4727-472e-8f3b-84da3ae63c09" />

---

## Possíveis melhorias futuras

- Adicionar um indicador visual (LED) de "ligado/desligado" e de "acerto/erro".
- Salvar o progresso (ambiente e índice do animal) em memória não-volátil (EEPROM), permitindo continuar de onde parou após desligar.

---

## Licença

Este projeto está licenciado sob a MIT License.
Você pode usar, copiar, modificar e distribuir o código livremente, desde que mantenha os créditos dos autores.
