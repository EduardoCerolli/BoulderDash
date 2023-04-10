#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "lib_boulder.h"
#include "defines.h"

// inicializa as funções da biblioteca e le o mapa do arquivo criando a matriz do jogo
void inicializa_jogo (int *num_mapa, char *nome, bloco_aux *personagem, mapa_t *mapa, ALLEGRO_DISPLAY **janela, ALLEGRO_BITMAP **buffer, ALLEGRO_BITMAP **sprite, ALLEGRO_FONT **fonte, ALLEGRO_TIMER **timer, ALLEGRO_TIMER **timer_2, ALLEGRO_EVENT_QUEUE **fila_eventos, ALLEGRO_EVENT evento) {
    int i, j;
    int display_alt, display_lar;
    int resultado;
    char nome_mapa_completo[100] = caminho_mapas; 
    DIR *diretorio;
    FILE *arquivo;

    al_init ();
    al_init_image_addon ();
    al_install_keyboard ();

    resultado = menu (nome);
    switch (resultado) {
        case 0:
            exit (0);
            break;

        case 1:
            strcat (nome_mapa_completo, "mapa_1.txt");
            *num_mapa = 1;
            break;
        
        case 2:
            strcat (nome_mapa_completo, "mapa_2.txt");
            *num_mapa = 2;
            break;

        case 3:
            strcat (nome_mapa_completo, "mapa_3.txt");
            *num_mapa = 3;
            break;
    }

    // abrindo o arquivo do mapa e lendo os dados da primeira linha
    diretorio = opendir (caminho_mapas);

    arquivo = fopen (nome_mapa_completo, "r");

    fscanf (arquivo, "%d", &mapa->max_alt);
    fscanf (arquivo, "%d", &mapa->max_lar);
    fscanf (arquivo, "%d", &mapa->max_moeadas);
    fscanf (arquivo, "%d", &mapa->moedas_necessarias);
    mapa->moedas_pegas = 0;
    mapa->buffer_alt = (mapa->max_alt * tam_bloco) + (inicio_alt_jogo * tam_bloco);
    mapa->buffer_lar = mapa->max_lar * tam_bloco;

    mapa->matriz = (bloco**) malloc (mapa->max_alt * sizeof (bloco*));
    for (i = 0; i < mapa->max_alt; i++) {
        mapa->matriz[i] = (bloco*) malloc (mapa->max_lar * sizeof (bloco));
    }

    // inicializando as funções da biblioteca allegro
    display_alt = mapa->buffer_alt * 2;
    display_lar = mapa->buffer_lar * 2;

    *fonte = al_create_builtin_font ();
    *fila_eventos = al_create_event_queue ();
    *timer = al_create_timer (1.0 / taxa_quadros);
    *timer_2 = al_create_timer (1.0);
    *janela = al_create_display (display_lar, display_alt);
    *buffer = al_create_bitmap (mapa->buffer_lar, mapa->buffer_alt);
    *sprite = al_load_bitmap (caminho_sprite);

    al_register_event_source (*fila_eventos, al_get_keyboard_event_source ());
    al_register_event_source (*fila_eventos, al_get_display_event_source (*janela));
    al_register_event_source (*fila_eventos, al_get_timer_event_source (*timer));
    al_register_event_source (*fila_eventos, al_get_timer_event_source (*timer_2));


    // preenchendo o mapa
    for (i = 0; i < mapa->max_alt; i++) {
        for (j = 0; j < mapa->max_lar; j++) {
            fscanf (arquivo, "%d", &mapa->matriz[i][j].tipo);
            mapa->matriz[i][j].movimento = 0;
            mapa->matriz[i][j].num_sprite = 0;
            mapa->matriz[i][j].y = (i + inicio_alt_jogo) * tam_bloco;
            mapa->matriz[i][j].x = j * tam_bloco;

            if (mapa->matriz[i][j].tipo == tipo_saida) {
                mapa->saida_y = i;
                mapa->saida_x = j;
                mapa->matriz[i][j].tipo = tipo_terra;
            }

            if (mapa->matriz[i][j].tipo == tipo_personagem) {
                personagem->y = i;
                personagem->x = j;
            }

            define_sprite (&mapa->matriz[i][j], *sprite);
        }
    }

    closedir (diretorio);
    fclose (arquivo);

    return;
}

// destroi as variaveis referente a biblioteca usadas no programa
void encerra_allegro (ALLEGRO_DISPLAY *janela, ALLEGRO_BITMAP *buffer, ALLEGRO_BITMAP *sprite, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *fila_eventos, ALLEGRO_TIMER *timer) {
    al_destroy_font (fonte);
    al_destroy_display (janela);
    al_destroy_bitmap (buffer);
    al_destroy_bitmap (sprite);
    al_destroy_event_queue (fila_eventos);
    al_destroy_timer (timer);

    return;
}

void destroi_mapa (mapa_t mapa) {
    int i;

    for (i = 0; i < mapa.max_alt; i++) {
        free (mapa.matriz[i]);
    }
    free (mapa.matriz);

    return;
}

// faz a animação da criação do personagem
void cria_personagem (bloco_aux personagem, mapa_t *mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro) {
    int x ,y, i;

    y = personagem.y;
    x = personagem.x;


    for (i = 0; i < 4; i++) {
        mapa->matriz[y][x].tipo = tipo_borda;
        mapa->matriz[y][x].movimento = mov_parado;
        define_sprite (&mapa->matriz[y][x], sprite);
        pisca_jogo (mapa, sprite);
        imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
        al_rest (0.2);

        mapa->matriz[y][x].tipo = tipo_inicio;
        mapa->matriz[y][x].movimento = mov_parado;
        define_sprite (&mapa->matriz[y][x], sprite);
        pisca_jogo (mapa, sprite);
        imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
        al_rest (0.2);
    }

    mapa->matriz[y][x].tipo = tipo_explosao_1;
    mapa->matriz[y][x].movimento = mov_parado;
    define_sprite (&mapa->matriz[y][x], sprite);
    pisca_jogo (mapa, sprite);
    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_rest (0.2);

    mapa->matriz[y][x].tipo = tipo_explosao_2;
    mapa->matriz[y][x].movimento = mov_parado;
    define_sprite (&mapa->matriz[y][x], sprite);
    pisca_jogo (mapa, sprite);
    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_rest (0.2);

    mapa->matriz[y][x].tipo = tipo_personagem;
    mapa->matriz[y][x].movimento = mov_parado;
    mapa->matriz[y][x].num_sprite = 0;
    define_sprite (&mapa->matriz[y][x], sprite);

    return;
}

// imprime o jogo na tela principal
void imprime_jogo (mapa_t mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro) {
    int i , j;
    int display_alt, display_lar;
    display_alt = mapa.buffer_alt * 2;
    display_lar = mapa.buffer_lar * 2;

    bloco moeda_painel;

    al_set_target_bitmap (buffer);

    al_clear_to_color (al_map_rgb (0, 0, 0));

    for (i = 0; i < mapa.max_alt; i++) {
        for (j = 0; j < mapa.max_lar; j++) {
            if (mapa.matriz[i][j].sprite != NULL)
                al_draw_bitmap (mapa.matriz[i][j].sprite, mapa.matriz[i][j].x, mapa.matriz[i][j].y, 0);
        }
    }

    moeda_painel.tipo = tipo_moeda_painel;
    define_sprite (&moeda_painel, sprite);

    al_draw_textf (fonte, al_map_rgb (255, 215, 0), tam_bloco * 3, 6, ALLEGRO_ALIGN_CENTRE, "%d", mapa.moedas_necessarias);
    al_draw_bitmap (moeda_painel.sprite, tam_bloco * 4, 0, 0);

    if (mapa.moedas_pegas == mapa.moedas_necessarias || mapa.moedas_pegas > mapa.moedas_necessarias)
        al_draw_textf (fonte, al_map_rgb (255, 255, 255), tam_bloco * 6, 6, ALLEGRO_ALIGN_CENTRE, "%d", valor_moeda_2);
    else 
        al_draw_textf (fonte, al_map_rgb (255, 255, 255), tam_bloco * 6, 6, ALLEGRO_ALIGN_CENTRE, "%d", valor_moeda_1);

    al_draw_textf (fonte, al_map_rgb (255, 215, 0), tam_bloco * 8, 6, ALLEGRO_ALIGN_CENTRE, "%d", mapa.moedas_pegas);

    al_draw_textf (fonte, al_map_rgb (255, 255, 255), mapa.buffer_lar - (tam_bloco * 8), 6, ALLEGRO_ALIGN_CENTRE, "%d", cronometro);

    al_draw_textf (fonte, al_map_rgb (255, 255, 255), mapa.buffer_lar - (tam_bloco * 4), 6, ALLEGRO_ALIGN_CENTRE, "Placar: %d", placar);


    al_set_target_backbuffer (janela);
    al_draw_scaled_bitmap (buffer, 0, 0, mapa.buffer_lar, mapa.buffer_alt, 0, 0, display_lar, display_alt, 0);
    al_flip_display ();

    return;
}

// o bloco_1 vira vazio e oque tinha nele vai para o bloco_2 e o movimento recebido vai no bloco_2
void troca_bloco (bloco *bloco_1, bloco *bloco_2, ALLEGRO_BITMAP *sprite, int mov) {

    if (bloco_1->tipo == tipo_personagem || bloco_1->tipo == tipo_personagem_esq || bloco_1->tipo == tipo_personagem_dir) {
        switch (mov) {
            case mov_parado:
                bloco_2->tipo = tipo_personagem;
                bloco_2->num_sprite = 0;
                break;

            case mov_esq:
                bloco_2->tipo = tipo_personagem_esq;
                if (bloco_1->movimento == mov_esq) {
                    bloco_2->num_sprite = bloco_1->num_sprite + 1;
                }
                else {
                    bloco_2->num_sprite = 0;
                }

                if (bloco_2->num_sprite == quant_sprites_personagem + 1) {
                    bloco_2->num_sprite = 0;
                }
                break;

            case mov_dir:
                bloco_2->tipo = tipo_personagem_dir;
                if (bloco_1->movimento == mov_dir) {
                    bloco_2->num_sprite = bloco_1->num_sprite + 1;
                }
                else {
                    bloco_2->num_sprite = 0;
                }

                if (bloco_2->num_sprite == quant_sprites_personagem + 1) {
                    bloco_2->num_sprite = 0;
                }
                break;
        }
    }
    else {
        bloco_2->tipo = bloco_1->tipo;
        bloco_2->num_sprite = 0;
    }

    bloco_1->tipo = tipo_vazio;

    bloco_1->movimento = mov_parado;    
    bloco_2->movimento = mov;

    bloco_1->num_sprite = 0;

    define_sprite (bloco_1, sprite);
    define_sprite (bloco_2, sprite);

    return;
}

// bloco_1 é o personagem, bloco_2 é a pedra e bloco_3 é o atual vazio
void empurra_pedra (bloco *bloco_1, bloco *bloco_2, bloco *bloco_3, ALLEGRO_BITMAP *sprite, int mov) {
    troca_bloco (bloco_2, bloco_3, sprite, mov_parado);
    troca_bloco (bloco_1, bloco_2, sprite, mov);

}

// retorna 1 caso o personagem possa se mover e 2 para caso possa empurrar uma pedra
int pode_mover (bloco proximo, bloco proximo_proximo, int mov) {

    if (mov == mov_cima || mov == mov_baixo) {
        if ((proximo.tipo == tipo_borda) || (proximo.tipo == tipo_parede) || (proximo.tipo == tipo_pedra)) {
            return 0;
        }
    }
    else if (mov == mov_dir || mov == mov_esq) {
            if (proximo.tipo == tipo_pedra &&  proximo_proximo.tipo == tipo_vazio) {
                return 2;
            }
            else if ((proximo.tipo == tipo_borda) || (proximo.tipo == tipo_parede) || (proximo.tipo == tipo_pedra)) {
                return 0;
            }
    }  
    
    return 1;
}

// imprime a tela de informações
void tela_ajuda (ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte) {
    ALLEGRO_KEYBOARD_STATE tecla;

    al_clear_to_color (al_map_rgb (0, 0, 0));

    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco, tam_bloco, 0, "Comandos:");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 2, 0, "Setinhas do teclado movem o personagem.");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 3, 0, "'esq' encerra o jogo.");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 4, 0, "'h' ou 'f1' abre essa tela de ajuda.");

    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco, tam_bloco * 6, 0, "Objetivo:");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 7, 0, "Pegue o minimo de moedas necessarias para abrir a saida, chegue nela para ganhar.");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 8, 0, "Cada moeda vale 10 pontos, depois de pegar o minimo cada uma restante vale 15.");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 9, 0, "Caso uma pedra ou uma moeda caia na sua cabeça voce morre.");

    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco, tam_bloco * 11, 0, "Autor:");
    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco * 2, tam_bloco * 12, 0, "Eduardo Henrique dos Santos Cerolli.");

    al_draw_text (fonte, al_map_rgb (255, 255, 255), tam_bloco, tam_bloco * 15, 0, "Pressione 'q' para sair dessa tela.");

    al_flip_display ();


    while (1) {
        al_get_keyboard_state (&tecla);
        if (al_key_down (&tecla, ALLEGRO_KEY_Q))
            return;
    }

    return;
}

// retornar fechou caso tenha sido apertado esc, ganhou caso tenha chego na saida e normal caso contrario
int move_personagem (mapa_t *mapa, bloco_aux *personagem, ALLEGRO_BITMAP *sprite, int *placar, ALLEGRO_EVENT evento, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte) {
    ALLEGRO_KEYBOARD_STATE tecla;
    int y, x;
    y = personagem->y;
    x = personagem->x;

    al_get_keyboard_state (&tecla);


    // esc
    if (al_key_down (&tecla, ALLEGRO_KEY_ESCAPE)){
        return fechou;
    }
    // h ou f1
    else if (al_key_down (&tecla, ALLEGRO_KEY_H) || al_key_down (&tecla, ALLEGRO_KEY_F1)) {
        tela_ajuda (janela, fonte);
    }
    // para cima
    else if (al_key_down (&tecla, ALLEGRO_KEY_UP) && pode_mover (mapa->matriz[y-1][x], mapa->matriz[y-1][x], mov_cima)) {
            if (mapa->matriz[y-1][x].tipo == tipo_moeda) {
                if (mapa->moedas_pegas < mapa->moedas_necessarias){
                    *placar = *placar + valor_moeda_1;
                }
                else {
                    *placar = *placar + valor_moeda_2;
                }
                mapa->moedas_pegas++;
            }
            else if (mapa->matriz[y-1][x].tipo == tipo_saida) {
                personagem->y--;
                troca_bloco (&mapa->matriz[y][x], &mapa->matriz[personagem->y][x], sprite, mov_parado);
                return ganhou;
            }

            personagem->y--;
            troca_bloco (&mapa->matriz[y][x], &mapa->matriz[personagem->y][x], sprite, mov_parado);
        }
    // para baixo
    else if (al_key_down (&tecla, ALLEGRO_KEY_DOWN) && pode_mover (mapa->matriz[y+1][x], mapa->matriz[y+1][x], mov_baixo)) {
            if (mapa->matriz[y+1][x].tipo == tipo_moeda) {
                if (mapa->moedas_pegas < mapa->moedas_necessarias){
                    *placar = *placar + valor_moeda_1;
                }
                else {
                    *placar = *placar + valor_moeda_2;
                }
                mapa->moedas_pegas++;
            }
            else if (mapa->matriz[y+1][x].tipo == tipo_saida) {
                personagem->y++;
                troca_bloco (&mapa->matriz[y][x], &mapa->matriz[personagem->y][x], sprite, mov_parado);
                return ganhou;
            }

            personagem->y++;
            troca_bloco (&mapa->matriz[y][x], &mapa->matriz[personagem->y][x], sprite, mov_parado);

    }
    // para esquerda
    else if (al_key_down (&tecla, ALLEGRO_KEY_LEFT)) {
            if (pode_mover (mapa->matriz[y][x-1], mapa->matriz[y][x-2], mov_esq) == 2) {
                personagem->x--;
                empurra_pedra (&mapa->matriz[y][x], &mapa->matriz[y][x-1], &mapa->matriz[y][x-2], sprite, mov_esq);
            }
            else if (pode_mover (mapa->matriz[y][x-1], mapa->matriz[y][x-2], mov_esq) == 1) {
                if (mapa->matriz[y][x-1].tipo == tipo_moeda) {
                    if (mapa->moedas_pegas < mapa->moedas_necessarias){
                        *placar = *placar + valor_moeda_1;
                    }
                    else {
                        *placar = *placar + valor_moeda_2;
                    }
                    mapa->moedas_pegas++;
                }
                else if (mapa->matriz[y][x-1].tipo == tipo_saida) {
                    personagem->x--;
                    troca_bloco (&mapa->matriz[y][x], &mapa->matriz[y][personagem->x], sprite, mov_parado);
                    return ganhou;
                }
                personagem->x--;
                troca_bloco (&mapa->matriz[y][x], &mapa->matriz[y][personagem->x], sprite, mov_esq);
            }
            else {
                mapa->matriz[y][x].tipo = tipo_personagem_esq;
                mapa->matriz[y][x].movimento = mov_esq;
                mapa->matriz[y][x].num_sprite++;
                define_sprite (&mapa->matriz[y][x], sprite);
                if (mapa->matriz[y][x].num_sprite == quant_sprites_personagem) {
                    mapa->matriz[y][x].num_sprite = 0;
                }
            }
    }
    // para direita
    else if (al_key_down (&tecla, ALLEGRO_KEY_RIGHT)) {
            if (pode_mover (mapa->matriz[y][x+1], mapa->matriz[y][x+2], mov_dir) == 2) {
                personagem->x++;
                empurra_pedra (&mapa->matriz[y][x], &mapa->matriz[y][x+1], &mapa->matriz[y][x+2], sprite, mov_dir);
            }
            else if (pode_mover (mapa->matriz[y][x+1], mapa->matriz[y][x+2], mov_dir) == 1) {
                if (mapa->matriz[y][x+1].tipo == tipo_moeda) {
                    if (mapa->moedas_pegas < mapa->moedas_necessarias){
                        *placar = *placar + valor_moeda_1;
                    }
                    else {
                        *placar = *placar + valor_moeda_2;
                    }
                    mapa->moedas_pegas++;
                }
                else if (mapa->matriz[y][x+1].tipo == tipo_saida) {
                    personagem->x++;
                    troca_bloco (&mapa->matriz[y][x], &mapa->matriz[y][personagem->x], sprite, mov_parado);
                    return ganhou;
                }
                personagem->x++;
                troca_bloco (&mapa->matriz[y][x], &mapa->matriz[y][personagem->x], sprite, mov_dir);
            }
            else {
                mapa->matriz[y][x].tipo = tipo_personagem_dir;
                mapa->matriz[y][x].movimento = mov_dir;
                mapa->matriz[y][x].num_sprite++;
                define_sprite (&mapa->matriz[y][x], sprite);
                if (mapa->matriz[y][x].num_sprite == quant_sprites_personagem) {
                    mapa->matriz[y][x].num_sprite = 0;
                }
            }
    }
    else {
        mapa->matriz[y][x].tipo = tipo_personagem;
        if (mapa->matriz[y][x].movimento == mov_parado) {
            mapa->matriz[y][x].num_sprite++;
        }
        else {
            mapa->matriz[y][x].num_sprite = 0;
        }
        mapa->matriz[y][x].movimento = mov_parado;
        define_sprite (&mapa->matriz[y][x], sprite);
        if (mapa->matriz[y][x].num_sprite == quant_sprites_personagem) {
            mapa->matriz[y][x].num_sprite = 0;
        }
    }

    return normal;
}

// faz as pedras e moedas cairem caso necessario
int move_pedras_moedas (mapa_t *mapa, ALLEGRO_BITMAP *sprite) {
    int i, j;
    int moveu = 0;

    for (i = 1; i < mapa->max_alt-1; i++) {
        for (j = 1; j < mapa->max_lar-1; j++) {
            if (mapa->matriz[i][j].tipo == tipo_pedra || mapa->matriz[i][j].tipo == tipo_moeda) {
                if (mapa->matriz[i+1][j].tipo == tipo_vazio) {
                    if (mapa->matriz[i][j].movimento == mov_parado || mapa->matriz[i][j].movimento == mov_caindo) {
                        mapa->matriz[i][j].movimento = mov_cair;
                        moveu = 1;
                    }
                    else if (mapa->matriz[i][j].movimento == mov_cair) {
                        troca_bloco (&mapa->matriz[i][j], &mapa->matriz[i+1][j], sprite, mov_caindo);
                        moveu = 1;
                    }
                }
                else if (mapa->matriz[i+1][j].tipo == tipo_pedra || mapa->matriz[i+1][j].tipo == tipo_parede || mapa->matriz[i+1][j].tipo == tipo_moeda){
                    if (mapa->matriz[i][j-1].tipo == tipo_vazio && mapa->matriz[i+1][j-1].tipo == tipo_vazio) {
                        troca_bloco (&mapa->matriz[i][j], &mapa->matriz[i][j-1], sprite, mov_parado);
                        moveu = 1;
                    }
                    else if (mapa->matriz[i][j+1].tipo == tipo_vazio && mapa->matriz[i+1][j+1].tipo == tipo_vazio) {
                        troca_bloco (&mapa->matriz[i][j], &mapa->matriz[i][j+1], sprite, mov_parado);
                        moveu = 1;
                    }
                    else if (! (mapa->matriz[i+1][j].tipo == tipo_personagem)) {
                        mapa->matriz[i][j].movimento = mov_parado;
                    }
                }
                else if (! (mapa->matriz[i+1][j].tipo == tipo_personagem)) {
                    mapa->matriz[i][j].movimento = mov_parado;
                }
            }
        }
    }

    return moveu;
}

// muda os sprites das moedas e do personagem para dar sensação de movimento
void pisca_jogo (mapa_t *mapa, ALLEGRO_BITMAP *sprite) {
    int i, j;

    for (i = 1; i < mapa->max_alt-1; i++) {
        for (j = 1; j < mapa->max_lar-1; j++) {
            if (mapa->matriz[i][j].tipo == tipo_moeda) {
                mapa->matriz[i][j].num_sprite++;
                define_sprite (&mapa->matriz[i][j], sprite);

                if (mapa->matriz[i][j].num_sprite == quant_sprites_moeda)
                    mapa->matriz[i][j].num_sprite = 0;
            }
            else if (mapa->matriz[i][j].tipo == tipo_saida) {
                if (mapa->matriz[i][j].num_sprite == 0) {
                    mapa->matriz[i][j].num_sprite = 1;
                    define_sprite (&mapa->matriz[i][j], sprite);
                }
                else if (mapa->matriz[i][j].num_sprite == 1) {
                    mapa->matriz[i][j].num_sprite = 0;
                    define_sprite (&mapa->matriz[i][j], sprite);
                }
            }
        }
    }

    return;
}

// move o personagem, as pedras e as moedas
int atualiza_jogo (mapa_t *mapa, bloco_aux *personagem, ALLEGRO_BITMAP *sprite, int *placar, ALLEGRO_EVENT evento, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte) {
    int esc = 0;

    esc = move_personagem (mapa, personagem, sprite, placar, evento, janela, fonte);
    move_pedras_moedas (mapa, sprite);
    pisca_jogo (mapa, sprite);

    return esc;
}

// retorna 0 caso o personagem tenha sido esmagado
int personagem_vivo (bloco_aux personagem, mapa_t mapa) {
    int x, y;
    y = personagem.y;
    x = personagem.x;

    if ((mapa.matriz[y-1][x].tipo == tipo_pedra || mapa.matriz[y-1][x].tipo == tipo_moeda) && (mapa.matriz[y-1][x].movimento == mov_caindo || mapa.matriz[y-1][x].movimento == mov_cair)) {
        return 0;
    }

    return 1;
}

// imprime a mensagem de fim de jogo e o placar final
void fim_jogo (int resultado, int placar, char *nome, int num_mapa) {
    int buffer_alt, buffer_lar, display_lar, display_alt;
    int opcoes = 1;
    buffer_alt = tam_bloco * 30;
    buffer_lar = tam_bloco * 30;
    display_alt = buffer_alt * 2;
    display_lar = buffer_lar * 2;

    ALLEGRO_KEYBOARD_STATE tecla;
    ALLEGRO_BITMAP *buffer = al_create_bitmap (buffer_lar, buffer_alt);
    ALLEGRO_DISPLAY *janela = al_create_display (display_lar, display_alt);
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_BITMAP *sprite = al_create_bitmap (1,1);

    fonte = al_create_builtin_font ();
    fila_eventos = al_create_event_queue ();
    timer = al_create_timer (1.0 / taxa_quadros);
    al_register_event_source (fila_eventos, al_get_keyboard_event_source ());
    al_register_event_source (fila_eventos, al_get_display_event_source (janela));
    al_register_event_source (fila_eventos, al_get_timer_event_source (timer));
    
    if (resultado == ganhou) {
        escreve_placares (nome, placar, num_mapa);
    }

    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        
        al_set_target_bitmap (buffer);

        al_clear_to_color (al_map_rgb (0, 0, 0));

        if (resultado == ganhou) {
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 6, ALLEGRO_ALIGN_CENTRE, "Parabens voce ganhou!");
        }
        else {
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 6, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
        }

        al_draw_textf (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 8, ALLEGRO_ALIGN_CENTRE, "%s", nome);
        al_draw_textf (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 10, ALLEGRO_ALIGN_CENTRE, "Placar: %d", placar);

        if (opcoes == 1)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 14, ALLEGRO_ALIGN_CENTRE, "Placar de Lideres");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 14, ALLEGRO_ALIGN_CENTRE, "Placar de Lideres");
        
        if (opcoes == 2)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 16, ALLEGRO_ALIGN_CENTRE, "Sair do jogo");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 16, ALLEGRO_ALIGN_CENTRE, "Sair do jogo");

        al_set_target_backbuffer (janela);
        al_draw_scaled_bitmap (buffer, 0, 0, buffer_lar, buffer_alt, 0, 0, display_lar, display_alt, 0);
        al_flip_display ();


        al_wait_for_event (fila_eventos, &evento);
        al_get_keyboard_state (&tecla);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (al_key_down (&tecla, ALLEGRO_KEY_DOWN)) {
                opcoes++;
                if (opcoes == 3)
                    opcoes = 1;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_UP)) {
                opcoes--;
                if (opcoes == 0)
                    opcoes = 2;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_ENTER) || al_key_down (&tecla, ALLEGRO_KEY_PAD_ENTER)) {
                switch (opcoes) {
                case 1:
                    mostra_lideres (buffer, janela, fonte, evento, fila_eventos);
                    break;

                case 2:
                    encerra_allegro (janela, buffer, sprite, fonte, fila_eventos, timer);
                    return;
                    break;
                }
            }
        }
    }

    encerra_allegro (janela, buffer, sprite, fonte, fila_eventos, timer);
    return;
}

// atualiza o arquivo de placares de lideres
void escreve_placares (char *nome, int placar, int mapa) {
    int tam, i;
    char nomes[12][12];
    int valores[12];
    char nome_arquivo[100] = caminho_lideres;
    DIR *diretorio;
    FILE *arquivo;
    diretorio = opendir (caminho_lideres);

    switch (mapa) {
        case 1:
            strcat (nome_arquivo, "placares_mapa1.txt");
            break;
        
        case 2:
            strcat (nome_arquivo, "placares_mapa2.txt");
            break;

        case 3:
            strcat (nome_arquivo, "placares_mapa3.txt");
            break;
    }
    arquivo = fopen (nome_arquivo, "r");

    fscanf (arquivo, "%d", &tam);
    for (i = 0; i < tam; i++) {
        fscanf (arquivo, "%s %d", nomes[i], &valores[i]);
    }

    if (tam == 0) {
        tam = 1;
        strcpy (nomes[0], nome);
        valores[0] = placar;

    }
    else if (tam < 10) {
        int aux;
        char aux_c[12];
        tam++;
        strcpy (nomes[tam-1], nome);
        valores[tam-1] = placar;

        i = tam-1;
        while (i > 0 && valores[i] > valores[i - 1]) {
            aux = valores[i];
            strcpy (aux_c, nomes[i]);

            valores[i] = valores[i-1];
            strcpy (nomes[i], nomes[i-1]);

            valores[i-1] = aux;
            strcpy (nomes[i-1], aux_c);
            i--;
        }
    }
    else if (tam == 10) {
        int aux;
        char aux_c[12];
        if (placar > valores[9]) {
            strcpy (nomes[9], nome);
            valores[9] = placar;

            i = tam-1;
            while (i > 0 && valores[i] > valores[i - 1]) {
                aux = valores[i];
                strcpy (aux_c, nomes[i]);

                valores[i] = valores[i-1];
                strcpy (nomes[i], nomes[i-1]);

                valores[i-1] = aux;
                strcpy (nomes[i-1], aux_c);
                i--;
            }
        }
    }

    arquivo = fopen (nome_arquivo, "w");
    fprintf (arquivo, "%d\n", tam);
    for (i = 0; i < tam; i++) {
        fprintf (arquivo, "%s %d\n", nomes[i], valores[i]);
    }

    closedir (diretorio);
    fclose (arquivo);

    return;
}

// explode os blocos envolta do personagem
void explode (bloco_aux personagem, mapa_t *mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro) {
    int x, y, i;
    y = personagem.y;
    x = personagem.x;


    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_rest (0.2);
    
    for (i = -1; i <= 1; i++) {
        if (mapa->matriz[y+i][x-1].tipo != tipo_borda) {
            mapa->matriz[y+i][x-1].tipo = tipo_explosao_1;
            define_sprite (&mapa->matriz[y+i][x-1], sprite);
        }

        if (mapa->matriz[y+i][x].tipo != tipo_borda) {
            mapa->matriz[y+i][x].tipo = tipo_explosao_1;
            define_sprite (&mapa->matriz[y+i][x], sprite);
        }

        if (mapa->matriz[y+i][x+1].tipo != tipo_borda) {
            mapa->matriz[y+i][x+1].tipo = tipo_explosao_1;
            define_sprite (&mapa->matriz[y+i][x+1], sprite);
        }
    }
    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);

    al_rest (0.2);
    for (i = -1; i <= 1; i++) {
        if (mapa->matriz[y+i][x-1].tipo != tipo_borda) {
            mapa->matriz[y+i][x-1].tipo = tipo_explosao_2;
            define_sprite (&mapa->matriz[y+i][x-1], sprite);
        }

        if (mapa->matriz[y+i][x].tipo != tipo_borda) {
            mapa->matriz[y+i][x].tipo = tipo_explosao_2;
            define_sprite (&mapa->matriz[y+i][x], sprite);
        }

        if (mapa->matriz[y+i][x+1].tipo != tipo_borda) {
            mapa->matriz[y+i][x+1].tipo = tipo_explosao_2;
            define_sprite (&mapa->matriz[y+i][x+1], sprite);
        }
    }
    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_rest (0.2);

    for (i = -1; i <= 1; i++) {
        if (mapa->matriz[y+i][x-1].tipo != tipo_borda) {
            mapa->matriz[y+i][x-1].tipo = tipo_vazio;
            define_sprite (&mapa->matriz[y+i][x-1], sprite);
        }

        if (mapa->matriz[y+i][x].tipo != tipo_borda) {
            mapa->matriz[y+i][x].tipo = tipo_vazio;
            define_sprite (&mapa->matriz[y+i][x], sprite);
        }

        if (mapa->matriz[y+i][x+1].tipo != tipo_borda) {
            mapa->matriz[y+i][x+1].tipo = tipo_vazio;
            define_sprite (&mapa->matriz[y+i][x+1], sprite);
        }
    }
    
    while (move_pedras_moedas (mapa, sprite)) {
        imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
        al_rest (0.1);
    }

    return;
}

// faz a animação e cria a saida
void cria_saida (mapa_t *mapa, ALLEGRO_BITMAP *sprite, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, int cronometro) {
    int i, j;

    for (i = 1; i < mapa->max_alt-1; i++) {
        for (j = 1; j < mapa->max_lar-1; j++) {
            if (mapa->matriz[i][j].tipo == tipo_vazio) {
                mapa->matriz[i][j].tipo = tipo_branco;
                define_sprite (&mapa->matriz[i][j], sprite);
            }
        }
    }

    imprime_jogo (*mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_rest (0.2);

    for (i = 1; i < mapa->max_alt-1; i++) {
        for (j = 1; j < mapa->max_lar-1; j++) {
            if (mapa->matriz[i][j].tipo == tipo_branco) {
                mapa->matriz[i][j].tipo = tipo_vazio;
                define_sprite (&mapa->matriz[i][j], sprite);
            }
        }
    }

    mapa->matriz[mapa->saida_y][mapa->saida_x].tipo = tipo_saida;
    mapa->matriz[mapa->saida_y][mapa->saida_x].num_sprite = 0;
    define_sprite (&mapa->matriz[mapa->saida_y][mapa->saida_x], sprite);

    return;
}

// define o sprite certo para o bloco recebido usando o campo tipo
void define_sprite (bloco *atual, ALLEGRO_BITMAP *sprite) {
    int x, y;

    switch (atual->tipo) {
        case tipo_borda:
            y = borda_alt;
            x = borda_lar;
            break;

        case tipo_inicio:
            y = inicio_alt;
            x = inicio_lar;
            break;

        case tipo_parede:
            y = parede_alt;
            x = parede_lar;
            break;

        case tipo_terra:
            y = terra_alt;
            x = terra_lar;
            break;

        case tipo_vazio:
            y = vazio_alt;
            x = vazio_lar;
            break;
        
        case tipo_pedra:
            y = pedra_alt;
            x = pedra_lar;
            break;
        
        case tipo_explosao_1:
            y = explosao_1_alt;
            x = explosao_1_lar;
            break;

        case tipo_explosao_2:
            y = explosao_2_alt;
            x = explosao_2_lar;
            break;

        case tipo_branco:
            y = branco_alt;
            x = branco_lar;
            break;

        case tipo_moeda_painel:
            y = moeda_painel_alt;
            x = moeda_painel_lar;
            break;

        case tipo_saida:
            switch (atual->num_sprite) {
                case 0:
                    y = borda_alt;
                    x = borda_lar;
                    break;
                
                case 1:
                    y = inicio_alt;
                    x = inicio_lar;
                    break;
            }
            break;

        case tipo_moeda:
            switch (atual->num_sprite) {
                case 0:
                    y = moeda_0_alt;
                    x = moeda_0_lar;
                    break;
                
                case 1:
                    y = moeda_1_alt;
                    x = moeda_1_lar;
                    break;
                
                case 2:
                    y = moeda_2_alt;
                    x = moeda_2_lar;
                    break;

                case 3:
                    y = moeda_3_alt;
                    x = moeda_3_lar;
                    break;

                case 4:
                    y = moeda_4_alt;
                    x = moeda_4_lar;
                    break;

                case 5:
                    y = moeda_5_alt;
                    x = moeda_5_lar;
                    break;

                case 6:
                    y = moeda_6_alt;
                    x = moeda_6_lar;
                    break;

                case 7:
                    y = moeda_7_alt;
                    x = moeda_7_lar;
                    break;
            }   
            break;
        
        case tipo_personagem:
            switch (atual->num_sprite) {
                case 0:
                    y = personagem_0_alt;
                    x = personagem_0_lar;
                    break;

                case 1:
                    y = personagem_1_alt;
                    x = personagem_1_lar;
                    break;
                
                case 2:
                    y = personagem_2_alt;
                    x = personagem_2_lar;
                    break;

                case 3:
                    y = personagem_3_alt;
                    x = personagem_3_lar;
                    break;

                case 4:
                    y = personagem_4_alt;
                    x = personagem_4_lar;
                    break;

                case 5:
                    y = personagem_5_alt;
                    x = personagem_5_lar;
                    break;

                case 6:
                    y = personagem_6_alt;
                    x = personagem_6_lar;
                    break;
            }
            break;
        
        case tipo_personagem_esq:
            switch (atual->num_sprite) {
                case 0:
                    y = personagem_0_esq_alt;
                    x = personagem_0_esq_lar;
                    break;

                case 1:
                    y = personagem_1_esq_alt;
                    x = personagem_1_esq_lar;
                    break;
                
                case 2:
                    y = personagem_2_esq_alt;
                    x = personagem_2_esq_lar;
                    break;

                case 3:
                    y = personagem_3_esq_alt;
                    x = personagem_3_esq_lar;
                    break;

                case 4:
                    y = personagem_4_esq_alt;
                    x = personagem_4_esq_lar;
                    break;

                case 5:
                    y = personagem_5_esq_alt;
                    x = personagem_5_esq_lar;
                    break;

                case 6:
                    y = personagem_6_esq_alt;
                    x = personagem_6_esq_lar;
                    break;
            }
            break;
        
        case tipo_personagem_dir:
            switch (atual->num_sprite) {
                case 0:
                    y = personagem_0_dir_alt;
                    x = personagem_0_dir_lar;
                    break;

                case 1:
                    y = personagem_1_dir_alt;
                    x = personagem_1_dir_lar;
                    break;
                
                case 2:
                    y = personagem_2_dir_alt;
                    x = personagem_2_dir_lar;
                    break;

                case 3:
                    y = personagem_3_dir_alt;
                    x = personagem_3_dir_lar;
                    break;

                case 4:
                    y = personagem_4_dir_alt;
                    x = personagem_4_dir_lar;
                    break;

                case 5:
                    y = personagem_5_dir_alt;
                    x = personagem_5_dir_lar;
                    break;

                case 6:
                    y = personagem_6_dir_alt;
                    x = personagem_6_dir_lar;
                    break;
            }
            break;
    }

    atual->sprite = al_create_sub_bitmap (sprite, x, y, tam_bloco, tam_bloco);

    return;
}

// abre o menu do jogo
int menu (char *nome) {
    int buffer_alt, buffer_lar, display_lar, display_alt;
    int opcoes = 1;
    int res;
    buffer_alt = tam_bloco * 30;
    buffer_lar = tam_bloco * 30;
    display_alt = buffer_alt * 2;
    display_lar = buffer_lar * 2;

    ALLEGRO_KEYBOARD_STATE tecla;
    ALLEGRO_BITMAP *buffer = al_create_bitmap (buffer_lar, buffer_alt);
    ALLEGRO_DISPLAY *janela = al_create_display (display_lar, display_alt);
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_BITMAP *sprite = al_create_bitmap (1,1);

    fonte = al_create_builtin_font ();
    fila_eventos = al_create_event_queue ();
    timer = al_create_timer (1.0 / taxa_quadros);
    al_register_event_source (fila_eventos, al_get_keyboard_event_source ());
    al_register_event_source (fila_eventos, al_get_display_event_source (janela));
    al_register_event_source (fila_eventos, al_get_timer_event_source (timer));


    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_set_target_bitmap (buffer);

        al_clear_to_color (al_map_rgb (0, 0, 0));

        al_draw_text (fonte, al_map_rgb (0, 255, 0), buffer_lar / 2, tam_bloco * 2, ALLEGRO_ALIGN_CENTRE, "MENU");

        if (opcoes == 1)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 7, ALLEGRO_ALIGN_CENTRE, "Iniciar Jogo");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 7, ALLEGRO_ALIGN_CENTRE, "Iniciar Jogo");
        
        if (opcoes == 2)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 9, ALLEGRO_ALIGN_CENTRE, "Placar de Lideres");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 9, ALLEGRO_ALIGN_CENTRE, "Placar de Lideres");

        if (opcoes == 3)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 11, ALLEGRO_ALIGN_CENTRE, "Informações");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 11, ALLEGRO_ALIGN_CENTRE, "Informações");

        if (opcoes == 4)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 13, ALLEGRO_ALIGN_CENTRE, "Fechar Jogo");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 13, ALLEGRO_ALIGN_CENTRE, "Fechar Jogo");

        al_set_target_backbuffer (janela);
        al_draw_scaled_bitmap (buffer, 0, 0, buffer_lar, buffer_alt, 0, 0, display_lar, display_alt, 0);
        al_flip_display ();  


        al_wait_for_event (fila_eventos, &evento);
        al_get_keyboard_state (&tecla);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (al_key_down (&tecla, ALLEGRO_KEY_DOWN)) {
                opcoes++;
                if (opcoes == 5)
                    opcoes = 1;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_UP)) {
                opcoes--;
                if (opcoes == 0)
                    opcoes = 4;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_ENTER) || al_key_down (&tecla, ALLEGRO_KEY_PAD_ENTER)) {
                switch (opcoes) {
                case 1:
                    res = iniciar_jogo_menu (nome, buffer, janela, fonte, evento, fila_eventos);
                    if (res) {
                        encerra_allegro (janela, buffer, sprite, fonte, fila_eventos, timer);
                        return res;
                    }
                    break;

                case 2:
                    mostra_lideres (buffer, janela, fonte, evento, fila_eventos);
                    break;

                case 3:
                    tela_ajuda (janela, fonte);
                    break;

                case 4:
                    encerra_allegro (janela, buffer, sprite, fonte, fila_eventos, timer);
                    return 0;
                    break;
                }
            }
        }
    }

    encerra_allegro (janela, buffer, sprite, fonte, fila_eventos, timer);

    return 0;
}

// abre a opção iniciar jogo do menu
int iniciar_jogo_menu (char *nome, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos) {
    int buffer_alt, buffer_lar, display_lar, display_alt;
    int opcoes = 1;
    buffer_alt = tam_bloco * 30;
    buffer_lar = tam_bloco * 20;
    display_alt = buffer_alt * 2;
    display_lar = buffer_lar * 2;

    ALLEGRO_KEYBOARD_STATE tecla;

    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_set_target_bitmap (buffer);

        al_clear_to_color (al_map_rgb (0, 0, 0));

        if (opcoes == 1)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 7, ALLEGRO_ALIGN_CENTRE, "Mapa 1");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 7, ALLEGRO_ALIGN_CENTRE, "Mapa 1");

        if (opcoes == 2)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 9, ALLEGRO_ALIGN_CENTRE, "Mapa 2");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 9, ALLEGRO_ALIGN_CENTRE, "Mapa 2");
        
        if (opcoes == 3)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 11, ALLEGRO_ALIGN_CENTRE, "Mapa 3");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 11, ALLEGRO_ALIGN_CENTRE, "Mapa 3");

        if (opcoes == 4)
            al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 13, ALLEGRO_ALIGN_CENTRE, "Voltar");
        else
            al_draw_text (fonte, al_map_rgb (255, 255, 255), buffer_lar / 2, tam_bloco * 13, ALLEGRO_ALIGN_CENTRE, "Voltar");

        al_set_target_backbuffer (janela);
        al_draw_scaled_bitmap (buffer, 0, 0, buffer_lar, buffer_alt, 0, 0, display_lar, display_alt, 0);
        al_flip_display ();  


        al_wait_for_event (fila_eventos, &evento);
        al_get_keyboard_state (&tecla);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (al_key_down (&tecla, ALLEGRO_KEY_DOWN)) {
                opcoes++;
                if (opcoes == 5)
                    opcoes = 1;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_UP)) {
                opcoes--;
                if (opcoes == 0)
                    opcoes = 4;
            }
            else if (al_key_down (&tecla, ALLEGRO_KEY_ENTER) || al_key_down (&tecla, ALLEGRO_KEY_PAD_ENTER)) {
                switch (opcoes) {
                case 1:
                    le_nome (nome, buffer, janela, fonte, evento, fila_eventos);
                    return 1;
                    break;

                case 2:
                    le_nome (nome, buffer, janela, fonte, evento, fila_eventos);
                    return 2;
                    break;

                case 3:
                    le_nome (nome, buffer, janela, fonte, evento, fila_eventos);
                    return 3;
                    break;

                case 4:
                    return 0;
                    break;
                }
            }
        }
    }

    return 0;
}

// imprime na tela as melhores pontuações de cada mapa
void mostra_lideres (ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos) {
    int buffer_alt, buffer_lar, display_lar, display_alt;
    int i, tam_1, tam_2, tam_3;
    char nomes_1[12][12];
    char nomes_2[12][12];
    char nomes_3[12][12];
    int valores_1[12];
    int valores_2[12];
    int valores_3[12];
    DIR *diretorio;
    FILE *arquivo;
    ALLEGRO_KEYBOARD_STATE tecla;


    buffer_alt = tam_bloco * 30;
    buffer_lar = tam_bloco * 30;
    display_alt = buffer_alt * 2;
    display_lar = buffer_lar * 2;
    diretorio = opendir (caminho_lideres);
    arquivo = fopen ("resources/lideres/placares_mapa1.txt", "r");

    fscanf (arquivo, "%d", &tam_1);
    for (i = 0; i < tam_1; i++) {
        fscanf (arquivo, "%s %d", nomes_1[i], &valores_1[i]);
    }
    fclose (arquivo);


    arquivo = fopen ("resources/lideres/placares_mapa2.txt", "r");
    fscanf (arquivo, "%d", &tam_2);
    for (i = 0; i < tam_2; i++) {
        fscanf (arquivo, "%s %d", nomes_2[i], &valores_2[i]);
    }
    fclose (arquivo);


    arquivo = fopen ("resources/lideres/placares_mapa3.txt", "r");
    fscanf (arquivo, "%d", &tam_3);
    for (i = 0; i < tam_3; i++) {
        fscanf (arquivo, "%s %d", nomes_3[i], &valores_3[i]);
    }
    fclose (arquivo);


    al_set_target_bitmap (buffer);

    al_clear_to_color (al_map_rgb (0, 0, 0));

    al_draw_text (fonte, al_map_rgb (255, 215, 0), buffer_lar / 2, tam_bloco * 20, ALLEGRO_ALIGN_CENTRE, "Voltar");
    al_draw_text (fonte, al_map_rgb (0, 255, 0), buffer_lar / 2, tam_bloco, ALLEGRO_ALIGN_CENTRE, "Melhores Pontuações");

    al_draw_text (fonte, al_map_rgb (0, 75, 0), tam_bloco + 8, tam_bloco * 4, 0, "Mapa 1");
    al_draw_text (fonte, al_map_rgb (0, 75, 0), (tam_bloco * 12) + 8, tam_bloco * 4, 0, "Mapa 2");
    al_draw_text (fonte, al_map_rgb (0, 75, 0), (tam_bloco * 22) + 8, tam_bloco * 4, 0, "Mapa 3");

    for (i = 0; i < tam_1; i++) {
        switch (i) {
            case 0:
                al_draw_textf (fonte, al_map_rgb (255, 215, 0), tam_bloco, tam_bloco * (i+6), 0, "%s %d", nomes_1[i], valores_1[i]);
                break;
            
            case 1:
                al_draw_textf (fonte, al_map_rgb (80, 80, 80), tam_bloco, tam_bloco * (i+6), 0, "%s %d", nomes_1[i], valores_1[i]);
                break;

            case 2:
                al_draw_textf (fonte, al_map_rgb (150, 75, 0), tam_bloco, tam_bloco * (i+6), 0, "%s %d", nomes_1[i], valores_1[i]);
                break;

            default:
                al_draw_textf (fonte, al_map_rgb (255, 255, 255), tam_bloco, tam_bloco * (i+6), 0, "%s %d", nomes_1[i], valores_1[i]);
                break;
        }
    }
    for (i = 0; i < tam_2; i++) {
        switch (i) {
            case 0:
                al_draw_textf (fonte, al_map_rgb (255, 215, 0), tam_bloco * 12, tam_bloco * (i+6), 0, "%s %d", nomes_2[i], valores_2[i]);
                break;
            
            case 1:
                al_draw_textf (fonte, al_map_rgb (80, 80, 80), tam_bloco * 12, tam_bloco * (i+6), 0, "%s %d", nomes_2[i], valores_2[i]);
                break;

            case 2:
                al_draw_textf (fonte, al_map_rgb (150, 75, 0), tam_bloco * 12, tam_bloco * (i+6), 0, "%s %d", nomes_2[i], valores_2[i]);
                break;

            default:
                al_draw_textf (fonte, al_map_rgb (255, 255, 255), tam_bloco * 12, tam_bloco * (i+6), 0, "%s %d", nomes_2[i], valores_2[i]);
                break;
        }
    }
    for (i = 0; i < tam_3; i++) {
        switch (i) {
            case 0:
                al_draw_textf (fonte, al_map_rgb (255, 215, 0), tam_bloco * 22, tam_bloco * (i+6), 0, "%s %d", nomes_3[i], valores_3[i]);
                break;
            
            case 1:
                al_draw_textf (fonte, al_map_rgb (80, 80, 80), tam_bloco * 22, tam_bloco * (i+6), 0, "%s %d", nomes_3[i], valores_3[i]);
                break;

            case 2:
                al_draw_textf (fonte, al_map_rgb (150, 75, 0), tam_bloco * 22, tam_bloco * (i+6), 0, "%s %d", nomes_3[i], valores_3[i]);
                break;

            default:
                al_draw_textf (fonte, al_map_rgb (255, 255, 255), tam_bloco * 22, tam_bloco * (i+6), 0, "%s %d", nomes_3[i], valores_3[i]);
                break;
        }
    }
    
    al_set_target_backbuffer (janela);
    al_draw_scaled_bitmap (buffer, 0, 0, buffer_lar, buffer_alt, 0, 0, display_lar, display_alt, 0);
    al_flip_display ();  

    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_wait_for_event (fila_eventos, &evento);
        al_get_keyboard_state (&tecla);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (al_key_down (&tecla, ALLEGRO_KEY_ENTER) || al_key_down (&tecla, ALLEGRO_KEY_PAD_ENTER)) {
                return;
            }
        }
    }
    
    closedir (diretorio);

    return;
}

// le o nome do jogador
void le_nome (char *nome, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos) {
    int i, tam;
    int buffer_alt, buffer_lar, display_lar, display_alt;
    char letra;

    buffer_alt = tam_bloco * 30;
    buffer_lar = tam_bloco * 20;
    display_alt = buffer_alt * 2;
    display_lar = buffer_lar * 2;

    ALLEGRO_KEYBOARD_STATE tecla;

    tam = 0;
    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_set_target_bitmap (buffer);

        al_clear_to_color (al_map_rgb (0, 0, 0));

        al_draw_text (fonte, al_map_rgb (0, 255, 0), buffer_lar / 2, tam_bloco * 3, ALLEGRO_ALIGN_CENTRE, "Coloque seu nome:");
        

        for (i = 0; i < tam; i++) {
            al_draw_textf (fonte, al_map_rgb (255, 255, 255), (tam_bloco * 10) + (i * 10), tam_bloco * 10, ALLEGRO_ALIGN_CENTRE, "%c", nome[i]);
        }

        al_set_target_backbuffer (janela);
        al_draw_scaled_bitmap (buffer, 0, 0, buffer_lar, buffer_alt, 0, 0, display_lar, display_alt, 0);
        al_flip_display ();  

        al_wait_for_event (fila_eventos, &evento);
        al_get_keyboard_state (&tecla);
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            letra = reconhece_tecla (evento);
            if (letra == '+') {
                return;
            }
            else if (letra != '*') {
                if (letra == '-') {
                    if (tam > 0) {
                        nome[tam] = '\0';
                        tam--;
                    }
                }
                else if (tam < 10){
                    nome[tam] = letra;
                    tam++;
                    nome[tam] = '\0';
                }
            }
        }
    }

    return; 
}

// soma o restante do cronometro ao placar
void atualiza_placar (mapa_t mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int *placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int *cronometro) {

    while (*cronometro > 0) {
        *cronometro = *cronometro - 1;
        *placar = *placar + 1;
        imprime_jogo (mapa, buffer, janela, *placar, fonte, sprite, *cronometro);
        al_rest (0.02);
    }

    return;
}

char reconhece_tecla (ALLEGRO_EVENT evento) {

    switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_ENTER:
            return '+';
            break;

        case ALLEGRO_KEY_PAD_ENTER:
            return '+';
            break;

        case ALLEGRO_KEY_BACKSPACE:
            return '-';
            break;

        case ALLEGRO_KEY_A:
            return 'a';
            break;

        case ALLEGRO_KEY_B:
            return 'b';
            break;
        
        case ALLEGRO_KEY_C:
            return 'c';
            break;

        case ALLEGRO_KEY_D:
            return 'd';
            break;

        case ALLEGRO_KEY_E:
            return 'e';
            break;
        
        case ALLEGRO_KEY_F:
            return 'f';
            break;

        case ALLEGRO_KEY_G:
            return 'g';
            break;

        case ALLEGRO_KEY_H:
            return 'h';
            break;

        case ALLEGRO_KEY_I:
            return 'i';
            break;

        case ALLEGRO_KEY_J:
            return 'j';
            break;

        case ALLEGRO_KEY_K:
            return 'k';
            break;

        case ALLEGRO_KEY_L:
            return 'l';
            break;

        case ALLEGRO_KEY_M:
            return 'm';
            break;

        case ALLEGRO_KEY_N:
            return 'n';
            break;

        case ALLEGRO_KEY_O:
            return 'o';
            break;

        case ALLEGRO_KEY_P:
            return 'p';
            break;

        case ALLEGRO_KEY_Q:
            return 'q';
            break;

        case ALLEGRO_KEY_R:
            return 'r';
            break;

        case ALLEGRO_KEY_S:
            return 's';
            break;

        case ALLEGRO_KEY_T:
            return 't';
            break;

        case ALLEGRO_KEY_U:
            return 'u';
            break;

        case ALLEGRO_KEY_V:
            return 'v';
            break;

        case ALLEGRO_KEY_W:
            return 'w';
            break;

        case ALLEGRO_KEY_X:
            return 'x';
            break;

        case ALLEGRO_KEY_Y:
            return 'y';
            break;

        case ALLEGRO_KEY_Z:
            return 'z';
            break;

        case ALLEGRO_KEY_0:
            return '0';
            break;
        
        case ALLEGRO_KEY_PAD_0:
            return '0';
            break;

        case ALLEGRO_KEY_1:
            return '1';
            break;
        
        case ALLEGRO_KEY_PAD_1:
            return '1';
            break;

        case ALLEGRO_KEY_2:
            return '2';
            break;

        case ALLEGRO_KEY_PAD_2:
            return '2';
            break;

        case ALLEGRO_KEY_3:
            return '3';
            break;

        case ALLEGRO_KEY_PAD_3:
            return '3';
            break;

        case ALLEGRO_KEY_4:
            return '4';
            break;
        
        case ALLEGRO_KEY_PAD_4:
            return '4';
            break;

        case ALLEGRO_KEY_5:
            return '5';
            break;

        case ALLEGRO_KEY_PAD_5:
            return '5';
            break;

        case ALLEGRO_KEY_6:
            return '6';
            break;

        case ALLEGRO_KEY_PAD_6:
            return '6';
            break;

        case ALLEGRO_KEY_7:
            return '7';
            break;
        
        case ALLEGRO_KEY_PAD_7:
            return '7';
            break;

        case ALLEGRO_KEY_8:
            return '8';
            break;

        case ALLEGRO_KEY_PAD_8:
            return '8';
            break;

        case ALLEGRO_KEY_9:
            return '9';
            break;

        case ALLEGRO_KEY_PAD_9:
            return '9';
            break;

        default:
            return '*';
            break;
    }
}