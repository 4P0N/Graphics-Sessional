#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define mov_amount 2
#define rot_amount 0.042
#define stack_amount 90
#define slice_amount 90

#define gun_rot_amount 2
#define cutoff_rot 45
#define bullet_wall 180
#define big_sphere_radius 20


int drawgrid;
int drawaxes;

/*************
gun params
**************/
double x_rotation=1.0;
double y_rotation=0.0;
double z_rotation=0.0;
double xs_rotation=0.0;

bool as;

/***********
fire params
***********/
double z_fire=299;
int bullet_cnt=0;
std::vector<bool> bullets;
std::vector<std::pair<double,double> > bullets_pos;


struct point
{
    double x,y,z;
};

struct point pos;
struct point l;
struct point u;
struct point r;
struct point cylinder_bottom;
struct point cylinder_front;

struct point crossProduct(struct point a, struct point b)
{
    struct point result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;

    return result;
}

void drawAxes()
{
    if(drawaxes==1)
    {

        glBegin(GL_LINES);
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f( 200,0,0);
            glVertex3f(-200,0,0);

            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(0,-200,0);
            glVertex3f(0, 200,0);

            glColor3f(0.0, 0.0, 1.0);
            glVertex3f(0,0, 200);
            glVertex3f(0,0,-200);
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

void drawSquare(double a,bool bullet)
{
    if(!bullet)
        glColor3f(0.7,0.7,0.7);
    else
        glColor3f(1,0,0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
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
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0; i<segments; i++)
    {
        //create shading effect
        if(i<segments/2)
            shade=2*(double)i/(double)segments;
        else
            shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks,bool semisphere,bool color)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                if(color)
                {
                    if(j%2==0)
                        glColor3f(1,1,1);
                    else
                        glColor3f(0,0,0);
                }
                else
                {
                    if(j%2==1)
                        glColor3f(1,1,1);
                    else
                        glColor3f(0,0,0);
                }


                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                if(!semisphere)
                {
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                }
            }
            glEnd();
        }
    }
}

void drawCylinder(double radius,double height,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=height*sin(((double)i/(double)stacks)*(pi/2));
        r=radius;
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                if(j%2==1)
                    glColor3f(1,1,1);
                else
                    glColor3f(0,0,0);

                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }
            glEnd();
        }
    }
}


void drawWeirdShape(double radius,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        r=2*radius-r;
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                if(j%2==0)
                    glColor3f(1,1,1);
                else
                    glColor3f(0,0,0);

                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }
            glEnd();
        }
    }
}


void drawSS()
{

    glPushMatrix();

    glRotatef(90,1,0,0);
    glRotatef(z_rotation,0,1,0);
    drawSphere(big_sphere_radius,slice_amount,stack_amount,true,false);

    glTranslatef(0,0,0);
    glRotatef(180,1,0,0);
    glRotatef(xs_rotation,1,0,0);
    drawSphere(big_sphere_radius,slice_amount,stack_amount,true,true);


    glRotatef(180,1,0,0);
    glTranslatef(0,0,-30);
    glRotatef(x_rotation,1,0,0);
    glRotatef(y_rotation,0,0,1);
    drawSphere(10,slice_amount,stack_amount,true,false);


    glTranslatef(0,0,-50);
    drawCylinder(10,50,slice_amount,stack_amount);


    glRotatef(180,1,0,0);
    drawWeirdShape(10,slice_amount,stack_amount);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,300,0);

    glRotatef(90,1,0,0);
    drawSquare(bullet_wall,false);

    glPopMatrix();


    glPushMatrix();

//    glBegin(GL_LINES);
//    {
//        glColor3f(1,1,0);
//        glVertex3f(cylinder_bottom.x,cylinder_bottom.y,cylinder_bottom.z);
//        glVertex3f(cylinder_front.x,cylinder_front.y,cylinder_front.z);
//    }
//    glEnd();

    glPopMatrix();

    for(int i=0; i<bullets.size(); i++)
    {
        if(bullets[i])
        {
            glPushMatrix();
            glTranslatef(bullets_pos[i].first,z_fire,bullets_pos[i].second);
            glRotatef(90,1,0,0);
            drawSquare(3,true);
            glPopMatrix();
        }

    }

}

void shootBullet()
{
    double t=(cylinder_bottom.y - 300)/((cylinder_front.y-cylinder_bottom.y));
    double eqx=cylinder_bottom.x-(cylinder_front.x-cylinder_bottom.x)*t;
    double eqz=cylinder_bottom.z-(cylinder_front.z-cylinder_bottom.z)*t;

    if(eqx<bullet_wall && eqx>-1*bullet_wall && eqz<bullet_wall && eqz>-1*bullet_wall)
    {
        bullets[bullet_cnt]=true;
        bullets_pos.push_back(std::make_pair(eqx,eqz));
        bullet_cnt++;
        bullets.push_back(false);
    }

}


void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {


    case '1':
        l.x=l.x*cos(-1*rot_amount) + r.x*sin(-1*rot_amount);
        l.y=l.y*cos(-1*rot_amount) + r.y*sin(-1*rot_amount);
        l.z=l.z*cos(-1*rot_amount) + r.z*sin(-1*rot_amount);
        r=crossProduct(l,u);
        break;

    case '2':
        l.x=l.x*cos(rot_amount) + r.x*sin(rot_amount);
        l.y=l.y*cos(rot_amount) + r.y*sin(rot_amount);
        l.z=l.z*cos(rot_amount) + r.z*sin(rot_amount);
        r=crossProduct(l,u);
        break;

    case '3':
        l.x=l.x*cos(rot_amount) + u.x*sin(rot_amount);
        l.y=l.y*cos(rot_amount) + u.y*sin(rot_amount);
        l.z=l.z*cos(rot_amount) + u.z*sin(rot_amount);
        u=crossProduct(r,l);
        break;

    case '4':
        l.x=l.x*cos(-1*rot_amount) + u.x*sin(-1*rot_amount);
        l.y=l.y*cos(-1*rot_amount) + u.y*sin(-1*rot_amount);
        l.z=l.z*cos(-1*rot_amount) + u.z*sin(-1*rot_amount);
        u=crossProduct(r,l);
        break;

    case '5':
        u.x=u.x*cos(-1*rot_amount) + r.x*sin(-1*rot_amount);
        u.y=u.y*cos(-1*rot_amount) + r.y*sin(-1*rot_amount);
        u.z=u.z*cos(-1*rot_amount) + r.z*sin(-1*rot_amount);
        r=crossProduct(l,u);
        break;

    case '6':
        u.x=u.x*cos(rot_amount) + r.x*sin(rot_amount);
        u.y=u.y*cos(rot_amount) + r.y*sin(rot_amount);
        u.z=u.z*cos(rot_amount) + r.z*sin(rot_amount);
        r=crossProduct(l,u);
        break;

    case 'q':
        if(z_rotation+gun_rot_amount<cutoff_rot)
        {
            z_rotation+=gun_rot_amount;
            cylinder_bottom.x=-1*big_sphere_radius*sin(z_rotation*pi/180);
            cylinder_bottom.y=big_sphere_radius*cos(z_rotation*pi/180);
            cylinder_front.x=-400*sin(z_rotation*pi/180);
            cylinder_front.y=400*cos(z_rotation*pi/180);
        }
        break;

    case 'w':
        if(z_rotation-gun_rot_amount>-1*cutoff_rot)
        {
            z_rotation-=gun_rot_amount;
            cylinder_bottom.x=-1*big_sphere_radius*sin(z_rotation*pi/180);
            cylinder_bottom.y=big_sphere_radius*cos(z_rotation*pi/180);
            cylinder_front.x=-400*sin(z_rotation*pi/180);
            cylinder_front.y=400*cos(z_rotation*pi/180);
        }

        break;

    case 'a':
        if(x_rotation+gun_rot_amount<cutoff_rot)
        {
            x_rotation+=gun_rot_amount;
            cylinder_front.z=400*sin((xs_rotation+x_rotation)*pi/180);
            cylinder_front.y=400*cos((xs_rotation+x_rotation)*pi/180);
        }
        break;

    case 's':
        if(x_rotation-gun_rot_amount>-1*cutoff_rot)
        {
            x_rotation-=gun_rot_amount;
            cylinder_front.z=400*sin((x_rotation+xs_rotation)*pi/180);
            cylinder_front.y=400*cos((x_rotation+xs_rotation)*pi/180);
        }
        break;

    case 'd':
        if(y_rotation+gun_rot_amount<cutoff_rot)
            y_rotation+=gun_rot_amount;
        break;

    case 'f':
        if(y_rotation-gun_rot_amount>-1*cutoff_rot)
            y_rotation-=gun_rot_amount;
        break;

    case 'e':
        if(xs_rotation+gun_rot_amount<cutoff_rot)
        {
            xs_rotation+=gun_rot_amount;
            cylinder_bottom.z=big_sphere_radius*sin(xs_rotation*pi/180);
            cylinder_bottom.y=big_sphere_radius*cos(xs_rotation*pi/180);
            cylinder_front.z=400*sin((xs_rotation+x_rotation)*pi/180);
            cylinder_front.y=400*cos((xs_rotation+x_rotation)*pi/180);
        }
        break;

    case 'r':
        if(xs_rotation-gun_rot_amount>-1*cutoff_rot)
        {
            xs_rotation-=gun_rot_amount;
            cylinder_bottom.z=big_sphere_radius*sin(xs_rotation*pi/180);
            cylinder_bottom.y=big_sphere_radius*cos(xs_rotation*pi/180);
            cylinder_front.z=400*sin((xs_rotation+x_rotation)*pi/180);
            cylinder_front.y=400*cos((xs_rotation+x_rotation)*pi/180);
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
        pos.x += mov_amount;
        break;
    case GLUT_KEY_UP:		// up arrow key
        pos.x -= mov_amount;
        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 0.03;
        pos.y+=mov_amount;
        break;
    case GLUT_KEY_LEFT:
        //cameraAngle -= 0.03;
        pos.y-=mov_amount;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.z+=mov_amount;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.z-=mov_amount;
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
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            shootBullet();
        }
        break;

    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
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

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    gluLookAt(pos.x,pos.y,pos.z,	pos.x+l.x,pos.y+l.y,pos.z+l.z,	u.x,u.y,u.z);


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
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=0;
    drawaxes=1;

    //clear the screen
    glClearColor(0,0,0,0);

    /******************
    / initialize pos and direction of camera
    **********************/
    //0,0,200,	0,0,0,	0,1,0
    pos.x=100;
    pos.y=100;
    pos.z=0;
    l.x=-1/sqrt(2);
    l.y=-1/sqrt(2);
    l.z=0;
    u.x=0;
    u.y=0;
    u.z=1;
    r.x=-1/sqrt(2);
    r.y=1/sqrt(2);
    r.z=0;

    as=false;

    cylinder_bottom.x=0.0;
    cylinder_bottom.y=big_sphere_radius;
    cylinder_bottom.z=0.0;

    cylinder_front.x=0.0;
    cylinder_front.y=400;
    cylinder_front.z=0.0;

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance


    bullets.push_back(false);
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(700, 700);
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
