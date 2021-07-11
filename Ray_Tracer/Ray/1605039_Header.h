#include<bits/stdc++.h>
using namespace std;

#define SLICES 90
#define STACKS 90

#define pi (2*acos(0.0))

int windowWidth=500;
int floorWidth=1000,tileWidth=20;

class Object;
class Light;

extern int level_recursion;
extern vector <Light*> lights;
extern vector <Object*> objects;


class Point
{
public:
    double x;
    double y;
    double z;

    Point(double a, double b, double c)
    {
        x=a;
        y=b;
        z=c;
    }
    Point()
    {
        x=0;
        y=0;
        z=0;
    }

    void normalize()
    {
        double val=sqrt(x*x+y*y+z*z);
        x=x/val;
        y=y/val;
        z=z/val;
    }
    double dotProduct(const Point &p)
    {
        return x*p.x+y*p.y+z*p.z;
    }
    Point crossProduct(const Point &p)
    {
        Point result;
        result.x = y * p.z - z * p.y;
        result.y = z * p.x - x * p.z;
        result.z = x * p.y - y * p.x;

        return result;
    }

    Point operator*(const double &a)
    {
        Point result;
        result.x=x*a;
        result.y=y*a;
        result.z=z*a;
        return result;
    }

    Point operator+(const Point &p)
    {
        Point result;
        result.x=x+p.x;
        result.y=y+p.y;
        result.z=z+p.z;
        return result;
    }

    Point operator-(const Point &p)
    {
        Point result;
        result.x=x-p.x;
        result.y=y-p.y;
        result.z=z-p.z;
        return result;
    }

    void printPoint()
    {
        cout<<"Printing a POINT---> "<<endl;
        cout<<x<<" "<<y<<" "<<z<<endl;
    }

};


class Ray
{
public:
    Point start;
    Point dir; // normalize for easier calculations

    Ray(Point a, Point b)
    {
        start=a;
        dir=b;
    }
};

class Light
{
public:
    Point light_pos;
    double color[3];
    Light(Point pos)
    {
        light_pos=pos;
    }
    void setColor(double x, double y, double z)
    {
        color[0]=x;
        color[1]=y;
        color[2]=z;
    }
    void draw()
    {
        glTranslatef(light_pos.x,light_pos.y,light_pos.z);
        Point points[100][100];
        int i,j;
        double h,r;
        //generate points
        for(i=0; i<=5; i++)
        {
            h=1.5*sin(((double)i/5)*(pi/2));
            r=1.5*cos(((double)i/5)*(pi/2));
            for(j=0; j<=5; j++)
            {
                points[i][j].x=r*cos(((double)j/5)*2*pi);
                points[i][j].y=r*sin(((double)j/5)*2*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0; i<5; i++)
        {
            glColor3f(color[0], color[1], color[2]);
            for(j=0; j<5; j++)
            {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);

                }
                glEnd();
            }
        }
    }
};



bool isClipped(Point distance,double length,double width,double height)
{
    if ((length>0) && (abs(distance.x)>length))
        return true;
    if ((width>0) && (abs(distance.y)>width))
        return true;
    if ((height>0) && (abs(distance.z)>height))
        return true;

    return false;
}



class Object
{
public:
    Point reference_point; // should have x, y, z
    double height, width, length;
    double color[3];
    double coEfficients[4]; // reflection coefficients
    int shine; // exponent term of specular component
    object() {}
    virtual void draw() {}
    void setColor(double x, double y, double z)
    {
        color[0]=x;
        color[1]=y;
        color[2]=z;
    }
    virtual double* getColor(Point &p)
    {
        double* c=new double[3];
        for(int i=0; i<3; i++)
            c[i]=color[i];
        return c;
    }
    void setShine(int s)
    {
        shine=s;
    }
    void setCoEfficients(double arr[])
    {
        for(int i=0; i<4; i++)
            coEfficients[i]=arr[i];
    }

    virtual double intersectParamT(Ray *r)
    {
        return -1.0;
    }
    virtual Point getNormal(Point &p) {}

    double* intersect(Ray *r,int level)
    {
        double t=intersectParamT(r);
        //cout<<t<<endl;
        Point intersectionPoint=r->start+(r->dir*t);

        Point normal=getNormal(intersectionPoint);
//        if (normal.dotProduct(r->dir)>0)
//        {
//            normal=normal * -1.0;
//        }
        double* c=getColor(intersectionPoint);
        for(int i=0; i<3; i++)
        {
            c[i]*=coEfficients[0];
            if(c[i]<0)
                c[i]=0;
            else if(c[i]>1)
                c[i]=1;
        }

//        for(int i=0;i<3;i++) cout<<c[i]<<"  ";
//        cout<<endl;

        for(int i=0; i<lights.size(); i++)
        {
            Point rayDir=intersectionPoint-lights[i]->light_pos;
            rayDir.normalize();
            Ray* ray=new Ray(lights[i]->light_pos,rayDir);
//            ray->start.printPoint();
//            ray->dir.printPoint();

            bool shadowed=false;
            double T=(intersectionPoint.x-ray->start.x)/ray->dir.x;
            for(int j=0; j<objects.size(); j++)
            {
                double t=objects[j]->intersectParamT(ray);
                if(t>0 && floor(t)<floor(T))
                {
                    shadowed=true;
                    break;
                }
            }
            if(!shadowed)
            {
                double lambertValue=normal.dotProduct(ray->dir);
                //lambertValue=max(lambertValue,0.0);
                Point R=ray->dir-(normal*(2*lambertValue));
                double phongValue=R.dotProduct(r->dir);
                //phongValue=max(phongValue,0.0);
                double* tem=getColor(intersectionPoint);
                for(int j=0; j<3; j++)
                {
                    c[j]+=lights[i]->color[j]*coEfficients[1]*lambertValue*tem[j];
                    c[j]+=lights[i]->color[j]*coEfficients[2]*pow(abs(phongValue),shine);//*tem[j];
                    if(c[j]<0)
                        c[j]=0;
                    else if(c[j]>1)
                        c[j]=1;
                }
                delete[] tem;
            }
            delete ray;
        }


        if(level>=level_recursion)
            return c;

        Point reflectedDir=r->dir - normal*(2*r->dir.dotProduct(normal));
        reflectedDir.normalize();
        Ray* reflected=new Ray(intersectionPoint+reflectedDir*2,reflectedDir);

        int nearest=-1;
        double tMin=9999999;
        for(int i=0; i<objects.size(); i++)
        {
            double t=objects[i]->intersectParamT(reflected);
            if (t>0 && t<tMin)
            {
                nearest=i;
                tMin=t;
            }
        }
        if(nearest>=0)
        {
            double* tem=objects[nearest]->intersect(reflected,level+1);
            for(int i=0; i<3; i++)
            {
                c[i]+=tem[i]*coEfficients[3];
                if(c[i]<0)
                    c[i]=0;
                else if(c[i]>1)
                    c[i]=1;
            }
            delete[] tem;
        }
        delete reflected;

        return c;
    }
};

class Triangle: public Object
{
public:
    Point p1,p2,p3;

    Triangle(Point x,Point y,Point z)
    {
        p1=x;
        p2=y;
        p3=z;
    }

    void draw()
    {
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(p1.x,p1.y,p1.z);
            glVertex3f(p2.x,p2.y,p2.z);
            glVertex3f(p3.x,p3.y,p3.z);
        }
        glEnd();
    }

    double intersectParamT(Ray *r)
    {
        const double EPSILON = 0.0000001;
        Point edge1, edge2, h, s, q;
        double a,f,u,v;
        edge1=p2-p1;
        edge2=p3-p1;
        h=r->dir.crossProduct(edge2);
        a=edge1.dotProduct(h);
        if(a>-EPSILON && a<EPSILON)
            return -1;    // This ray is parallel to this triangle.
        f=1.0/a;
        s=r->start-p1;
        u = f * s.dotProduct(h);
        if (u < 0.0 || u > 1.0)
            return -1;
        q = s.crossProduct(edge1);
        v = f * r->dir.dotProduct(q);
        if (v < 0.0 || u + v > 1.0)
            return -1;
        // At this stage we can compute t to find out where the intersection point is on the line.
        double t=f*edge2.dotProduct(q);
        if(t<=EPSILON)
            return -1;

        return t;
    }

    Point getNormal(Point &p)
    {
        Point result;
        Point edge1=p2-p1;
        Point edge2=p3-p1;
        result=edge1.crossProduct(edge2);
        result.normalize();
        return result;
    }
};

class Sphere: public Object
{
public:
    Sphere(Point center, double radius)
    {
        reference_point=center;
        length=radius;
    }

    void draw()
    {
        glTranslatef(reference_point.x,reference_point.y,reference_point.z);
        Point points[100][100];
        int i,j;
        double h,r;
        //generate points
        for(i=0; i<=STACKS; i++)
        {
            h=length*sin(((double)i/(double)STACKS)*(pi/2));
            r=length*cos(((double)i/(double)STACKS)*(pi/2));
            for(j=0; j<=SLICES; j++)
            {
                points[i][j].x=r*cos(((double)j/(double)SLICES)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)SLICES)*2*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0; i<STACKS; i++)
        {
            glColor3f(color[0], color[1], color[2]);
            for(j=0; j<SLICES; j++)
            {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);

                }
                glEnd();
            }
        }
    }

    double intersectParamT(Ray *r)
    {
        Point rayCenter=r->start-reference_point;
        double a=r->dir.dotProduct(r->dir);
        double b=2.0*(rayCenter.dotProduct(r->dir));
        double c=rayCenter.dotProduct(rayCenter)-length*length;
        double discriminant=b*b-4*a*c;
        if(discriminant<0.0)
        {
            return -1.0;
        }
        else
        {
            double numerator=-b-sqrt(discriminant);
            if(numerator>0.0)
            {
                return numerator/(2.0 * a);
            }

            numerator=-b+sqrt(discriminant);
            if(numerator>0.0)
            {
                return numerator / (2.0 * a);
            }
            else
                return -1;

        }
    }

    Point getNormal(Point &p)
    {
        Point result;
        result=p-reference_point;
        result.normalize();
        return result;
    }

};



class General: public Object
{
public:
    double a,b,c,d,e,f,g,h,i,j;

    General(double arr[],Point p,double l,double w,double _h)
    {
        a=arr[0];
        b=arr[1];
        c=arr[2];
        d=arr[3];
        e=arr[4];
        f=arr[5];
        g=arr[6];
        h=arr[7];
        i=arr[8];
        j=arr[9];
        reference_point=p;
        length=l;
        width=w;
        height=_h;
    }

    double intersectParamT(Ray *r)
    {
        double xs,ys,zs,xd,yd,zd;
        xs=r->start.x;
        ys=r->start.y;
        zs=r->start.z;

        xd = r->dir.x;
        yd = r->dir.y;
        zd = r->dir.z;

//        Aq = Axd2 + Byd2 + Czd2 + Dxdyd + Exdzd + Fydzd
//        Bq = 2*Axoxd + 2*Byoyd + 2*Czozd + D(xoyd + yoxd) + E(xozd + zoxd) + F(yozd + ydzo) + Gxd + Hyd + Izd
//        Cq = Axo2 + Byo2 + Czo2 + Dxoyo + Exozo + Fyozo + Gxo + Hyo + Izo + J
        double aq=a*xd*xd + b*yd*yd + c*zd*zd + d*xd*yd + e*xd*zd + f*yd*zd;
        double bq=2*a*xs*xd + 2*b*ys*yd + 2*c*zs*zd
                  + d*(xs*yd+ys*xd) + e*(xs*zd+zs*xd) + f*(ys*zd+zs*yd)
                  + g*xd + h*yd + i*zd;
        double cq=a*xs*xs + b*ys*ys + c*zs*zs + d*xs*ys + e*xs*zs + f*ys*zs
                  + g*xs + h*ys + i*zs + j;

//        Check Aq = 0 (If Aq = 0 then t = -Cq / Bq)
//        If Aq <> 0, then check the discriminant. If (Bq2 - 4AqCq ) <0.0 then there is no intersection
//        Compute t0 and if t0 > 0 then done else compute t1
//        Once t is found compute Ri = (xi yi zi)
        if (aq==0)
            return -cq/bq;
        double discriminant=bq*bq-4*aq*cq;
        if(discriminant<0.0)
            return -1.0;

//        t0 =( - Bq - ((Bq2 - 4AqCq))^0.5)/ 2Aq
//        t1 =( - Bq + ((Bq2 - 4AqCq))^0.5)/ 2Aq
        double t0=(-bq - sqrt(discriminant))/(2*aq);
        double t1=(-bq + sqrt(discriminant))/(2*aq);

        if(t0<0.0 && t1<0.0)
            return -1.0;
        if(t0>=0 && t1>=0)
        {
            Point dis0=r->start + r->dir*t0 - reference_point;
            Point dis1=r->start + r->dir*t1 - reference_point;

            bool state_t0=isClipped(dis0,length,width,height);
            bool state_t1=isClipped(dis1,length,width,height);

            if(!state_t0 && !state_t1)
                return min(t0, t1);
            if(!state_t0)
                return t0;
            if (!state_t1)
                return t1;
            return -1.0;

        }
        if(t0>0.0)
            return t0;
        return t1;
    }

    Point getNormal(Point &p)
    {
        double x,y,z;
        x=p.x;
        y=p.y;
        z=p.z;

        Point result(2*a*x+d*y+e*z+g, 2*b*y+d*x+f*z+h, 2*c*z+e*x+f*y+i);
        result.normalize();

        return result;
    }
};



class Floor: public Object
{
public:
    Floor(double floorWidth,double tileWidth)
    {
        reference_point.x=-floorWidth/2;
        reference_point.y=-floorWidth/2;
        reference_point.z=0;
        length=tileWidth;
    }
    void draw()
    {

        glBegin(GL_QUADS);
        {
            for(int i=reference_point.x,k=0; i<-reference_point.x+length; i+=length,k++)
            {
                for(int j=reference_point.y,l=0; j<-reference_point.y+length; j+=length,l++)
                {
                    if((k+l)%2==0)
                        glColor3f(1,1,1);
                    else
                        glColor3f(0,0,0);
                    glVertex3f(i,j,0);
                    glVertex3f(i+length,j,0);
                    glVertex3f(i+length,j+length,0);
                    glVertex3f(i,j+length,0);

                }
            }
        }
        glEnd();
    }

    double intersectParamT(Ray *r)
    {
        Point origin(0,0,0);
        Point normal(0,0,1);

        double tem=normal.dotProduct(r->dir);

        double t=-1;
        if(abs(tem)>0.00001f)
        {
            t=((origin-r->start).dotProduct(normal))/tem;
            Point intersectionPoint=r->start+r->dir*t;
            if(abs(intersectionPoint.x)>windowWidth/2 || abs(intersectionPoint.y)>windowWidth/2)
                t=-1;
        }
        return t;
    }

    double* getColor(Point &p)
    {
        double* color=new double[3];
        int tileX=(abs(p.x)/tileWidth)+(p.x<0);
        int tileY=(abs(p.y)/tileWidth)+(p.y<0);

        if(tileX%2==tileY%2)
        {
            for(int i=0; i<3; i++)
                color[i]=1;
        }
        else
        {
            for(int i=0; i<3; i++)
                color[i]=0;
        }
        return color;
    }

    Point getNormal(Point &p)
    {
        Point result(0,0,1);
        return result;
    }
};






