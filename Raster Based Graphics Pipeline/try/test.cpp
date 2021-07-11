#include<bits/stdc++.h>
using namespace std;
#define INF 99999.99

#include "bitmap_image.hpp"

class Point
{
public:
    double x;
    double y;
    double z;
    Point(string command)
    {
        istringstream ss(command);
        string subs;
        double arr[3];
        int i=0;
        while (ss >> subs)
        {
            arr[i++]=atof(subs.c_str());
        }
        x=arr[0];
        y=arr[1];
        z=arr[2];
    }
    Point(double a, double b, double c)
    {
        x=a;
        y=b;
        z=c;
    }
    Point()
    {
        x=0;y=0;z=0;
    }

    void normalize()
    {
        double val=sqrt(x*x+y*y+z*z);
        x=x/val;
        y=y/val;
        z=z/val;
    }

    void printPoint()
    {
        cout<<"Printing a POINT---> "<<endl;
        cout<<x<<" "<<y<<" "<<z<<endl;
    }

};

class Matrix
{
public:
    double mat[4][4];
    Matrix(Point a, Point b, Point c)
    {
        mat[0][0]=a.x;
        mat[1][0]=b.x;
        mat[2][0]=c.x;
        mat[3][0]=0.0;
        mat[0][1]=a.y;
        mat[1][1]=b.y;
        mat[2][1]=c.y;
        mat[3][1]=0.0;
        mat[0][2]=a.z;
        mat[1][2]=b.z;
        mat[2][2]=c.z;
        mat[3][2]=0.0;
        mat[0][3]=0.0;
        mat[1][3]=0.0;
        mat[2][3]=0.0;
        mat[3][3]=1.0;

    }
    Matrix()
    {
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                mat[i][j]=0;
    }
    void printMatrix()
    {
        cout<<"Printing A  MATRIX  --> "<<endl;
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<4; j++)
            {
                cout<<mat[i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }
};

class Triangle
{
public:
    Point points[3];
    int color[3];
};

Matrix getIdentityMatrix()
{
    Matrix idenMat;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(i==j)
                idenMat.mat[i][j]=1;
        }
    }
    return idenMat;
}

Point multiply(Point p,double val)
{
    Point r(0,0,0);
    r.x=p.x*val;
    r.y=p.y*val;
    r.z=p.z*val;

    return r;
}

double dotProduct(Point p,Point q)
{
    return p.x*q.x + p.y*q.y + p.z*q.z;
}

Point crossProduct(Point p,Point q)
{
    Point r(0,0,0);

    r.x=p.y*q.z - q.y*p.z;
    r.y=p.z*q.x - q.z*p.x;
    r.z=p.x*q.y - q.x*p.y;

    return r;
}

double findMax(double a,double b,double c)
{
    if(a>=b){
        if(a>=c) return a;
        else return c;
    }else{
        if(b>=c) return b;
        else return c;
    }
}

double findMin(double a,double b,double c)
{
    if(a<=b){
        if(a<=c) return a;
        else return c;
    }else{
        if(b<=c) return b;
        else return c;
    }
}

Matrix transformMatrix(Matrix a,Matrix b)
{
    Matrix res;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            for(int k=0; k<4; k++)
            {
                res.mat[i][j]+=a.mat[i][k]*b.mat[k][j];
            }
    return res;
}

Point transformPoint(Matrix a,Point p)
{
    double triPoint[4];
    double *res=new double[4];
    for(int i=0; i<4; i++)
        res[i]=0.0;
    triPoint[0]=p.x;
    triPoint[1]=p.y;
    triPoint[2]=p.z;
    triPoint[3]=1.0;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
        {
            res[i]+=a.mat[i][j]*triPoint[j];
        }
    Point r(0,0,0);
    r.x=res[0]/res[3];
    r.y=res[1]/res[3];
    r.z=res[2]/res[3];
    return r;
}

Point addPoint(Point p,Point q,Point r)
{
    Point tem(0,0,0);
    tem.x=p.x+q.x+r.x;
    tem.y=p.y+q.y+r.y;
    tem.z=p.z+q.z+r.z;

    return tem;
}

Point subtractPoint(Point p,Point q)
{
    Point tem(0,0,0);
    tem.x=p.x-q.x;
    tem.y=p.y-q.y;
    tem.z=p.z-q.z;

    return tem;
}

Point rodrigues(Point u,Point a,double angle)
{
    double rad=M_PI/180*angle;

    Point c=addPoint(multiply(u,cos(rad)), multiply(a,(1-cos(rad))*dotProduct(a,u)), multiply(crossProduct(a,u),sin(rad)));

    return c;
}


int main()
{
    string command,changedX="none";
    double fovX, fovY, aspectRatio, near, far;
    ifstream sceneFile ("scene.txt");
    if (!sceneFile.is_open())
    {
        cout<<"File not found"<<endl;
    }
    ofstream stage1File("stage1.txt");
    if (!stage1File.is_open())
    {
        cout<<"File not found"<<endl;
    }
    stage1File<<setprecision(7)<<fixed;
    ofstream stage2File("stage2.txt");
    if (!stage2File.is_open())
    {
        cout<<"File not found"<<endl;
    }
    stage2File<<setprecision(7)<<fixed;
    ofstream stage3File("stage3.txt");
    if (!stage3File.is_open())
    {
        cout<<"File not found"<<endl;
    }
    stage3File<<setprecision(7)<<fixed;

    getline(sceneFile,command);
    Point eye(command);
    getline(sceneFile,command);
    Point look(command);
    getline(sceneFile,command);
    Point up(command);

    Point l=subtractPoint(look,eye);
    l.normalize();
    Point r=crossProduct(l,up);
    r.normalize();
    Point u=crossProduct(r,l);

    Matrix T=getIdentityMatrix();
    T.mat[0][3]=-1*eye.x;
    T.mat[1][3]=-1*eye.y;
    T.mat[2][3]=-1*eye.z;

    Matrix R=getIdentityMatrix();
    R.mat[0][0]=r.x;
    R.mat[0][1]=r.y;
    R.mat[0][2]=r.z;
    R.mat[1][0]=u.x;
    R.mat[1][1]=u.y;
    R.mat[1][2]=u.z;
    R.mat[2][0]=-1*l.x;
    R.mat[2][1]=-1*l.y;
    R.mat[2][2]=-1*l.z;

    Matrix V=transformMatrix(R,T);

    sceneFile>>fovY>>aspectRatio>>near>>far;
    fovX = fovY * aspectRatio;
    double radY=M_PI/180*(fovY/2);
    double radX=M_PI/180*(fovX/2);
    double t = near * tan(radY);
    double rr = near * tan(radX);
    Matrix proj;
    proj.mat[0][0]=near/rr;
    proj.mat[1][1]=near/t;
    proj.mat[2][2]=-1*(far+near)/(far-near);
    proj.mat[2][3]=-1*(2*far*near)/(far-near);
    proj.mat[3][2]=-1;
    proj.printMatrix();

    stack<Matrix> stack;

    stack.push(getIdentityMatrix());

    while(true)
    {
        getline(sceneFile,command);
        //cout<<command<<endl;
        if(command=="triangle")
        {
            getline(sceneFile,command);
            Point a(command);
            getline(sceneFile,command);
            Point b(command);
            getline(sceneFile,command);
            Point c(command);

            //stack.top().printMatrix();
            Point a_res=transformPoint(stack.top(),a);
            Point b_res=transformPoint(stack.top(),b);
            Point c_res=transformPoint(stack.top(),c);

            Point va_res=transformPoint(V,a_res);
            Point vb_res=transformPoint(V,b_res);
            Point vc_res=transformPoint(V,c_res);

            Point pva_res=transformPoint(proj,va_res);
            Point pvb_res=transformPoint(proj,vb_res);
            Point pvc_res=transformPoint(proj,vc_res);

            //for(int i=0;i<4;i++) cout<<a_res[i]<<"  --  ";
            //cout<<endl;
            stage1File<<a_res.x<<" "<<a_res.y<<" "<<a_res.z<<endl;
            stage1File<<b_res.x<<" "<<b_res.y<<" "<<b_res.z<<endl;
            stage1File<<c_res.x<<" "<<c_res.y<<" "<<c_res.z<<endl;
            stage1File<<endl;

            stage2File<<va_res.x<<" "<<va_res.y<<" "<<va_res.z<<endl;
            stage2File<<vb_res.x<<" "<<vb_res.y<<" "<<vb_res.z<<endl;
            stage2File<<vc_res.x<<" "<<vc_res.y<<" "<<vc_res.z<<endl;
            stage2File<<endl;

            stage3File<<pva_res.x<<" "<<pva_res.y<<" "<<pva_res.z<<endl;
            stage3File<<pvb_res.x<<" "<<pvb_res.y<<" "<<pvb_res.z<<endl;
            stage3File<<pvc_res.x<<" "<<pvc_res.y<<" "<<pvc_res.z<<endl;
            stage3File<<endl;
        }
        else if(command=="translate")
        {
            getline(sceneFile,command);
            Point t(command);
            Matrix translateMat=getIdentityMatrix();
            translateMat.mat[0][3]=t.x;
            translateMat.mat[1][3]=t.y;
            translateMat.mat[2][3]=t.z;
            //translateMat.printMatrix();
            Matrix res=transformMatrix(stack.top(),translateMat);
            //res.printMatrix();
            stack.pop();
            stack.push(res);;
        }
        else if(command=="scale")
        {
            getline(sceneFile,command);
            Point s(command);
            Matrix scaleMat=getIdentityMatrix();
            scaleMat.mat[0][0]=s.x;
            scaleMat.mat[1][1]=s.y;
            scaleMat.mat[2][2]=s.z;

            Matrix res=transformMatrix(stack.top(),scaleMat);
            stack.pop();
            stack.push(res);
        }
        else if(command=="rotate")
        {
            double angle,ax,ay,az;
            sceneFile>>angle>>ax>>ay>>az;

            Point a(ax,ay,az);
            a.normalize();


            Point i(1.0,0.0,0.0);
            Point j(0.0,1.0,0.0);
            Point k(0.0,0.0,1.0);

            Point c1=rodrigues(i,a,angle);
            Point c2=rodrigues(j,a,angle);
            Point c3=rodrigues(k,a,angle);

            Matrix rotMatrix=getIdentityMatrix();
            rotMatrix.mat[0][0]=c1.x;
            rotMatrix.mat[1][0]=c1.y;
            rotMatrix.mat[2][0]=c1.z;
            rotMatrix.mat[0][1]=c2.x;
            rotMatrix.mat[1][1]=c2.y;
            rotMatrix.mat[2][1]=c2.z;
            rotMatrix.mat[0][2]=c3.x;
            rotMatrix.mat[1][2]=c3.y;
            rotMatrix.mat[2][2]=c3.z;

            //cout<<"ptinting rot--->"<<endl;
            //rotMatrix.printMatrix();

            Matrix res=transformMatrix(stack.top(),rotMatrix);
            stack.pop();
            stack.push(res);

        }
        else if(command=="push")
        {
            stack.push(stack.top());
        }
        else if(command=="pop")
        {
            stack.pop();
        }
        else if(command=="end")
            break;
    }

    sceneFile.close();
    stage1File.close();
    stage2File.close();
    stage3File.close();

    int rgbValues[21];
    rgbValues[0]=148; rgbValues[1]=0; rgbValues[2]=211; //violet
    rgbValues[3]=75; rgbValues[4]=0; rgbValues[5]=130; //indigo
    rgbValues[6]=0; rgbValues[7]=0; rgbValues[8]=255; //blue
    rgbValues[9]=0; rgbValues[10]=255; rgbValues[11]=0; //green
    rgbValues[12]=255; rgbValues[13]=255; rgbValues[14]=0; //yellow
    rgbValues[15]=255; rgbValues[16]=127; rgbValues[17]=0; //orange
    rgbValues[18]=255; rgbValues[19]=0; rgbValues[20]=0; //red

    ifstream conFile("config.txt");
    if (!conFile.is_open())
    {
        cout<<"File not found"<<endl;
    }
    ifstream projFile("stage3.txt");
    if (!projFile.is_open())
    {
        cout<<"File not found"<<endl;
    }

    int width,height;
    double leftLimX,rightLimX,botLimY,topLimY,frontZ,rearZ;
    conFile>>width>>height>>leftLimX>>botLimY>>frontZ>>rearZ;
    rightLimX=-leftLimX;
    topLimY=-botLimY;
    //cout<<leftLimX<<" "<<botLimY<<endl;

    vector<Triangle> triangles;
    srand(time(0));
    int colorFlag=0;
    while(getline(projFile,command))
    {
        Point t1(command);
        getline(projFile,command);
        //cout<<"2nd-->"<<command<<endl;
        Point t2(command);
        getline(projFile,command);
        //cout<<"3rd-->"<<command<<endl;
        Point t3(command);
        getline(projFile,command);

//        t1.printPoint();
//        t2.printPoint();
//        t3.printPoint();

        Triangle triangle;
        triangle.points[0]=t1;
        triangle.points[1]=t2;
        triangle.points[2]=t3;

//        int random=rand()%7;
        int random=(colorFlag++)%7;
        //cout<<"rand--> "<<random<<endl;
        for(int i=0;i<3;i++)
        {
            triangle.color[i]=rgbValues[random*3+i];
        }
        triangles.push_back(triangle);
    }

    double dx=(rightLimX-leftLimX)/width;
    double dy=(topLimY-botLimY)/height;
    float topY=topLimY-dy/2;
    float leftX=leftLimX+dx/2;

    double** zBuffer=new double*[width];
    for(int i=0;i<width;i++)
        zBuffer[i]=new double[height];
    for(int i=0;i<width;i++)
        for(int j=0;j<height;j++)
            zBuffer[i][j]=rearZ;

    bitmap_image image(width,height);
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            image.set_pixel(i,j,0,0,0);
        }
    }


    double x1,x2,x3,y1,y2,y3,z1,z2,z3;
    for(int i=0;i<2;i++)
    {
        //cout<<"here"<<endl;
        Triangle t=triangles.at(i);
        x1=t.points[0].x;
        y1=t.points[0].y;
        z1=t.points[0].z;
        x2=t.points[1].x;
        y2=t.points[1].y;
        z2=t.points[1].z;
        x3=t.points[2].x;
        y3=t.points[2].y;
        z3=t.points[2].z;

        double minY,maxY,minX,maxX;
        minY=findMin(y1,y2,y3);
        maxY=findMax(y1,y2,y3);

        //cout<<"(x1,y1,z1,x2,y2,z2,x3,y3,z3) "<<x1<<" "<<y1<<" "<<z1<<"     "<<x2<<" "<<y2<<" "<<z2<<"    "<<x3<<" "<<y3<<" "<<z3<<endl;

        int topScanline,bottomScanline,leftScanline,rightScanline;
        if(maxY>=topY) topScanline=0;
        else topScanline=ceil((topY-maxY)/dy);
        if(minY<=-topY) bottomScanline=height;
        else bottomScanline=floor((topY-minY)/dy);

        cout<<"(botscan,topscan)--> "<<bottomScanline<<"  "<<topScanline<<endl;
        double X1=INF,X2=INF,X3=INF;
        double Z1=INF,Z2=INF,Z3=INF;
        for(int j=topScanline;j<bottomScanline;j++)
        {
            double currY=topY-j*dy;

            if(y1-y2!=0) X1=x1+((x1-x2)/(y1-y2))*(currY-y1);
            if(y2-y3!=0) X2=x2+((x2-x3)/(y2-y3))*(currY-y2);
            if(y3-y1!=0) X3=x3+((x3-x1)/(y3-y1))*(currY-y3);

            if(X1<findMin(x1,x2,x3) || X1>findMax(x1,x2,x3)){
                swap(X1,X3);
                changedX="X1";
            }else if(X2<findMin(x1,x2,x3) || X2>findMax(x1,x2,x3)){
                swap(X2,X3);
                changedX="X2";
            }else changedX="none";

            minX = min(X1, X2);
            maxX = max(X1, X2);

            if(minX<=leftX) leftScanline=0;
            else leftScanline=round((minX-leftX)/dx);
            if(maxX>=-leftX) rightScanline=width;
            else rightScanline=round((maxX-leftX)/dx);

            float a1 = x2 - x1;
            float b1 = y2 - y1;
            float c1 = z2 - z1;
            float a2 = x3 - x1;
            float b2 = y3 - y1;
            float c2 = z3 - z1;
            float a = b1 * c2 - b2 * c1;
            float b = a2 * c1 - a1 * c2;
            float c = a1 * b2 - b1 * a2;
            float d = (-a*x1 - b*y1 - c*z1);

            for (int k=leftScanline;k<rightScanline;k++) {
                float _x =leftX+k*dx;

                float Z = (-a/c)*_x + (-b/c)*currY + (-d/c);
                // cout << a << " " << b << " " << c << " " << d << " " << Z << "\n";

                if (Z > frontZ && Z < zBuffer[j][k]) {
                    zBuffer[j][k] = Z;
                    image.set_pixel(k, j, t.color[0],t.color[1],t.color[2]);
                }
            }

        }
    }

    image.save_image("out.bmp");
    ofstream bufFile("z-buffer.txt");
    if (!bufFile.is_open()) {
        cout<< "File not found"<<endl;
    }
    bufFile << setprecision(6) << fixed;

    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            if(zBuffer[i][j]<rearZ)
                bufFile<<zBuffer[i][j]<<"    ";
        }
        bufFile<<endl;
    }

    for(int i=0;i<width;i++)
        delete[] zBuffer[i];
    delete[] zBuffer;

    image.clear();

    conFile.close();
    bufFile.close();
    projFile.close();


}
























