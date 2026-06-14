# Artigo Tutorial — Construindo o 'Cadê o Bicho?'

## Introdução

Este tutorial apresenta, passo a passo, como construir o jogo **Cadê o Bicho?**, jogo sonoro educativo para crianças cegas, no qual figuras de animais (Fazenda, Zoológico e Aquário) são identificadas por tags RFID. Ao aproximar a figura correta do leitor, o sistema toca o som daquele animal; ao errar, toca um som de "tente novamente".

O objetivo é que qualquer pessoa com conhecimentos básicos de Arduino consiga **reproduzir o protótipo do zero**, entendendo cada etapa da montagem, da programação e da preparação dos áudios.

---

## 1. O que você vai precisar

| Componente | Quantidade |
|---|---|
| Arduino Mega 2560 | 1 |
| Módulo leitor RFID MFRC522 | 1 |
| Tags/cartões RFID | 15 (5 por ambiente) |
| Módulo DFPlayer Mini | 1 |
| Cartão microSD (até 32GB) | 1 |
| Alto-falante (3–8 Ω) | 1 |
| Botões push-button | 6 |
| Potenciômetro 10kΩ | 1 |
| Resistor 1kΩ | 1 |
| Protoboard e jumpers | conforme necessário |

> 💡 As tags RFID podem ser coladas no verso de figuras impressas, brinquedos pequenos ou peças de madeira/EVA representando cada animal.

---

## 2. Entendendo a lógica do jogo

Antes de montar qualquer coisa, vale entender o "fluxo" do programa:

1. Ao ligar, o sistema toca um áudio de boas-vindas, apresenta o jogo e espera o usuário escolher um ambiente.
2. Ao escolher um ambiente (Fazenda, Zoológico ou Aquário), o sistema "carrega" o primeiro dos 5 animais daquele ambiente e toca o som dele.
3. O usuário aproxima uma figura/tag do leitor RFID.
4. O sistema compara o UID lido com o UID esperado:
   - Se for igual → toca som de acerto, espera alguns segundos e carrega o próximo animal.
   - Se for diferente → toca som de erro e continua esperando o animal correto.
5. Ao acertar os 5 animais, toca um som de finalização daquele ambiente e o sistema volta a aguardar a escolha de um novo ambiente.

Em paralelo, o sistema está sempre:
- Lendo o potenciômetro para ajustar o volume.
- Verificando os botões de pause/play, repetir som e ligar/desligar.

---

## 3. Montagem do circuito

### 3.1 Leitor RFID MFRC522

O MFRC522 opera em **3.3V** — ligá-lo em 5V pode danificar o módulo.

| MFRC522 | Arduino Mega |
|---|---|
| SDA (SS) | 10 |
| SCK | 52 |
| MOSI | 51 |
| MISO | 50 |
| RST | 9 |
| 3.3V | 3.3V |
| GND | GND |

### 3.2 DFPlayer Mini + alto-falante

| DFPlayer Mini | Arduino Mega |
|---|---|
| VCC | 5V |
| GND | GND |
| RX | TX3 (pino 14), com resistor de 1kΩ em série |
| TX | RX3 (pino 15) |
| SPK_1 / SPK_2 | terminais do alto-falante |

O resistor de 1kΩ entre o TX3 do Arduino e o RX do DFPlayer protege o módulo, já que ele opera com lógica de 3.3V.

### 3.3 Botões

Todos os botões usam o resistor de pull-up **interno** do Arduino (`INPUT_PULLUP`), então basta ligar uma perna de cada botão ao pino correspondente e a outra ao GND — sem precisar de resistores externos.

| Função | Pino |
|---|---|
| Selecionar Fazenda | 23 |
| Selecionar Zoológico | 25 |
| Selecionar Aquário | 27 |
| Pause/Play | 29 |
| Repetir som atual | 31 |
| Ligar/Desligar | 33 |

### 3.4 Potenciômetro de volume

Ligue os dois terminais externos do potenciômetro ao 5V e ao GND, e o terminal central (cursor) ao pino **A0**.

### 3.5 Diagrama de ligações

Um diagrama esquemático simplificado está disponível em `esquematico/esquema_ligacoes.svg`, mostrando todas as conexões descritas acima.

> 📸 Adicione fotos da sua montagem real nesta seção para facilitar a reprodução por outras pessoas.

---

## 4. Preparando o ambiente de programação

1. Instale a [IDE do Arduino](https://www.arduino.cc/en/software).
2. Em **Sketch → Incluir Biblioteca → Gerenciar Bibliotecas**, instale:
   - `MFRC522` (by GithubCommunity / miguelbalboa)
   - `DFRobotDFPlayerMini` (by DFRobot)
3. Conecte o Arduino Mega 2560 ao computador via USB.
4. Em **Ferramentas**, selecione a placa **Arduino Mega 2560** e a porta serial correta.

---

## 5. Identificando as tags RFID

Cada tag tem um identificador único (UID) que precisa ser cadastrado no código. Para descobrir o UID de cada uma:

1. Com o leitor RFID já conectado conforme a seção 3.1, abra o exemplo **Arquivo → Exemplos → MFRC522 → DumpInfo**.
2. Carregue esse exemplo no Arduino.
3. Abra o **Monitor Serial** (velocidade 9600).
4. Aproxime cada tag do leitor, uma por vez, e anote o UID exibido (algo como `05 68 8F BD`).
5. Anote qual UID corresponde a qual animal — você vai usar essa lista no próximo passo.

---

## 6. Preparando os áudios no cartão microSD

O DFPlayer Mini, com o comando `playMp3Folder(N)`, procura arquivos dentro de uma pasta chamada **`mp3`** (em minúsculas) na raiz do cartão, nomeados como `0001.mp3`, `0002.mp3`, etc.

1. Formate o cartão microSD em **FAT16** ou **FAT32**.
2. Crie, na raiz do cartão, uma pasta chamada `mp3`.
3. Grave dentro dela os arquivos MP3 numerados conforme a tabela abaixo (a numeração precisa corresponder exatamente ao que está no código):

| Número | Conteúdo |
|---|---|
| 0001 | Áudio de boas-vindas/introdução |
| 0002 | Som de "acerto" |
| 0003 | Som de "erro / tente novamente" |
| 0004 | Finalização do ambiente Fazenda |
| 0005 | Finalização do ambiente Zoológico |
| 0006 | Finalização do ambiente Aquário |
| 0010–0019 | Galinha, Vaca, Porco, Ovelha, Pato (nome e som de cada um) |
| 0020–0029 | Girafa, Leão, Macaco, Elefante, Tucano (nome e som de cada um) |
| 0030–0039 | Baleia, Golfinho, Tartaruga, Polvo, Foca (nome e som de cada um) |

> No código atual, apenas os arquivos de **som** de cada animal (números ímpares da faixa 10–39, conforme definido em `audioSom`) são reproduzidos. Os arquivos de "nome" (`audioNome`) podem ser gravados para uso futuro.

---

## 7. Carregando o código no Arduino

1. Abra o arquivo `codigo/jogo_animais_comentado.ino`.
2. No início do código, localize os arrays `fazenda[]`, `zoologico[]` e `aquario[]`.
3. Substitua os valores de `uid` de cada animal pelos UIDs que você anotou no passo 5, mantendo a correspondência entre animal e tag física.
4. Clique em **Verificar** para compilar e depois em **Carregar** para enviar o código ao Arduino.
5. Abra o Monitor Serial (9600 bps) — ao final da inicialização deve aparecer a mensagem `Pronto!`.

---

## 8. Testando o protótipo

1. Insira o cartão microSD no DFPlayer Mini com o sistema desligado.
2. Ligue o Arduino — o áudio `0001.mp3` (boas-vindas) deve ser reproduzido.
3. Gire o potenciômetro e confirme que o volume muda.
4. Pressione o botão de um dos ambientes (ex.: Fazenda) — o som do primeiro animal deve tocar.
5. Aproxime a tag correspondente do leitor RFID — deve tocar o som de "acerto" e, em seguida, o som do próximo animal.
6. Aproxime uma tag errada — deve tocar o som de "erro" e o desafio permanece o mesmo.
7. Teste os botões de **pause/play**, **repetir** e **ligar/desligar**.
8. Complete os 5 animais de um ambiente e confirme que o áudio de finalização correspondente é reproduzido.

---

## 9. Como jogar

| Ação | Resultado |
|---|---|
| Ligar o sistema | Toca o áudio de boas-vindas |
| Escolher um ambiente (Fazenda, Zoológico ou Aquário) | Inicia o desafio com o primeiro de 5 animais |
| Aproximar a figura/tag correta | Som de acerto + avança para o próximo animal |
| Aproximar a figura/tag errada | Som de erro, mesmo desafio continua |
| Pause/Play | Pausa ou retoma o áudio |
| Repetir | Toca novamente o som do animal atual |
| Ligar/Desligar | Liga ou desliga o sistema inteiro |
| Potenciômetro | Ajusta o volume |

---

## 10. Solução de problemas

| Problema | Possível causa |
|---|---|
| Mensagem "Erro no DFPlayer" no Monitor Serial | Cartão SD não inserido, mal formatado, ou fiação TX/RX invertida |
| Leitor RFID não reconhece nenhuma tag | RFID ligado em 5V em vez de 3.3V, ou fiação SPI incorreta |
| Áudio não toca, mas não dá erro no setup | Pasta `mp3` com nome incorreto (deve ser minúsculo) ou arquivos com numeração errada |
| Volume não muda | Verificar ligação do potenciômetro ao pino A0 |
| Botão não responde ou responde várias vezes | Verificar se está ligado ao GND corretamente (uso de `INPUT_PULLUP`) |

---

## Conclusão

Com este tutorial, é possível reproduzir um jogo sonoro educativo completo, combinando leitura de tags RFID, reprodução de áudio e lógica de jogo em um Arduino Mega. O código está totalmente comentado em `codigo/jogo_animais_comentado.ino`, facilitando adaptações e personalizações para diferentes temas e públicos.
