typedef struct bloco {
    ALLEGRO_BITMAP * sprite;
    int x, y;
    int num_sprite;
    int tipo;
    int movimento;
} bloco;

typedef struct mapa_t {
    bloco **matriz;
    int max_alt, max_lar;
    int max_moeadas;
    int moedas_pegas;
    int moedas_necessarias;
    int buffer_lar, buffer_alt;
    int saida_y, saida_x;
} mapa_t;

typedef struct bloco_aux {
    int x, y;
} bloco_aux;

void inicializa_jogo (int *num_mapa, char *nome, bloco_aux *personagem, mapa_t *mapa, ALLEGRO_DISPLAY **janela, ALLEGRO_BITMAP **buffer, ALLEGRO_BITMAP **sprite, ALLEGRO_FONT **fonte, ALLEGRO_TIMER **timer, ALLEGRO_TIMER **timer_2, ALLEGRO_EVENT_QUEUE **fila_eventos, ALLEGRO_EVENT evento);
void encerra_allegro (ALLEGRO_DISPLAY *janela, ALLEGRO_BITMAP *buffer, ALLEGRO_BITMAP *sprite, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *fila_eventos, ALLEGRO_TIMER *timer);
void destroi_mapa (mapa_t mapa);
void cria_personagem (bloco_aux personagem, mapa_t *mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro);
void imprime_jogo (mapa_t mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro);
void troca_bloco (bloco *bloco_1, bloco *bloco_2, ALLEGRO_BITMAP *sprite, int movimento);
void empurra_pedra (bloco *bloco_1, bloco *bloco_2, bloco *bloco_3, ALLEGRO_BITMAP *sprite, int mov);
int pode_mover (bloco proximo, bloco proximo_proximo, int mov);
void tela_ajuda (ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte);
int move_personagem (mapa_t *mapa, bloco_aux *personagem, ALLEGRO_BITMAP *sprite, int *placar, ALLEGRO_EVENT evento, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte);
int move_pedras_moedas (mapa_t *mapa, ALLEGRO_BITMAP *sprite);
void pisca_jogo (mapa_t *mapa, ALLEGRO_BITMAP *sprite);
int atualiza_jogo (mapa_t *mapa, bloco_aux *personagem, ALLEGRO_BITMAP *sprite, int *placar, ALLEGRO_EVENT evento, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte);
int personagem_vivo (bloco_aux personagem, mapa_t mapa);
void fim_jogo (int resultado, int placar, char *nome, int num_mapa);
void escreve_placares (char *nome, int placar, int num_mapa);
void explode (bloco_aux personagem, mapa_t *mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int cronometro);
void cria_saida (mapa_t *mapa, ALLEGRO_BITMAP *sprite, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int placar, ALLEGRO_FONT *fonte, int cronometro);
void define_sprite (bloco *atual, ALLEGRO_BITMAP *sprite);
int menu (char *nome);
int iniciar_jogo_menu (char *nome, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos);
void mostra_lideres (ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos);
void le_nome (char *nome, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *fonte, ALLEGRO_EVENT evento, ALLEGRO_EVENT_QUEUE *fila_eventos);
void atualiza_placar (mapa_t mapa, ALLEGRO_BITMAP *buffer, ALLEGRO_DISPLAY *janela, int *placar, ALLEGRO_FONT *fonte, ALLEGRO_BITMAP *sprite, int *cronometro);
char reconhece_tecla (ALLEGRO_EVENT evento);