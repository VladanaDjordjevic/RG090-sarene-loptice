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
static void drawSphere();
static void drawBase();
static void drawCone();
static void drawCube();
static void drawLosingMessage();
static void drawStars();
static void printPoints(float x, float y, float z);

static int animation_ongoing = 0;//to start animation
static float animation_parameter_moving = 0;//to move cone and cube
static int animation_parameter_making_cone = 1;//making cone
static int animation_parameter_making_cube = 1;//making cube
static int x_coor_cone = 0;
static int x_coor_cube = 0;
static int number_of_cones = 0;
static int number_of_cube = 0;//this number will represent points

static int moving = 0;//to start moving sphere
static float moving_parameter = 0;
static float curr_x = 0;//for sphere
static int flag_color = 0;//we use flag to signal us to change color of sphere

static float rotation_parameter1 = 0;//parameter for rotation around y-axis
static float rotation_parameter2 = 0;//parameter for rotation around x-axis
static int direction_change = 1;//to change direction of rotation around x-axis

static int game_over = 0;//flag to stop animation when sphere and cone come to contant

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
        number_of_cube = 0;
        curr_x = 0;
        flag_color = 0;
        game_over = 0;
        rotation_parameter1 = 0;
        rotation_parameter2 = 0;
        glutPostRedisplay();
        break;
    }
}
//function to start animation, signal to make cone
static void on_timer1(int value){
    if(TIMER_ID1 != value)
        return;
    
    animation_parameter_moving += 0.085+number_of_cones*0.005;//number_of_cones will increase, so we use that to increase speed with time
    rotation_parameter1 += 0.15;
    rotation_parameter2 += 0.1*direction_change;
    //our rotation will go from 20 to -20 
    if(rotation_parameter2>=20){//direction change when we go over 20
        direction_change = - direction_change;
    }
    else if(rotation_parameter2<=-20){//direction change when we go below -20
        direction_change = - direction_change;
    }

    if(animation_parameter_moving>=7){//to stop cone and cube from moving off track and behind the sphere
        animation_parameter_moving = 0;
        animation_parameter_making_cone = 1;
        animation_parameter_making_cube = 1; 
        number_of_cones += 1; 
    }
    
    glutPostRedisplay();
    if(animation_ongoing){
        glutTimerFunc(TIMER_ID1, on_timer1, TIMER_ID1);
    }
}
//function to move to the left, decreasing x coordinate
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
//function to move to the right, by increasing x coordinate
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
        case 2://red, for tracks
            diffuse_coeffs[0] = 1;
            break;
        case 3:if(flag_color == 1){//if cone is in the same track as sphere we change sphere color, to alarm the player
                diffuse_coeffs[1] = diffuse_coeffs[1] - animation_parameter_moving/15;
                diffuse_coeffs[0] = diffuse_coeffs[0] + animation_parameter_moving/15;//to go to purple color
            }
            else if(flag_color == 2){//if cube is in the same track as sphere we change sphere color, to notify the player
                diffuse_coeffs[0] = diffuse_coeffs[0] + animation_parameter_moving/10;//to go to yellow color
                diffuse_coeffs[1] = diffuse_coeffs[1] + animation_parameter_moving/10;
            }else{//green, for sphere, start color
                diffuse_coeffs[1] = 1;
            }
            break;
        case 4://purple, for cones
            diffuse_coeffs[0] = 1;
            diffuse_coeffs[2] = 1;
            break;
        case 5://yellow, for rating stars and cubes
            diffuse_coeffs[0] = 1;
            diffuse_coeffs[1] = 1;
            diffuse_coeffs[2] = 0;
            break;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}
//creation and translation of sphere
static void drawSphere(){
    glPushMatrix();
        set_material(3);
        glTranslatef(0+curr_x, 1.75, 3.00);
        glutSolidSphere(0.5, 50, 50);
    glPopMatrix();
    
}

static void drawBase(){
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
}
static void drawCone(){
    glPushMatrix();
        set_material(4);
        if(animation_ongoing){//when animation is active we make cones
            if(animation_parameter_making_cone){//when one cone is gone we make another one on random track using rand() function
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
                animation_parameter_making_cone = 0;
            }
            glTranslatef(x_coor_cone, 1.25, -4.5+animation_parameter_moving);//increasing z-ose
            glutSolidCone(0.1,0.5,20,20);
            flag_color = 0;
            if(abs(x_coor_cone - curr_x)<=0.5)
                flag_color = 1;
            //stop of animation when sphere and cone come to contant
            if(abs(x_coor_cone - curr_x)<=0.5 && animation_parameter_moving >= 6.0){
                animation_ongoing = 0;
                game_over = 1;
            }
        }
    glPopMatrix();
}
static void drawCube(){
    glPushMatrix();
        set_material(5);
        if(animation_ongoing){//when animation is active we make cube
            if(animation_parameter_making_cube){//when one cube is gone we make another one on random track using rand() function
                int same_x_coor_as_cone = 1; 
                while(same_x_coor_as_cone){
                    float rand_value = rand()/(float)RAND_MAX;//value is in [0,1] interval
                    if(rand_value<=0.33){
                        x_coor_cube = -2;
                    }
                    else if(rand_value>=0.66){
                        x_coor_cube = 2;
                    }
                    else{
                        x_coor_cube = 0;
                    }
                    if(x_coor_cone != x_coor_cube){//for cube and cone, we want for them to be on different tracks, so we use 'while' loop until coordinates are different
                        same_x_coor_as_cone = 0;
                    }
                }
                animation_parameter_making_cube = 0;
            }
            glTranslatef(x_coor_cube, 1.25, -4.5+animation_parameter_moving);//increasing z-ose
            glutSolidCube(0.2);
            if(abs(x_coor_cube - curr_x)<=0.5)
                flag_color = 2;
            //when sphere make contact with cube we increase number of cube as points
            if(abs(x_coor_cube - curr_x)<=0.5 && (animation_parameter_moving > 6.8)){
                number_of_cube+=1;
            }
        }
    glPopMatrix();
}
static void drawLosingMessage(){
    set_material(2);//set material for lines
    glLineWidth(12);
    glBegin(GL_LINES);//making letters, we are not using glutBitmapCharacter function because we want bigger font
        // letter 'Y'
        glVertex3f(-4.80f, 4.00f, 0.00f);
        glVertex3f(-4.00f, 3.00f, 0.00f);
        
        glVertex3f(-4.00f, 3.00f, 0.00f);
        glVertex3f(-3.20f, 4.00f, 0.00f);
    
        glVertex3f(-4.00f, 3.00, 0.00f);
        glVertex3f(-4.00f, 2.00f,0.00f);
        //'O'
        glVertex3f(-3.00f, 4.00f, 0.00f);
        glVertex3f(-3.00f, 2.00f, 0.00f);
        
        glVertex3f(-3.00f, 2.080f, 0.00f);
        glVertex3f(-2.00f, 2.080f, 0.00f);
    
        glVertex3f(-2.00f, 2.00, 0.00f);
        glVertex3f(-2.00f, 4.00f,0.00f);
        
        glVertex3f(-2.00f, 3.930, 0.00f);
        glVertex3f(-3.00f, 3.930f,0.00f);
        //'U'
        glVertex3f(-1.70f, 4.00f, 0.00f);
        glVertex3f(-1.70f, 2.00f, 0.00f);
    
        glVertex3f(-1.70f, 2.080, 0.00f);
        glVertex3f(-0.70f, 2.080f,0.00f);
        
        glVertex3f(-0.70f, 2.00, 0.00f);
        glVertex3f(-0.70f, 4.00f,0.00f);
        //'L'
        glVertex3f(0.00f, 2.00f, 0.00f);
        glVertex3f(0.00f, 4.00f, 0.00f);
        
        glVertex3f(0.00f, 2.080, 0.00f);
        glVertex3f(1.00f, 2.080f,0.00f);
        //'O'
        glVertex3f(1.20f, 2.080f, 0.00f);
        glVertex3f(2.20f, 2.080f, 0.00f);
        
        glVertex3f(2.20f, 2.00f, 0.00f);
        glVertex3f(2.20f, 4.00f, 0.00f);
    
        glVertex3f(2.20f, 3.93f, 0.00f);
        glVertex3f(1.20f, 3.93f,0.00f);
        
        glVertex3f(1.20f, 4.00f, 0.00f);
        glVertex3f(1.20f, 2.00f,0.00f);
        //'S'
        glVertex3f(2.40f, 2.080f, 0.00f);
        glVertex3f(3.40f, 2.080f, 0.00f);
        
        glVertex3f(3.33f, 2.00f, 0.00f);
        glVertex3f(3.33f, 3.00f, 0.00f);
    
        glVertex3f(3.40f, 3.00f, 0.00f);
        glVertex3f(2.40f, 3.00f,0.00f);
        
        glVertex3f(2.48f, 3.00f, 0.00f);
        glVertex3f(2.48f, 4.00f,0.00f);
        
        glVertex3f(2.40f, 3.93f, 0.00f);
        glVertex3f(3.40f, 3.93f,0.00f);
        //'T'
        glVertex3f(4.30f, 2.00f, 0.00f);
        glVertex3f(4.30f, 4.00f,0.00f);
        
        glVertex3f(3.50f, 3.93f, 0.00f);
        glVertex3f(5.10f, 3.93f,0.00f);
    glEnd();
}
static void drawStars(){
    glPushMatrix();
    set_material(5);    
    if(number_of_cube > 3){//showing one star if number of cones is higher then 3
        glBegin (GL_POLYGON);//making of pentagon for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00,-1.00f,0.00f);
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(1.20f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.90f,-3.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.90f,-3.50f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-1.20f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
    }
    if(number_of_cube > 10){//showing two stars if number of cones is higher then 10
        glTranslatef(-2.5, 0, 0);
            glBegin (GL_POLYGON);//making of pentagon for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00,-1.00f,0.00f);
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(1.20f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.90f,-3.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.90f,-3.50f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-1.20f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
    }
    if(number_of_cube > 20){//showing three stars if number of cones is higher then 20
        glTranslatef(5, 0, 0);
            glBegin (GL_POLYGON);//making of pentagon for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00,-1.00f,0.00f);
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.30f,-2.00f,0.00f);
            glVertex3f(1.20f,-2.00f,0.00f);
            glVertex3f(0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.50f,-2.50f,0.00f);
            glVertex3f(0.90f,-3.50f,0.00f);
            glVertex3f(0.00f,-2.80f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(0.00f,-2.80f,0.00f);
            glVertex3f(-0.90f,-3.50f,0.00f);
            glVertex3f(-0.50f,-2.50f,0.00f);
        glEnd();
        glBegin(GL_POLYGON);//making of triangle for star
            glVertex3f(-0.50f,-2.50f,0.00f);
            glVertex3f(-1.20f,-2.00f,0.00f);
            glVertex3f(-0.30f,-2.00f,0.00f);
        glEnd();
    }
    glPopMatrix();
}
//function that will print number of points won in the game
static void printPoints(float x, float y, float z){
    
    glPushMatrix();
	glDisable(GL_LIGHTING);//we disable lighting to set color for letters
	glColor3f(1.0, 1.0, 1.0);
        char *c, text[20];
        sprintf(text, "Points: %d", number_of_cube);
        glRasterPos3f(x, y, z);
        for (c=text; *c != '\0'; c++) 
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        glEnable(GL_LIGHTING);
    glPopMatrix();
}
static void on_display(void)
{
    //delete previous contents of the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(!game_over){
        //define a viewing transformation
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 7, 7, 0, 0, 0, 0, 1, 0);
    
        //light positiongrafika
        GLfloat light_position[] = { 10, 10, 10, 0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        
        glPushMatrix();
            //rotation of scene to make game harder to play
            glRotatef(rotation_parameter1, 0, 1, 0);//y-axis
            glRotatef(rotation_parameter2, 1, 0, 0);//x-axis
        
            glPushMatrix();
                drawBase();
                drawSphere();
                drawCone();
                drawCube();
            glPopMatrix();
        glPopMatrix();
        printPoints(-3.5, 4.5, 0);
    }
    else{//this part of code is active if player lost the game, it will show : "YOU LOST" and one, two or three stars with number of points
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
    
        //light position
        GLfloat light_position[] = { 10, 10, 10, 0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        drawLosingMessage();
        drawStars();
        printPoints(-0.5, 0, 0);
    }
    glutSwapBuffers();
}