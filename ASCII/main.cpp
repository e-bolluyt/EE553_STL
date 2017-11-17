#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class Vector3d{
public:
    Vector3d(float x, float y, float z) : x(x), y(y), z(z){}
    float x,y,z; //using float for STL compatibility

    friend Vector3d operator -(Vector3d v);
    friend Vector3d operator -(Vector3d a, Vector3d b);
    friend Vector3d operator *(float c, Vector3d v);
};

Vector3d operator *(float c, Vector3d v){
    return Vector3d(c*v.x, c*v.y, c*v.z);
}

Vector3d operator -(Vector3d v){
    return Vector3d(0-v.x,0-v.y,0-v.z);
}

Vector3d operator -(Vector3d a, Vector3d b){
    float xn, yn, zn;
    xn = a.x - b.x;
    yn = a.y - b.y;
    zn = a.z - b.z;
    return Vector3d(xn, yn, zn);
}

class Triangle{
public:
    Vector3d p1,p2,p3;
    Vector3d normal;
    Triangle(Vector3d p1, Vector3d p2, Vector3d p3, Vector3d normal) :
            p1(p1), p2(p2), p3(p3), normal(normal){
    }
};

class Shape {
public:
    vector<Triangle> tris;
    uint32_t triCount(){
        return tris.size();
    }
};

class Cylinder : public Shape{
private:
    double r, h;
    int facets;
    double x,y,z;
    double pi;
    //vector<Triangle> tris;
public:
    Cylinder(double x, double y, double z, double r, double h, int facets) :
            x(x), y(y), z(z), r(r), h(h), facets(facets){
        double pi = 3.14159265358979;
        vector<Vector3d> p;
        Vector3d center_b(x,y,z);
        Vector3d center_t(x,y,z+h);
        for (int i = 0; i < facets; i++){
            double angle_n = 2*pi*i/facets;
            double angle_1 = angle_n - pi/facets;
            double angle_2 = angle_n + pi/facets;
            Vector3d normal = (Vector3d(cos(angle_n),sin(angle_n),0));
            p.clear();
            p.push_back(Vector3d(r*cos(angle_1)+x,r*sin(angle_1)+y,z));
            p.push_back(Vector3d(r*cos(angle_1)+x,r*sin(angle_1)+y,z+h));
            p.push_back(Vector3d(r*cos(angle_2)+x,r*sin(angle_2)+y,z));
            p.push_back(Vector3d(r*cos(angle_2)+x,r*sin(angle_2)+y,z+h));

            tris.push_back(Triangle(p[0],p[2],p[1],normal));
            tris.push_back(Triangle(p[1],p[2],p[3],normal));
            tris.push_back(Triangle(p[2],p[0],center_b, Vector3d(0,0,-1)));
            tris.push_back(Triangle(p[1],p[3],center_t, Vector3d(0,0,1)));
        }
    }
};

class Cube: public Shape {
private:
    double l;
    double x,y,z;
    //vector<Triangle> tris;
public:
    Cube(double x, double y, double z, double size) : x(x), y(y), z(z), l(size) {
        Vector3d x_n(1,0,0);
        Vector3d y_n(0,1,0);
        Vector3d z_n(0,0,1);
        vector<Vector3d> c;
        c.push_back(Vector3d(x,y,x));
        c.push_back(Vector3d(x+l,y,z));
        c.push_back(Vector3d(x+l,y+l,z));
        c.push_back(Vector3d(x,y+l,z));
        c.push_back(Vector3d(x,y,z+l));
        c.push_back(Vector3d(x+l,y,z+l));
        c.push_back(Vector3d(x+l,y+l,z+l));
        c.push_back(Vector3d(x,y+l,z+l));

        tris.push_back(Triangle(c[0],c[1],c[4],-y_n));
        tris.push_back(Triangle(c[1],c[5],c[4],-y_n));
        tris.push_back(Triangle(c[1],c[2],c[6],x_n));
        tris.push_back(Triangle(c[1],c[6],c[5],x_n));
        tris.push_back(Triangle(c[5],c[6],c[4],z_n));
        tris.push_back(Triangle(c[6],c[7],c[4],z_n));

        tris.push_back(Triangle(c[0],c[3],c[1],-z_n));
        tris.push_back(Triangle(c[1],c[3],c[2],-z_n));
        tris.push_back(Triangle(c[0],c[4],c[7],-x_n));
        tris.push_back(Triangle(c[0],c[7],c[3],-x_n));
        tris.push_back(Triangle(c[2],c[3],c[7],y_n));
        tris.push_back(Triangle(c[2],c[7],c[6],y_n));
    }
};

class CAD {
private:
    vector<Shape*> shapes;
public:
    CAD(){

    }
    void add(Shape* s){
        shapes.push_back(s);
    }
    void write(string fname){
        ofstream file(fname);
        if (file.good())
            cout << "yes";
        file << "solid ebolluyt_HW\n";
        for (auto s : shapes){
            for (auto t : s->tris){
                file << "\tfacet normal " << t.normal.x << ' ' << t.normal.y << ' ' << t.normal.z << '\n';
                file << "\t\touter loop\n";
                file << "\t\t\tvertex " << t.p1.x << ' ' << t.p1.y << ' ' << t.p1.z << '\n';
                file << "\t\t\tvertex " << t.p2.x << ' ' << t.p2.y << ' ' << t.p2.z << '\n';
                file << "\t\t\tvertex " << t.p3.x << ' ' << t.p3.y << ' ' << t.p3.z << '\n';
                file <<"\t\tendloop\n";
                file <<"\tendfacet\n";
            }
        }
        file << "endsolid ebolluyt_HW";
    }

};

//https://www.stratasysdirect.com/resources/how-to-prepare-stl-files/
//https://www.viewstl.com/
int main() {
    CAD c;
    c.add(new Cube(0,0,0,   5));
    c.add(new Cylinder(100,0,0,    3, 10, 10));
    c.write("test.stl");
}