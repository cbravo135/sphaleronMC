#define ELE_MASS 0.000511
#define MU_MASS 0.10566
#define TAU_MASS 1.7768
#define TQ_MASS 173.34
#define BQ_MASS 4.18
#define CQ_MASS 1.275
#define SQ_MASS 0.095
#define DQ_MASS 0.0048
#define UQ_MASS 0.0023

#define ELE_PID 11
#define ENU_PID 12
#define MU_PID 13
#define MNU_PID 14
#define TAU_PID 15
#define TNU_PID 16
#define TQ_PID 6
#define BQ_PID 5
#define CQ_PID 4
#define SQ_PID 3
#define DQ_PID 1
#define UQ_PID 2

struct particle
{
    int pid;
    float mass;
    int color;
};
