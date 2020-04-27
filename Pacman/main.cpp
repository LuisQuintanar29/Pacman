#include<allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXFILAS 19
#define MAXCOLUMNAS 31

struct MAPAS{
    BITMAP *buffer;
    BITMAP *roca;
    BITMAP *comida;
    FONT *fuente;
    char **mapa_gen;
};
typedef struct MAPAS mapas;

struct DIRECCIONES{
    int dir;
    int p_x;
    int p_y;
};
typedef struct DIRECCIONES Direcciones;

struct PACMAN{
    BITMAP *pacbmp;
    BITMAP *pacman;
    BITMAP *muerte;
    Direcciones *dirs;
    int anterior_px;
    int anterior_py;
    int puntos;
};
typedef struct PACMAN Pacman;

struct FANTASMA{
    BITMAP *fantasma_bmp;
    BITMAP *fantasma;
    Direcciones *dirs;
};
typedef struct FANTASMA Fantasma;

char **AsignarMapa(int n){
    int a=0,b;
    char **Mapa;
    char c;
    FILE *fich;
    if(n==1)
        fich = fopen("mapa_1.txt", "r");
    else
        if(n==2)
            fich = fopen("mapa_2.txt", "r");
        else
            fich = fopen("mapa_3.txt","r");
    Mapa = (char**)malloc(19*sizeof(char*));
    while(a<19){
        Mapa[a] = (char*)malloc(32*sizeof(char));
        fgets(Mapa[a], 32, (FILE*) fich);
        strtok(Mapa[a],"\n");
        a++;
    }
    fclose(fich);
    return Mapa;
}

void dibujar_mapa(mapas *Mapas, Pacman *pac){
    int filas, columnas;
    textprintf(screen,Mapas->fuente,0,0,-1,"Presione [ESC] para salir            Puntaje %d",pac->puntos);
    for(filas=0;filas<MAXFILAS;filas++){
        for(columnas=0;columnas<MAXCOLUMNAS;columnas++){
            if(Mapas->mapa_gen[filas][columnas]=='X'){
                draw_sprite(Mapas->buffer,Mapas->roca,columnas*30, filas*30); //30 es el tamaño de la imagen
            }
            else{
                if(Mapas->mapa_gen[filas][columnas] == '0'){
                    draw_sprite(Mapas->buffer,Mapas->comida,columnas*30, filas*30);
                    if(pac->dirs->p_y/30 == filas && pac->dirs->p_x/30 == columnas){
                        Mapas->mapa_gen[filas][columnas] = ' ';
                        pac->puntos+=10;
                    }
                }
            }
        }
    }

}
void pantalla(mapas *Mapas){
    blit(Mapas->buffer,screen,0,0,0,30,900,570);
}
Pacman *nuevo_pacman(){
    Pacman *n_pacman =(Pacman*) malloc(sizeof(Pacman));
    n_pacman->dirs =(Direcciones*) malloc(sizeof(Direcciones));
    n_pacman->pacbmp = load_bitmap("pacman.bmp",NULL);
    n_pacman->muerte = load_bitmap("muerte.bmp",NULL);
    n_pacman->pacman = create_bitmap(33,33);
    n_pacman->dirs->dir = 5;
    n_pacman->dirs->p_x = 30*15;
    n_pacman->dirs->p_y = 30*13;
    n_pacman->puntos = 0;
    return n_pacman;
}
Fantasma *nuevo_fantasma(){
    Fantasma *n_fantasma = (Fantasma*) malloc(sizeof(Fantasma));
    n_fantasma->dirs =(Direcciones*) malloc(sizeof(Direcciones));
    n_fantasma->fantasma_bmp = load_bitmap("enemigo.bmp",NULL);
    n_fantasma->fantasma = create_bitmap(30,30);
    n_fantasma->dirs->dir = 0;
    n_fantasma->dirs->p_x = 30*(rand()%3+1)*5;
    n_fantasma->dirs->p_y = 30*(rand()%3+1)*5;

    return n_fantasma;
}
void dibujar_personaje(Pacman *pac, BITMAP *buffer){
    blit(pac->pacbmp,pac->pacman,pac->dirs->dir*33,0,0,0,33,33);
    draw_sprite(buffer,pac->pacman,pac->dirs->p_x,pac->dirs->p_y);
}
void dibujar_Fantasma(Fantasma *fan, BITMAP *buffer){
    blit(fan->fantasma_bmp,fan->fantasma,0,0,0,0,30,30);
    draw_sprite(buffer,fan->fantasma,fan->dirs->p_x,fan->dirs->p_y);
}
int Game_Over(mapas *mapa){
    int filas, columnas;
    for(filas=0;filas<MAXFILAS;filas++){
        for(columnas=0;columnas<MAXCOLUMNAS;columnas++){
            if(mapa->mapa_gen[filas][columnas]=='0'){
                return 1;
            }
        }
    }
    mapa->mapa_gen = AsignarMapa(rand()%3+1);
    return 1;
}
void choque(Fantasma *fan,Pacman *pac,mapas *mapas){
    int i;
    if((pac->dirs->p_y == fan->dirs->p_y && pac->dirs->p_x == fan->dirs->p_x )|| (fan->dirs->p_y == pac->anterior_py && fan->dirs->p_x == pac->anterior_px)){
        for (i=0;i< 5;i++){
            clear(pac->pacman);
            clear(mapas->buffer);
            dibujar_mapa(mapas,pac);
            blit(pac->muerte,pac->pacman,i*33,0,0,0,33,33);
            draw_sprite(mapas->buffer,pac->pacman,pac->dirs->p_x,pac->dirs->p_y);
            pantalla(mapas);
            rest(80);
        }
        pac->dirs->p_x =15*30;
        pac->dirs->p_y =13*30;
        pac->dirs->dir = 4;
        fan->dirs->p_x = 30*(rand()%3+1)*5;
        fan->dirs->p_y = 30*(rand()%3+1)*5;
    }
}
void fantasma(Fantasma *fan,mapas *Mapas,Pacman *pac){
    dibujar_Fantasma(fan,Mapas->buffer);
    choque(fan,pac,Mapas);
    if(Mapas->mapa_gen[fan->dirs->p_y/30][fan->dirs->p_x/30] == '|'){
        fan->dirs->dir = rand()%4;
    }
    if(fan->dirs->dir == 0){
        if(Mapas->mapa_gen[fan->dirs->p_y/30][(fan->dirs->p_x-30)/30] != 'X'){
            fan->dirs->p_x -=30;
        }
        else{
            fan->dirs->dir = rand()%4;
        }
    }
    else
        if(fan->dirs->dir == 1){
            if(Mapas->mapa_gen[fan->dirs->p_y/30][(fan->dirs->p_x+30)/30] != 'X'){
                fan->dirs->p_x +=30;
            }
            else{
                fan->dirs->dir = rand()%4;
            }
        }
        else
            if(fan->dirs->dir == 2){
                if(Mapas->mapa_gen[(fan->dirs->p_y-30)/30][fan->dirs->p_x/30] != 'X'){
                    fan->dirs->p_y -=30;
                }
                else{
                    fan->dirs->dir = rand()%4;
                }
            }
            else
                if(fan->dirs->dir == 3){
                    if(Mapas->mapa_gen[(fan->dirs->p_y+30)/30][fan->dirs->p_x/30] != 'X'){
                        fan->dirs->p_y +=30;
                    }
                    else{
                        fan->dirs->dir = rand()%4;
                    }
                }
    if(fan->dirs->p_x <= -30) fan->dirs->p_x = 900;
        else
            if(fan->dirs->p_x >= 900) fan->dirs->p_x = -30;
}
int main(){
    allegro_init();
    install_keyboard();
    install_mouse();

    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED,900,600,0,0);
    int i,j,salida = 0;
    int GHOST = 5;
    BITMAP *portada,*cursor,*portada_start,*portada_exit;
    mapas *nuevos_mapas = (mapas*)malloc(sizeof(mapas));
    Pacman *pac = nuevo_pacman();
    Fantasma *p[GHOST];
    for(i=0;i<GHOST;i++){
        p[i]=nuevo_fantasma();
    }

    nuevos_mapas->mapa_gen = AsignarMapa(2);
    nuevos_mapas->buffer = create_bitmap(900,570);
    nuevos_mapas->roca = load_bitmap("roca.bmp",NULL);
    nuevos_mapas->comida = load_bitmap("Comida.bmp",NULL);
    nuevos_mapas->fuente = load_font("font1.pcx",NULL,NULL);

    portada = load_bitmap("principal.bmp",NULL);
    portada_exit = load_bitmap("principal_SAlIR.bmp",NULL);
    portada_start = load_bitmap("principal_START.bmp",NULL);
    cursor = load_bitmap("cursor.bmp",NULL);

    while(!salida){
        if(mouse_x>140 && mouse_x<450 && mouse_y>410 && mouse_y < 470){
            blit(portada_start,nuevos_mapas->buffer,0,0,0,30,900,570);
            if(mouse_b & 1){
                salida = 1;
            }
        }
        else
            if(mouse_x>150 && mouse_x<290 && mouse_y>480 && mouse_y <540){
                blit(portada_exit,nuevos_mapas->buffer,0,0,0,30,900,570);
                if(mouse_b & 1){
                    return 0;
                }
            }
            else
                blit(portada,nuevos_mapas->buffer,0,0,0,30,900,570);
        masked_blit(cursor,nuevos_mapas->buffer,0,0,mouse_x,mouse_y,13,22);
        blit(nuevos_mapas->buffer,screen,0,0,0,0,900,570);
        clear(nuevos_mapas->buffer);
    }
    while(!key[KEY_ESC] && Game_Over(nuevos_mapas)){
        pac->anterior_px = pac->dirs->p_x;
        pac->anterior_py = pac->dirs->p_y;
        if(key[KEY_RIGHT]) pac->dirs->dir=1;
        else
            if(key[KEY_LEFT]) pac->dirs->dir=0;
            else
                if(key[KEY_UP]) pac->dirs->dir=2;
                else
                    if(key[KEY_DOWN]) pac->dirs->dir=3;
        if(pac->dirs->dir == 0){
            if(nuevos_mapas->mapa_gen[pac->dirs->p_y/30][(pac->dirs->p_x-30)/30]!= 'X')
                pac->dirs->p_x -=30;
        }
        else
            if(pac->dirs->dir == 1){
                if(nuevos_mapas->mapa_gen[pac->dirs->p_y/30][(pac->dirs->p_x+30)/30]!= 'X')
                    pac->dirs->p_x +=30;
            }
            else
                if(pac->dirs->dir == 2){
                    if(nuevos_mapas->mapa_gen[(pac->dirs->p_y-30)/30][pac->dirs->p_x/30]!= 'X')
                        pac->dirs->p_y -=30;
                }
                else
                    if(pac->dirs->dir == 3){
                        if(nuevos_mapas->mapa_gen[(pac->dirs->p_y+30)/30][pac->dirs->p_x/30]!= 'X')
                            pac->dirs->p_y +=30;
                    }
        if(pac->dirs->p_x <= -30 || pac->anterior_px <=-30) pac->dirs->p_x = 900;
        else
            if(pac->dirs->p_x >= 900) pac->dirs->p_x = 0;
        clear(nuevos_mapas->buffer);

        dibujar_mapa(nuevos_mapas,pac);
        dibujar_personaje(pac,nuevos_mapas->buffer);
        for(i =0;i<GHOST;i++){
            fantasma(p[i],nuevos_mapas,pac);
        }
        pantalla(nuevos_mapas);
        rest(70);

        clear(pac->pacman);
        blit(pac->pacbmp,pac->pacman,4*33,0,0,0,33,33);
        draw_sprite(nuevos_mapas->buffer,pac->pacman,pac->dirs->p_x,pac->dirs->p_y);
        pantalla(nuevos_mapas);
        rest(70);
    }
    return 0;
}
END_OF_MAIN();
