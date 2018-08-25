#include <GL/glut.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define TIMER_ID1 1
#define TIMER_ID2 2
#define TIMER_ID3 3
#define TIMER_INTERVAL1 20
#define TIMER_INTERVAL2 20
#define TIMER_INTERVAL3 20

static int window_width, window_height;
static void init_light(void);

static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_timer1(int value);
static void on_timer2(int value);
static void on_timer3(int value);
static void set_material(int num);

static int animation_ongoing = 0;//to start animation
static float animation_parameter_moving = 0;//to move cone
static int animation_parameter_making = 1;//making cone
static int x_coor_cone = 0;
static int number_of_cones = 0;

static int moving = 0;//to start moving sphere
static float moving_parameter = 0;
static float curr_x = 0;//for sphere
static int flag_color = 0;//we use flag to signal us to change color of sphere

int main(int argc, char **argv)
{
    //window initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    
    srand(time(NULL));
    init_light();

    glutMainLoop();

    return 0;
}

static void init_light(void)
{
    //setup of light and background
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    glClearColor(0, 0, 0, 0);

    glEnable(GL_DEPTH_TEST);

   
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        exit(0);
        break;
    case 's':
    case 'S'://star of animation, moving of cones
        if(!animation_ongoing){
            glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
            animation_ongoing = 1;
        }
        break;
    case 'j'://moving to left
        if(!moving){
            glutTimerFunc(TIMER_INTERVAL2, on_timer2, TIMER_ID2);
            moving = 1;
        }
        break;
        
    case 'l'://moving to right
        if(!moving){
            glutTimerFunc(TIMER_INTERVAL3, on_timer3, TIMER_ID3);
            moving = 1;
        }
        break;
    case 'r':
    case 'R'://restart of animation by restarting all parameters
        animation_ongoing = 0;
        moving = 0;
        moving_parameter = 0;
        animation_parameter_moving = 0;
        number_of_cones = 0;
        curr_x = 0;
        flag_color = 0;
        glutPostRedisplay();
        break;
    }
}
static void on_timer1(int value){
    if(TIMER_ID1 != value)
        return;
    
    animation_parameter_moving += 0.2;

    if(animation_parameter_moving>=7){
        animation_parameter_moving = 0;
        animation_parameter_making = 1;
        number_of_cones += 1; 
    }
    
    glutPostRedisplay();
    if(animation_ongoing){
        glutTimerFunc(TIMER_ID1, on_timer1, TIMER_ID1);
    }
}
//function to move to the left, decreasing x coordinates
static void on_timer2(int value){
    if(TIMER_ID2 != value)
        return;
    
    moving_parameter -= 0.2;
    curr_x -= 0.2;
    if(moving_parameter<=-2){
        moving = 0;
        moving_parameter = 0;
    }
    //limited movement so that the ball does not go off the track
    if(curr_x<=-2)
        curr_x = -2;
    
    glutPostRedisplay();
    if(moving){
        glutTimerFunc(TIMER_ID2, on_timer2, TIMER_ID2);
    }
}
//function to move to the right, by increasing x coordinates
static void on_timer3(int value){
    if(TIMER_ID3 != value)
        return;
    
    moving_parameter += 0.2;
    curr_x += 0.2;
    if(moving_parameter>=2){
        moving = 0;
        moving_parameter = 0;
    }
    
    //limited movement so that the ball does not go off the track
    if(curr_x>=2)
        curr_x = 2;
    
    glutPostRedisplay();
    if(moving){
        glutTimerFunc(TIMER_ID3, on_timer3, TIMER_ID3);
    }
}

//function when window dimension is changed
static void on_reshape(int width, int height)
{   
    window_width = width;
    window_height = height;
    //setup of viewport
    glViewport(0, 0, width, height);
    
    //setup of projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 15);
}
//setup of material properties
static void set_material(int num){
    GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat diffuse_coeffs[] = { 0.4, 0.4, 0.4, 1 };
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
    GLfloat shininess = 30;
    
    switch (num){
        case 1: //blue, for base
            diffuse_coeffs[2] = 1;
            break;
        case 2://red, for track
            diffuse_coeffs[0] = 1;
            break;
        case 3:
            if(!flag_color)//green, for sphere, start color
                diffuse_coeffs[1] = 1;
            else{//if cone is in the same track as sphere we change sphere color, to alarm player
                diffuse_coeffs[1] = diffuse_coeffs[1]-animation_parameter_moving/15;
                diffuse_coeffs[0] = diffuse_coeffs[0] + animation_parameter_moving/15;//to go to purple color
            }
            break;
        case 4://purple, for cones
            diffuse_coeffs[0] = 1;
            diffuse_coeffs[2] = 1;
            break;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

static void on_display(void)
{
    //delete previous contents of the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //define a viewing transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 7, 7, 0, 0, 0, 0, 1, 0);
    
    //light positiongrafika
    GLfloat light_position[] = { 10, 10, 10, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);


    glPushMatrix();
        //creating base
        glPushMatrix();
            set_material(1);
            glScalef(7, 1, 7);
            glutSolidCube(1);
        glPopMatrix();
        //creating track
        glPushMatrix();
            set_material(2);
            glTranslatef(2, 0.75, 0);
            glScalef(2, 1, 14);
            glutSolidCube(0.5);
        glPopMatrix();
        //creating track
        glPushMatrix();
            set_material(2);
            glTranslatef(0, 0.75, 0);
            glScalef(2, 1, 14);
            glutSolidCube(0.5);
        glPopMatrix();
        //creating track
        glPushMatrix();
            set_material(2);
            glTranslatef(-2, 0.75, 0);
            glScalef(2, 1, 14);
            glutSolidCube(0.5);
        glPopMatrix();
        //creation and translation of sphere
        glPushMatrix();
            set_material(3);
            glTranslatef(0+curr_x, 1.75, 3.25);
            glutSolidSphere(0.5, 50, 50);
        glPopMatrix();
        glPushMatrix();
            set_material(4);
            if(animation_ongoing){//when animation is active we make cones
                if(animation_parameter_making){//when one cone is gone we make another one on random track using rand() function
                    float rand_value = rand()/(float)RAND_MAX;//value is in [0,1] interval
                    if(rand_value<=0.33){
                        x_coor_cone = -2;
                    }
                    else if(rand_value>=0.66){
                        x_coor_cone = 2;
                    }
                    else{
                        x_coor_cone = 0;
                    }
                    animation_parameter_making = 0;
                }
                glTranslatef(x_coor_cone, 1.25, -4.5+animation_parameter_moving);//decreasing z-ose
                glutSolidCone(0.1,0.5,20,20);
                if(abs(x_coor_cone - curr_x)<=0.5)
                    flag_color = 1;
                else
                    flag_color = 0;
                //stop of animation when sphere and cone come to contant
                if(abs(x_coor_cone - curr_x)<=0.5 && animation_parameter_moving >= 6.0){
                    animation_ongoing = 0;
                    //show_message();
                }
            }
        glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}