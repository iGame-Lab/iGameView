//
// Created by teitoku on 2022-03-11.
//

#ifndef IGAMEVIEW_GEOMETRYSMALLLIB_H
#define IGAMEVIEW_GEOMETRYSMALLLIB_H

#endif //IGAMEVIEW_GEOMETRYSMALLLIB_H
class GeometrySmallLib {
    double eps;
    bool zero(double x) {
        return ((x)>0?(x):-(x))<eps;
    }
    struct point3 {
        double x, y, z;
    };

public:
    GeometrySmallLib(){eps  = 1e-8;}
    GeometrySmallLib(double eps){
        this->eps=eps;
    }
    int check_dot_inplane_in(std::vector< double> pp,std::vector< double> ss1,std::vector< double> ss2,std::vector< double> ss3)
    {
        point3 p{pp[0],pp[1],pp[2]};
        point3 s1{ss1[0],ss1[1],ss1[2]};
        point3 s2{ss2[0],ss2[1],ss2[2]};
        point3 s3{ss3[0],ss3[1],ss3[2]};
        return zero(vlen(xmult(subt(s1,s2),subt(s1,s3)))-vlen(xmult(subt(p,s1),subt(p,s2)))-vlen(xmult(subt(p,s2),subt(p,s3)))-vlen(xmult(subt(p,s3),subt(p,s1))));
    }
    bool check_tri_intersect(std::vector< double>p1,std::vector< double>p2,std::vector< double>p3,std::vector< double>s1,
                             std::vector< double>s2,std::vector< double>s3){


        return check(p1,p2,s1,s2,s3) || check(p1,p3,s1,s2,s3) || check(p2,p3,s1,s2,s3)
               || check(s1,s2,p1,p2,p3)   || check(s1,s3,p1,p2,p3)   || check(s2,s3,p1,p2,p3);
    }

    bool check(std::vector< double>p1,std::vector< double>p2,std::vector< double>s1,
               std::vector< double>s2,std::vector< double>s3){

        return intersect_ex2(point3{p1[0],p1[1],p1[2]},point3{p2[0],p2[1],p2[2]},
                             point3{s1[0],s1[1],s1[2]},point3{s2[0],s2[1],s2[2]},point3{s3[0],s3[1],s3[2]}
        );
    }
    int same_sides(std::vector< double>p1,std::vector< double>p2,std::vector< double>s1,
                   std::vector< double>s2,std::vector< double>s3) {
        point3 S1 =point3{s1[0],s1[1],s1[2]};
        point3 S2 =point3{s2[0],s2[1],s2[2]};
        point3 S3 =point3{s3[0],s3[1],s3[2]};
        point3 P1 =point3{p1[0],p1[1],p1[2]};
        point3 P2 =point3{p2[0],p2[1],p2[2]};
        return dmult(pvec(S1, S2, S3), subt(P1, S1)) * dmult(pvec(S1, S2, S3), subt(P2, S1)) > eps;
    }

    std::vector<double> get_intersection_vertex(std::vector<double> ll1,std::vector<double> ll2,
                                                std::vector<double> ss1,std::vector<double> ss2,std::vector<double> ss3)
    {
        point3 l1{ll1[0],ll1[1],ll1[2]};
        point3 l2{ll2[0],ll2[1],ll2[2]};
        point3 s1{ss1[0],ss1[1],ss1[2]};
        point3 s2{ss2[0],ss2[1],ss2[2]};
        point3 s3{ss3[0],ss3[1],ss3[2]};
        auto p3 = intersection( l1,  l2,  s1,  s2,  s3);
        if(! dot_inplane_in(p3 , s1,  s2,  s3) ){
            return std::vector<double>{-1,0,0,0};
        }
        return std::vector<double>{1,p3.x,p3.y,p3.z};
    }
    double get_point_to_lane_length(std::vector<double> pp,std::vector<double> ll1,std::vector<double> ll2){
        point3 p{pp[0],pp[1],pp[2]};
        point3 l1{ll1[0],ll1[1],ll1[2]};
        point3 l2{ll2[0],ll2[1],ll2[2]};
        return ptoline_length(p,l1,l2);
    }

    std::vector<double> get_point_to_lane(std::vector<double> pp,std::vector<double> ll1,std::vector<double> ll2){
        point3 p{pp[0],pp[1],pp[2]};
        point3 l1{ll1[0],ll1[1],ll1[2]};
        point3 l2{ll2[0],ll2[1],ll2[2]};
        point3 ret = ptoline(p,l1,l2);
        return {ret.x,ret.y,ret.z};
    }
    bool get_in_line(std::vector<double>  pp,std::vector<double>  ll1,std::vector<double>  ll2){
        point3 p1{pp[0],pp[1],pp[2]};
        point3 l1{ll1[0],ll1[1],ll1[2]};
        point3 l2{ll2[0],ll2[1],ll2[2]};
        double maxx= std::max(l1.x,l2.x);
        double maxy= std::max(l1.y,l2.y);
        double maxz= std::max(l1.z,l2.z);
        double minx= std::min(l1.x,l2.x);
        double miny= std::min(l1.y,l2.y);
        double minz= std::min(l1.z,l2.z);

        return minx <= p1.x && p1.x <= maxx &&
               miny <= p1.y && p1.y <= maxy &&
               minz <= p1.z && p1.z <= maxz ;
    }



private:

//计算 cross product U x V
    point3 xmult(point3 u, point3 v) {
        point3 ret;
        ret.x = u.y * v.z - v.y * u.z;
        ret.y = u.z * v.x - u.x * v.z;
        ret.z = u.x * v.y - u.y * v.x;
        return ret;
    }

//计算 dot product U . V
    double dmult(point3 u, point3 v) {
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

//矢量差 U - V
    point3 subt(point3 u, point3 v) {
        point3 ret;
        ret.x = u.x - v.x;
        ret.y = u.y - v.y;
        ret.z = u.z - v.z;
        return ret;
    }
    point3 add(point3 u, point3 v) {
        point3 ret;
        ret.x = u.x + v.x;
        ret.y = u.y + v.y;
        ret.z = u.z + v.z;
        return ret;
    }
    point3 multi(point3 u, double k) {
        point3 ret;
        ret.x = u.x * k ;
        ret.y = u.y * k;
        ret.z = u.z * k ;
        return ret;
    }
    point3 div(point3 u, double k) {
        point3 ret;
        ret.x = u.x / k ;
        ret.y = u.y / k;
        ret.z = u.z / k ;
        return ret;
    }

//取平面法向量
    point3 pvec(point3 s1, point3 s2, point3 s3) {
        return xmult(subt(s1, s2), subt(s2, s3));
    }
//两点距离,单参数取向量大小

    int same_side(point3 p1, point3 p2, point3 s1, point3 s2, point3 s3) {
        return dmult(pvec(s1, s2, s3), subt(p1, s1)) * dmult(pvec(s1, s2, s3), subt(p2, s1)) > eps;
    }

    int opposite_side(point3 p1, point3 p2, point3 s1, point3 s2, point3 s3) {
        return !same_side(p1, p2, s1, s2, s3);
    }

    int intersect_ex(point3 l1, point3 l2, point3 s1, point3 s2, point3 s3) {

        return opposite_side(l1, l2, s1, s2, s3) && opposite_side(s1, s2, l1, l2, s3) &&
               opposite_side(s2, s3, l1, l2, s1) && opposite_side(s3, s1, l1, l2, s2);
    }

    bool in_line(point3 p1,point3 l1,point3 l2){
        double maxx= std::max(l1.x,l2.x);
        double maxy= std::max(l1.y,l2.y);
        double maxz= std::max(l1.z,l2.z);
        double minx= std::min(l1.x,l2.x);
        double miny= std::min(l1.y,l2.y);
        double minz= std::min(l1.z,l2.z);

        return minx <= p1.x && p1.x <= maxx &&
               miny <= p1.y && p1.y <= maxy &&
               minz <= p1.z && p1.z <= maxz ;
    }

    int intersect_ex2(point3 l1, point3 l2, point3 s1, point3 s2, point3 s3) {

        auto p3 = intersection( l1,  l2,  s1,  s2,  s3);

        return dot_inplane_in(p3 , s1,  s2,  s3) && in_line(p3, l1, l2);
    }
    point3 intersection(point3 l1,point3 l2,point3 s1,point3 s2,point3 s3)
    {
        point3 ret=pvec(s1,s2,s3);
        double t=(ret.x*(s1.x-l1.x)+ret.y*(s1.y-l1.y)+ret.z*(s1.z-l1.z))/
                 (ret.x*(l2.x-l1.x)+ret.y*(l2.y-l1.y)+ret.z*(l2.z-l1.z));
        ret.x=l1.x+(l2.x-l1.x)*t;
        ret.y=l1.y+(l2.y-l1.y)*t;
        ret.z=l1.z+(l2.z-l1.z)*t;
        return ret;
    }
    double vlen(point3 p)
    {
        return sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    }
    double vlen2(point3 p)
    {
        return (p.x*p.x+p.y*p.y+p.z*p.z);
    }
    int dot_inplane_in(point3 p,point3 s1,point3 s2,point3 s3)
    {
        return zero(vlen(xmult(subt(s1,s2),subt(s1,s3)))-vlen(xmult(subt(p,s1),subt(p,s2)))-vlen(xmult(subt(p,s2),subt(p,s3)))-vlen(xmult(subt(p,s3),subt(p,s1))));
    }
    double distance(point3 p1,point3 p2)
    {
        return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
    }
    double ptoline_length(point3 p,point3 l1,point3 l2)
    {
        return vlen(xmult(subt(p,l1),subt(l2,l1)))/distance(l1,l2);
    }
    point3 ptoline(point3 p,point3 l1,point3 l2){
        double h = ptoline_length( p, l1, l2);
        double x1 = sqrt(vlen2(subt(p,l1)) - h * h);
        point3 possible_1 = add(l1 , div(multi(subt(l2,l1), x1) , distance(l1,l2)));
        point3 possible_2 = subt(l1 , div(multi(subt(l2,l1), x1) , distance(l1,l2)));
        if(zero(distance(possible_1,p)-h)){
            return  possible_1;
        }
        else if(zero(distance(possible_2,p)-h)){
            return  possible_2;
        }
        else{
            assert(0);
            return{0,0,0};
        }
    }



};