#include<bits/stdc++.h>

#include <windows.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"
#include "1605039_Header.h"

#define pi (2*acos(0.0))
#define mov_amount 2
#define rot_amount 0.042
#define stack_amount 90
#define slice_amount 90

using namespace std;

string currDirLoc;
string inputFileName="input.txt";
int noOfCaptures=1;

int drawaxes=1;
int level_recursion,imageWidth;
int viewAngle=80;


int objectCount,lightCount;
vector <Object*> objects;
vector <Light*> lights;


Point cameraPos;
Point l;
Point u;
Point r;

void drawAxes()
{
    if(drawaxes==1)
    {

        glBegin(GL_LINES);
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f( 500,0,0);
            glVertex3f(-500,0,0);

            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(0,-500,0);
            glVertex3f(0, 500,0);

            glColor3f(0.0, 0.0, 1.0);
            glVertex3f(0,0, 200);
            glVertex3f(0,0,-200);
        }
        glEnd();
    }
}


void drawSS()
{
    for(int i=0; i<objects.size(); i++)
    {
        glPushMatrix();
        objects[i]->draw();
        glPopMatrix();
    }
    for(int i=0; i<lights.size(); i++)
    {
        glPushMatrix();
        lights[i]->draw();
        glPopMatrix();
    }
}


void capture()
{
    cout<<"Image capture no#"<<noOfCaptures<<" has started."<<endl;
    bitmap_image image(imageWidth,imageWidth);
    for(int i=0;i<imageWidth;i++){
        for(int j=0;j<imageWidth;j++){
            image.set_pixel(i,j,0,0,0);
        }
    }

    double planeDistance=(windowWidth/2.0)/tan(viewAngle/2.0*(pi/180.0));

    Point topLeft=cameraPos+(l*planeDistance)-(r*(windowWidth/2.0))+(u*(windowWidth/2.0));
    //topLeft.printPoint();

    double du=windowWidth*1.0/imageWidth;
    double dv=du;
    // Choose middle of the grid cell
    topLeft=topLeft+(r*(0.5*du))-(u*(0.5*dv));
    //topLeft.printPoint();

    int nearest;
    double t, tMin;
    for(int i=0;i<imageWidth;i++)
    {
        for(int j=0;j<imageWidth;j++)
        {
            nearest=-1;
            tMin=999999;
            Point currPixel=topLeft+(r*(i*du))-(u*(j*dv));

            Point currPixelEyeDir=currPixel-cameraPos;
            currPixelEyeDir.normalize();
            //cout<<"normalized z : "<<currPixel.z<<endl;
            Ray* eyePixelRay=new Ray(cameraPos,currPixelEyeDir);
            //eyePixelRay->dir.printPoint();
            double* color;
            for(int k=0;k<objects.size();k++)
            {
                t=objects[k]->intersectParamT(eyePixelRay);
                if(t>0 && t<tMin)
                {
                    tMin=t;
                    nearest=k;
                }
            }
            //cout<<"value of tMin = "<<tMin<<endl;
            if(nearest>=0)
            {
                color=objects[nearest]->intersect(eyePixelRay,1);
                image.set_pixel(i,j,color[0]*255,color[1]*255,color[2]*255);
            }
            delete[] color;
            delete eyePixelRay;

        }
        //cout<<"value of i = "<<i<<endl;
    }
    cout<<"Image is saved as out_"<<noOfCaptures<<".bmp"<<endl;
    image.save_image( currDirLoc + "out_" + to_string(noOfCaptures++) + ".bmp");



}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {


    case '1':
        l.x=l.x*cos(-1*rot_amount) + r.x*sin(-1*rot_amount);
        l.y=l.y*cos(-1*rot_amount) + r.y*sin(-1*rot_amount);
        l.z=l.z*cos(-1*rot_amount) + r.z*sin(-1*rot_amount);
        r=l.crossProduct(u);
        break;

    case '2':
        l.x=l.x*cos(rot_amount) + r.x*sin(rot_amount);
        l.y=l.y*cos(rot_amount) + r.y*sin(rot_amount);
        l.z=l.z*cos(rot_amount) + r.z*sin(rot_amount);
        r=l.crossProduct(u);
        break;

    case '3':
        l.x=l.x*cos(rot_amount) + u.x*sin(rot_amount);
        l.y=l.y*cos(rot_amount) + u.y*sin(rot_amount);
        l.z=l.z*cos(rot_amount) + u.z*sin(rot_amount);
        u=r.crossProduct(l);
        break;

    case '4':
        l.x=l.x*cos(-1*rot_amount) + u.x*sin(-1*rot_amount);
        l.y=l.y*cos(-1*rot_amount) + u.y*sin(-1*rot_amount);
        l.z=l.z*cos(-1*rot_amount) + u.z*sin(-1*rot_amount);
        u=r.crossProduct(l);
        break;

    case '5':
        u.x=u.x*cos(-1*rot_amount) + r.x*sin(-1*rot_amount);
        u.y=u.y*cos(-1*rot_amount) + r.y*sin(-1*rot_amount);
        u.z=u.z*cos(-1*rot_amount) + r.z*sin(-1*rot_amount);
        r=l.crossProduct(u);
        break;

    case '6':
        u.x=u.x*cos(rot_amount) + r.x*sin(rot_amount);
        u.y=u.y*cos(rot_amount) + r.y*sin(rot_amount);
        u.z=u.z*cos(rot_amount) + r.z*sin(rot_amount);
        r=l.crossProduct(u);
        break;

    case '0':
        capture();

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        cameraPos.x += mov_amount;
        break;
    case GLUT_KEY_UP:		// up arrow key
        cameraPos.x -= mov_amount;
        break;

    case GLUT_KEY_RIGHT:
        //cameraAngle += 0.03;
        cameraPos.y+=mov_amount;
        break;
    case GLUT_KEY_LEFT:
        //cameraAngle -= 0.03;
        cameraPos.y-=mov_amount;
        break;

    case GLUT_KEY_PAGE_UP:
        cameraPos.z+=mov_amount;
        break;
    case GLUT_KEY_PAGE_DOWN:
        cameraPos.z-=mov_amount;
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
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?
    gluLookAt(cameraPos.x,cameraPos.y,cameraPos.z,	cameraPos.x+l.x,cameraPos.y+l.y,cameraPos.z+l.z,	u.x,u.y,u.z);
    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);
    /****************************
    / Add your objects from here
    ****************************/
    drawAxes();
    drawSS();

    glutSwapBuffers();
}


string getCurrDirectory()
{
    stringstream ss(__FILE__);
    string segment;
    vector<string> seglist;
    while(getline(ss, segment, '\\'))
    {
        seglist.push_back(segment);
    }
    string str="";
    for(int i=0; i<seglist.size()-1; i++)
        str+=seglist[i]+"\\\\";
    return str;
}

void loadData()
{
    currDirLoc=getCurrDirectory();
    string currDir=currDirLoc + inputFileName;
    ifstream inputFile;
    inputFile.open(currDir.c_str());
    if (!inputFile.is_open())
    {
        cout<<"Input File not found"<<endl;
    }
    inputFile>>level_recursion>>imageWidth>>objectCount;
    //level_recursion=1;
    //cout<<level_recursion<<endl;
    //cout<<imageWidth<<endl<<endl<<objectCount<<endl;

    string obj;
    double a,b,c,d,arr[4];
    int s;
    for(int i=0; i<objectCount; i++)
    {
        inputFile>>obj;
        //cout<<obj<<endl;
        if(obj=="triangle")
        {
            inputFile>>a>>b>>c;
            Point p1(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            inputFile>>a>>b>>c;
            Point p2(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            inputFile>>a>>b>>c;
            Point p3(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            Object* triangle=new Triangle(p1,p2,p3);
            inputFile>>a>>b>>c;
            triangle->setColor(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            for(int j=0; j<4; j++)
            {
                inputFile>>arr[j];
                //cout<<arr[i] << " ";
            }
            //cout<<endl;
            triangle->setCoEfficients(arr);
            //for(int j=0;j<4;j++) cout<<arr[j]<<" ";
            //cout<<endl;
            inputFile>>s;
            //cout<<s<<endl;
            triangle->setShine(s);
            //cout<<endl;
            objects.push_back(triangle);

        }
        else if(obj=="sphere")
        {
            inputFile>>a>>b>>c>>d;
            Point p1(a,b,c);
            Object* sphere=new Sphere(p1,d);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            //cout<<d<<endl;
            inputFile>>a>>b>>c;
            sphere->setColor(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            for(int j=0;j<4;j++){
                inputFile>>arr[j];
                //cout<<arr[i] << " ";
            }
            //cout<<endl;
            sphere->setCoEfficients(arr);
            //for(int j=0;j<4;j++) cout<<arr[j]<<" ";
            //cout<<endl;
            inputFile>>s;
            //cout<<s<<endl;
            sphere->setShine(s);
            //cout<<endl;
            objects.push_back(sphere);
        }
        else if(obj=="general")
        {
            double tem[10];
            for(int j=0;j<10;j++)
                inputFile>>tem[j];
            //for(int j=0;j<10;j++) cout<<tem[j]<<" ";
            //cout<<endl;
            inputFile>>a>>b>>c;
            //cout<<a<<" "<<b<<" "<<c<<" ";
            Point p1(a,b,c);
            inputFile>>a>>b>>c;
            //cout<<a<<" "<<b<<" "<<c<<endl;
            Object* general=new General(tem,p1,a,b,c);
            //cout<<general->height<<" this is height"<<endl;
            inputFile>>a>>b>>c;
            general->setColor(a,b,c);
            //cout<<a<<" "<<b<<" "<<c<<endl;
            for(int j=0;j<4;j++){
                inputFile>>arr[j];
                //cout<<arr[i] << " ";
            }
            //cout<<endl;
            general->setCoEfficients(arr);
            //for(int j=0;j<4;j++) cout<<arr[j]<<" ";
            //cout<<endl;
            inputFile>>s;
            //cout<<s<<endl;
            general->setShine(s);
            //cout<<endl;
            objects.push_back(general);

        }
    }
    Object* floor=new Floor(floorWidth,tileWidth);
    double tem[4]={0.4, 0.2, 0.1, 0.3};
    floor->setCoEfficients(tem);
    floor->setShine(5);
    objects.push_back(floor);

    inputFile>>lightCount;
    //cout<<lightCount<<endl;
    for(int i=0; i<lightCount; i++)
    {
        inputFile>>a>>b>>c;
        Point p(a,b,c);
        Light* light=new Light(p);
        //cout<<a<<" "<<b<<" "<<c<<endl;
        inputFile>>a>>b>>c;
        light->setColor(a,b,c);
        //cout<<a<<" "<<b<<" "<<c<<endl;
        lights.push_back(light);
    }

    inputFile.close();

}

void animate()
{
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //clear the screen
    glClearColor(0,0,0,0);
    /******************
    / initialize pos and direction of camera
    **********************/
    //0,0,200,	0,0,0,	0,1,0
    cameraPos.x=100;
    cameraPos.y=100;
    cameraPos.z=0;
    l.x=-1/sqrt(2);
    l.y=-1/sqrt(2);
    l.z=0;
    u.x=0;
    u.y=0;
    u.z=1;
    r.x=-1/sqrt(2);
    r.y=1/sqrt(2);
    r.z=0;

    loadData();

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);
    //initialize the matrix
    glLoadIdentity();
    //give PERSPECTIVE parameters
    gluPerspective(viewAngle,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance,far distance
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
