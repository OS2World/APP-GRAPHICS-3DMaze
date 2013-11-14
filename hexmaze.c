#include <stdio.h>
#define INCL_WIN
#define INCL_GPI
#define INCL_PM
#define INCL_DOS
#include <os2.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "hexmaze.h"
 
#define ERROR_COLOR            CLR_RED
#define NUM_COLORS                  16
#define PIXELS_PER_ROOM             40
#define RESOLUTION                   4
#define WM_PLOT_DONE           WM_USER
 
/* screen constants */
#define WIDTH_OF_SCREEN        8.0
#define HEIGHT_OF_SCREEN       6.0 /* same units as WIDTH_OF_SCREEN */
 
typedef struct
          {
            unsigned char red;
            unsigned char green;
            unsigned char blue;
          } rgb_rec;
 
typedef struct stack_rec_record
          {
            char          index_1;
            int           index_2;
          } stack_rec;
 
typedef struct
          {
            double x;
            double y;
            double z;
          } vertex_rec;
 
static void             adjust_perspective(volatile int,volatile int,
                         volatile float * volatile,volatile float * volatile,
                         volatile float * volatile,volatile double,
                         volatile double,volatile double,volatile double,
                         volatile double);
       MRESULT EXPENTRY client_window_proc(HWND,ULONG,MPARAM,MPARAM);
static void             destroy_maze(volatile float * volatile *,
                         volatile float * volatile *,
                         volatile float * volatile *,volatile int * volatile *,
                         volatile int * volatile *,
                         volatile unsigned char * volatile *,
                         volatile unsigned char * volatile *,
                         volatile stack_rec * volatile *);
static void             draw_line_on_page(int,int,int,int);
static void             evaluate_and_transform(volatile double,volatile double,
                         volatile double,volatile double,volatile int,
                         volatile int,volatile double,volatile double,
                         volatile float * volatile,volatile float * volatile,
                         volatile float * volatile,volatile double *,
                         volatile double *,volatile double *,
                         volatile double *,volatile double *,
                         volatile vertex_rec *,volatile int * volatile,
                         volatile int * volatile,
                         volatile unsigned char * volatile);
static double           f(double,double);
static void             generate_maze(void);
static void             hash(int *,int *,int *,int *,int *,int *,int *,int *);
static void             increment(int *,int *,int *,int *,int *,int *,int *,
                         int *);
       MRESULT EXPENTRY help_proc(HWND,ULONG,MPARAM,MPARAM);
       int              main(void);
static int              memory_allocated(volatile long,
                         volatile float * volatile *,
                         volatile float * volatile *,
                         volatile float * volatile *,
                         volatile int * volatile *,
                         volatile int * volatile *,
                         volatile unsigned char * volatile *,
                         volatile unsigned char * volatile *,volatile int,
                         volatile stack_rec * volatile *);
static void             plot(void *);
static void             set_point_on_page(int,int);
static void             shade(volatile int,volatile int,
                         volatile float * volatile,volatile float * volatile,
                         volatile float * volatile,
                         volatile unsigned char * volatile,
                         volatile vertex_rec *);
static void             size_maze(void *);
static void             sort_back_to_front(volatile long,
                         volatile float * volatile,volatile int * volatile,
                         volatile int * volatile);
static void             solve_maze(volatile stack_rec * volatile,
                         volatile char * volatile * volatile,int *,int *,
                         volatile int,volatile int);
 
static volatile double                              aspect_ratio = 0.0;
static volatile char          * volatile * volatile base_page
 = (volatile char * volatile * volatile) NULL;
static volatile unsigned char * volatile            base_z
 = (volatile unsigned char * volatile) NULL;
static volatile unsigned char * volatile            color
 = (volatile unsigned char * volatile) NULL;
static volatile int                                 delta_x [6] [720]
 ={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
static volatile int                                 delta_y [6] [720]
 ={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
       volatile HPS                                 hPS = (HPS) 0;
static volatile short                               horizontal_scroll_position
 = (short) 0;
static volatile HWND                                hwndHscroll = (HWND) 0;
static volatile HWND                                hwndVscroll = (HWND) 0;
static volatile int                                 iFatalError = 0;
static volatile int                                 iMaxX = 0;
static volatile int                                 iMaxY = 0;
static volatile vertex_rec                          light = {0.0,0.0,0.0};
static volatile int                                 max_x = 0;
static volatile int                                 max_x_plus_1 = 0;
static volatile int                                 max_y = 0;
static volatile int                                 max_y_plus_1 = 0;
static volatile int                                 num_columns = 0;
static volatile long                                num_primes = 0l;
static volatile int                                 num_rooms_in_maze = 0;
static volatile int                                 num_rows = 0;
static volatile int                                 num_x_divisions = 0;
static volatile int                                 num_x_dots = 0;
static volatile int                                 num_y_divisions = 0;
static volatile int                                 num_y_dots = 0;
static volatile char          * volatile * volatile page
 = (volatile char * volatile * volatile) NULL;
       volatile HEV                                 plot_active = (HEV) 0;
static volatile int                                 plot_command = 0;
       volatile HWND                                plot_hwnd = (HWND) 0;
static volatile int                                 plot_kill = 0;
static volatile ULONG                               plot_post_count = (ULONG) 0;
static volatile ULONG                               plotting = (ULONG) 0;
static volatile int                                 r_n [8]
 = {0,0,0,0,0,0,0,0};
static volatile int                                 r_n_seed [7]
 = {0,0,0,0,0,0,0};
static volatile int                                 resized = 0;
static volatile double                              rotation = 0.0;
       volatile HEV                                 size_maze_active = (HEV) 0;
static volatile int                                 size_maze_kill = 0;
static volatile ULONG                               size_maze_post_count 
 = (ULONG) 0;
static volatile ULONG                               sizing = (ULONG) 0;
static volatile int                                 solve_draw_or_clear = 0;
static volatile stack_rec     * volatile            stack
 = (volatile stack_rec * volatile) NULL;
static volatile double                              tilt = 0.0;
static volatile short                               vertical_scroll_position
 = (short) 0;
static volatile int           * volatile            x_division_index
 = (volatile int * volatile) NULL;
static volatile int                                 x_dot_max = 0;
static volatile double                              x_max = 0.0;
static volatile double                              x_min = 0.0;
static volatile float         * volatile            x_prime
 = (volatile float * volatile) NULL;
static volatile double                              x_prime_max = 0.0;
static volatile int           * volatile            y_division_index
 = (volatile int * volatile) NULL;
static volatile int                                 y_dot_max = 0;
static volatile double                              y_max = 0.0;
static volatile double                              y_min = 0.0;
static volatile float         * volatile            y_prime
 = (volatile float * volatile) NULL;
static volatile double                              y_prime_max = 0.0;
static volatile double                              y_prime_min = 0.0;
static volatile float         * volatile            z_prime
 = (volatile float * volatile) NULL;
static volatile double                              z_prime_max = 0.0;
static volatile double                              z_prime_min = 0.0;
 
int main(void)
  {
           ULONG ctldata = (ULONG) 0;
           HAB   hAB = (HAB) 0;
           HMQ   hmq = (HMQ) 0;
           HWND  hwndClient = (HWND) 0;
           HWND  hwndFrame = (HWND) 0;
           QMSG  qmsg
     ={(HWND) 0,(ULONG) 0,(MPARAM) 0,(MPARAM) 0,(ULONG) 0,{0l,0l}};
    static CHAR  szClientClass [] = "3D Maze";
 
    DosCreateEventSem((PSZ) NULL,(PHEV) &plot_active,(ULONG) 0,(BOOL32) 1);
    DosCreateEventSem((PSZ) NULL,(PHEV) &size_maze_active,(ULONG) 0,(BOOL32) 1);
    hAB=WinInitialize((ULONG) 0);
    hmq=WinCreateMsgQueue(hAB,0l);
    WinRegisterClass(hAB,(PSZ) szClientClass,(PFNWP) client_window_proc,
     (ULONG) CS_SIZEREDRAW,(ULONG) 0);
    ctldata=(((ULONG) FCF_STANDARD) | ((ULONG) FCF_HORZSCROLL) |
    ((ULONG) FCF_VERTSCROLL));
    hwndFrame=WinCreateStdWindow((HWND) HWND_DESKTOP,(ULONG) WS_VISIBLE,
     &ctldata,(PSZ) szClientClass,(PSZ) NULL,(ULONG) 0,(HMODULE) NULL,
     (ULONG) ID_MAINMENU,(PHWND) &hwndClient);
    WinShowWindow((HWND) hwndFrame,(BOOL) TRUE);
    while (WinGetMsg((HAB) hAB,(PQMSG) &qmsg,(HWND) NULL,(ULONG) 0,(ULONG) 0))
     WinDispatchMsg((HAB) hAB,(PQMSG) &qmsg);
    WinDestroyWindow((HWND) hwndFrame);
    WinDestroyMsgQueue((HMQ) hmq);
    WinTerminate((HAB) hAB);
    return(0);
  }
 
MRESULT EXPENTRY client_window_proc(
  HWND   hwnd,
  ULONG  msg,
  MPARAM mp1,
  MPARAM mp2)
    {
      DATETIME dateSeed
       ={(UCHAR) 0,(UCHAR) 0,(UCHAR) 0,(UCHAR) 0,(UCHAR) 0,(UCHAR) 0,
       (USHORT) 0,(SHORT) 0,(UCHAR) 0};
      MRESULT  result = (MRESULT) 0;
  
      switch (msg)
        {
          case WM_CREATE:
            iFatalError=FALSE;
            DosGetDateTime(&dateSeed);
            r_n_seed[0]=dateSeed.year%10;
            r_n_seed[1]=dateSeed.month%10;
            r_n_seed[2]=dateSeed.day%10;
            r_n_seed[3]=dateSeed.hours%10;
            r_n_seed[4]=dateSeed.minutes%10;
            r_n_seed[5]=dateSeed.seconds%10;
            r_n_seed[6]=dateSeed.hundredths%10;
            hwndHscroll=WinWindowFromID(WinQueryWindow(hwnd,QW_PARENT),
             FID_HORZSCROLL);
            WinSendMsg(hwndHscroll,SBM_SETSCROLLBAR,MPFROM2SHORT(0,0),
             MPFROM2SHORT(0,360));
            horizontal_scroll_position=0;
            rotation=(double) (((short) 180)-horizontal_scroll_position);
            hwndVscroll=WinWindowFromID(WinQueryWindow(hwnd,QW_PARENT),
             FID_VERTSCROLL);
            WinSendMsg(hwndVscroll,SBM_SETSCROLLBAR,MPFROM2SHORT(60,0),
             MPFROM2SHORT(0,90));
            vertical_scroll_position=60;
            tilt=(double) (((short) 90)-vertical_scroll_position);
            solve_draw_or_clear=(int) 'D';
            iMaxX=0;
            iMaxY=0;
            resized=FALSE;
            plot_command=FALSE;
            aspect_ratio=(HEIGHT_OF_SCREEN/WIDTH_OF_SCREEN)
             /(((double) WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN))
             /((double) WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN)));
            color=(volatile unsigned char * volatile) NULL;
            base_z=(volatile unsigned char * volatile) NULL;
            x_division_index=(volatile int * volatile) NULL;
            x_prime=(volatile float * volatile) NULL;
            y_division_index=(volatile int * volatile) NULL;
            y_prime=(volatile float * volatile) NULL;
            z_prime=(volatile float * volatile) NULL;
            base_page=(volatile char * volatile * volatile) NULL;
            page=(volatile char * volatile * volatile) NULL;
            stack=(volatile stack_rec * volatile) NULL;
            break;
          case WM_HSCROLL:
            switch (SHORT2FROMMP(mp2))
              {
                case SB_LINELEFT:
                  horizontal_scroll_position--;
                  break;
                case SB_LINERIGHT:
                  horizontal_scroll_position++;
                  break;
                case SB_PAGELEFT:
                  horizontal_scroll_position=
                   (volatile short) (((int) horizontal_scroll_position)-10);
                  break;
                case SB_PAGERIGHT:
                  horizontal_scroll_position=
                   (volatile short) (((int) horizontal_scroll_position)+10);
                  break;
                case SB_SLIDERPOSITION:
                  horizontal_scroll_position=(volatile short) SHORT1FROMMP(mp2);
                case SB_ENDSCROLL:
                  plot_command=TRUE;
                  DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
                  if (plotting == (ULONG) 0)
                    plot_kill=TRUE;
                  DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
                  if (sizing == (ULONG) 0)
                    {
                      size_maze_kill=TRUE;
                      DosWaitEventSem((HEV) size_maze_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                    }
                  if (plot_kill)
                    {
                      DosWaitEventSem((HEV) plot_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                      WinEndPaint(hPS);
                    }
                  rotation=(double) (((short) 180)-horizontal_scroll_position);
                  if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
                  &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
                   /((double) PIXELS_PER_ROOM)) > 2.0))
                   {
                      DosResetEventSem((HEV) size_maze_active,
                       (PULONG) &size_maze_post_count);
                      size_maze_kill=FALSE;
                      _beginthread(size_maze,(void *) NULL,16384,(void *) NULL);
                    }
                  switch (solve_draw_or_clear)
                    {
                      case 'C':
                        solve_draw_or_clear='D';
                        break;
                      case 'S':
                        solve_draw_or_clear='B';
                        break;
                      default:
                        break;
                    }
                  WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                  break;
                default:
                  break;
              }
            horizontal_scroll_position=(volatile short)
             max(0,min(horizontal_scroll_position,360));
            if (horizontal_scroll_position
             != SHORT1FROMMR(WinSendMsg(hwndHscroll,SBM_QUERYPOS,NULL,NULL)))
              WinSendMsg(hwndHscroll,SBM_SETPOS,
               MPFROMSHORT(horizontal_scroll_position),NULL);
            break;
          case WM_VSCROLL:
            switch (SHORT2FROMMP(mp2))
              {
                case SB_LINEUP:
                  vertical_scroll_position--;
                  break;
                case SB_LINEDOWN:
                  vertical_scroll_position++;
                  break;
                case SB_PAGEUP:
                  vertical_scroll_position=
                   (volatile short) (((int) vertical_scroll_position)-10);
                  break;
                case SB_PAGEDOWN:
                  vertical_scroll_position=
                   (volatile short) (((int) vertical_scroll_position)+10);
                  break;
                case SB_SLIDERPOSITION:
                  vertical_scroll_position=(volatile short) SHORT1FROMMP(mp2);
                case SB_ENDSCROLL:
                  plot_command=TRUE;
                  DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
                  if (plotting == (ULONG) 0)
                    plot_kill=TRUE;
                  DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
                  if (sizing == (ULONG) 0)
                    {
                      size_maze_kill=TRUE;
                      DosWaitEventSem((HEV) size_maze_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                    }
                  if (plot_kill)
                    {
                      DosWaitEventSem((HEV) plot_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                      WinEndPaint(hPS);
                    }
                  tilt=(double) (((short) 90)-vertical_scroll_position);
                  if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
                  &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
                   /((double) PIXELS_PER_ROOM)) > 2.0))
                    {
                      DosResetEventSem((HEV) size_maze_active,
                       (PULONG) &size_maze_post_count);
                      size_maze_kill=FALSE;
                      _beginthread(size_maze,(void *) NULL,16384,(void *) NULL);
                    }
                  switch (solve_draw_or_clear)
                    {
                      case 'C':
                        solve_draw_or_clear='D';
                        break;
                      case 'S':
                        solve_draw_or_clear='B';
                        break;
                      default:
                        break;
                    }
                  WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                  break;
                default:
                  break;
              }
            vertical_scroll_position=(volatile short)
             max(0,min(vertical_scroll_position,90));
            if (vertical_scroll_position
             != SHORT1FROMMR(WinSendMsg(hwndVscroll,SBM_QUERYPOS,NULL,NULL)))
              WinSendMsg(hwndVscroll,SBM_SETPOS,
               MPFROMSHORT(vertical_scroll_position),NULL);
            break;
          case WM_COMMAND:
            switch (COMMANDMSG(&msg)->cmd)
              {
                case IDM_CLEAR:
                  plot_command=TRUE;
                  if (iFatalError)
                    WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                  else
                    {
                      DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
                      DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
                      if ((sizing == (ULONG) 0)
                      ||  (plotting == (ULONG) 0))
                        DosBeep((ULONG) 60,(ULONG) 333);
                      else
                        {
                          solve_draw_or_clear=(int) 'C';
                          WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                        }
                    }
                  break;
                case IDM_NEW:
                  plot_command=TRUE;
                  DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
                  if (plotting == (ULONG) 0)
                    plot_kill=TRUE;
                  DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
                  if (sizing == (ULONG) 0)
                    {
                      size_maze_kill=TRUE;
                      DosWaitEventSem((HEV) size_maze_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                    }
                  if (plot_kill)
                    {
                      DosWaitEventSem((HEV) plot_active,
                       (ULONG) SEM_INDEFINITE_WAIT);
                      WinEndPaint(hPS);
                    }
                  if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
                  &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
                   /((double) PIXELS_PER_ROOM)) > 2.0))
                    {
                      DosGetDateTime(&dateSeed);
                      r_n_seed[0]=dateSeed.year%10;
                      r_n_seed[1]=dateSeed.month%10;
                      r_n_seed[2]=dateSeed.day%10;
                      r_n_seed[3]=dateSeed.hours%10;
                      r_n_seed[4]=dateSeed.minutes%10;
                      r_n_seed[5]=dateSeed.seconds%10;
                      r_n_seed[6]=dateSeed.hundredths%10;
                      DosResetEventSem((HEV) size_maze_active,
                       (PULONG) &size_maze_post_count);
                      size_maze_kill=FALSE;
                      _beginthread(size_maze,(void *) NULL,16384,(void *) NULL);
                    }
                  solve_draw_or_clear=(int) 'D';
                  WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                  break;
                case IDM_SOLVE:
                  plot_command=TRUE;
                  if (iFatalError)
                    WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                  else
                    {
                      DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
                      DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
                      if ((sizing == (ULONG) 0)
                      ||  (plotting == (ULONG) 0))
                        DosBeep(60,333);
                      else
                        {
                          if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
                          &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
                          /((double) PIXELS_PER_ROOM)) > 2.0))
                            solve_draw_or_clear=(int) 'S';
                          WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
                        }
                    }
                  break;
                case IDM_HELP:
                  WinDlgBox(HWND_DESKTOP,hwnd,(PFNWP) help_proc,(HMODULE) 0,
                   (ULONG) IDD_HELPBOX,(PVOID) NULL);
                  break;
                default:
                  break;
              }
            break;
          case WM_SIZE:
            DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
            if (plotting == (ULONG) 0)
              plot_kill=TRUE;
            DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
            if (sizing == (ULONG) 0)
              {
                size_maze_kill=TRUE;
                DosWaitEventSem((HEV) size_maze_active,
                 (ULONG) SEM_INDEFINITE_WAIT);
              }
            if (plot_kill)
              {
                DosWaitEventSem((HEV) plot_active,(ULONG) SEM_INDEFINITE_WAIT);
                WinEndPaint(hPS);
              }
            resized=TRUE;
            solve_draw_or_clear=(int) 'D';
            iMaxX=SHORT1FROMMP(mp2)-1;
            iMaxY=SHORT2FROMMP(mp2)-1;
            if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
            &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
             /((double) PIXELS_PER_ROOM)) > 2.0))
              {
                DosGetDateTime(&dateSeed);
                r_n_seed[0]=dateSeed.year%10;
                r_n_seed[1]=dateSeed.month%10;
                r_n_seed[2]=dateSeed.day%10;
                r_n_seed[3]=dateSeed.hours%10;
                r_n_seed[4]=dateSeed.minutes%10;
                r_n_seed[5]=dateSeed.seconds%10;
                r_n_seed[6]=dateSeed.hundredths%10;
                DosResetEventSem((HEV) size_maze_active,
                 (PULONG) &size_maze_post_count);
                size_maze_kill=FALSE;
                _beginthread(size_maze,(void *) NULL,16384,(void *) NULL);
              }
            break;
          case WM_ERASEBACKGROUND:
            result=(MRESULT) TRUE; 
            break;
          case WM_PAINT:
            DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
            if (plotting == (ULONG) 0)
              {
                plot_kill=TRUE;
                DosWaitEventSem((HEV) plot_active,(ULONG) SEM_INDEFINITE_WAIT);
                WinEndPaint(hPS);
                WinInvalidateRect(hwnd,(PRECTL) NULL,(BOOL) FALSE);
              }
            plot_hwnd=hwnd;
            if ((2*(2*iMaxX/(3*PIXELS_PER_ROOM)-1)+1 > 2)
            &&  (((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
             /((double) PIXELS_PER_ROOM)) > 2.0))
              {
                DosResetEventSem((HEV) plot_active,(PULONG) &plot_post_count);
                hPS=WinBeginPaint(hwnd,(HPS) NULL,(PRECTL) NULL);
                plot_kill=FALSE;
                if ((! resized) && (! plot_command))
                  switch (solve_draw_or_clear)
                    {
                      case 'C':
                        solve_draw_or_clear='D';
                        break;
                      case 'S':
                        solve_draw_or_clear='B';
                        break;
                      default:
                        break;
                    }
                _beginthread(plot,(void *) NULL,16384,(void *) NULL);
              }
            resized=FALSE;
            plot_command=FALSE;
            break;
          case WM_PLOT_DONE:
            WinEndPaint(hPS);
            break;
          case WM_DESTROY:
            DosQueryEventSem((HEV) plot_active,(PULONG) &plotting);
            if (plotting == (ULONG) 0)
              plot_kill=TRUE;
            DosQueryEventSem((HEV) size_maze_active,(PULONG) &sizing);
            if (sizing == (ULONG) 0)
              {
                size_maze_kill=TRUE;
                DosWaitEventSem((HEV) size_maze_active,
                 (ULONG) SEM_INDEFINITE_WAIT);
              }
            if (plot_kill)
              {
                DosWaitEventSem((HEV) plot_active,(ULONG) SEM_INDEFINITE_WAIT);
                WinEndPaint(hPS);
              }
            destroy_maze(&x_prime,&y_prime,&z_prime,&x_division_index,
             &y_division_index,&color,&base_z,&stack);
            break;
          default:
            result=(MRESULT) WinDefWindowProc(hwnd,msg,mp1,mp2);
            break;
        }
      return(result);
    }
 
MRESULT EXPENTRY help_proc(
  HWND   hwnd,
  ULONG  msg,
  MPARAM mp1,
  MPARAM mp2)
    {
      MRESULT  result = (MRESULT) 0;
 
      switch (msg)
        {
          case WM_COMMAND:
            switch (COMMANDMSG(&msg)->cmd)
              {
                case DID_OK:
                case DID_CANCEL:
                  WinDismissDlg(hwnd,(ULONG) TRUE);
                  break;
                default:
                  break;
              }
            break;
          default:
            result=(MRESULT) WinDefDlgProc(hwnd,msg,mp1,mp2);
        }
      return(result);
    }
  
static void size_maze(
  void *arg)
    {
      if (arg != (void *) NULL)
        DosBeep((ULONG) 300,(ULONG) 300);
      destroy_maze(&x_prime,&y_prime,&z_prime,&x_division_index,
       &y_division_index,&color,&base_z,&stack);
      num_columns=2*iMaxX/(3*PIXELS_PER_ROOM)-1;
      num_columns=2*num_columns+1;
      max_x=8*(num_columns/2)+6;
      max_x_plus_1=max_x+1;
      num_x_dots=RESOLUTION*max_x_plus_1;
      x_dot_max=num_x_dots-1;
      num_rows=(int) ((2.0/sqrt(3.0))*aspect_ratio*((double) iMaxY)
       /((double) PIXELS_PER_ROOM));
      max_y=4*num_rows;
      max_y_plus_1=max_y+1;
      num_y_dots=RESOLUTION*max_y_plus_1;
      y_dot_max=num_y_dots-1;
      num_rooms_in_maze=num_rows*num_columns-(num_columns/2);
      num_x_divisions=num_y_dots+2;
      num_y_divisions=num_x_dots+2;
      num_primes=(long) num_x_divisions;
      num_primes*=((long) num_y_divisions);
      if (! size_maze_kill)
        {
          if (memory_allocated(num_primes,&x_prime,&y_prime,
           &z_prime,&x_division_index,&y_division_index,&color,&base_z,
           num_rooms_in_maze,&stack))
            {
              if (! size_maze_kill)
                generate_maze();
              x_min=-1.0;
              x_max=(double) num_y_dots;
              y_min=-1.0;
              y_max=(double) num_x_dots;
              light.x=1.5;
              light.y=-1.0;
              light.z=2.6;
              if (! size_maze_kill)
                evaluate_and_transform(x_min,x_max,y_min,y_max,num_x_divisions,
                 num_y_divisions,rotation,tilt,x_prime,y_prime,z_prime,
                 &x_prime_max,&y_prime_min,&y_prime_max,&z_prime_min,
                 &z_prime_max,&light,x_division_index,y_division_index,base_z);
              if (! size_maze_kill)
                shade(num_x_divisions,num_y_divisions,x_prime,y_prime,z_prime,
                 color,&light);
              if (! size_maze_kill)
                adjust_perspective(num_x_divisions,num_y_divisions,x_prime,
                 y_prime,z_prime,x_prime_max,y_prime_min,y_prime_max,
                 z_prime_min,z_prime_max);
              if (! size_maze_kill)
                sort_back_to_front(num_primes,x_prime,x_division_index,
                 y_division_index);
            }
          else
            iFatalError=TRUE;
        }
      DosEnterCritSec();
      DosPostEventSem((HEV) size_maze_active);
      _endthread();
    }
 
static int memory_allocated(
  volatile long                     num_primes,
  volatile float         * volatile *x_prime,
  volatile float         * volatile *y_prime,
  volatile float         * volatile *z_prime,
  volatile int           * volatile *x_division_index,
  volatile int           * volatile *y_division_index,
  volatile unsigned char * volatile *color,
  volatile unsigned char * volatile *base_z,
  volatile int                      num_rooms_in_maze,
  volatile stack_rec     * volatile *stack)
    {
               int result = 0;
      register int y = 0;
 
      result=((base_page=(volatile char * volatile * volatile)
       malloc(((unsigned int) max_y_plus_1)*sizeof(volatile char * volatile)))
       != NULL);
      if (result)
        {
          for (y=0; ((result) && (y < max_y_plus_1)); y++)
            result=((base_page[y]=(volatile char * volatile)
             malloc(((unsigned int) max_x_plus_1)*sizeof(volatile char)))
             != NULL);
          if (! result)
            {
              --y;
              while (y > 0)
                free((void *) base_page[--y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          result=((page=(volatile char * volatile * volatile)
           malloc(((unsigned int) num_y_dots)*sizeof(volatile char * volatile)))
           != NULL);
          if (result)
            {
              for (y=0; ((result) && (y < num_y_dots)); y++)
                result=((page[y]=(volatile char * volatile)
                 malloc(((unsigned int) num_x_dots)*sizeof(volatile char)))
                 != NULL);
              if (! result)
                {
                  --y;
                  while (y > 0)
                    free((void *) page[--y]);
                  free((void *) page);
                  page=(volatile char * volatile * volatile) NULL;
                  for (y=0; y < max_y_plus_1; y++)
                    free((void *) base_page[y]);
                  free((void *) base_page);
                  base_page=(volatile char * volatile * volatile) NULL;
                }
            }
          else
            {
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*x_prime=(volatile float * volatile)
           malloc(((unsigned int) num_primes)*sizeof(volatile float)))
           != NULL)))
            {
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*y_prime=(volatile float * volatile)
           malloc(((unsigned int) num_primes)*sizeof(volatile float)))
           != NULL)))
            {
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*z_prime=(volatile float * volatile)
           malloc(((unsigned int) num_primes)*sizeof(volatile float)))
           != NULL)))
            {
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*x_division_index=(volatile int * volatile)
           malloc(((unsigned int) num_primes)*sizeof(volatile int))) != NULL)))
            {
              free((void *) *z_prime);
              *z_prime=(volatile float * volatile) NULL;
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*y_division_index=(volatile int * volatile)
           malloc(((unsigned int) num_primes)*sizeof(volatile int))) != NULL)))
            {
              free((void *) *x_division_index);
              *x_division_index=(volatile int * volatile) NULL;
              free((void *) *z_prime);
              *z_prime=(volatile float * volatile) NULL;
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*color=(volatile unsigned char * volatile) 
           malloc(((unsigned int) num_primes)*sizeof(volatile unsigned char)))
           != NULL)))
            {
              free((void *) *y_division_index);
              *y_division_index=(volatile int * volatile) NULL;
              free((void *) *x_division_index);
              *x_division_index=(volatile int * volatile) NULL;
              free((void *) *z_prime);
              *z_prime=(volatile float * volatile) NULL;
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*base_z=(volatile unsigned char * volatile) 
           malloc(((unsigned int) num_primes)*sizeof(volatile unsigned char)))
           != NULL)))
            {
              free((void *) *color);
              *color=(volatile unsigned char * volatile) NULL;
              free((void *) *y_division_index);
              *y_division_index=(volatile int * volatile) NULL;
              free((void *) *x_division_index);
              *x_division_index=(volatile int * volatile) NULL;
              free((void *) *z_prime);
              *z_prime=(volatile float * volatile) NULL;
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      if (result)
        {
          if (! (result=((*stack=(volatile stack_rec * volatile) malloc(
           ((unsigned int) num_rooms_in_maze)*sizeof(volatile stack_rec)))
           != NULL)))
            {
              free((void *) *base_z);
              *base_z=(volatile unsigned char * volatile) NULL;
              free((void *) *color);
              *color=(volatile unsigned char * volatile) NULL;
              free((void *) *y_division_index);
              *y_division_index=(volatile int * volatile) NULL;
              free((void *) *x_division_index);
              *x_division_index=(volatile int * volatile) NULL;
              free((void *) *z_prime);
              *z_prime=(volatile float * volatile) NULL;
              free((void *) *y_prime);
              *y_prime=(volatile float * volatile) NULL;
              free((void *) *x_prime);
              *x_prime=(volatile float * volatile) NULL;
              for (y=0; y < num_y_dots; y++)
                free((void *) page[y]);
              free((void *) page);
              page=(volatile char * volatile * volatile) NULL;
              for (y=0; y < max_y_plus_1; y++)
                free((void *) base_page[y]);
              free((void *) base_page);
              base_page=(volatile char * volatile * volatile) NULL;
            }
        }
      return(result);
    }
 
static void set_point_on_page(
  int x,
  int y)
    {
      int x_offset = 0;
      int x_out = 0;
      int y_offset = 0;
      int y_out = 0;
 
      for (x_offset = 0; x_offset < RESOLUTION; x_offset++)
        {
          x_out=x+x_offset;
          for (y_offset=0; y_offset < RESOLUTION; y_offset++)
            {
              y_out=y+y_offset;
              page[y_out][x_out]='W';
            }
        }
      return;
    }
 
 
static void draw_line_on_page(
  int x1,
  int y1,
  int x2,
  int y2)
    {
      int error = 0;
      int error_prime_x = 0;
      int error_prime_y = 0;
      int permissible_delta_x = 0;
      int permissible_delta_y = 0;
      int x = 0;
      int x_diff = 0;
      int y = 0;
      int y_diff = 0;
 
      if (x2 >= x1)
        permissible_delta_x=1;
      else
        permissible_delta_x=-1;
      if (y2 >= y1)
        permissible_delta_y=1;
      else
        permissible_delta_y=-1;
      x=x1;
      y=y1;
      x_diff=x2-x1;
      y_diff=y2-y1;
      set_point_on_page(x,y);
      while ((x != x2) || (y != y2))
        {
          error_prime_x=error+permissible_delta_x*y_diff;
          error_prime_y=error-permissible_delta_y*x_diff;
          if (abs(error_prime_x) <= abs(error_prime_y)) 
            {
              x+=permissible_delta_x;
              error=error_prime_x;
            } 
          else
            {
              y+=permissible_delta_y;
              error=error_prime_y;
            }
          set_point_on_page(x,y);
        }
      return;
    } 

static void solve_maze(
  volatile stack_rec * volatile            stack,
  volatile char      * volatile * volatile base_page,
           int                  *          num_rooms_in_solution,
           int                  *          adjacency,
  volatile int                             max_x,
  volatile int                             max_y)
    {
      int delta_index=0;
      int passage_found=0;
      int stack_head=-1;
      int x=3;
      int x_next=0;
      int y=2;
      int y_next=0;

      *num_rooms_in_solution=1;
      *adjacency=0;
      x=3;
      y=2;
      stack_head=-1;
      base_page[y][x]='S';
      do
        {
          delta_index=0;
          passage_found=FALSE;
          do
            {
              while ((delta_index < 6) && (! passage_found))
                {
                  x_next=x+delta_x[delta_index][0];
                  y_next=y+delta_y[delta_index][0];
                  if (base_page[y_next][x_next] == ' ')
                    passage_found=TRUE;
                  else
                    delta_index++;
                }
              if (! passage_found) 
                {
                  delta_index=(int) (stack[stack_head].index_1);
                  base_page[y][x]=' ';
                  x-=delta_x[delta_index][0];
                  y-=delta_y[delta_index][0];
                  base_page[y][x]=' ';
                  x-=delta_x[delta_index][0];
                  y-=delta_y[delta_index][0];
                  stack_head--;
                  delta_index++;
                }
            }
          while (! passage_found);
          base_page[y_next][x_next]='S';
          x_next+=delta_x[delta_index][0];
          y_next+=delta_y[delta_index][0];
          if (y_next <= max_y) 
            {
              stack_head++;
              stack[stack_head].index_1=(char) delta_index;
              base_page[y_next][x_next]='S';
              x=x_next;
              y=y_next;
            }
        }
      while (y_next < max_y);
      x=max_x-3;
      y=max_y-2;
      *adjacency=0;
      while (stack_head >= 0)
        {
          for (delta_index=0; delta_index < 6; delta_index++)
            {
              x_next=x+delta_x[delta_index][0];
              y_next=y+delta_y[delta_index][0];
              if (base_page[y_next][x_next] != 'S')
                {
                  if (base_page[y_next][x_next] == 'W')
                    {
                      x_next+=delta_x[delta_index][0];
                      y_next+=delta_y[delta_index][0];
                      if (x_next < 0)
                        (*adjacency)++;
                      else
                        if (x_next > max_x)
                          (*adjacency)++;
                        else
                          if (y_next < 0)
                            (*adjacency)++;
                          else
                            if (y_next > max_y)
                              (*adjacency)++;
                            else
                              {
                                if (base_page[y_next][x_next] == 'S')
                                  (*adjacency)++;
                              }
                    }
                }
            }
          x-=(2*delta_x[stack[stack_head].index_1][0]);
          y-=(2*delta_y[stack[stack_head].index_1][0]);
          stack_head--;
          (*num_rooms_in_solution)++;
        }
      for (delta_index=0; delta_index < 6; delta_index++)
        {
          x_next=x+delta_x[delta_index][0];
          y_next=y+delta_y[delta_index][0];
          if (base_page[y_next][x_next] != ' ')
            {
              if (base_page[y_next][x_next] == 'W')
                {
                  x_next+=delta_x[delta_index][0];
                  y_next+=delta_y[delta_index][0];
                  if (x_next < 0)
                    (*adjacency)++;
                  else
                    if (x_next > max_x)
                      (*adjacency)++;
                    else
                      if (y_next < 0)
                        (*adjacency)++;
                      else
                        if (y_next > max_y)
                          (*adjacency)++;
                        else
                          {
                            if (base_page[y_next][x_next] == 'S')
                              (*adjacency)++;
                          }
                }
            }
        }
      return;
    }
 
static void generate_maze()
    {
               int       adjacency = 0;
               int       column_num = 0;
               int       counter_0 = 0;
               int       counter_1 = 0;
               int       counter_2 = 0;
               int       counter_3 = 0;
               int       counter_4 = 0;
               int       counter_5 = 0;
               int       counter_6 = 0;
               int       counter_7 = 0;
               int       delta_index_1a = 0;
               int       delta_index_1b = 0;
               int       delta_index_1c = 0;
               int       delta_index_1d = 0;
               int       delta_index_1e = 0;
               int       delta_index_1f = 0;
               int       delta_index_2 = 0;
               int       num_rooms_in_solution = 0;
               int       passage_found = 0;
      register int       r_n_index_1 = 0;
      register int       r_n_index_2 = 0;
               int       row_num = 0;
               int       search_complete = 0;
               int       stack_head = 0;
               int       tem_int = 0;
               int       x = 0;
               int       x_mod_8 = 0;
               int       x_next = 0;
               int       y = 0;
               int       y_mod_4 = 0;
               int       y_next = 0;
               int       y_previous = 0;

      r_n[0]=r_n_seed[0];
      r_n[1]=r_n_seed[1];
      r_n[2]=r_n_seed[2];
      r_n[3]=r_n_seed[3];
      r_n[4]=r_n_seed[4];
      r_n[5]=r_n_seed[5];
      r_n[6]=r_n_seed[6];
      r_n[7]=0;
      counter_0=r_n[0];
      counter_1=r_n[1];
      counter_2=r_n[2];
      counter_3=r_n[3];
      counter_4=r_n[4];
      counter_5=r_n[5];
      counter_6=r_n[6];
      counter_7=r_n[7];
      hash(&counter_0,&counter_1,&counter_2,&counter_3,&counter_4,&counter_5,
       &counter_6,&counter_7);
      delta_y[0][0]=-1;
      delta_x[0][0]=-2;
      delta_y[1][0]=1;
      delta_x[1][0]=-2;
      delta_y[2][0]=-2;
      delta_x[2][0]=0;
      delta_y[3][0]=2;
      delta_x[3][0]=0;
      delta_y[4][0]=-1;
      delta_x[4][0]=2;
      delta_y[5][0]=1;
      delta_x[5][0]=2;
      delta_index_2=0;
      for (delta_index_1a=0; delta_index_1a < 6; delta_index_1a++)
        for (delta_index_1b=0; delta_index_1b < 6; delta_index_1b++)
          if (delta_index_1a != delta_index_1b)
           for (delta_index_1c=0; delta_index_1c < 6; delta_index_1c++)
             if ((delta_index_1a != delta_index_1c)
             &&  (delta_index_1b != delta_index_1c))
               for (delta_index_1d=0; delta_index_1d < 6; delta_index_1d++)
                  if ((delta_index_1a != delta_index_1d)
                  &&  (delta_index_1b != delta_index_1d)
                  &&  (delta_index_1c != delta_index_1d))
                    for (delta_index_1e=0; delta_index_1e < 6; 
                     delta_index_1e++)
                      if ((delta_index_1a != delta_index_1e)
                      &&  (delta_index_1b != delta_index_1e)
                      &&  (delta_index_1c != delta_index_1e)
                      &&  (delta_index_1d != delta_index_1e))
                        for (delta_index_1f=0; delta_index_1f < 6; 
                         delta_index_1f++) 
                          if ((delta_index_1a != delta_index_1f)
                          &&  (delta_index_1b != delta_index_1f)
                          &&  (delta_index_1c != delta_index_1f)
                          &&  (delta_index_1d != delta_index_1f)
                          &&  (delta_index_1e != delta_index_1f))
                            {
                              delta_x[delta_index_1a][delta_index_2]
                               =delta_x[0][0];
                              delta_y[delta_index_1a][delta_index_2]
                               =delta_y[0][0];
                              delta_x[delta_index_1b][delta_index_2]
                               =delta_x[1][0];
                              delta_y[delta_index_1b][delta_index_2]
                               =delta_y[1][0];
                              delta_x[delta_index_1c][delta_index_2]
                               =delta_x[2][0];
                              delta_y[delta_index_1c][delta_index_2]
                               =delta_y[2][0];
                              delta_x[delta_index_1d][delta_index_2]
                               =delta_x[3][0];
                              delta_y[delta_index_1d][delta_index_2]
                               =delta_y[3][0];
                              delta_x[delta_index_1e][delta_index_2]
                               =delta_x[4][0];
                              delta_y[delta_index_1e][delta_index_2]
                               =delta_y[4][0];
                              delta_x[delta_index_1f][delta_index_2]
                               =delta_x[5][0];
                              delta_y[delta_index_1f][delta_index_2]
                               =delta_y[5][0];
                              delta_index_2++;
                            };
      do
        {
          y_mod_4=1;
          for (y=0; y <= max_y; y++) 
            {
              if (y_mod_4 == 1)
                {
                  x_mod_8=1;
                  for (x=0; x <= max_x; x++)
                    {
                      if (((x_mod_8 == 0)
                        && (y != 0)
                        && (y != max_y))
                      ||  (x_mod_8 == 3)
                      ||  (x_mod_8 == 4)
                      ||  (x_mod_8 == 5))
                        base_page[y][x]='W';
                      else
                        base_page[y][x]=' ';
                      x_mod_8++;
                      if (x_mod_8 >= 8)
                        x_mod_8=0;
                    }
                } 
              else
                {
                  if (y_mod_4 == 0 || y_mod_4 == 2)
                    {
                      x_mod_8=1;
                      for (x=0; x <= max_x; x++)
                        {
                          if ((x_mod_8 == 2) || (x_mod_8 == 6))
                            base_page[y][x]='W';
                          else
                            base_page[y][x]=' ';
                          x_mod_8++;
                          if (x_mod_8 >= 8)
                            x_mod_8 = 0;
                        }
                    }
                  else
                    {
                      x_mod_8=1;
                      for (x=0; x <= max_x; x++)
                        {
                          if ((x_mod_8 == 0)
                          ||  (x_mod_8 == 1)
                          ||  (x_mod_8 == 4)
                          ||  (x_mod_8 == 7))
                            base_page[y][x]='W';
                          else
                            base_page[y][x]=' ';
                          x_mod_8++;
                          if (x_mod_8 >= 8)
                            x_mod_8=0;
                      }
                    }
                }
              y_mod_4++;
              if (y_mod_4 >= 4)
                y_mod_4=0;
            }
          r_n[0]=counter_0+1;
          r_n[1]=counter_1+1;
          r_n[2]=counter_2+1;
          r_n[3]=counter_3+1;
          r_n[4]=counter_4+1;
          r_n[5]=counter_5+1;
          r_n[6]=counter_6+1;
          r_n[7]=counter_7+1;
          column_num=r_n[0];
          r_n_index_1=0;
          r_n_index_2=1;
          while (r_n_index_2 < 8)
            {
              tem_int=r_n[r_n_index_2];
              r_n[r_n_index_1]=tem_int;
              column_num+=tem_int;
              if (column_num >= 727)
                column_num-=727;
              r_n_index_1=r_n_index_2;
              r_n_index_2++;
            }
          r_n[7]=column_num;
          column_num%=num_columns;
          x=4*column_num+3;
          row_num=r_n[0];
          r_n_index_1=0;
          r_n_index_2=1;
          while (r_n_index_2 < 8)
            {
              tem_int=r_n[r_n_index_2];
              r_n[r_n_index_1]=tem_int;
              row_num+=tem_int;
              if (row_num >= 727)
                row_num-=727;
              r_n_index_1=r_n_index_2;
              r_n_index_2++;
            }
          r_n[7]=row_num;
          if (column_num%2)
            {
              row_num%=(num_rows-1);
              y=4*row_num+4;
            }
          else
            {
              row_num%=num_rows;
              y=4*row_num+2;
            }
          base_page[y][x]=' ';
          stack_head=-1;
          do
            {
              delta_index_1a=0;
              do
                {
                  delta_index_2=r_n[0];
                  r_n_index_1=0;
                  r_n_index_2=1;
                  while (r_n_index_2 < 8)
                    {
                      tem_int=r_n[r_n_index_2];
                      r_n[r_n_index_1]=tem_int;
                      delta_index_2+=tem_int;
                      if (delta_index_2 >= 727)
                        delta_index_2-=727;
                      r_n_index_1=r_n_index_2;
                      r_n_index_2++;
                    }
                  r_n[7]=delta_index_2;
                }
              while (delta_index_2 >= 720);
              passage_found=FALSE;
              search_complete=FALSE;
              while (! search_complete)
                {
                  while ((delta_index_1a < 6) && (! passage_found))
                    {
                      x_next=x+2*delta_x[delta_index_1a][delta_index_2];
                      if (x_next <= 0)
                        delta_index_1a++;
                      else
                        if (x_next > max_x)
                          delta_index_1a++;
                        else
                          {
                            y_next=y+2*delta_y[delta_index_1a][delta_index_2];
                            if (y_next <= 0)
                              delta_index_1a++;
                            else
                              if (y_next > max_y)
                                delta_index_1a++;
                              else
                                if (base_page[y_next][x_next] == 'W')
                                  passage_found=TRUE;
                                else
                                  delta_index_1a++;
                          }
                    }
                  if (! passage_found)
                    {
                      if (stack_head >= 0)
                        {
                          delta_index_1a=(int) (stack[stack_head].index_1);
                          delta_index_2=stack[stack_head].index_2;
                          x-=2*delta_x[delta_index_1a][delta_index_2];
                          y-=2*delta_y[delta_index_1a][delta_index_2];
                          stack_head--;
                          delta_index_1a++;
                        }
                    }
                  search_complete=((passage_found)
                   || ((stack_head == -1) && (delta_index_1a >= 6)));
                }
              if (passage_found)
                {
                  stack_head++;
                  stack[stack_head].index_1=(char) delta_index_1a;
                  stack[stack_head].index_2=delta_index_2;
                  base_page[y_next][x_next]=' ';
                  base_page[(y+y_next)/2][(x+x_next)/2]=' ';
                  x=x_next;
                  y=y_next;
                }
            }
          while (stack_head != -1);
          base_page[0][3]='S';
          base_page[max_y][max_x-3]=' ';
          solve_maze(stack,base_page,&num_rooms_in_solution,&adjacency,max_x,
           max_y);
          increment(&counter_0,&counter_1,&counter_2,&counter_3,&counter_4,
           &counter_5,&counter_6,&counter_7);
        }
      while ((! size_maze_kill)
      &&     ((3*num_rooms_in_solution < num_rooms_in_maze)
          ||  (2*adjacency > 3*num_rooms_in_solution)));
      for (x = 0; x <= x_dot_max; x++) 
        for (y = 0; y <= y_dot_max; y++)
          page[y][x]=' ';
      y_previous=-1;
      y_next=1;
      for (y = 0; y <= max_y; y++)
        {
          x=0;
          for (x_next = 1; x_next <= max_x; x_next++)
            {
              if (base_page[y][x] == 'W')
                {
                  if (base_page[y][x_next] == 'W')
                    draw_line_on_page(RESOLUTION*x,RESOLUTION*y,
                     RESOLUTION*x_next,RESOLUTION*y);
                  if (y_previous >= 0)
                    {
                      if (base_page[y_previous][x_next] == 'W')
                        draw_line_on_page(RESOLUTION*x,RESOLUTION*y,
                         RESOLUTION*x_next,RESOLUTION*y_previous);
                    }
                  if (y_next <= max_y) 
                    {
                      if (base_page[y_next][x_next] == 'W')
                        draw_line_on_page(RESOLUTION*x,RESOLUTION*y,
                         RESOLUTION*x_next,RESOLUTION*y_next);
                    }
                }
              x=x_next;
            }
          y_previous=y;
          y_next++;
        }
      return;
    }
 
static int substitution_high [100] =
             { 4,1,2,8,8,9,9,6,5,7,2,1,2,9,8,8,6,3,5,1,9,5,4,4,9,8,6,0,8,0,
               6,0,2,4,1,9,2,0,7,4,7,3,0,0,2,6,8,9,4,0,8,3,2,3,2,5,2,4,6,9,
               7,9,1,3,5,7,1,1,4,5,8,1,6,0,5,7,8,2,3,3,7,3,5,1,7,5,4,0,3,6,
               3,7,7,1,9,4,0,5,6,6 
             };
static int substitution_low [100] =
             { 1,2,2,1,5,5,4,6,4,6,4,4,5,6,6,3,0,9,6,5,7,2,0,9,3,4,2,3,9,1,
               9,9,9,3,8,9,3,4,1,5,0,5,2,7,0,8,8,0,4,5,0,3,6,8,1,7,8,8,7,1,
               3,2,7,7,1,8,0,3,7,5,2,6,4,0,9,9,7,7,4,6,2,0,0,1,7,3,6,6,1,1,
               2,4,5,9,8,2,8,8,3,5 
             };
static void hash(
  int *counter_0,
  int *counter_1,
  int *counter_2,
  int *counter_3,
  int *counter_4,
  int *counter_5,
  int *counter_6,
  int *counter_7)
    {
      register int iteration = 0;
               int seed_0 = 0;
               int seed_1 = 0;
               int seed_2 = 0;
               int seed_3 = 0;
               int seed_4 = 0;
               int seed_5 = 0;
               int seed_6 = 0;
               int seed_7 = 0;
      register int substitution_index = 0;
               int tem_0 = 0;
               int tem_1 = 0;
               int tem_2 = 0;
 
      seed_0=(*counter_0);
      seed_1=(*counter_1);
      seed_2=(*counter_2);
      seed_3=(*counter_3);
      seed_4=(*counter_4);
      seed_5=(*counter_5);
      seed_6=(*counter_6);
      seed_7=(*counter_7);
      for (iteration=1; iteration <= 8; iteration++)
        {
          substitution_index=10*seed_1+seed_0;
          tem_0=substitution_low[substitution_index];
          tem_1=substitution_high[substitution_index];
          substitution_index=10*seed_3+seed_2;
          seed_0=substitution_low[substitution_index];
          tem_2=substitution_high[substitution_index];
          substitution_index=10*seed_5+seed_4;
          seed_2=substitution_low[substitution_index];
          seed_1=substitution_high[substitution_index];
          substitution_index=10*seed_7+seed_6;
          seed_5=substitution_low[substitution_index];
          seed_7=substitution_high[substitution_index];
          seed_3=tem_0;
          seed_6=tem_1;
          seed_4=tem_2;
        }
      (*counter_0)=seed_0;
      (*counter_1)=seed_1;
      (*counter_2)=seed_2;
      (*counter_3)=seed_3;
      (*counter_4)=seed_4;
      (*counter_5)=seed_5;
      (*counter_6)=seed_6;
      (*counter_7)=seed_7;
      return;
    }
 
static void increment(
  int *counter_0,
  int *counter_1,
  int *counter_2,
  int *counter_3,
  int *counter_4,
  int *counter_5,
  int *counter_6,
  int *counter_7)
    {
      register int tem = 0;
 
      tem=(*counter_0)+1;
      if (tem <= 9)
        (*counter_0)=tem;
      else
        {
          (*counter_0)=0;
          tem=(*counter_1)+1;
          if (tem <= 9)
            (*counter_1)=tem;
          else
            {
              (*counter_1)=0;
              tem=(*counter_2)+1;
              if (tem <= 9)
                (*counter_2)=tem;
              else
                {
                  (*counter_2)=0;
                  tem=(*counter_3)+1;
                  if (tem <= 9)
                    (*counter_3)=tem;
                  else
                    {
                      (*counter_3)=0;
                      tem=(*counter_4)+1;
                      if (tem <= 9)
                        (*counter_4)=tem;
                      else
                        {
                          (*counter_4)=0;
                          tem=(*counter_5)+1;
                          if (tem <= 9)
                            (*counter_5)=tem;
                          else
                            {
                              (*counter_5)=0;
                              tem=(*counter_6)+1;
                              if (tem <= 9)
                                (*counter_6)=tem;
                              else
                                {
                                  (*counter_6)=0;
                                  tem=(*counter_7)+1;
                                  if (tem <= 9)
                                    (*counter_7)=tem;
                                  else
                                    (*counter_7)=0;
                                }
                            }
                        }
                    }
                }
            }
        }
      return;
    }
 
static void evaluate_and_transform(
  volatile double                   x_min,
  volatile double                   x_max,
  volatile double                   y_min,
  volatile double                   y_max,
  volatile int                      num_x_divisions,
  volatile int                      num_y_divisions,
  volatile double                   rotation,
  volatile double                   tilt,
  volatile float         * volatile x_prime,
  volatile float         * volatile y_prime,
  volatile float         * volatile z_prime,
  volatile double        *          x_prime_max,
  volatile double        *          y_prime_min,
  volatile double        *          y_prime_max,
  volatile double        *          z_prime_min,
  volatile double        *          z_prime_max,
  volatile vertex_rec    *          light,
  volatile int           * volatile x_division_index,
  volatile int           * volatile y_division_index,
  volatile unsigned char * volatile base_z)
    {
               double cos_rotation = 0.0;
               double cos_tilt = 0.0;
               double magnitude = 0.0;
               long   prime_num = 0l;
               double radians = 0.0;
               double radians_per_degree = 0.0;
               double sin_rotation = 0.0;
               double sin_tilt = 0.0;
               double x = 0.0;
               double x_delta = 0.0;
      register int    x_division_num = 0;
               double x_rotated = 0.0;
               double y = 0.0;
               double y_delta = 0.0;
      register int    y_division_num = 0;
               double z = 0.0;
 
      radians_per_degree=atan(1.0)/45.0;
      radians=tilt*radians_per_degree;
      cos_tilt=cos(radians);
      sin_tilt=sin(radians);
      radians=rotation*radians_per_degree;
      cos_rotation=cos(radians);
      sin_rotation=sin(radians);
      z=fabs(f(x_min,y_min));
      x_rotated=x_min*cos_rotation+y_min*sin_rotation;
      *y_prime_min=-x_min*sin_rotation+y_min*cos_rotation;
      *z_prime_min=-x_rotated*sin_tilt+z*cos_tilt;
      *y_prime_max=*y_prime_min;
      *z_prime_max=*z_prime_min;
      *x_prime_max=x_rotated*cos_tilt+z*sin_tilt;
      x_delta=(double) (num_x_divisions-1);
      x_delta=(x_max-x_min)/x_delta;
      y_delta=(double) (num_y_divisions-1);
      y_delta=(y_max-y_min)/y_delta;
      x=x_min;
      prime_num=0l;
      for (x_division_num=0; 
       ((! size_maze_kill) && (x_division_num < num_x_divisions));
       x_division_num++)
        {
          y=y_min;
          for (y_division_num=0; 
           ((! size_maze_kill) && (y_division_num < num_y_divisions));
           y_division_num++)
            {
              z=f(x,y);
              if (z > 0.0)
                base_z[prime_num]=(unsigned char) 1;
              else
                if (z < 0.0)
                  base_z[prime_num]=(unsigned char) 2;
                else
                  base_z[prime_num]=(unsigned char) 0;
              z=fabs(z);
              x_division_index[prime_num]=x_division_num;
              y_division_index[prime_num]=y_division_num;
              x_rotated=x*cos_rotation+y*sin_rotation;
              y_prime[prime_num]=(float) (-x*sin_rotation+y*cos_rotation);
              x_prime[prime_num]=(float) (x_rotated*cos_tilt+z*sin_tilt);
              z_prime[prime_num]=(float) (-x_rotated*sin_tilt+z*cos_tilt);
              if (((double) (x_prime[prime_num])) > *x_prime_max)
                *x_prime_max=(double) (x_prime[prime_num]);
              if (((double) (y_prime[prime_num])) < *y_prime_min)
                *y_prime_min=(double) (y_prime[prime_num]);
              if (((double) (y_prime[prime_num])) > *y_prime_max)
                *y_prime_max=(double) (y_prime[prime_num]);
              if (((double) (z_prime[prime_num])) < *z_prime_min)
                *z_prime_min=(double) (z_prime[prime_num]);
              if (((double) (z_prime[prime_num])) > *z_prime_max)
                *z_prime_max=(double) (z_prime[prime_num]);
              y+=y_delta;
              prime_num++;
            }
          x+=x_delta;
        }
      if ((! size_maze_kill))
        {
          magnitude=(*light).x*(*light).x;
          magnitude+=((*light).y*(*light).y);
          magnitude+=((*light).z*(*light).z);
          magnitude=sqrt(magnitude);
          (*light).x/=magnitude;
          (*light).y/=magnitude;
          (*light).z/=magnitude;
        }
      return;
    }
 
static void shade(
  volatile int                            num_x_divisions,
  volatile int                            num_y_divisions,
  volatile float               * volatile x_prime,
  volatile float               * volatile y_prime,
  volatile float               * volatile z_prime,
  volatile unsigned char       * volatile color,
  volatile vertex_rec          *          light)
    {
               double     magnitude = 0.0;
               vertex_rec normal = {0.0,0.0,0.0};
               long       prime_num = 0l;
               vertex_rec vertex [4]
       = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
      register int        x_division_num = 0;
      register int        y_division_num = 0;
 
      for (x_division_num=0;
       ((! size_maze_kill) && (x_division_num < num_x_divisions));
       x_division_num++)
        {
          for (y_division_num=0; 
           ((! size_maze_kill) && (y_division_num < num_y_divisions));
           y_division_num++)
            {
              vertex[0].x=(double) (x_prime[prime_num]);
              vertex[0].y=(double) (y_prime[prime_num]);
              vertex[0].z=(double) (z_prime[prime_num]);
              if (x_division_num < (num_x_divisions-1))
                if (y_division_num < (num_y_divisions-1))
                  {
                    prime_num+=((long) num_y_divisions);
                    vertex[1].x=(double) (x_prime[prime_num]);
                    vertex[1].y=(double) (y_prime[prime_num]);
                    vertex[1].z=(double) (z_prime[prime_num]);
                    prime_num++;
                    vertex[2].x=(double) (x_prime[prime_num]);
                    vertex[2].y=(double) (y_prime[prime_num]);
                    vertex[2].z=(double) (z_prime[prime_num]);
                    prime_num-=((long) num_y_divisions);
                    vertex[3].x=(double) (x_prime[prime_num]);
                    vertex[3].y=(double) (y_prime[prime_num]);
                    vertex[3].z=(double) (z_prime[prime_num]);
                    prime_num--;
                  }
                else
                  {
                    prime_num--;
                    vertex[1].x=(double) (x_prime[prime_num]);
                    vertex[1].y=(double) (y_prime[prime_num]);
                    vertex[1].z=(double) (z_prime[prime_num]);
                    prime_num+=((long) num_y_divisions);
                    vertex[2].x=(double) (x_prime[prime_num]);
                    vertex[2].y=(double) (y_prime[prime_num]);
                    vertex[2].z=(double) (z_prime[prime_num]);
                    prime_num++;
                    vertex[3].x=(double) (x_prime[prime_num]);
                    vertex[3].y=(double) (y_prime[prime_num]);
                    vertex[3].z=(double) (z_prime[prime_num]);
                    prime_num-=((long) num_y_divisions);
                  }
              else
                if (y_division_num < (num_y_divisions-1))
                  {
                    prime_num++;
                    vertex[1].x=(double) (x_prime[prime_num]);
                    vertex[1].y=(double) (y_prime[prime_num]);
                    vertex[1].z=(double) (z_prime[prime_num]);
                    prime_num-=((long) num_y_divisions);
                    vertex[2].x=(double) (x_prime[prime_num]);
                    vertex[2].y=(double) (y_prime[prime_num]);
                    vertex[2].z=(double) (z_prime[prime_num]);
                    prime_num--;
                    vertex[3].x=(double) (x_prime[prime_num]);
                    vertex[3].y=(double) (y_prime[prime_num]);
                    vertex[3].z=(double) (z_prime[prime_num]);
                    prime_num+=((long) num_y_divisions);
                  }
                else
                  {
                    prime_num-=((long) num_y_divisions);
                    vertex[1].x=(double) (x_prime[prime_num]);
                    vertex[1].y=(double) (y_prime[prime_num]);
                    vertex[1].z=(double) (z_prime[prime_num]);
                    prime_num--;
                    vertex[2].x=(double) (x_prime[prime_num]);
                    vertex[2].y=(double) (y_prime[prime_num]);
                    vertex[2].z=(double) (z_prime[prime_num]);
                    prime_num+=((long) num_y_divisions);
                    vertex[3].x=(double) (x_prime[prime_num]);
                    vertex[3].y=(double) (y_prime[prime_num]);
                    vertex[3].z=(double) (z_prime[prime_num]);
                    prime_num++;
                  }
              normal.x
               =(vertex[1].y-vertex[0].y)*(vertex[3].z-vertex[0].z)
               -(vertex[3].y-vertex[0].y)*(vertex[1].z-vertex[0].z)
               +(vertex[2].y-vertex[1].y)*(vertex[0].z-vertex[1].z)
               -(vertex[0].y-vertex[1].y)*(vertex[2].z-vertex[1].z)
               +(vertex[3].y-vertex[2].y)*(vertex[1].z-vertex[2].z)
               -(vertex[1].y-vertex[2].y)*(vertex[3].z-vertex[2].z)
               +(vertex[0].y-vertex[3].y)*(vertex[2].z-vertex[3].z)
               -(vertex[2].y-vertex[3].y)*(vertex[0].z-vertex[3].z);
              normal.y
               =(vertex[3].x-vertex[0].x)*(vertex[1].z-vertex[0].z)
               -(vertex[1].x-vertex[0].x)*(vertex[3].z-vertex[0].z)
               +(vertex[0].x-vertex[1].x)*(vertex[2].z-vertex[1].z)
               -(vertex[2].x-vertex[1].x)*(vertex[0].z-vertex[1].z)
               +(vertex[1].x-vertex[2].x)*(vertex[3].z-vertex[2].z)
               -(vertex[3].x-vertex[2].x)*(vertex[1].z-vertex[2].z)
               +(vertex[2].x-vertex[3].x)*(vertex[0].z-vertex[3].z)
               -(vertex[0].x-vertex[3].x)*(vertex[2].z-vertex[3].z);
              normal.z
               =(vertex[1].x-vertex[0].x)*(vertex[3].y-vertex[0].y)
               -(vertex[3].x-vertex[0].x)*(vertex[1].y-vertex[0].y)
               +(vertex[2].x-vertex[1].x)*(vertex[0].y-vertex[1].y)
               -(vertex[0].x-vertex[1].x)*(vertex[2].y-vertex[1].y)
               +(vertex[3].x-vertex[2].x)*(vertex[1].y-vertex[2].y)
               -(vertex[1].x-vertex[2].x)*(vertex[3].y-vertex[2].y)
               +(vertex[0].x-vertex[3].x)*(vertex[2].y-vertex[3].y)
               -(vertex[2].x-vertex[3].x)*(vertex[0].y-vertex[3].y);
              if (normal.x < 0.0)
                color[prime_num]=NUM_COLORS;
              else
                {
                  magnitude
                   =sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
                  if (magnitude == 0.0)
                    color[prime_num]=(unsigned char) 0;
                  else
                    {
                      color[prime_num]
                       =(unsigned char) ((((float) (NUM_COLORS-1))/2.0)*(1.0
                       +((*light).x*normal.x+(*light).y*normal.y
                       +(*light).z*normal.z)/magnitude));
                      if (color[prime_num] >= (unsigned char) (NUM_COLORS-1))
                        color[prime_num]=(unsigned char) (NUM_COLORS-2);
                    }
                }
              prime_num++;
            }
        }
      return;
    }
 
static void adjust_perspective(
  volatile int               num_x_divisions,
  volatile int               num_y_divisions,
  volatile float  * volatile x_prime,
  volatile float  * volatile y_prime,
  volatile float  * volatile z_prime,
  volatile double            x_prime_max,
  volatile double            y_prime_min,
  volatile double            y_prime_max,
  volatile double            z_prime_min,
  volatile double            z_prime_max)  
    {
               long       prime_num = 0l;
               vertex_rec vertex [4]
       = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
      register int        x_division_num = 0;
               double     x_eye = 0.0;
               double     y_center = 0.0;
      register int        y_division_num = 0;
               double     z_center = 0.0;
 
      if ((y_prime_max-y_prime_min) > (z_prime_max-z_prime_min))
        x_eye=1.1*(y_prime_max-y_prime_min)+x_prime_max;
      else
        x_eye=1.1*(z_prime_max-z_prime_min)+x_prime_max;
      if (((y_prime_max-y_prime_min) > (z_prime_max-z_prime_min))
      ||  (z_prime_max != z_prime_min))
        {
          y_center=(y_prime_max+y_prime_min)/2.0;
          z_center=(z_prime_max+z_prime_min)/2.0;
          prime_num=0l;
          for (x_division_num=0; 
           ((! size_maze_kill) && (x_division_num < num_x_divisions));
           x_division_num++)
            {
              for (y_division_num=0; 
               ((! size_maze_kill) && (y_division_num < num_y_divisions));
               y_division_num++)
                {
                  vertex[0].x=(double) (x_prime[prime_num]);
                  vertex[0].y=(double) (y_prime[prime_num]);
                  vertex[0].z=(double) (z_prime[prime_num]);
                  if (x_division_num < (num_x_divisions-1))
                    if (y_division_num < (num_y_divisions-1))
                      {
                        prime_num+=((long) num_y_divisions);
                        vertex[1].x=(double) (x_prime[prime_num]);
                        vertex[1].y=(double) (y_prime[prime_num]);
                        vertex[1].z=(double) (z_prime[prime_num]);
                        prime_num++;
                        vertex[2].x=(double) (x_prime[prime_num]);
                        vertex[2].y=(double) (y_prime[prime_num]);
                        vertex[2].z=(double) (z_prime[prime_num]);
                        prime_num-=((long) num_y_divisions);
                        vertex[3].x=(double) (x_prime[prime_num]);
                        vertex[3].y=(double) (y_prime[prime_num]);
                        vertex[3].z=(double) (z_prime[prime_num]);
                        prime_num--;
                      }
                    else
                      {
                        prime_num--;
                        vertex[1].x=(double) (x_prime[prime_num]);
                        vertex[1].y=(double) (y_prime[prime_num]);
                        vertex[1].z=(double) (z_prime[prime_num]);
                        prime_num+=((long) num_y_divisions);
                        vertex[2].x=(double) (x_prime[prime_num]);
                        vertex[2].y=(double) (y_prime[prime_num]);
                        vertex[2].z=(double) (z_prime[prime_num]);
                        prime_num++;
                        vertex[3].x=(double) (x_prime[prime_num]);
                        vertex[3].y=(double) (y_prime[prime_num]);
                        vertex[3].z=(double) (z_prime[prime_num]);
                        prime_num-=((long) num_y_divisions);
                      }
                  else
                    if (y_division_num < (num_y_divisions-1))
                      {
                        prime_num++;
                        vertex[1].x=(double) (x_prime[prime_num]);
                        vertex[1].y=(double) (y_prime[prime_num]);
                        vertex[1].z=(double) (z_prime[prime_num]);
                        prime_num-=((long) num_y_divisions);
                        vertex[2].x=(double) (x_prime[prime_num]);
                        vertex[2].y=(double) (y_prime[prime_num]);
                        vertex[2].z=(double) (z_prime[prime_num]);
                        prime_num--;
                        vertex[3].x=(double) (x_prime[prime_num]);
                        vertex[3].y=(double) (y_prime[prime_num]);
                        vertex[3].z=(double) (z_prime[prime_num]);
                        prime_num+=((long) num_y_divisions);
                      }
                    else
                      {
                        prime_num-=((long) num_y_divisions);
                        vertex[1].x=(double) (x_prime[prime_num]);
                        vertex[1].y=(double) (y_prime[prime_num]);
                        vertex[1].z=(double) (z_prime[prime_num]);
                        prime_num--;
                        vertex[2].x=(double) (x_prime[prime_num]);
                        vertex[2].y=(double) (y_prime[prime_num]);
                        vertex[2].z=(double) (z_prime[prime_num]);
                        prime_num+=((long) num_y_divisions);
                        vertex[3].x=(double) (x_prime[prime_num]);
                        vertex[3].y=(double) (y_prime[prime_num]);
                        vertex[3].z=(double) (z_prime[prime_num]);
                        prime_num++;
                      }
                  y_prime[prime_num]=(float) (y_center
                   +(vertex[0].y-y_center)*(x_eye-x_prime_max)
                   /(x_eye-vertex[0].x));
                  z_prime[prime_num]=(float) (z_center
                   +(vertex[0].z-z_center)*(x_eye-x_prime_max)
                   /(x_eye-vertex[0].x));
                  x_prime[prime_num]=(float)
                   (-(vertex[0].x+vertex[1].x+vertex[2].x+vertex[3].x)/4.0);
                  prime_num++;
                }
            }
         }
      return;
    }
 
static void sort_back_to_front(
  volatile long             num_primes,
  volatile float * volatile x_prime,
  volatile int   * volatile x_division_index,
  volatile int   * volatile y_division_index)
    {
               int   finished = 0;
               long  key_index_1 = 0l;
               long  key_index_2 = 0l;
               long  left = 0l;
               long  right = 0l;
               float t1 = (float) 0.0;
      register int   t2 = 0;
      register int   t3 = 0;
 
      left=num_primes/((long) 2);
      right=num_primes-1l;
      t1=x_prime[0];
      t2=x_division_index[0];
      t3=y_division_index[0];
      while ((! size_maze_kill) && (right > 0l))
        {
          if (left > 0l)
            {
              left--;
              t1=x_prime[left];
              t2=x_division_index[left];
              t3=y_division_index[left];
            }
          else
            {
              t1=x_prime[right];
              t2=x_division_index[right];
              t3=y_division_index[right];
              x_prime[right]=x_prime[0];
              x_division_index[right]=x_division_index[0];
              y_division_index[right]=y_division_index[0];
              right--;
            }
          if (right > 0l)
            {
              finished=FALSE;
              key_index_2=left;
              while (! finished)
                {
                  key_index_1=key_index_2;
                  key_index_2+=key_index_2;
                  key_index_2++;
                  if (key_index_2 > right)
                    finished=TRUE;
                  else
                    {
                      if (key_index_2 != right)
                        {
                          if (x_prime[key_index_2] > x_prime[key_index_2+1])
                            key_index_2++;
                        }
                      if (t1 <= x_prime[key_index_2])
                        finished=TRUE;
                      else
                        {
                          x_prime[key_index_1]=x_prime[key_index_2];
                          x_division_index[key_index_1]
                           =x_division_index[key_index_2];
                          y_division_index[key_index_1]
                           =y_division_index[key_index_2];
                        }
                    }
                }
              x_prime[key_index_1]=t1;
              x_division_index[key_index_1]=t2;
              y_division_index[key_index_1]=t3;
            }
        }
      x_prime[0]=t1;
      x_division_index[0]=t2;
      y_division_index[0]=t3;
      return;
    }
 
static void plot(
  void *arg)
    {  
      POINTL     box [4] = {{0l,0l},{0l,0l},{0l,0l},{0l,0l}};
      int        box_num = 0;
      short      color_num = (short) 0;
      double     pixels_per_unit = 0.0;
      long       prime_num = 0l;
      POINTL     ptlPosition = { 0l,0l };
      long       rgb [NUM_COLORS] 
       = { 0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l,0l };
      long       rgb_increment = 0l;
      int        solution = 0;
      long       tint = 0l;
      vertex_rec vertex [4] 
       = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
      int        x_division_num = 0;
      long       x_prime_num = 0l;
      int        y_division_num = 0;
      double     y_offset = 0.0;
      double     y_out_max = 0.0;
      double     z_offset = 0.0;
      double     z_out_max = 0.0;
 
      if (arg != (void *) NULL)
        DosBeep((ULONG) 300,(ULONG) 300);
      DosWaitEventSem((HEV) size_maze_active,(ULONG) SEM_INDEFINITE_WAIT);
      if ((! plot_kill) && (iFatalError))
        {
          GpiSetColor(hPS,ERROR_COLOR);
          ptlPosition.x=0;
          ptlPosition.y=0;
          GpiMove(hPS,&ptlPosition);
          ptlPosition.x=iMaxX;
          ptlPosition.y=iMaxY;
          GpiBox(hPS,DRO_FILL,&ptlPosition,(long) 0,(long) 0);
          DosBeep(60,333);
        }
      else
        {
          if (! plot_kill)
            {
              if ((solve_draw_or_clear == ((int) 'D')) 
              ||  (solve_draw_or_clear == ((int) 'B')))
                GpiErase(hPS);
              rgb_increment=0x111111;
              tint=0l;
              for (color_num=0; color_num < (NUM_COLORS-1); color_num++)
                {
                  rgb[color_num]=tint;
                  tint+=rgb_increment;
                }
              rgb[NUM_COLORS-1]=0xff0000;
              GpiCreateLogColorTable(hPS,LCOL_RESET,LCOLF_CONSECRGB,0L,16L,
               &rgb[0]);
            }
          if (! plot_kill)
            {
              y_out_max=(double) iMaxX;
              z_out_max=(double) iMaxY;
              if (aspect_ratio*z_out_max*(y_prime_max-y_prime_min)
               > y_out_max*(z_prime_max-z_prime_min))
                {
                  pixels_per_unit
                   =y_out_max/(aspect_ratio*(y_prime_max-y_prime_min));
                  y_offset=0.0;
                  z_offset
                   =(z_out_max-pixels_per_unit*(z_prime_max-z_prime_min))/2.0;
                }
              else
                if (aspect_ratio*z_out_max*(y_prime_max-y_prime_min)
                 < y_out_max*(z_prime_max-z_prime_min))
                  {
                    pixels_per_unit=z_out_max/(z_prime_max-z_prime_min);
                    y_offset=(y_out_max
                     -aspect_ratio*pixels_per_unit*(y_prime_max-y_prime_min))/2.0;
                    z_offset=0.0;
                  }
                else
                  {
                    pixels_per_unit=1.0;
                    y_offset=y_out_max/2.0;
                    z_offset=z_out_max/2.0;
                  }
            }
          for (x_prime_num=0l; ((! plot_kill) && (x_prime_num < num_primes));
           x_prime_num++)
            {
              x_division_num=x_division_index[x_prime_num];
              if (x_division_num < (num_x_divisions-1))
                {
                  y_division_num=y_division_index[x_prime_num];
                  if (y_division_num < (num_y_divisions-1))
                    {
                      prime_num
                       =((long) num_y_divisions)*((long) x_division_num)
                       +((long) y_division_num);
                      color_num=(short) (color[prime_num]);
                      if (color_num < NUM_COLORS)
                        {
                          vertex[0].y=(double) (y_prime[prime_num]);
                          vertex[0].z=(double) (z_prime[prime_num]);
                          solution=(base_z[prime_num] == (unsigned char) 2);
                          prime_num+=((long) num_y_divisions);
                          vertex[1].y=(double) (y_prime[prime_num]);
                          vertex[1].z=(double) (z_prime[prime_num]);
                          if (solution)
                            solution=(base_z[prime_num] == (unsigned char) 2);
                          prime_num++;
                          vertex[2].y=(double) (y_prime[prime_num]);
                          vertex[2].z=(double) (z_prime[prime_num]);
                          if (solution)
                            solution=(base_z[prime_num] == (unsigned char) 2);
                          prime_num-=((long) num_y_divisions);
                          vertex[3].y=(double) (y_prime[prime_num]);
                          vertex[3].z=(double) (z_prime[prime_num]);
                          if (solution)
                            solution=(base_z[prime_num] == (unsigned char) 2);
                          if ((solve_draw_or_clear == ((int) 'D')) 
                          ||  (solve_draw_or_clear == ((int) 'B'))
                          ||  ((solve_draw_or_clear == ((int) 'S'))
                            && (solution))
                          ||  ((solve_draw_or_clear == ((int) 'C'))
                            && (solution)))
                            {
                              if ((solve_draw_or_clear == ((int) 'S'))
                              ||  ((solve_draw_or_clear == ((int) 'B'))
                                && (solution)))
                                color_num=(short) (NUM_COLORS-1);
                              for (box_num=0; box_num < 4; box_num++)
                                {
                                  box[box_num].x=(long) (y_offset
                                   +pixels_per_unit*aspect_ratio
                                   *(vertex[box_num].y-y_prime_min));
                                  box[box_num].y=(long) (z_offset
                                   +pixels_per_unit
                                   *(vertex[box_num].z-z_prime_min));
                                }
                              GpiBeginPath(hPS,1L);
                              ptlPosition.x=box[3].x;
                              ptlPosition.y=box[3].y;
                              GpiMove(hPS,&ptlPosition);
                              GpiPolyLine(hPS,4l,box);
                              GpiEndPath(hPS);
                              GpiSetColor(hPS,color_num);
                              GpiFillPath(hPS,1l,FPATH_ALTERNATE);
                            }
                        }
                    }
                }
            }
        }
      DosEnterCritSec();
      if (! plot_kill)
        WinPostMsg(plot_hwnd,(ULONG) WM_PLOT_DONE,(MPARAM) NULL,
         (MPARAM) NULL);
      DosPostEventSem((HEV) plot_active);
      _endthread();
    }
 
static void destroy_maze(
  volatile float         * volatile *x_prime,
  volatile float         * volatile *y_prime,
  volatile float         * volatile *z_prime,
  volatile int           * volatile *x_division_index,
  volatile int           * volatile *y_division_index,
  volatile unsigned char * volatile *color,
  volatile unsigned char * volatile *base_z,
  volatile stack_rec     * volatile *stack)
    {
      int y = 0;
 
      if (*stack != (volatile stack_rec * volatile) NULL)
        {
          free((void *) *stack);
          *stack=(volatile stack_rec * volatile) NULL;
        }
      if (*base_z != (unsigned char * volatile) NULL)
        {
          free((void *) *base_z);
          *base_z=(volatile unsigned char * volatile) NULL;
        }
      if (*color != (unsigned char * volatile) NULL)
        {
          free((void *) *color);
          *color=(volatile unsigned char * volatile) NULL;
        }
      if (*y_division_index != (int * volatile) NULL)
        {
          free((void *) *y_division_index);
          *y_division_index=(volatile int * volatile) NULL;
        }
      if (*x_division_index != (int * volatile) NULL)
        {
          free((void *) *x_division_index);
          *x_division_index=(volatile int * volatile) NULL;
        }
      if (*z_prime != (float * volatile) NULL)
        {
          free((void *) *z_prime);
          *z_prime=(volatile float * volatile) NULL;
        }
      if (*y_prime != (float * volatile) NULL)
        {
          free((void *) *y_prime);
          *y_prime=(volatile float * volatile) NULL;
        }
      if (*x_prime != (float * volatile) NULL)
        {
          free((void *) *x_prime);
          *x_prime=(volatile float * volatile) NULL;
        }
      if (page != (char ** volatile) NULL)
        {
          for (y=0; y < num_y_dots; y++)
            free((void *) (page[y]));
          free((void *) page);
          page=(volatile char * volatile * volatile) NULL;
        }
      if (base_page != (char ** volatile) NULL)
        {
          for (y=0; y < max_y_plus_1; y++)
            free((void *) (base_page[y]));
          free((void *) base_page);
          base_page=(volatile char * volatile * volatile) NULL;
        }
      return;
    }

static double f(
  double x,
  double y)
    {
               int    base_x = 0;
               int    base_y = 0;
               int    solution = 0;
      register int    x_next = 0;
               int    x_offset = 0;
               int    x_out = 0;
      register int    y_next = 0;
               int    y_offset = 0;
               int    y_out = 0;
               double z = 0.0;
 
      y_out=(int) x;
      if (y_out < 0)
        z=0.0;
      else
        if (y_out > y_dot_max)
          z=0.0;
        else
          {
            x_out=(int) y;
            if (x_out < 0)
              z=0.0;
            else
              if (x_out > x_dot_max)
                z=0.0;
              else
                if (page[y_out][x_out] == 'W')
                  {
                    solution=FALSE;
                    base_x=x_out/RESOLUTION;
                    base_y=y_out/RESOLUTION;
                    for (y_offset=-4; ((! solution) && (y_offset <= 4));
                     y_offset++)
                      for (x_offset=-4; ((! solution) && (x_offset <= 4));
                       x_offset++)
                        if (abs(x_offset)+abs(y_offset) <= 4)
                          {
                            y_next=base_y+y_offset;
                            if ((y_next >= 0) && (y_next <= max_y))
                              {
                                x_next=base_x+x_offset;
                                if ((x_next >= 0) && (x_next <= max_x))
                                  {
                                     if (base_page[y_next][x_next] == 'S')
                                       solution=TRUE; 
                                  }
                              }
                          };
                    z=((double) (5*RESOLUTION));
                    if (solution)
                      z=-z;
                  }
                else
                  z=0.0;
          }
      return(z);
    }
 
