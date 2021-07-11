#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include <windows.h>
#include <GL\glut.h>

#define pi (2*acos(0.0))
#define square_side 120
#define circle_radius 75
#define bubble_radius 13
#define bubble_count 5


int drawgrid;
int drawaxes;

double bubble_positions[5][2];
bool bubbles[5];
bool inCircle[5];
double bubble_speeds[5][2];
double speed_factor;
double saved_speed_factor;

clock_t start,finish;


struct point
{
    double x,y,z;
};



double fRand()
{
    double f = (double)rand() / RAND_MAX;
    return 0.0 + f * (1.0 - 0.0);
}

void drawAxes()
{
    if(drawaxes==1)
    {
        glBegin(GL_LINES);
        {
            glColor3f(1.0, 0, 0);
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glColor3f(0, 1.0, 0);
            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}



void drawCircle(double radius,int segments,bool bubble)
{
    int i;
    struct point points[100];
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0; i<segments; i++)
    {
        glBegin(GL_LINES);
        {
            if(!bubble)
                glColor3f(1,0,0);
            else
                glColor3f(0.8,0.5,0.1);
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}




void drawSS()
{

    glBegin(GL_LINES);
    {
        glColor3f(0, 1.0, 0);
        glVertex3f( square_side,square_side,0);
        glVertex3f(-square_side,square_side,0);

        glColor3f(0, 1.0, 0);
        glVertex3f( -square_side,square_side,0);
        glVertex3f(-square_side,-square_side,0);

        glColor3f(0, 1.0, 0);
        glVertex3f( -square_side,-square_side,0);
        glVertex3f(square_side,-square_side,0);

        glColor3f(0, 1.0, 0);
        glVertex3f( square_side,-square_side,0);
        glVertex3f(square_side,square_side,0);


    }
    glEnd();

    drawCircle(circle_radius,60,false);

    for(int i=0; i<bubble_count; i++)
    {
        finish=clock();
        if(double(finish - start) / double(CLOCKS_PER_SEC)>(i+1)*3)
        {
            //bubble initialization
            bubbles[i]=true;
            //bubble draw
            glPushMatrix();
            glTranslatef(bubble_positions[i][0],bubble_positions[i][1],0);
            drawCircle(bubble_radius,30,true);
            glPopMatrix();
        }

    }


}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case 'p':
        if(speed_factor==0.0)
            speed_factor=saved_speed_factor;
        else
        {
            saved_speed_factor=speed_factor;
            speed_factor=0.0;
        }
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        if(speed_factor>0.04)
            speed_factor -= 0.01;
        break;
    case GLUT_KEY_UP:
        if(speed_factor<1)		// up arrow key
            speed_factor += 0.01;
        break;

    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_LEFT:
        break;

    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        break;

    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}



void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    gluLookAt(0,0,200,	0,0,0,	0,1,0);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();
    drawGrid();

    drawSS();

    glutSwapBuffers();
}


void animate()
{
    for(int i=0; i<bubble_count; i++)
    {
        if(bubbles[i])
        {
            ////////////rectangle reflection
            if(sqrt(bubble_positions[i][0]*bubble_positions[i][0]+bubble_positions[i][1]*bubble_positions[i][1])
                    >circle_radius-bubble_radius)
            {
                if(bubble_positions[i][0]+bubble_speeds[i][0]*speed_factor<square_side-bubble_radius &&
                        bubble_positions[i][1]+bubble_speeds[i][1]*speed_factor<square_side-bubble_radius &&
                        bubble_positions[i][0]+bubble_speeds[i][0]*speed_factor>-1*(square_side-bubble_radius) &&
                        bubble_positions[i][1]+bubble_speeds[i][1]*speed_factor>-1*(square_side-bubble_radius))
                {
                    bubble_positions[i][0]+=bubble_speeds[i][0]*speed_factor;
                    bubble_positions[i][1]+=bubble_speeds[i][1]*speed_factor;
                }
                else if(bubble_positions[i][0]+bubble_speeds[i][0]*speed_factor>square_side-bubble_radius ||
                        bubble_positions[i][0]+bubble_speeds[i][0]*speed_factor<-1*(square_side-bubble_radius))
                {
                    bubble_speeds[i][0]*=(-1);
                }
                else if(bubble_positions[i][1]+bubble_speeds[i][1]*speed_factor>square_side-bubble_radius ||
                        bubble_positions[i][1]+bubble_speeds[i][1]*speed_factor<-1*(square_side-bubble_radius))
                {
                    bubble_speeds[i][1]*=(-1);
                }
            }
            /////////circle reflection(       r=d−2(d⋅n)n      )
            else
            {
                inCircle[i]=true;
                double temX=bubble_positions[i][0]+bubble_speeds[i][0]*speed_factor;
                double temY=bubble_positions[i][1]+bubble_speeds[i][1]*speed_factor;

                if(sqrt(temX*temX+temY*temY)<circle_radius-bubble_radius)
                {
                    int j;
                    for(j=0; j<bubble_count; j++)
                    {
                        if(j==i)
                        {
                            continue;
                        }
                        double disX=temX-bubble_positions[j][0];
                        double disY=temY-bubble_positions[j][1];

                        if(sqrt(disX*disX+disY*disY)<2*bubble_radius && inCircle[j])
                        {

                            double cendx=bubble_positions[i][0]-bubble_positions[j][0];
                            double cendy=bubble_positions[i][1]-bubble_positions[j][1];

                            double normalVal=sqrt(cendx*cendx+cendy*cendy);

                            double nbx=cendx/normalVal;
                            double nby=cendy/normalVal;
                            double dbx=temX-bubble_positions[i][0];
                            double dby=temY-bubble_positions[i][1];

                            double rbx=dbx-2*(dbx*nbx+dby*nby)*nbx;
                            double rby=dby-2*(dbx*nbx+dby*nby)*nby;

                            bubble_speeds[i][0]=rbx/speed_factor;
                            bubble_speeds[i][1]=rby/speed_factor;

                            bubble_positions[i][0]+=bubble_speeds[i][0]*speed_factor;
                            bubble_positions[i][1]+=bubble_speeds[i][1]*speed_factor;



                            break;
                        }
                    }

                    if(j==bubble_count)
                    {
                        bubble_positions[i][0]=temX;
                        bubble_positions[i][1]=temY;
                    }

                }
                else
                {
                    double normalVal=sqrt(bubble_positions[i][0]*bubble_positions[i][0]+
                                          bubble_positions[i][1]*bubble_positions[i][1]);
                    double nx=bubble_positions[i][0]/normalVal;
                    double ny=bubble_positions[i][1]/normalVal;
                    double dx=temX-bubble_positions[i][0];
                    double dy=temY-bubble_positions[i][1];

                    double rx=dx-2*(dx*nx+dy*ny)*nx;
                    double ry=dy-2*(dx*nx+dy*ny)*ny;

                    bubble_speeds[i][0]=rx/speed_factor;
                    bubble_speeds[i][1]=ry/speed_factor;

                    bubble_positions[i][0]+=bubble_speeds[i][0]*speed_factor;
                    bubble_positions[i][1]+=bubble_speeds[i][1]*speed_factor;
                }
            }
        }
    }
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=0;
    drawaxes=0;


    /****************
    /bubble init
    ***************/
    start=clock();

    for(int i=0; i<bubble_count; i++)
    {
        double d=fRand();
        bubble_positions[i][0]=-(square_side-bubble_radius);
        bubble_positions[i][1]=-(square_side-bubble_radius);
        bubbles[i]=false;
        inCircle[i]=false;
        bubble_speeds[i][0]=fRand();
        bubble_speeds[i][1]=fRand();

        printf("%f %f\n",bubble_speeds[i][0],bubble_speeds[i][1]);
    }

    speed_factor=0.1;

    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}
