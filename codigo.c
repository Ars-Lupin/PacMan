#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_linha 40       // Maximo de linhas
#define MAX_coluna 101     // Maximo de colunas
#define MAX_PATH_SIZE 1001 // Tamanho maximo do diretorio

typedef struct
{
    int num_linhas;
    int num_colunas;
    int limite_movimentos;
    char celulas[MAX_linha][MAX_coluna];
    int pontosmax;
} tMapa; // armazena o mapa

typedef struct
{
    int linhas;
    int colunas;
    int parede;
    int celulas[MAX_linha][MAX_coluna];
} tTrilha; // armazena a trilha do jogo

typedef struct
{
    int Numjogadas;
    char Jogada;
    int pontos;
    int batidas;
    int cima;
    int baixo;
    int direita;
    int esquerda;
} tJogada; // armazena dados relacionados a jogada

typedef struct
{
    int PacMan_paclinha;
    int Fantasma_B_linha;
    int Fantasma_P_linha;
    int Fantasma_I_linha;
    int Fantasma_C_linha;
    int tunel_1_linha;
    int tunel_2_linha;
    int PacMan_paccoluna;
    int Fantasma_B_coluna;
    int Fantasma_P_coluna;
    int Fantasma_I_coluna;
    int Fantasma_C_coluna;
    int tunel_1_coluna;
    int tunel_2_coluna;
} tLocal; // posicoes

typedef struct
{
    char Fantasma_B;
    char Fantasma_P;
    char Fantasma_I;
    char Fantasma_C;
} tAntigo; // Lugar onde o fantasma passou

typedef struct
{
    int Fantasma_B;
    int Fantasma_P;
    int Fantasma_I;
    int Fantasma_C;
    int tunel;
} tExiste; // condicional de existencia

typedef struct
{
    char letra[2];
    int comidas;
    int colisoes;
    int jogadas;
} tRanking; // armazena dados para ranking

void RealizaJogo(const char *diretorio, tMapa mapa, tLocal local, tExiste existe); // declarando variáveis implicitas
void ImprimeSaida(tMapa mapa, tJogada atual);
void ImprimeSaidaVenceu(tJogada atual);
void ImprimeSaidaGameover(tJogada atual);

tTrilha MapaTrilha(tTrilha trilha) // cria o mapa da trilha
{
    int i, j;
    for (i = 0; i < trilha.linhas; i++)
    {
        for (j = 0; j < trilha.colunas; j++)
        {
            trilha.celulas[i][j] = trilha.parede + 1;
        }
    }
    return trilha;
}

void ImprimeTrilha(const char *diretorio, tTrilha trilha) // imprime a trilha
{
    char caminho_trilha[MAX_PATH_SIZE];
    sprintf(caminho_trilha, "%s/saida/trilha.txt", diretorio);
    FILE *arquivo = fopen(caminho_trilha, "w");
    int i, j;
    for (i = 0; i < trilha.linhas; i++)
    {
        for (j = 0; j < trilha.colunas; j++)
        {
            if (trilha.celulas[i][j] == trilha.parede + 1)
            {
                fprintf(arquivo, "# ");
            }
            else
            {
                fprintf(arquivo, "%d ", trilha.celulas[i][j]);
            }
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}

void ResumoComida(const char *diretorio, tJogada jogada) // Escreve o resumo de quando pegou a comida
{
    char caminho_resumo[MAX_PATH_SIZE];
    sprintf(caminho_resumo, "%s/saida/resumo.txt", diretorio);
    FILE *arquivo = fopen(caminho_resumo, "a");
    fprintf(arquivo, "Movimento %d (%c) pegou comida\n", jogada.Numjogadas, jogada.Jogada);
    fclose(arquivo);
}

void ResumoParede(const char *diretorio, tJogada jogada) // Escreve o resumo de quando bateu na parede
{
    char caminho_resumo[MAX_PATH_SIZE];
    sprintf(caminho_resumo, "%s/saida/resumo.txt", diretorio);
    FILE *arquivo = fopen(caminho_resumo, "a");
    fprintf(arquivo, "Movimento %d (%c) colidiu na parede\n", jogada.Numjogadas, jogada.Jogada);
    fclose(arquivo);
}

void ResumoFantasma(const char *diretorio, tJogada jogada) // Escreve o resumo se bateu no fantasma
{
    char caminho_resumo[MAX_PATH_SIZE];
    sprintf(caminho_resumo, "%s/saida/resumo.txt", diretorio);
    FILE *arquivo = fopen(caminho_resumo, "a");
    fprintf(arquivo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", jogada.Numjogadas, jogada.Jogada);
    fclose(arquivo);
}

int perde(tLocal local, tLocal old) // verifica se perdeu por colisao
{
    if ((local.PacMan_paclinha == local.Fantasma_B_linha) && (local.PacMan_paccoluna == local.Fantasma_B_coluna))
    {
        return 1;
    }
    else if (((old.PacMan_paclinha == local.Fantasma_B_linha) && (old.PacMan_paccoluna == local.Fantasma_B_coluna)) && ((local.PacMan_paclinha == old.Fantasma_B_linha) && (local.PacMan_paccoluna == old.Fantasma_B_coluna)))
    {
        return 1;
    }
    else if ((local.PacMan_paclinha == local.Fantasma_C_linha) && (local.PacMan_paccoluna == local.Fantasma_C_coluna))
    {
        return 1;
    }
    else if (((old.PacMan_paclinha == local.Fantasma_C_linha) && (old.PacMan_paccoluna == local.Fantasma_C_coluna)) && ((local.PacMan_paclinha == old.Fantasma_C_linha && local.PacMan_paccoluna == old.Fantasma_C_coluna)))
    {
        return 1;
    }
    else if ((local.PacMan_paclinha == local.Fantasma_I_linha) && (local.PacMan_paccoluna == local.Fantasma_I_coluna))
    {
        return 1;
    }
    else if (((old.PacMan_paclinha == local.Fantasma_I_linha) && (old.PacMan_paccoluna == local.Fantasma_I_coluna)) && ((local.PacMan_paclinha == old.Fantasma_I_linha && local.PacMan_paccoluna == old.Fantasma_I_coluna)))
    {
        return 1;
    }
    else if ((local.PacMan_paclinha == local.Fantasma_P_linha) && (local.PacMan_paccoluna == local.Fantasma_P_coluna))
    {
        return 1;
    }
    else if (((old.PacMan_paclinha == local.Fantasma_P_linha) && (old.PacMan_paccoluna == local.Fantasma_P_coluna)) && ((local.PacMan_paclinha == old.Fantasma_P_linha) && (local.PacMan_paccoluna == old.Fantasma_P_coluna)))
    {
        return 1;
    }
    return 0;
}

tLocal tuneis(tLocal local) // faz os tuneis funcionarem
{
    if ((local.PacMan_paclinha == local.tunel_1_linha) && (local.PacMan_paccoluna) == (local.tunel_1_coluna))
    {
        local.PacMan_paclinha = local.tunel_2_linha;
        local.PacMan_paccoluna = local.tunel_2_coluna;
    }
    else if ((local.PacMan_paclinha == local.tunel_2_linha) && (local.PacMan_paccoluna) == (local.tunel_2_coluna))
    {
        local.PacMan_paclinha = local.tunel_1_linha;
        local.PacMan_paccoluna = local.tunel_1_coluna;
    }
    return local;
}

tMapa devolvetuneis(tLocal local, tMapa mapa) // devolve os tuneis pos jogada do pacman
{
    if ((local.PacMan_paclinha != local.tunel_1_linha) || (local.PacMan_paccoluna) != (local.tunel_1_coluna))
    {
        mapa.celulas[local.tunel_1_linha][local.tunel_1_coluna] = '@';
    }
    if ((local.PacMan_paclinha != local.tunel_2_linha) || (local.PacMan_paccoluna) != (local.tunel_2_coluna))
    {
        mapa.celulas[local.tunel_2_linha][local.tunel_2_coluna] = '@';
    }
    return mapa;
}

// funções da movimentação do fantasma
void MovimentaFantasmaB(tMapa *mapa, tAntigo *antigo, tLocal *local, int *constanteB)
{
    if (*constanteB == 0)
    {
        local->Fantasma_B_coluna--; // pra esquerda
        if (mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] != '#')
        {
            antigo->Fantasma_B = mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna];
        }
        if (mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] == '#')
        {
            local->Fantasma_B_coluna++;
            mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] = antigo->Fantasma_B;
            local->Fantasma_B_coluna++; // vira pra direita
            antigo->Fantasma_B = mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna];
            *constanteB = 1;
        }
    }
    else if (*constanteB == 1)
    {
        local->Fantasma_B_coluna++; // pra direita
        if (mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] != '#')
        {
            antigo->Fantasma_B = mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna];
        }
        if (mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] == '#')
        {
            local->Fantasma_B_coluna--;
            mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna] = antigo->Fantasma_B;
            local->Fantasma_B_coluna--; // vira pra esquerda
            antigo->Fantasma_B = mapa->celulas[local->Fantasma_B_linha][local->Fantasma_B_coluna];
            *constanteB = 0;
        }
    }
}

void MovimentaFantasmaI(tMapa *mapa, tAntigo *antigo, tLocal *local, int *constanteI)
{
    if (*constanteI == 0)
    {
        local->Fantasma_I_linha++; // para baixo
        if (mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] != '#')
        {
            antigo->Fantasma_I = mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna];
        }
        if (mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] == '#')
        {
            local->Fantasma_I_linha--;
            mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] = antigo->Fantasma_I;
            local->Fantasma_I_linha--; // vira para cima
            antigo->Fantasma_I = mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna];
            *constanteI = 1;
        }
    }
    else if (*constanteI == 1)
    {
        local->Fantasma_I_linha--; // para cima
        if (mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] != '#')
        {
            antigo->Fantasma_I = mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna];
        }
        if (mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] == '#')
        {
            local->Fantasma_I_linha++;
            mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna] = antigo->Fantasma_I;
            local->Fantasma_I_linha++; // vira para baixo
            antigo->Fantasma_I = mapa->celulas[local->Fantasma_I_linha][local->Fantasma_I_coluna];
            *constanteI = 0;
        }
    }
}

void MovimentaFantasmaP(tMapa *mapa, tAntigo *antigo, tLocal *local, int *constanteP)
{
    if (*constanteP == 0)
    {
        local->Fantasma_P_linha--; // para cima
        if (mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] != '#')
        {
            antigo->Fantasma_P = mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna];
        }
        if (mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] == '#')
        {
            local->Fantasma_P_linha++;
            mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] = antigo->Fantasma_P;
            local->Fantasma_P_linha++; // vira para baixo
            antigo->Fantasma_P = mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna];
            *constanteP = 1;
        }
    }
    else if (*constanteP == 1)
    {
        local->Fantasma_P_linha++; // para baixo
        if (mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] != '#')
        {
            antigo->Fantasma_P = mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna];
        }
        if (mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] == '#')
        {
            local->Fantasma_P_linha--;
            mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna] = antigo->Fantasma_P;
            local->Fantasma_P_linha--; // vira para cima
            antigo->Fantasma_P = mapa->celulas[local->Fantasma_P_linha][local->Fantasma_P_coluna];
            *constanteP = 0;
        }
    }
}

void MovimentaFantasmaC(tMapa *mapa, tAntigo *antigo, tLocal *local, int *constanteC)
{
    if (*constanteC == 0)
    {
        local->Fantasma_C_coluna++; // para a direita
        if (mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] != '#')
        {
            antigo->Fantasma_C = mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna];
        }
        if (mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] == '#')
        {
            local->Fantasma_C_coluna--;
            mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] = antigo->Fantasma_C;
            local->Fantasma_C_coluna--; // vira para a esquerda
            antigo->Fantasma_C = mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna];
            *constanteC = 1;
        }
    }
    else if (*constanteC == 1)
    {
        local->Fantasma_C_coluna--; // para a esquerda
        if (mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] != '#')
        {
            antigo->Fantasma_C = mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna];
        }
        if (mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] == '#')
        {
            local->Fantasma_C_coluna++;
            mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna] = antigo->Fantasma_C;
            local->Fantasma_C_coluna++; // vira para a direita
            antigo->Fantasma_C = mapa->celulas[local->Fantasma_C_linha][local->Fantasma_C_coluna];
            *constanteC = 0;
        }
    }
}

void GerarEstatisticas(const char *diretorio, tJogada jogada) // imprime as estatisticas
{
    char caminho_estatisticas[MAX_PATH_SIZE];

    sprintf(caminho_estatisticas, "%s/saida/estatisticas.txt", diretorio);
    FILE *arquivo = fopen(caminho_estatisticas, "a");

    fprintf(arquivo, "Numero de movimentos: %d\n", jogada.Numjogadas);
    fprintf(arquivo, "Numero de movimentos sem pontuar: %d\n", jogada.Numjogadas - jogada.pontos);
    fprintf(arquivo, "Numero de colisoes com parede: %d\n", jogada.batidas);
    fprintf(arquivo, "Numero de movimentos para baixo: %d\n", jogada.baixo);
    fprintf(arquivo, "Numero de movimentos para cima: %d\n", jogada.cima);
    fprintf(arquivo, "Numero de movimentos para esquerda: %d\n", jogada.esquerda);
    fprintf(arquivo, "Numero de movimentos para direita: %d\n", jogada.direita);

    fclose(arquivo);
}

void GerarRanking(const char *diretorio, tRanking *jogada) // analisa os dados e forma o ranking
{
    char caminho_ranking[MAX_PATH_SIZE];
    tRanking maior[4];
    tRanking temp;
    int i, j;

    for (i = 0; i < 4; i++)
    {
        maior[i] = jogada[i];
    }

    // Ordenar em ordem decrescente
    for (i = 0; i < 3; i++)
    {
        for (j = i + 1; j < 4; j++)
        {
            if (maior[j].comidas > maior[i].comidas ||
                (maior[j].comidas == maior[i].comidas &&
                 maior[j].colisoes < maior[i].colisoes) ||
                (maior[j].comidas == maior[i].comidas &&
                 maior[j].colisoes == maior[i].colisoes &&
                 maior[j].jogadas > maior[i].jogadas) ||
                (maior[j].comidas == maior[i].comidas &&
                 maior[j].colisoes == maior[i].colisoes &&
                 maior[j].jogadas == maior[i].jogadas &&
                 strcmp(maior[j].letra, maior[i].letra) < 0))
            {
                temp = maior[i];
                maior[i] = maior[j];
                maior[j] = temp;
            }
        }
    }

    sprintf(caminho_ranking, "%s/saida/ranking.txt", diretorio);
    FILE *arquivo = fopen(caminho_ranking, "a");
    for (i = 0; i < 4; i++)
    {
        fprintf(arquivo, "%s,%d,%d,%d\n", maior[i].letra, maior[i].comidas, maior[i].colisoes, maior[i].jogadas);
    }

    fclose(arquivo);
}

void RealizaJogo(const char *diretorio, tMapa mapa, tLocal local, tExiste existe) // funcao que desempenha todo o jogo
{
    tJogada atual;
    atual.pontos = 0;
    atual.Numjogadas = 0;
    atual.batidas = 0;
    atual.cima = 0;
    atual.baixo = 0;
    atual.direita = 0;
    atual.esquerda = 0;

    tLocal old;
    tAntigo antigo;
    tTrilha trilha;
    trilha.linhas = mapa.num_linhas;
    trilha.colunas = mapa.num_colunas;
    trilha.parede = mapa.limite_movimentos;

    tRanking RankJogada[4];
    RankJogada[0].letra[0] = 'w'; // Define como strings as jogadas
    RankJogada[1].letra[0] = 's';
    RankJogada[2].letra[0] = 'a';
    RankJogada[3].letra[0] = 'd';

    char caminho_jogadas[MAX_PATH_SIZE];
    int constanteB = 0, constanteP = 0, constanteI = 0, constanteC = 0, bateu = 0, i = 0, antX = 0, antY = 0;

    trilha = MapaTrilha(trilha); // cria um mapa para trilha
    trilha.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] = 0;

    if (existe.tunel == 1) // se tem tunel, limpa a posicao deles
    {
        trilha.celulas[local.tunel_1_linha][local.tunel_1_coluna] = 0;
        trilha.celulas[local.tunel_2_linha][local.tunel_2_coluna] = 0;
    }

    for(i = 0; i < 4; i++){ // inicializa a variável de ranking
        RankJogada[i].jogadas = 0;
        RankJogada[i].colisoes = 0;
        RankJogada[i].comidas = 0;
    }

    // Limpa a primeira posicao do fantasma e inicializa a variável "antigo"
    if (existe.Fantasma_B == 1)
    {
        mapa.celulas[local.Fantasma_B_linha][local.Fantasma_B_coluna] = ' ';
        antigo.Fantasma_B = ' ';
    }
    if (existe.Fantasma_C == 1)
    {
        mapa.celulas[local.Fantasma_C_linha][local.Fantasma_C_coluna] = ' ';
        antigo.Fantasma_C = ' ';
    }
    if (existe.Fantasma_I == 1)
    {
        mapa.celulas[local.Fantasma_I_linha][local.Fantasma_I_coluna] = ' ';
        antigo.Fantasma_I = ' ';
    }
    if (existe.Fantasma_P == 1)
    {
        mapa.celulas[local.Fantasma_P_linha][local.Fantasma_P_coluna] = ' ';
        antigo.Fantasma_P = ' ';
    }
    while (1)
    {
        scanf(" %c", &atual.Jogada);
        atual.Numjogadas++;
        old = local;
        mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] = ' '; // Come as comidas
        int UsouTunel = 1;

        // verifica a existencia
        if (existe.Fantasma_B == 1)
        {
            MovimentaFantasmaB(&mapa, &antigo, &local, &constanteB);
            mapa.celulas[local.Fantasma_B_linha][local.Fantasma_B_coluna] = 'B';
        }

        if (existe.Fantasma_C == 1)
        {
            MovimentaFantasmaC(&mapa, &antigo, &local, &constanteC);
            mapa.celulas[local.Fantasma_C_linha][local.Fantasma_C_coluna] = 'C';
        }

        if (existe.Fantasma_I == 1)
        {
            MovimentaFantasmaI(&mapa, &antigo, &local, &constanteI);
            mapa.celulas[local.Fantasma_I_linha][local.Fantasma_I_coluna] = 'I';
        }

        if (existe.Fantasma_P == 1)
        {
            MovimentaFantasmaP(&mapa, &antigo, &local, &constanteP);
            mapa.celulas[local.Fantasma_P_linha][local.Fantasma_P_coluna] = 'P';
        }

        // jogadas do pacman
        if (atual.Jogada == 'w')
        {
            local.PacMan_paclinha--;
            if (mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] == '#') // caso bata
            {
                local.PacMan_paclinha++;
                bateu = 1;
                RankJogada[0].colisoes++;
                atual.batidas++;
            }
            RankJogada[0].jogadas++;
            atual.cima++;
        }
        else if (atual.Jogada == 's')
        {
            local.PacMan_paclinha++;
            if (mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] == '#') // caso bata
            {
                local.PacMan_paclinha--;
                bateu = 1;
                RankJogada[1].colisoes++;
                atual.batidas++;
            }
            RankJogada[1].jogadas++;
            atual.baixo++;
        }
        else if (atual.Jogada == 'a')
        {
            local.PacMan_paccoluna--;
            if (mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] == '#') // caso bata
            {
                local.PacMan_paccoluna++;
                bateu = 1;
                RankJogada[2].colisoes++;
                atual.batidas++;
            }
            RankJogada[2].jogadas++;
            atual.esquerda++;
        }
        else if (atual.Jogada == 'd')
        {
            local.PacMan_paccoluna++;
            if (mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] == '#') // caso bata
            {
                local.PacMan_paccoluna--;
                bateu = 1;
                RankJogada[3].colisoes++;
                atual.batidas++;
            }
            RankJogada[3].jogadas++;
            atual.direita++;
        }

        
        antX = local.PacMan_paclinha; // armazena a posição do pacman, para verificar se usou tunel
        antY = local.PacMan_paccoluna; // armazena a posição do pacman, para verificar se usou tunel

        if (existe.tunel == 1)
        {
            local = tuneis(local);
        }

        if ((antX == local.PacMan_paclinha) && (antY == local.PacMan_paccoluna)){
            UsouTunel = 0;
        }

        if (perde(local, old) == 0) // caso não perca
        {
            if (mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] == '*')
            {
                atual.pontos++; // aumenta a pontuacao
                ResumoComida(diretorio, atual);
                if (atual.Jogada == 'w')
                {
                    RankJogada[0].comidas++;
                }
                else if (atual.Jogada == 's')
                {
                    RankJogada[1].comidas++;
                }
                else if (atual.Jogada == 'a')
                {
                    RankJogada[2].comidas++;
                }
                else if (atual.Jogada == 'd')
                {
                    RankJogada[3].comidas++;
                }
            }
            mapa.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] = '>';
            trilha.celulas[local.PacMan_paclinha][local.PacMan_paccoluna] = atual.Numjogadas;
        }

        // coloca o valor das trilhas dos tuneis iguais
        if (trilha.celulas[local.tunel_1_linha][local.tunel_1_coluna] < trilha.celulas[local.tunel_2_linha][local.tunel_2_coluna])
        {
            trilha.celulas[local.tunel_1_linha][local.tunel_1_coluna] = trilha.celulas[local.tunel_2_linha][local.tunel_2_coluna];
        }
        else if (trilha.celulas[local.tunel_1_linha][local.tunel_1_coluna] > trilha.celulas[local.tunel_2_linha][local.tunel_2_coluna])
        {
            trilha.celulas[local.tunel_2_linha][local.tunel_2_coluna] = trilha.celulas[local.tunel_1_linha][local.tunel_1_coluna];
        }

        if (perde(local, old) == 1)
        {
            ResumoFantasma(diretorio, atual);
        }
        if ((bateu == 1) && (UsouTunel == 0))
        {
            ResumoParede(diretorio, atual);
        }
        bateu = 0; // limpa a variável

        if (existe.tunel == 1)
        {
            mapa = devolvetuneis(local, mapa);
        }
        ImprimeSaida(mapa, atual);

        if ((atual.Numjogadas == mapa.limite_movimentos) || (perde(local, old) == 1))
        {
            ImprimeSaidaGameover(atual);
            break;
        }
        else if (atual.pontos == mapa.pontosmax)
        {
            ImprimeSaidaVenceu(atual);
            break;
        }

        // Devolve o caractere que o fantasma passou
        if (existe.Fantasma_B == 1)
        {
            mapa.celulas[local.Fantasma_B_linha][local.Fantasma_B_coluna] = antigo.Fantasma_B;
        }
        if (existe.Fantasma_C == 1)
        {
            mapa.celulas[local.Fantasma_C_linha][local.Fantasma_C_coluna] = antigo.Fantasma_C;
        }
        if (existe.Fantasma_I == 1)
        {
            mapa.celulas[local.Fantasma_I_linha][local.Fantasma_I_coluna] = antigo.Fantasma_I;
        }
        if (existe.Fantasma_P == 1)
        {
            mapa.celulas[local.Fantasma_P_linha][local.Fantasma_P_coluna] = antigo.Fantasma_P;
        }
    }

    GerarRanking(diretorio, RankJogada);
    GerarEstatisticas(diretorio, atual);
    ImprimeTrilha(diretorio, trilha);
}

void ImprimeSaida(tMapa mapa, tJogada atual) // imprime os mapas apos jogada
{
    int i;
    printf("Estado do jogo apos o movimento '%c':\n", atual.Jogada);
    for (i = 0; i < mapa.num_linhas; i++)
    {
        printf("%s\n", mapa.celulas[i]);
    }
    printf("Pontuacao: %d\n\n", atual.pontos);
}

void ImprimeSaidaVenceu(tJogada atual) // imprime se venceu
{
    printf("Voce venceu!\nPontuacao final: %d\n", atual.pontos);
}

void ImprimeSaidaGameover(tJogada atual) // imprime se perdeu
{
    printf("Game over!\nPontuacao final: %d\n", atual.pontos);
}

void inicializarJogo(const char *diretorio) // le o mapa e cria a inicializacao
{
    int i = 0, j = 0, tunel = 0;
    char caminho_mapa[MAX_PATH_SIZE];
    tMapa mapa;
    tExiste existe;
    existe.Fantasma_B = 0;
    existe.Fantasma_C = 0;
    existe.Fantasma_I = 0;
    existe.Fantasma_P = 0;
    existe.tunel = 0;
    mapa.pontosmax = 0;
    tLocal local;

    sprintf(caminho_mapa, "%s/mapa.txt", diretorio);

    FILE *arquivo = fopen(caminho_mapa, "r");
    if (arquivo == NULL)
    {
        printf("ERRO: Nao foi possivel ler o arquivo '%s'\n", caminho_mapa);
        exit(1);
    }

    fscanf(arquivo, "%d %d %d\n", &mapa.num_linhas, &mapa.num_colunas, &mapa.limite_movimentos);

    for (i = 0; i < mapa.num_linhas; i++)
    {
        fscanf(arquivo, "%[^\n]\n", mapa.celulas[i]);
        for (j = 0; j < mapa.num_colunas; j++)
        {

            // Armazena os locais dos personagens e o maximo de pontos
            if (mapa.celulas[i][j] == '>') // Pacman
            {
                local.PacMan_paclinha = i;
                local.PacMan_paccoluna = j;
            }
            else if (mapa.celulas[i][j] == '@') // Tunel
            {
                if (tunel < 1)
                {
                    local.tunel_1_linha = i;
                    local.tunel_1_coluna = j;
                    existe.tunel = 1;
                    tunel++;
                }
                else if (tunel >= 1) // Tunel
                {
                    local.tunel_2_linha = i;
                    local.tunel_2_coluna = j;
                }
            }
            else if (mapa.celulas[i][j] == 'B') // Fantasma
            {
                local.Fantasma_B_linha = i;
                local.Fantasma_B_coluna = j;
                existe.Fantasma_B = 1;
            }
            else if (mapa.celulas[i][j] == 'P') // Fantasma
            {
                local.Fantasma_P_linha = i;
                local.Fantasma_P_coluna = j;
                existe.Fantasma_P = 1;
            }
            else if (mapa.celulas[i][j] == 'I') // Fantasma
            {
                local.Fantasma_I_linha = i;
                local.Fantasma_I_coluna = j;
                existe.Fantasma_I = 1;
            }
            else if (mapa.celulas[i][j] == 'C') // Fantasma
            {
                local.Fantasma_C_linha = i;
                local.Fantasma_C_coluna = j;
                existe.Fantasma_C = 1;
            }
            else if (mapa.celulas[i][j] == '*') // comida
            {
                mapa.pontosmax++;
            }
        }
    }

    fclose(arquivo);

    char caminho_inicializacao[MAX_PATH_SIZE];
    sprintf(caminho_inicializacao, "%s/saida/inicializacao.txt", diretorio);
    arquivo = fopen(caminho_inicializacao, "w");
    if (arquivo == NULL)
    {
        printf("ERRO: Nao foi possivel criar o arquivo de inicializacao\n");
        exit(1);
    }

    for (i = 0; i < mapa.num_linhas; i++)
    {
        fprintf(arquivo, "%s\n", mapa.celulas[i]);
    }

    fprintf(arquivo, "Pac-Man comecara o jogo na linha %d e coluna %d\n", local.PacMan_paclinha + 1, local.PacMan_paccoluna + 1);

    fclose(arquivo);

    RealizaJogo(diretorio, mapa, local, existe);
}

int main(int argc, char *argv[])
{
    if (argc < 2) // caso o diretorio nao seja informado
    {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char diretorio[MAX_PATH_SIZE];
    strcpy(diretorio, argv[1]);

    inicializarJogo(diretorio);

    return 0;
}
