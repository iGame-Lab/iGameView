#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class STLWrite {
    class Buffer{
    public:
        char* p;
        int count;
        char* buffer ;
        
        Buffer(){
            buffer = new char[5192000];
            p = buffer;
        }
        ~Buffer(){
            delete[] buffer;
        }
        void writehead(char h[]){
            int a = 0;
            for(int i = 0; h[i] != '\0'; i++){
                a++;
                *p = h[i];
                p++;
            }
            for(; a < 80; a++ ){
                buffer[a] = 0;
            }
            p = buffer + 80;
            count = 80;
        }
        void writeNum(uint32_t num){
            *p++ = num & 0xff;
            *p++ = (num >> 8) & 0xff;
            *p++ = (num >> 16) & 0xff;
            *p++ = (num >> 24) & 0xff;
            count += sizeof(uint32_t);
        }
        void writePoint(float x, float y, float z){
            float* p1 = (float*) p;
            *p1 = x; p1++;
            *p1 = y; p1++;
            *p1 = z; p1++;
            p += 3*sizeof(float);
            count += 3*sizeof(float);
        }
        void writeart(){
            *p++ = 0;
            *p++ = 0;
            count += sizeof(uint16_t);
        }
        int size(){
            return count;
        }
    };

    class Shape{
    public:
        float x, y, z;
        Shape(float x, float y, float z):x(x), y(y), z(z){}
        vector<float> getpoint() {
            return vector<float>{x,y,z};
        }
    };

private:
    vector<Shape*> shapes;
public:
    STLWrite(){}
    ~STLWrite(){
        for(int i = 0; i < shapes.size(); i++)
            delete shapes[i];
    }
    void add(float x,float y,float z){
        shapes.push_back(new Shape(x,y,z));
    }
    void write(string filename){
        ofstream stl;
        stl.open(filename, std::ofstream::binary);
        Buffer a;
        char head[8] = "Shape3D";
        a.writehead(head);
        a.writeNum(shapes.size()/3);
        for(int i=0;i<shapes.size();i+=3) {
            a.writePoint(0.0, 0.0, 0.0);
            a.writePoint(shapes[i]->x, shapes[i]->y, shapes[i]->z);
            a.writePoint(shapes[i+1]->x, shapes[i+1]->y, shapes[i+1]->z);
            a.writePoint(shapes[i+2]->x, shapes[i+2]->y, shapes[i+2]->z);
            a.writeart();
        }
        stl.write(a.buffer, a.size());
        stl.close();
    }
};