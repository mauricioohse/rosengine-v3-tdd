# **Game Design Document**

## **1\. Visão Geral**

* **Título do Jogo**: \[NOME?\]

* **Gênero**: Tower Defense roguelike

* **Plataforma(s)**: PC

* **Público-alvo**: Gatos e Humanos

* **Resumo/Pitch**:  
   Com lógica, estratégia e um pouco de sorte, proteja o \[reino\] dos \[INIMIGO?\] em uma jornada divertida e desafiadora que testa as habilidades até mesmo do mais experiente player.

* **Diferenciais**:

  * Escolha de torre combinada a RNG  
  * Historia envolvente  
  * Sistema de sinergia (diversas combinações)  
  * Mudança dinâmica de board  
  * Rogue-like learning of the element combination (level-up brain to get stronger)  
    ---

    ## **2\. Mecânicas de Jogo**

    ### **2.1 Objetivo Principal**

O jogador deve vencer todas as rodadas para finalizar o jogo.

### **2.2 Regras e Comandos**

* É necessário vencer uma rodada para avançar à próxima  
* Ao perder uma rodada, o jogador pode tentar novamente enquanto tiver vidas restantes, e ganha um bônus ao perder vida  
* Cada nova rodada permite que uma nova gema elemental seja escolhida pelo jogador.  
* Ao início de cada rodada o jogador pode escolher entre 3 gemas elementais aleatoriamente selecionadas para adicionar ao board.  
* Torres são combinações de gemas elementais  
* O jogador pode livremente mover gemas \[entre ou durante?\] rodadas, ou a cada x rodadas, uma troca de gemas pode ser feita  
* A rodada \[X\] é marcada pela seleção de um herói pelo jogador, sendo este uma peça especial para enfrentar o primeiro boss.  
* O herói evolui ao longo da partida, a cada \[X\] rodadas.  
* Cada gema elemental possui um elemento, e a combinação de elementos diferentes gera novos projeteis. Elementos duplicados apenas aumentam a força da torre, mas nao geram projeteis diferentes, exceto em combinaçoes especiais..  
* Em rodadas específicas, as rotas e o board sofrerão mudanças e torres aleatórias serão perdidas, forçando o jogador a adaptar sua estratégia a eventos na história  
* 

* **Controles**:

  * Point-And-Click  
  * \[keyboard shortcuts\]

    ### 

    ### **2.3 Interface (HUD)**

Described in the image below

![][image1]

https://excalidraw.com/\#json=kmrKXyQgxvvaDyMoe5y\_5,gLFP\_nEgV7nsyRjCiWN8aA

### 

### **2.3 Towers and Elements**

* In this tower defense, towers are created when gems are moved to an empty grid slot.  
* Each tower can hold up to three element gems.   
  * In the future, this might change  
* The Spell/projectile type depends only on the different elements, but not in the order of them. So a FIRE \+ WATER has the same projectile as FIRE \+ 2xWATER tower  
* There are exception for the above rule, for example 3xFIRE may be a special spell  
* Repeated elements usually only boosts stats of towers without the repeated elements  
* TBD: we might create elementless gems that affect spell casting stats only (like double cast, or triple cast, etc, inspired by Noita spell mechanics)  
* The elements and its combinations are described in the spreadsheet below under “Tower descriptions“

**Tower design guidelines:**

* When designing a tower, it needs to be:  
* easily distinguishable from other towers,  
* needs to have advantage against at least one archetype of enemy and disadvantage against at least one archetype.  
* Be interesting\!

[element overview](https://docs.google.com/spreadsheets/d/1dGyrgYWTFoeY0QZ0DaDIDLTja7ldGrAdptLG_7_Ia5g/edit?usp=sharing)

[https://excalidraw.com/\#json=kmrKXyQgxvvaDyMoe5y\_5,gLFP\_nEgV7nsyRjCiWN8aA](https://excalidraw.com/#json=kmrKXyQgxvvaDyMoe5y_5,gLFP_nEgV7nsyRjCiWN8aA) \< some ideas drawn here

---

## **3\. Gameplay**

### **3.1 Ciclos de Jogo**

The whole game is separated into three Acts. Each act has a different map and a boss in the end. Each act is comprised of many levels, which can be a normal or elite fight, or an event 

During an act, the order of events is:

* Planning:   
  * game is paused, waiting for player to ask to send next wave  
  * Player can move gems in and out from inventory without tower cooldown  
* Wave running  
  * Wave runs until all enemies of the wave have been spawned and killed  
  * Player can move gems between towers and inventory, but when he does, there is a 10 second cooldown on the modified towers  
* Select reward and Next Level  
  * If player beat the level, he can choose a reward based on the level type and choose the next level type

### 

### Level types

* Fights  
  * Normal fights  
    * Rewards with a new element gem  
  * Elite fights  
    * Rewards with a new element gem \+ a relic  
  * Boss fights  
    * Last fight from an ACT, and always rewards an element \+ boss relic  
* Events  
  * Events are unique dialogs with each special rewards/consequences

    ### **3.2 Desafios e Recompensas**

* Challenges:  
  * Knowing how to prepare against enemy waves  
  * Knowing enemy and spell archetypes, what is good against what  
  * Long term planning of the towers  
  * Planning around relics  
* Items or rewards:  
  * Relics  
  * Elemental gems

    ---

    ## **4\. Mundo e Ambientação**

* **Lore / História**: Uma gatuna, a mando do \[Mago Supremo\], roubou \[O Cristal Ancestral\] do \[Inimigo\] e o levou para a \[Aliança Felina\]. Enquanto o \[Inimigo\] tenta recuperá-lo, é necessário fazer de tudo para defender o território da \[Aliança Felina\] e ganhar tempo para que o \[Mago Supremo\] canalize o poder do \[O Cristal Ancestral\] e invoque os \[golens mágicos?\].  
  O líder do \[Inimigo\] quer usar o \[O Cristal Ancestral\] para invocar os \[golens mágicos?\] e ordená-los a reabrir a \[fenda espectral\], o que devastaria todo o reino de \[TBD2\].  
  Certa vez, a muito tempo atrás, um mago andarilho em busca de runas mágicas encontrou \[O Cristal Ancestral\] e invocou os \[golens mágicos?\], mas a magia bruta do cristal o corrompeu e o tornou um escravo da vontade dela. Ele então abriu a \[fenda espectral\], o que fez todos os povos se unirem e travarem uma guerra contra as criaturas malignas que dele saíam. Depois de 1000 anos de guerra, o bem prevaleceu e  o mago derrotado, então a fenda foi fechada com ajuda d\[O Cristal Ancestral\]. Um jovem gato mago foi encumbido com a missão de esconder o artefato no lugar mais inóspito do o reino de \[TBD2\] e selá-lo com magia.  
  Séculos se passaram, e um necromante que foi pupilo da academia a qual o mago andarilho havia fundado encontrou antigos pergaminhos escritos por esse. Enganado pelos escritos malignos da magia bruta, parte em missão de encontrar o \[O Cristal Ancestral\] e fazer o que acredita ser a salvação e purificação do reino.  
   \[MIGOS RATOS\]  
* **Ambientações / Biomas**: Medieval, mágico

* **Mapa Geral**: Board com gramados e cenários lúdicos de florestas mágicas, campos e castelos que remetem ao medieval.

* **Fases / Áreas**: rodadas?

  ---

  ## **5\. Personagens**

* **Protagonista(s)**:

  * Nome: \[O major, general? É o próprio player\]

  * Evolução: Apenas narrativa

* **Inimigos / Chefes**:

  * Inimigo: nome \+ descrição

  * boss: nome \+ habilidades e fase

* **NPCs / Aliados**:

  * Herois: \[TBD\]  
  * Habilidades: \[TBD\]

    ---

    ## **6\. Direção de Arte**

* **Estilo visual**: Pixel Art

* **Paleta de cores**: Colorido e vívido

* **Design de personagens**: Personagens cheios de vida e animados

![][image2]![][image3]

![][image4]![][image5]

* **Cenários e ambientes**: Campos, Florestas, Castelos, Masmorras, Montanhas, etc

![][image6]

![][image7]

![][image8]

![][image9]

![][image10]

![][image11]

* **Animações**: \[Preferível\]

  ---

  ## **7\. Áudio**

* **Trilha sonora**:

  * Tema principal: Som medieval animado, remetendo ao som tocado pelo Bardo

  * Músicas por fase: Músicas tensas em fases de boss e música de taverna na interface de escolha de torre

* **Efeitos sonoros**:

  * Seleção de torres, Ataques de torres, ataques de inimigos, sons aleatórios de gritos e expressões.

* **Dublagem / Voz**:

  * Personagem principal: \[?\]

  * Narrador / NPCs: \[?\]

    ---

    ## **8\. Tecnologia e Ferramentas**

* **Engine**: Rose Engine

* **Linguagens**: C++

* **Ferramentas complementares**:

  * Arte (Photoshop, Illustrator)

  * Som (Audacity)

  * Versionamento (Git)

* **Requisitos de Sistema**:

  * Mínimos: Intel Potato3, 1kB RAM, Geforce RTX 0,5

  * Recomendados: Torradeira Phillips

    ---

    ## **9\. UI/UX**

* **Menus**:

  * Menu principal, pause, escolha de torre, game over

* **Fluxo de telas**:

  * Do boot ao menu → tutorial → fase → pause, game over, escolha de torre → fase, menu   
  * Escolha de torre em uma taverna, onde as torres (gatos) estão a tomar um gole de ceva. O jogador irá os convocar com um breve dialogo.  
* **Feedback visual e sonoro**:

  * Popup de vitória ou derrota, animação e som de escolha de herói

* **Tutorial / onboarding**:

  * Tutorial simples escrito

    ---

    ## **10\. Monetização \[TBD\]**

* **Modelo**:

  * \[F2P, Ads\]

* **Itens pagos**:

  * \[Cosméticos, passes, pacotes de conteúdo\]

* **Economia interna**:

  * \[Moedas, gemas, sistemas de compra\]

    ---

    ## **11\. Cronograma & Milestones**

| Fase | Objetivo | Data Estimada |
| :---: | :---: | :---: |
| Pré-produção | Protótipo | TBD |
| Produção inicial | Sistemas básicos | TBD |
| Beta | Fase jogável completa | TBD |
| Lançamento | Release final | TBD |
| Pós-lançamento | Patches / DLC etc | TBD |

    ## ---

    ## 

    ## **12\. Apêndices**

* **Referências e inspiração**:

  * Magicka 2: Sinergias de magia  
  * Luck-be-a-Land-Lord: montagem de deck  
  * Bloons TD: heróis, board livre (fullbox)  
  * Spell Brigade: roguelite com elementos de progressão de magias interessante

# 14\. Appendix B: Relics

**Relic design rules**

* Relics are graded by rarity: common, uncommon, rare and boss  
* The general power level should be:  
  * Common: small upgrade, or good upgrade with a downside, or good upgrade for only specific hard-to-get builds  
  * Uncommon: medium power level, or high power level with a downside  
  * Rare: specific build defining, or general good relics, with a chance of breaking certain builds  
  * Boss: Not sure yet, but they are only rewarded after killing the boss in the end  
* Unless stated: bonuses stack additively; effects apply to **all towers**; “different elements” refers to *unique* elements in a tower (1–3)

**Note: for the prototype, I will only implement three relics marked in blue**

### **COMMON RELICS**

* Name: Lighter  
  * Effect: burn ticks twice as fast  
  * Comments:  
* Name: Water meter  
  * Effect: Increase range of all towers with water by 15%  
  * Comments: increases water hybrids  
* Name: Feather token  
  * Effect: \+20% knockback  
  * Comments:  
* Name: Grounded bolts  
  * Effect: ELECTRO chains \+1 target  
  * Comments: probably too good  
* Name: Stone sling  
  * Effect: 15% Less spread on EARTH towers  
  * Comments:  
* Name: Cracked Hourglass  
  * Effect: \-5% global CD  
  * Comments:  
* Name: Paved street  
  * Effect: Projectile are 20% faster  
  * Comments:  
* Name: Marble dice  
  * Effect: can gem rewards reroll up to three times per act  
  * Comments: Three uses only  
* Name:  
  * Effect:  
  * Comments:  
* Name:  
  * Effect:  
  * Comments:

### UNCOMMON RELICS

* Name: Steam Valve  
  * Effect: FIRE+WATER towers gain 25% AOR radius, \-10% base damage  
  * Comments:  
* Name: Volcanic puddle  
  * Effect: FIRE+EARTH tower inflict scorch (2% max HP over 3s)  
  * Comments:  
* Name:  
  * Effect:  
  * Comments:  
* Name:  
  * Effect:  
  * Comments:  
* Name:  
  * Effect:  
  * Comments:

### RARE RELICS

* Name: Seismic Effect  
  * Effect: EARTH pellets now deal AOE damage  
  * Comments:  
* Name: Chocked fork  
  * Effect: ELECTRO chain range \+50%  
  * Comments:  
* Name: Burning charcoal  
  * Effect: burn lasts twice as long  
  * Comments:  
* Name: Scope  
  * Effect: increases the range of the top right-most tower by 100%  
  * Comments:  
* Name: Cascade  
  * Effect: Water tower now chains once  
  * Comments:  
* Name: Golem’s pact  
  * Effect: EARTH-only tower now pierces twice  
  * Comments:

### BOSS RELICS

* Name:  
  * Effect:  
  * Comments:  
* Name:  
  * Effect:  
  * Comments:  
* Name:  
  * Effect:  
  * Comments: