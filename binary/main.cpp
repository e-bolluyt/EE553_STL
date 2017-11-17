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
/*
class Buffer {
public:
    constexpr static int SIZE = 8192;//32768;
    string filename;
    void end(){
        flush();
    }
private:
    char buffer[SIZE];
    char* current;
    uint32_t avail;
    void flush() {
        ofstream file(filename);
        file.write(buffer, SIZE);
        //cout << current - buffer;
        current = buffer;
        avail = SIZE;
    }
    void internalWrite(float v) {
        //* (float*)current = v;
        current = (char*)&v;
        //current = (v >> 24) & 0xFF;
        current += sizeof(float);
        avail -= sizeof(float);
    }
public:
    Buffer() : current(buffer), avail(SIZE) {}
    void write(uint32_t v) {
        if (avail < sizeof(uint32_t)) {
            flush();
        }
        //*(uint32_t*)current = v;
        current = (char*)&v;
        current+= sizeof(uint32_t);
        avail -= sizeof(uint32_t);
    }
    void write(uint16_t v) {
        if (avail < sizeof(uint16_t)) {
            flush();
        }
        //*(uint16_t*)current = v;
        current = (char*)&v;
        current+= sizeof(uint16_t);
        avail -= sizeof(uint16_t);
    }
    void write(const char m[], uint32_t LEN) {
        uint32_t i;
        for (i = 0; m[i] != '\0' && i < LEN; i++)
            *current++ = m[i];
        for ( ; i <LEN; i++)
            *current++ = 0;
    }
    void writeAsFloats(const Vector3d& v) {
        if (avail < sizeof(float) * 3)
            flush();
    }
    void writeAsFloats(const Triangle& t) {
        if (avail < 4*3*sizeof(float))
            flush();
        //writeAsFloats(t.p1);
        internalWrite(t.normal.x);
        internalWrite(t.normal.y);
        internalWrite(t.normal.z);

        internalWrite(t.p1.x);
        internalWrite(t.p1.y);
        internalWrite(t.p1.z);

        internalWrite(t.p2.x);
        internalWrite(t.p2.y);
        internalWrite(t.p2.z);

        internalWrite(t.p3.x);
        internalWrite(t.p3.y);
        internalWrite(t.p3.z);
        //...
    }
};
*/
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
    char header[80];
public:
    /*
    CAD(){
        //create header
        char h[] = "Elijah D. Bolluyt, EE553 STL Writing Homework.";
        int len = sizeof(h) / sizeof(char);
        int i = 0;
        for (i = 0; i < 80; i++){
            if (i < 80)
                header[i] = h[i];
            else
                header[i] = ' ';
        }
    }*/
    void add(Shape* s){
        shapes.push_back(s);
    }
    //void write(string fname){
    //   ofstream file(fname);

    //}
    /*
    void write_old(string fname){
        //ofstream file(fname);

        Buffer b;
        b.filename = fname;
        //b.write(header,80);

        uint32_t LEN = 0;
        for (auto s : shapes)
            LEN += s->triCount();
        b.write(LEN);
        //cout << LEN;
        //file.write((char*)LEN,4);
        for (auto s : shapes){
            for (auto t : s->tris){
                b.writeAsFloats(t);
            }
        }

        char e[2];
        uint16_t attr = 0;
        e[0] = (attr >> 8) & 0xFF;
        e[1] = attr & 0xFF;
        //b.write(e,2);
        b.write(attr);
        b.end();
    }
     */
    void write(string fname){
        ofstream file;
        cout << "function called" << '\n';
        file.open(fname, ios::out | ios::binary);
        if (file.good()){
            cout << "yes";
        }
        float f = 900;
        uint32_t i = 10;
        uint16_t j = 10;
        //file.write((char*)&f,sizeof(float));
        //file.write((char*)&i,sizeof(uint32_t));
        //file.write((char*)&j,sizeof(uint16_t));
        //file.write(header,sizeof(header));

        file.write(header,80);
        uint32_t LEN = 0;
        for (auto s : shapes)
            LEN+= s->triCount();
        file.write((char*)&LEN,sizeof(LEN));
        uint16_t attr = 0;
        for (auto s: shapes){
            for (auto t : s->tris){
                file.write((char*)&t.normal.x,sizeof(float));
                file.write((char*)&t.normal.y,sizeof(float));
                file.write((char*)&t.normal.z,sizeof(float));
                file.write((char*)&t.p1.x,sizeof(float));
                file.write((char*)&t.p1.y,sizeof(float));
                file.write((char*)&t.p1.z,sizeof(float));
                file.write((char*)&t.p2.x,sizeof(float));
                file.write((char*)&t.p2.y,sizeof(float));
                file.write((char*)&t.p2.z,sizeof(float));
                file.write((char*)&t.p3.x,sizeof(float));
                file.write((char*)&t.p3.y,sizeof(float));
                file.write((char*)&t.p3.z,sizeof(float));
                file.write((char*)&attr,sizeof(uint16_t));
            }
        }


        //file.close();

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