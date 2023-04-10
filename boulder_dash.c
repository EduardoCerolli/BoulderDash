#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "lib_boulder.h"
#include "defines.h"



int main () {
    bloco_aux personagem;
    mapa_t mapa;
    int resultado = normal;
    int placar = 0;
    char nome[12];
    int desenhar = 0;
    int cronometro = tempo_total;
    int contador = 0;
    int num_mapa;

    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *buffer = NULL;
    ALLEGRO_BITMAP *sprite = NULL;
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_TIMER *timer_2 = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_EVENT evento;


    inicializa_jogo (&num_mapa, nome, &personagem, &mapa, &janela, &buffer, &sprite, &fonte, &timer, &timer_2, &fila_eventos, evento);
    cria_personagem (personagem, &mapa, buffer, janela, placar, fonte, sprite, cronometro);
    al_start_timer (timer);
    al_start_timer (timer_2);
    
    while (evento.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_wait_for_event (fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            if (evento.timer.source == timer) {
                desenhar = 1;
            }
            else {
                cronometro--;
            }
        }

        if (contador == 8) {
            if (personagem_vivo (personagem, mapa)) {
                resultado = atualiza_jogo (&mapa, &personagem, sprite, &placar, evento, janela, fonte);
                if (resultado == fechou) {
                    break;
                }
                else if (resultado == ganhou) {
                    while (move_pedras_moedas (&mapa, sprite)) {
                        al_wait_for_event (fila_eventos, &evento);
                        imprime_jogo (mapa, buffer, janela, placar, fonte, sprite, cronometro);
                    }
                    imprime_jogo (mapa, buffer, janela, placar, fonte, sprite, cronometro);
                    atualiza_placar (mapa, buffer, janela, &placar, fonte, sprite, &cronometro);
                    al_rest (1);
                    fim_jogo (ganhou, placar, nome, num_mapa);
                    break;
                }

                if (mapa.moedas_pegas == mapa.moedas_necessarias && mapa.matriz[mapa.saida_y][mapa.saida_x].tipo != tipo_saida) {
                    cria_saida (&mapa, sprite, buffer, janela, placar, fonte, cronometro);
                }
            }
            else {
                explode (personagem, &mapa, buffer, janela, placar, fonte, sprite, cronometro);
                while (move_pedras_moedas (&mapa, sprite)) {
                    al_wait_for_event (fila_eventos, &evento);
                    imprime_jogo (mapa, buffer, janela, placar, fonte, sprite, cronometro);
                }
                fim_jogo (perdeu, placar, nome, num_mapa);
                break;
            }
        }

        if (desenhar) {
            imprime_jogo (mapa, buffer, janela, placar, fonte, sprite, cronometro);
            desenhar = 0;
        }

        contador++;
        if (contador == 9) {
            contador = 1;
        }
    }

    destroi_mapa (mapa);

    return 0;
}