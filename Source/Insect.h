#include "mpc555.h"
#include "music.h"

#define NUM_POLL	4
#define NUM_POLL_x2 8
#define D_SAFETY 400
#define Q_SIZE 5
#define AVG_NUM 5
#define TURN_SUB_LEN 45
#define EVADE_TIME 30
#define TURN_AMOUNT 34
#define MOVE_MIN 300000
#define NUM_SONGS 5
#define SONG_LEN 10

extern void init_pwm_wheels();
extern void init_pwm();
extern void init_timer();
extern void TLBInit();
extern void init_QADC();
extern void turnLeft(int);
extern void turnRight(int);
extern void forward();
extern void stop();
extern void turning();
extern void init_cam();
extern void stationary();
extern void moving();
extern void init_data_set();
extern void confirmation();

extern char turnTime;
extern char position;
extern char twitching;

extern char move;

extern unsigned int dist0[Q_SIZE];
extern unsigned int dist1[Q_SIZE];

extern int delta0[AVG_NUM-1];
extern int delta1[AVG_NUM-1];

extern unsigned int dist0_avg[AVG_NUM];
extern unsigned int dist1_avg[AVG_NUM];

extern int TurnLen;
extern unsigned volatile short pos;
extern char	whichSensor;

extern char song_num;