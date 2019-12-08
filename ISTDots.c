
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX(a,b)    (((a)>(b))?(a):(b))
#define SQR(a)      (a)*(a)
#define M_PI 3.14159265
#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 850        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 15      // maximum size of the board
#define MIN_BOARD_POS 5
#define MAX_COLORS 5        //maximum number of diferent colors
#define MARGIN 5
#define MAXROWS 50
#define MAXCOLS 50
#define MAXNAME 9       //maximum size of the username of the player
#define MAX_POINTS 99   //maximum number of total points of colors allowed
#define MAX_MOVES 99    //maximum number of moves allowed
#define VECTOR_MAX 10000  //max value of any given vector

//declaration of the functions related to the program running
void generate_game(int *,int *,int *,int *,int [VECTOR_MAX],char [VECTOR_MAX],int [MAX_BOARD_POS][MAX_BOARD_POS]);    //function that generates the game
void generate_board(int ,int ,int,int [MAX_BOARD_POS][MAX_BOARD_POS]);      ////function that generates the board for the user
int play_test(int,int [VECTOR_MAX],int [VECTOR_MAX],int [VECTOR_MAX],int ,int );                 //function that tests if a play is valid or not
void removedots(int ,int ,int ,int[MAX_BOARD_POS][MAX_BOARD_POS] ,int[VECTOR_MAX] ,int[VECTOR_MAX] );       //function that marks the dots on the board to be later removed
void dropdots(int [MAX_BOARD_POS][MAX_BOARD_POS],int ,int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS]);        //function that makes the drops fall down and create space for new ones to appear
void generatedots(int [MAX_BOARD_POS][MAX_BOARD_POS],int ,int ,int );   //function that generates new dots to appear aftear a play is done
int square_detetion(int [VECTOR_MAX],int [VECTOR_MAX],int ,int ,int,int [MAX_BOARD_POS][MAX_BOARD_POS]);        //function that detects squares
void remove_colordots(int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS], int ,int *);      //function that markes all the color dots of the square
void generatedots_square(int [MAX_BOARD_POS][MAX_BOARD_POS],int ,int ,int ,int );       //function that generates dots that are not fromm the same color than the square
void explode_dots(int [MAX_BOARD_POS][MAX_BOARD_POS],int ,int );        //function that makes dots disapear if they are inside a square or any other given figure
void undo_board_save(int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS],int [MAX_BOARD_POS][MAX_BOARD_POS]);      //function that saves a board for undo
void undo_board_creation(int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS],int [MAX_BOARD_POS][MAX_BOARD_POS]);        //function tat applies the undo
int shuffle_test(int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS]);     //function that is periodically checking if it is need to shuffle or not
void shuffle_board(int ,int ,int [MAX_BOARD_POS][MAX_BOARD_POS]);       //function that shuffles the board of the game
int victory_test(int ,int [VECTOR_MAX],int );       //function that prints victory sentence for the game
void file_creat(char [VECTOR_MAX],int ,int ,int ,char [VECTOR_MAX] ,int ,int [VECTOR_MAX]);       //generates a file and writes over it

// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, TTF_Font **, TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
int RenderTable(int, int, int [],int ,int ,int ,int ,int ,int ,int [VECTOR_MAX],int [VECTOR_MAX], TTF_Font *, SDL_Surface **, SDL_Renderer *);
void ProcessMouseEvent(int , int , int [], int , int *, int * );
void RenderPoints(int [][MAX_BOARD_POS], int, int, int [],int ,int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, TTF_Font *, int [], int , int );
void filledCircleRGBA(SDL_Renderer * , int , int , int , int , int , int );

// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Andre Pereira";
const char myNumber[] = "IST190016";
const int colors[3][MAX_COLORS] = {{91, 170, 54, 255, 253},{32, 240, 61, 174, 118},{94, 183, 105, 3, 144}};
//work done by Andre Pereira,number 190016,game istdots first project of programing chair,to be delivered on the day 15/04/2018

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( void )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *sans = NULL;
    TTF_Font *sansbig= NULL;
    SDL_Surface *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px = 0, board_size_px[2] = {0};
    int board_pos_x = 0, board_pos_y = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int pt_x = 0, pt_y = 0;
    int nrows=0,ncols=0,moves=0,movesi=0;       //saves the rows,columns,moves and the beginning moves that the player chooses
    int h=0,l=0,t=0;        //counters used in for functions through the function
    int ncolors=0;     //number of diferents colors allowed to appear on board
    int points[VECTOR_MAX]={0};     //vector that saves the points of each color through the game
    char username[VECTOR_MAX]={0};      //vector that saves the username
    int color_delete=0,countcompare=0;  //color_delete marks the color of the play;countcompoare is always cheking the play at any given moment
    int compare[VECTOR_MAX]={0};    //vector that saves a path comparison to evaluate the play
    int mousebuttondown=0;      //flag to check is the mouse button is down
    int valid_play=0,pt_x_i=0,pt_y_i=0;     //flag to say if a play is valid,and x and y coordenate of the point of the first "house" pressed by the player
    int path_track=0,outside_board_error=0;     //path_track will save the real amount of houses the player has "travelled"
    int pt_x_r[VECTOR_MAX]={0},pt_y_r[VECTOR_MAX]={0};      //really important.Keeps track of the true path of the player in a simple way(places where he moved the mouse)
    int square=0;       //flag to tell if it was a square in the play
    int helping_board[MAX_BOARD_POS][MAX_BOARD_POS]={{0}};      //board to process square plays
    int undo_board[MAX_BOARD_POS][MAX_BOARD_POS]={{0}};     //board to do undo
    int n_times=0,instant_defeat=0;     //number of times the n key is pressed,and if it is pressed during a game,instant_defeat keeps track of how many losts were done that way
    int shuffle_test_result=0;      //flag to tell if it is possible to make a play with the current state of the board or if a shuffle is needed
    int undo_color_points=0,undo_color_points_save=0;       //saves points to later be used in the undo
    int victory_result=0,defeat_result=0;       //flags to tell if the user won or lost the game
    int pointsi[VECTOR_MAX]={0};        //stores the points of the game
    int victory_count=0,defeat_count=0,true_defeat=0;       //counts how many times the player won and lost
    int victory_counter=0,defeat_counter=0;     //to keep from counting multiple victories or defeats on a single game
    int color_counter=0;        //for square plays only
    int games_played=0;     //amount of games the player played
    int moves_done[VECTOR_MAX]={0};     //stores the moves the player took in each win
    char play_final[VECTOR_MAX]={0};        //Stores the final state of each game.Win or defeat
    int play_counter=0,play_write=0;        //counter to store wins and their moves;counter of games to write on file stats.txt
    int start_game=0;       //flag to tell if the game is on

    generate_game(&nrows,&ncols,&ncolors,&moves,points,username,board);        //asks for the parameters to inicialize the game

    movesi=moves;       //saves the moves of the game start
    for(l=0;l<ncolors;l++)
        pointsi[l]=points[l];       //saves the points of each color at the game start

    board_pos_x = ncols;
    board_pos_y = nrows;


    // initialize graphics
    InitEverything(width, height, &serif, &sans,&sansbig, imgs, &window, &renderer);

    srand(time(NULL));      //seed to generate random values

    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                // quit the game window
                quit = 1;
            }
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_n:
                    {
                       generate_board(nrows,ncols,ncolors,board);       //generates the board for the user
                       n_times++;       //amount of times the n key has been pressed since the start
                       if(n_times==1)       //first time pressed it should not count as a victory or defeat
                       {
                            victory_result=0;
                            start_game=1;
                       }
                       else     //after the n key as been pressed the first time there are multiple options
                       {
                            if(victory_result==1)       //if the player had won the game and starts a new one,resets game values for victory
                            {
                                victory_counter=0;      //resets both flags
                                victory_result=0;
                            }
                            else if(defeat_result==1)       //if the player had lost and starts a new one,resets values for defeat
                            {
                                defeat_result=0;        //resets both flags
                                defeat_counter=0;
                            }
                            else        //if he was mid-game,we instantly count it as a defeat
                            {
                                defeat_counter=0;       //resets flag
                                instant_defeat++;       //adds a defeat to the play record
                            }

                            moves=movesi;       //resets the values of the moves and points each time a new game starts
                            for(t=0;t<ncolors;t++)      //resets the points of all colors
                                points[t]=pointsi[t];
                       }

                       break;
                    }
                    case SDLK_q:
                    {
                        for(h=0;h<instant_defeat;h++)
                            play_final[play_counter+h]='D';     //adds the defeats of leaving mid-game to the record

                        play_write=play_counter+h;      //counter to keep track of how many games where done
                        true_defeat=defeat_count+instant_defeat;        //amount of times the player trully lost
                        games_played=true_defeat+victory_count;     //number of games done

                        file_create(username,true_defeat,games_played,victory_count,play_final,play_write,moves_done);      //creates a file storing all the statistics of the player

                        return EXIT_SUCCESS;        //exits the game
                        break;
                    }
                    case SDLK_u:
                    {
                        undo_board_creation(nrows,ncols,undo_board,board);       //applies the undo board on the game board
                        moves++;        //adds the move lost in the previous play
                        undo_color_points=undo_color_points_save;       //undos the points of the color removed
                        points[color_delete-1]=undo_color_points;
                        break;
                    }
                    default:
                        break;
                }
            }
            else if ( event.type == SDL_MOUSEBUTTONDOWN )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                printf("Button down: %d %d\n", pt_x, pt_y);

                pt_x_i=pt_x;        //saves the point pressed by the player
                pt_y_i=pt_y;

                outside_board_error=0;      //resets a error parameter

                undo_board_save(nrows,ncols,undo_board,board);      //saves the board before it is changed by a play

                color_delete=board[pt_x][pt_y];     //the color being removed is the one in the "house" chosen by the player,and it gets marked as the one of a valid play
                undo_color_points_save=points[color_delete-1];      //saves the points of the color before the play

                if(pt_x_i==-1 || pt_y_i==-1)        //doesn allow a invalid outside the board
                    outside_board_error=1;

                mousebuttondown=1;      //begin of the play;keeps track if the mouse button is down,terefore saving the play
                countcompare=0;     //resets the play comparison value
                path_track=0;       //resetsthe players path
                square=0;       //resets the flag of a square move
            }
            else if ( event.type == SDL_MOUSEBUTTONUP )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                printf("Button up: %d %d\n", pt_x, pt_y);

                for(int y=0;y<=path_track-1;y++)
                    printf("%d  %d \n",pt_x_r[y],pt_y_r[y]);

                mousebuttondown=0;      //untags the mouse button,meaning the play itś finished and is ready for a new one to be made

                valid_play=play_test(countcompare,pt_x_r,pt_y_r,compare,path_track,outside_board_error);        //tests if a play is valid

                if(valid_play==1 && victory_result==0 && defeat_result==0 && start_game==1)      //if  a play is valid and game is still going on
                {
                    moves--;        //uses up a move

                    square=square_detetion(pt_x_r,pt_y_r,path_track,nrows,ncols,helping_board);     //detects if the play was a square

                    if(square==0)       //if it isn a play with a square
                    {
                        removedots(path_track,nrows,ncols,board,pt_x_r,pt_y_r);     //marks points to remove
                        dropdots(board,ncolors,nrows,ncols,helping_board);      //drops points and prepares new ones to be filled
                        generatedots(board,nrows,ncols,ncolors);        //generates the new points

                        points[color_delete-1]=(points[color_delete-1]-(path_track));       //if the play is valid,removes the amount of "houses" moved by the player to the points of the color selected

                        if(points[color_delete-1]<0)        //doesn allow the value of points of any color to go bellow 0
                            points[color_delete-1]=0;

                    }
                    else        //if it is a play with a square
                    {
                        remove_colordots(nrows,ncols,board,color_delete,&color_counter);        //marks all points with the same color as the square
                        explode_dots(helping_board,nrows,ncols);        //marks points inside the square
                        dropdots(board,ncolors,nrows,ncols,helping_board);      //drops dots once again and prepares new ones
                        generatedots_square(board,nrows,ncols,ncolors,color_delete);        //generate new points that cannot be equal to the points of the square

                        points[color_delete-1]=(points[color_delete-1]-(color_counter));       //if the play is valid,removes the amount dots with the same color than the selected on the board

                        if(points[color_delete-1]<0)        //doesn allow the value of points of any color to go bellow 0
                            points[color_delete-1]=0;

                    }

                }
                else;
                //doesn make a play

            }
            else if ( event.type == SDL_MOUSEMOTION )
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
               // printf("Moving Mouse: %d %d\n", pt_x, pt_y);

                shuffle_test_result= shuffle_test(nrows,ncols,board);       //sees if it is possible for the user to make a move with the current state of the board

                if(mousebuttondown==1)      //if a play is being made
                {
                   if(board[pt_x][pt_y]==color_delete && pt_x!=-1)      //if the user passes through a "house"with the same color as the one of the play and doesn go outside the board
                    {
                        compare[countcompare]=1;        //marks the comparisor with one,if he is on a house with the same color as the one of the play

                        if(countcompare==0)     //if it is the first point being marked,aka,the begginning of the play
                        {
                            pt_x_r[0]=pt_x;     //saves that inicial point
                            pt_y_r[0]=pt_y;
                            path_track++;       //prepares to save the next valid point of the user
                            countcompare=1;     //prepares the comparisor to the next point of comparison
                        }
                        else if(pt_x_r[path_track-1]==pt_x && pt_y_r[path_track-1]==pt_y)       //if he is still in the same "house" of the previous point saved,keep marking as valid
                        {
                            countcompare++;
                        }
                        else        //if he passes through a new house
                        {
                            pt_x_r[path_track]=pt_x;        //save the new point of the path of the player
                            pt_y_r[path_track]=pt_y;
                            path_track++;       //prepares to save a new point
                            countcompare++;     //marks as valid play
                        }

                    }

                    else if(pt_x==-1)       //if he moves through the blank spots of the "houses" still mark it as valid
                    {
                        compare[countcompare]=1;
                        countcompare++;
                    }
                    else        //if he passes through a not valid point,mark the comparisor as 0.This means that the play will become unvalid.
                    {
                        compare[countcompare]=0;
                        countcompare++;
                    }

                    if(path_track>=2)       //stops the user of doing diagonal plays by checking the last 2 spots moved through and see if the were in a diagonal.Marks the play unvalid.Also stop playing outside the board
                    {
                        for(int kl=0;kl<=path_track;kl++)   //checks the hole play to see if there was ever a diagonal move
                        {
                            if( (pt_x_r[path_track-1]==(pt_x_r[path_track-2]+1) && pt_y_r[path_track-1]==(pt_y_r[path_track-2]+1))||(pt_x_r[path_track-1]==(pt_x_r[path_track-2]-1) && pt_y_r[path_track-1]==(pt_y_r[path_track-2]-1)))
                            {
                                compare[countcompare]=0;
                                countcompare++;
                                break;
                            }
                            else if((pt_x_r[path_track-1]==pt_x_r[path_track-2]+1 && pt_y_r[path_track-1]==pt_y_r[path_track-2]-1)||(pt_x_r[path_track-1]==pt_x_r[path_track-2]-1 && pt_y_r[path_track-1]==pt_y_r[path_track-2]+1))
                            {
                                compare[countcompare]=0;
                                countcompare++;
                                break;
                            }
                            else if(pt_x_r[path_track-1]>=pt_x_r[path_track-2]+2||pt_x_r[path_track-1]<=pt_x_r[path_track-2]-2||pt_y_r[path_track-1]>=pt_y_r[path_track-2]+2||pt_y_r[path_track-1]<=pt_y_r[path_track-2]-2)     //stops the player from going around the board and jump into a different spot
                            {
                                compare[countcompare]=0;
                                countcompare++;
                                break;
                            }
                            else
                                break;
                        }
                    }
                }
            }
        }
        // render game table
        square_size_px = RenderTable( board_pos_x, board_pos_y, board_size_px, ncolors,victory_result,defeat_result,shuffle_test_result,mousebuttondown,path_track,pt_x_r,pt_y_r, serif, imgs, renderer);
        // render board
        RenderPoints(board, board_pos_x, board_pos_y, board_size_px,ncolors, square_size_px, renderer);

        RenderStats(renderer,sans,points,ncolors,moves);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);

        victory_result=victory_test(ncolors,points,moves);      //checks if the game is finished and if it was a victory

        if(victory_result==1)       //if the player won
        {
            victory_counter++;      //stops the program of counting multiple times victory

            if(victory_counter==1)
            {
                moves_done[play_counter]=movesi-moves;      //stores the moves it took to win the game
                victory_count++;        //counts a victoru
                play_final[play_counter]='V';       //sabes victory in the vector for the file
                play_counter++;     //increments to save to a new position
            }
        }

        if(moves==0 && victory_result==0)       //if the player lsot
        {
            defeat_result=1;        //stops the game by saying it was a lost and doesn allow more moves

            defeat_counter++;       //stops the program from counting multiple times lost
            if(defeat_counter==1)
            {
                moves_done[play_counter]=0;     //saves a lost to the moves vectr
                defeat_count++;     //counts a lost
                play_final[play_counter]='D';       //stores a lost in the vector to write in the file
                play_counter++;     //increments to save a new position
            }
        }

        if(shuffle_test_result==1)      //if the computer detects there is no possible play to be done
        {
            SDL_Delay(delay);

            shuffle_board(nrows,ncols,board);       //shuffles the board to make it possible to play again
        }

        // add a delay
        SDL_Delay( delay );
    }

    // free memory allocated for images and textures and closes everything including fonts
    TTF_CloseFont(serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

void file_create(char _username [VECTOR_MAX],int _true_defeat,int _games_played,int _victory_count,char _play_final [VECTOR_MAX],int _play_write,int _moves_done [VECTOR_MAX])
{
    int i=0;        //counter to for function

    FILE *stats;


    stats=fopen("stats.txt","w");       //creates a file
    fprintf(stats,"username:%s \ngames played:%d\nVictory=%d     Defeat=%d\n",_username,_games_played,_victory_count,_true_defeat);     //writes over it

    for(i=0;i<_play_write;i++)
    {
        fprintf(stats,"%d %c\n",_moves_done[i],_play_final[i]);     //writes victories and defeats over it
    }
}

int victory_test(int _ncolors,int _points[VECTOR_MAX],int _moves)
{
    int n=0,victory=0,game_on=0;

    for(n=0;n<_ncolors;n++)     //test if every color has reach 0 points
    {
        if(_points[n]!=0)
        {
            game_on=1;
        }
    }

    if(game_on==0 && _moves>=0)     //if it has and there are still moves left,then the player won
        victory=1;

    return victory;
}

void shuffle_board(int _nrows,int _ncols,int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
  int i=0,j=0,k=0,h=0;
  int changer=0;        //something to store the values while changing

  for(i=0;i<_nrows;i++)     //sees every row
  {
    for(j=0;j<_ncols;j++)       //sees every column
    {
        k= rand()%_nrows;       //picks a random point of the board
        h= rand()%_ncols;       //picks a random point of the board
        changer=_board[j][i];       //stoes the value of the point we are switching
        _board[j][i]=_board[h][k];      //switches the point with another one random in the board
        _board[h][k]=changer;       //places the value of the point previously store in the place where was the point he switched to
    }
  }
}

int shuffle_test(int _nrows,int _ncols,int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0;
    int shuffle_counter=0,shuffle_test=0;       //counts how many dots need shuffle;flag to say if shuffle is needed or not

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            if(i==0 && j==0)        //checks point 0,0
            {
                if((_board[j][i+1]!=_board[j][i])&&(_board[j+1][i]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(i==0 && j==(_ncols-1))      //checks point 0,last column
            {
                if((_board[j-1][i]!=_board[j][i])&&(_board[j][i+1]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(i==0 &&(j!=0 && j!=(_ncols-1)))     //checks first row except the 2 corners
            {
                if((_board[j][i+1]!=_board[j][i]) && (_board[j-1][i]!=_board[j][i]) && (_board[j+1][i]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(i==(_nrows-1)&& j==0)       //checks point last row,0
            {
                if((_board[j][i-1]!=_board[j][i]) && (_board[j+1][i]!=_board[j][i]))        //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(i==(_nrows-1)&& j==(_ncols-1))      //checks point last row,last column
            {
                if((_board[j-1][i]!=_board[j][i]) && (_board[j][i-1]!=_board[j][i]))        //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(i==(_nrows-1) && (j!=0 && j!=(_ncols-1)))       //checks last row except corners
            {
                if((_board[j][i-1]!=_board[j][i]) && (_board[j-1][i]!=_board[j][i]) && (_board[j+1][i]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(j==0 && (i!=0 && i!=(_nrows-1)))        //checks the first column except the corners
            {
                if((_board[j+1][i]!=_board[j][i]) && (_board[j][i-1]!=_board[j][i]) && (_board[j][i+1]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else if(j==(_ncols-1)&&(i!=0 &&(i!=_nrows-1)))      //checks the last column,except the corners
            {
                if((_board[j-1][i]!=_board[j][i]) && (_board[j][i-1]!=_board[j][i]) && (_board[j][i+1]!=_board[j][i]))      //if its not possibly to do a play,count
                    shuffle_counter++;
            }
            else
            {
                if((_board[j][i+1]!=_board[j][i])||(_board[j][i-1]!=_board[j][i])||(_board[j+1][i]!=_board[j][i])||(_board[j-1][i]!=_board[j][i]))      //checks the "inside of the board.if its not possibly to do a play,count
                {
                    shuffle_counter++;
                }
            }
        }
    }

    if(shuffle_counter == (_nrows*_ncols))      //if the amount of points that is not possibly to do any play equals to the amount of points it exist,then it most do a shuffle
    {
        shuffle_test=1;
    }

    return shuffle_test;        //returns the flag of shuffle.If it is 1,it does a shuffle
}

void undo_board_creation(int _nrows,int _ncols,int _undo_board[MAX_BOARD_POS][MAX_BOARD_POS],int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0;

    for(i=0;i<_nrows;i++)       //check all the rows
    {
        for(j=0;j<_ncols;j++)       //check all the columns
        {
            _board[j][i]=_undo_board[j][i];     //places the old board saved before the play on top of the newest one
        }
    }
}

void undo_board_save(int _nrows,int _ncols,int _undo_board[MAX_BOARD_POS][MAX_BOARD_POS],int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0;

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            _undo_board[j][i]=_board[j][i];     //saves the current state of the board before any play is done
        }
    }
}

void explode_dots(int _helping_board[MAX_BOARD_POS][MAX_BOARD_POS],int _nrows,int _ncols)
{
    int i=0,j=0,k=0,l=0,b=0,v=0;        //counters to for functions
    int times=1,stop=0,counter=0;      //
    int last_line[VECTOR_MAX]={0};      //vector that stores all the lines of the figure of the player

    for(b=_nrows-1;b>=0;b--)        //checks all rows from the bottom all the way to the top
    {
        for(v=_ncols-1;v>=0;v--)        //checks all columns
        {
            if((_helping_board[v][b]==1 || _helping_board[v][b]==2)&&(_helping_board[v-1][b]==1||_helping_board[v-1][b]==2||_helping_board[v+1][b]==1||_helping_board[v+1][b]==2))      //if he finds a place where the player passed by,store the place
            {
                last_line[counter]=b;       //saves the row only
                counter++;
            }
        }
    }

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all the columns
        {
            stop=0;     //reset the flag
            times=1;        //reset the flag

            if(last_line[0]!=i)     //applies this way if not the last line of the figure
            {
                if(_helping_board[j][i]==1 || _helping_board[j][i]==2)      //if it finds a point of the path of the player
                {
                    if(_helping_board[j][i+1]==0 &&(_helping_board[j+1][i]==1||_helping_board[j+1][i]==2))      //check the point beneath and next to it.If it is a point inside the figure
                    {
                        do
                        {
                        k=j;        //start at that column
                        l=i+times;      //start in the point beneath the one the player passed through

                        if(_helping_board[k][l]==0)     //if it hasnt been marked,mark it to remove
                        {
                            _helping_board[k][l]=-1;        //marks it
                            times++;        //counts ao many times it was marked
                        }
                        else        //marks points all the way until it reaches a point that the player crossed again
                            stop=1;     //stops at that point
                        }while(stop!=1);        //goes on until it doesnt stop
                    }
                    else;
                }
            }
            else;       //after the last line no point is needed to be marked
        }
    }

}

void generatedots_square(int _board[MAX_BOARD_POS][MAX_BOARD_POS],int _nrows,int _ncols,int _ncolors,int _color_delete)
{
    int i=0,j=0;        //counters to for functions
    int colour=0;       //color that it generates

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            if(_board[j][i]==-2)        //if the points are marked to be generated
            {
                colour=(rand()%_ncolors)+1;     //generate a color for the points
                while(colour==_color_delete)        //if the color is equal to the one of the square
                {
                    colour=(rand()%_ncolors)+1;     //continue generating points until it isnt
                }
                _board[j][i]=colour;        //save the color to the point it was meant to be generated
            }
        }
    }
}

void remove_colordots(int _nrows,int _ncols,int _board[MAX_BOARD_POS][MAX_BOARD_POS],int _color_delete,int *_color_counter)
{
    int i=0,j=0,color_counter_function=0;       //counters to for functions;counts how many colors has been marked

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            if(_board[j][i]==_color_delete)     //if the "house" color equals to the one of the square
            {
                _board[j][i]=-1;        //mark the house to be removed
                color_counter_function++;       //count how many points have been marked
            }
        }
    }
    *_color_counter=color_counter_function;     //send it to main function to update the graphics
}

int square_detetion(int _pt_x_r[MAX_BOARD_POS],int _pt_y_r[MAX_BOARD_POS],int _path_track,int _nrows,int _ncols,int _helping_board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0,y=0,t=0;        //counters to for functions
    int square_play=0,path_track_function=0;        //flag to tell if it is a square or not;

    for(y=0;y<_nrows;y++)       //checks all rows
    {
        for(t=0;t<_ncols;t++)       //checks all columns
        {
            _helping_board[t][y]=0;     //places the helping board=0 in all positions
        }
    }

    for(path_track_function=0;path_track_function<=_path_track-1;path_track_function++)     //starts marking down the path the player made to later analize it
    {
        for(i=0;i<_nrows;i++)       //checks all rows
        {
            for(j=0;j<_ncols;j++)       //checks all columns
            {
                if(_pt_x_r[path_track_function]==j && _pt_y_r[path_track_function]==i)      //if it is a house the player as pased through
                {
                    if(_helping_board[j][i]==1)     //if it was already marked as crossed,then specially mark it,signalizing a square figure
                    {
                        _helping_board[j][i]=2;     //special marker
                        square_play=1;      //already acknowledges it was a play with a square
                    }
                    else        //if it wasnt mark yet
                    {
                        _helping_board[j][i]=1;     //mark it
                    }
                }
            }
        }
    }

    return square_play;     //returns saying if it was a square or not
}

void generatedots(int _board[MAX_BOARD_POS][MAX_BOARD_POS],int _nrows,int _ncols,int _ncolors)
{
    int i=0,j=0;        //counters to for function

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            if(_board[j][i]==-2)        //if it is a point marked to generate new color
                _board[j][i]=(rand()%_ncolors)+1;       //generate a color for that point
        }
    }
}

void dropdots(int _board[MAX_BOARD_POS][MAX_BOARD_POS],int _ncolors,int _nrows,int _ncols,int _helping_board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0,t=0,m=0,q=0,y=0,u=0;        //counters to for functions
    int amount_deletepoint=0;

    for(y=0;y<_nrows;y++)       //checks all rows
    {
        for(u=0;u<_ncols;u++)       //checks all columns
        {
            if(_helping_board[u][y]==-1)        //passes the points marked on the helping board to be removed to the board to be all processed together
            {
                _board[u][y]=-1;
            }
        }
    }

    do
    {
        for(i=0;i<_nrows;i++)       //check all rows
        {
            for(j=0;j<_ncols;j++)       //check all columns
            {
                if(_board[j][i]==-1)        //if that "house" was marked
                {
                    for(t=_nrows-1;t>=1;t--)        //starts from the last row all the way to the top one
                    {
                        for(m=0;m<_ncols;m++)       //cheks all columns
                        {
                            if(m==j && t<=i)
                                _board[m][t]=_board[m][t-1];        //pulls all the points downwards
                        }
                    }
                    for(q=0;q<=0;q++)   //when it reaches the first row,marks all the points on that column that was pulled down to generate a new point on that spot
                    {
                        _board[j][0]=-2;        //point marked to be generated
                    }
                }
            }
        }

        amount_deletepoint=0;

        for(i=0;i<_nrows;i++)       //cheks all rows
        {
            for(j=0;j<_ncols;j++)       //checks all columns
            {
                if(_board[j][i]==-1)        //checks if there are still points that havent been deleted
                    amount_deletepoint++;       //if it hisbigger than 0,it means there still are
            }
        }
    }while(amount_deletepoint!=0);      //do all the process until it has been completed,meaning,there arent any more points to remove

}

void removedots(int _path_track,int _nrows,int _ncols,int board[MAX_BOARD_POS][MAX_BOARD_POS],int _pt_x_r[VECTOR_MAX],int _pt_y_r[VECTOR_MAX])
{
    int remover=0,v=0,k=0;      //tracks all the points needing to be removed;counters to for functions

    for(remover=0;remover<=_path_track-1;remover++)     //checks all the points stored that the player has marked
    {
        for(v=0;v<_nrows;v++)       //checks all rows
        {
            for(k=0;k<_ncols;k++)       //chekcs all columns
            {
                if(_pt_x_r[remover]==k && _pt_y_r[remover]==v)      //mark the points that were chosen by the player
                {
                    board[k][v]=-1;     //marks them for removal
                }
            }
        }
    }
}

int play_test(int _countcompare,int pt_x_r[VECTOR_MAX],int pt_y_r[VECTOR_MAX],int compare[VECTOR_MAX],int _path_track,int _outside_board_error)
{
    int test_value=0,compare_f=0,i=0;       //flag to tell if the play is valid;flag that warns if there has been any point not valid during the move;counter to for function
    int onepoint=0,outsideboard_alert=0;        //flag that tells if there was any movement;flag that warns if the chosen point to begin the play is valid or not

    if(_outside_board_error==1)     //if the play started in an invalid point
        outsideboard_alert++;       //mark as not valid

    if(_path_track==1 || _countcompare==0)      //if the player hasnt moved from the "house" he chose
        onepoint=1;     //mark hs "the player didnt move"

    for(i=0;i<_countcompare;i++)        //check the vector that stores if all the places the player has passed are equal to the color he chose
    {
        if(compare[i]==0)       //if any of the points isnt the same as the one chosen by the player(in color),them mark the flag
            compare_f++;
    }

    if(onepoint==0 && compare_f==0 && outsideboard_alert==0)        //if none of the warning flags is up
    {
        test_value=1;       //play is valid
    }
    else
    {
        test_value=0;       //else it isnt
    }
    return test_value;      //return the value of the test to the main
}

void generate_game(int *_nrows,int *_ncols,int *_ncolors,int *_moves,int _points[VECTOR_MAX],char _username[VECTOR_MAX],int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int sumpoints_true=0,sumpoints=0,i=0;     //sees if the sum of all the points is valid;sums the points of each color;counters to for functions
    int pointcolor=0;      //sees the point of each color

    printf("Insert the size of the board in this order:\nFirst rows,then columns:\n");
    scanf("%d %d",&*_nrows,&*_ncols);
    while(*_nrows<MIN_BOARD_POS || *_nrows>MAX_BOARD_POS || *_ncols<MIN_BOARD_POS || *_ncols>MAX_BOARD_POS)     //sees if the values are above 4 and below 16
    {
        printf("Not allowed.Please insert again:\n");
        scanf("%d %d",&*_nrows,&*_ncols);
    }
    printf("And the number of colours,of course:\n");
    scanf("%d",&*_ncolors);
    while(*_ncolors>MAX_COLORS || *_ncolors<0)      //sees if the number os colors is positive and below 6
    {
        printf("Not allowed.Please insert again:\n");
        scanf("%d",&*_ncolors);
    }
    do
    {
        sumpoints_true=0;
        sumpoints=0;
        for(pointcolor=0;pointcolor<*_ncolors;pointcolor++)
        {
            printf("Right.How many points should the %d color have?\n",pointcolor+1);
            scanf("%d",&_points[pointcolor]);       //save the points of each color
        }
        for(i=0;i<*_ncolors;i++)
        {
            sumpoints+=_points[i];      //sum the points of all colors
        }
        if(sumpoints>MAX_POINTS)        //if the sum is greater than 99
        {
            printf("Sorry you went overboard on points.Do it again please:\n");         //ask for it again
            sumpoints_true++;
        }
    }while(sumpoints_true==1);      //do while the test is valid,meaning,while the sum is greater than 99
    printf("Finally,how many moves should you have?\n");
    scanf("%d",&*_moves);
    while(*_moves>MAX_MOVES || *_moves<0)       //see if the number of moves is positive and greather then 99
    {
        printf("Not allowed.Please insert again:\n");       //if it isnt,ask for it again
        scanf("%d",&*_moves);
    }
    printf("Great.We will start generating the board in no time.By the way,your name is?\n");
    scanf("%s",&*_username);        //ask for the username

}

void generate_board(int _nrows,int _ncols,int _ncolors,int _board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int i=0,j=0;        //counters to for functions
    int Colour=0;       //color that is chosen for each "house"

    for(i=0;i<_nrows;i++)       //checks all rows
    {
        for(j=0;j<_ncols;j++)       //checks all columns
        {
            Colour=(rand()%(_ncolors))+1;       //generates a color
            _board[j][i]=Colour;        //places it on the "house" of the board
        }
    }
}

/**
 * ProcessMouseEvent: gets the square pos based on the click positions !
 * \param _mouse_pos_x position of the click on pixel coordinates
 * \param _mouse_pos_y position of the click on pixel coordinates
 * \param _board_size_px size of the board !
 * \param _square_size_px size of each square
 * \param _pt_x square nr
 * \param _pt_y square nr
 */
void ProcessMouseEvent(int _mouse_pos_x, int _mouse_pos_y, int _board_size_px[], int _square_size_px,
        int *_pt_x, int *_pt_y )
{
    // corner of the board
    int x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    int y_corner = (TABLE_SIZE - _board_size_px[1] - 15);
    int sqr_x=0,sqr_y=0;
    int dist=0;
    int circleX,circleY,circleR;

    // verify if valid cordinates
    if (_mouse_pos_x < x_corner || _mouse_pos_y < y_corner || _mouse_pos_x > (x_corner + _board_size_px[0])
        || _mouse_pos_y > (y_corner + _board_size_px[1]) )
    {
        *_pt_x = -1;
        *_pt_y = -1;
        return;
    }

    // computes the square where the mouse position is
    sqr_x=(_mouse_pos_x - x_corner) / (_square_size_px + SQUARE_SEPARATOR);
    sqr_y=(_mouse_pos_y - y_corner) / (_square_size_px + SQUARE_SEPARATOR);

    circleX= x_corner + (sqr_x +1)*SQUARE_SEPARATOR + sqr_x*(_square_size_px)+(_square_size_px>>1);
    circleY= y_corner + (sqr_y +1)*SQUARE_SEPARATOR + sqr_y*(_square_size_px)+(_square_size_px>>1);
    circleR= (int)(_square_size_px*0.4f);

    dist=((int)sqrt(SQR(_mouse_pos_x-circleX)+SQR(_mouse_pos_y-circleY)));

    if (dist<circleR)
    {
        *_pt_x=sqr_x;
        *_pt_y=sqr_y;
    }
    else
    {
        *_pt_x=-1;
        *_pt_y=-1;
    }

}

/**
 * RenderPoints: renders the board
 * \param _board 2D array with integers representing board colors
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _square_size_px size of each square
 * \param _board_size_px size of the board
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPoints(int _board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y,
        int _board_size_px[],int _ncolors, int _square_size_px, SDL_Renderer *_renderer )
{
    int clr, x_corner, y_corner, circleX, circleY, circleR;
    int n=0;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // renders the squares where the dots will appear
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                // define the size and copy the image to display
                circleX = x_corner + (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                circleY = y_corner + (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                circleR = (int)(_square_size_px*0.4f);
                // draw a circle
                clr = _board[i][j];
                filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);
        }
    }

    for(n=0;n<_ncolors;n++)
    {
        // define the size and copy the image to display
        circleX = 215 + (125*n);        //renders circles to place on the display to the player
        circleY = 100;
        circleR = (int)(_square_size_px*0.125f);
        // draw a circle
        clr = n+1;
        filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);
    }
}

/**
 * filledCircleRGBA: renders a filled circle
 * \param _circleX x pos
 * \param _circleY y pos
 * \param _circleR radius
 * \param _r red
 * \param _g gree
 * \param _b blue
 */
void filledCircleRGBA(SDL_Renderer * _renderer, int _circleX, int _circleY, int _circleR, int _r, int _g, int _b)
{
    int off_x = 0;
    int off_y = 0;
    float degree = 0.0;
    float step = M_PI / (_circleR*8);

    SDL_SetRenderDrawColor(_renderer, _r, _g, _b, 255);

    while (_circleR > 0)
    {
        for (degree = 0.0; degree < M_PI/2; degree+=step)
        {
            off_x = (int)(_circleR * cos(degree));
            off_y = (int)(_circleR * sin(degree));
            SDL_RenderDrawPoint(_renderer, _circleX+off_x, _circleY+off_y);
            SDL_RenderDrawPoint(_renderer, _circleX-off_y, _circleY+off_x);
            SDL_RenderDrawPoint(_renderer, _circleX-off_x, _circleY-off_y);
            SDL_RenderDrawPoint(_renderer, _circleX+off_y, _circleY-off_x);
        }
        _circleR--;
    }
}

/*
 * Shows some information about the game:
 * - Goals of the game
 * - Number of moves
 * \param _renderer renderer to handle all rendering in a window
 * \param _font font to display the scores
 * \param _goals goals of the game
 * \param _ncolors number of colors
 * \param _moves number of moves remaining
 */
void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font, int _points[VECTOR_MAX], int _ncolors, int _moves)
{
    int n=0;        //counter to for function

    char moves[VECTOR_MAX]={0};         //string that will show the moves
    char point_color_1[VECTOR_MAX]={0};     //string that will show the points of each color
    char point_color_2[VECTOR_MAX]={0};
    char point_color_3[VECTOR_MAX]={0};
    char point_color_4[VECTOR_MAX]={0};
    char point_color_5[VECTOR_MAX]={0};

    SDL_Color light={205,193,101};
    SDL_Color dark={120,110,102};


    sprintf(moves,"moves=%d",_moves); //string with the moves on it

    SDL_Rect text_area;

    text_area.x=30;
    text_area.y=90;

    RenderText(text_area.x,text_area.y,moves,_font,&dark,_renderer);        //render moves on the screen to the player

    for(n=0;n<_ncolors;n++)
    {
        switch (n+1)        //for each color,render the amount of points of each one while the game is running
        {
            case 1:
            {
                sprintf(point_color_1,"%d",_points[n]);

                SDL_Rect text_area;

                text_area.x=100+150;
                text_area.y=90;

                RenderText(text_area.x,text_area.y,point_color_1,_font,&dark,_renderer);

                break;
            }
            case 2:
            {
                sprintf(point_color_2,"%d",_points[n]);

                SDL_Rect text_area;

                text_area.x=100+270;

                text_area.y=90;

                RenderText(text_area.x,text_area.y,point_color_2,_font,&dark,_renderer);

                break;
            }
            case 3:
            {
                sprintf(point_color_3,"%d",_points[n]);

                SDL_Rect text_area;

                text_area.x=100+390;
                text_area.y=90;

                RenderText(text_area.x,text_area.y,point_color_3,_font,&dark,_renderer);

                break;
            }
            case 4:
            {
                sprintf(point_color_4,"%d",_points[n]);

                SDL_Rect text_area;

                text_area.x=100+515;
                text_area.y=90;

                RenderText(text_area.x,text_area.y,point_color_4,_font,&dark,_renderer);

                break;
            }
            case 5:
            {
                sprintf(point_color_5,"%d",_points[n]);

                SDL_Rect text_area;

                text_area.x=100+640;
                text_area.y=90;

                RenderText(text_area.x,text_area.y,point_color_5,_font,&dark,_renderer);

                break;
            }
        }


    }

}

/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _board_size_px size of the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderTable( int _board_pos_x, int _board_pos_y, int _board_size_px[],int _ncolors,int _victory_result,int _defeat_result,int _shuffle_result,
                int _mouse_button_down,int _path_track,int _pt_x_r[VECTOR_MAX],int _pt_y_r[VECTOR_MAX],
                TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer )
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 205, 193, 181 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size, square_size_px, max_pos;
    int n=0,h=0;
    char victory_sentence[VECTOR_MAX]="Victory!";       //sentences to be displayed after an event occurs
    char defeat_sentence[VECTOR_MAX]="Defeat!";
    char shuffle_sentence[VECTOR_MAX]="Shuffle!";

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    max_pos = MAX(_board_pos_x, _board_pos_y);
    board_size = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size - (max_pos+1)*SQUARE_SEPARATOR) / max_pos;
    _board_size_px[0] = _board_pos_x*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;
    _board_size_px[1] = _board_pos_y*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - _board_size_px[0]) >> 1;
    board.y = (TABLE_SIZE - _board_size_px[1] - 15);
    board.w = _board_size_px[0];
    board.h = _board_size_px[1];
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    for(n=0;n<(_ncolors+1);n++)     //create rectangles where the moves and points of each color can be written in.Creates as many rectangles as the amount of colors plus one(for the moves)
    {
        if(n!=0)
        {
            board_square.x = 75+(125*n);
            board_square.y = 75;
            board_square.w = 100;
            board_square.h = 60;
            SDL_RenderFillRect(_renderer, &board_square);
        }
        else
        {
            board_square.x = 25;
            board_square.y = 75;
            board_square.w = 100;
            board_square.h = 60;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    if(_victory_result==1||_defeat_result==1||_shuffle_result==1)       //whenever a message needs to be displayed to the player
    {
        if(_victory_result==1)
        {
            board_square.x = 350;
            board_square.y = 150;
            board_square.w = 100;
            board_square.h = 50;
            SDL_RenderFillRect(_renderer, &board_square);

            RenderText(370,160,victory_sentence,_font,&dark,_renderer);
        }
        else if(_defeat_result==1)
        {
            board_square.x = 350;
            board_square.y = 150;
            board_square.w = 100;
            board_square.h = 50;
            SDL_RenderFillRect(_renderer, &board_square);

            RenderText(370,160,defeat_sentence,_font,&dark,_renderer);
        }
        else if(_shuffle_result==1)
        {
            board_square.x = 350;
            board_square.y = 150;
            board_square.w = 100;
            board_square.h = 50;
            SDL_RenderFillRect(_renderer, &board_square);

            RenderText(370,160,shuffle_sentence,_font,&dark,_renderer);

            SDL_Delay(500);
        }
        else;
    }

    if(_mouse_button_down==1)
    {
        SDL_SetRenderDrawColor(_renderer,120,110,102,0);        //renders the path change so the player can keep track of his movement

        for(h=0;h<_path_track;h++)
        {
            board_square.x = board.x + (_pt_x_r[h]+1)*SQUARE_SEPARATOR + _pt_x_r[h]*square_size_px;
            board_square.y = board.y + (_pt_y_r[h]+1)*SQUARE_SEPARATOR + _pt_y_r[h]*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // return for later use
    return square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, TTF_Font **_font2, TTF_Font **_font3, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    *_font2 = TTF_OpenFont("OpenSans.ttf", 20);
    if(!*_font2)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    *_font3 = TTF_OpenFont("OpenSans.ttf", 60);
    if(!*_font3)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "IST DOTS", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}
