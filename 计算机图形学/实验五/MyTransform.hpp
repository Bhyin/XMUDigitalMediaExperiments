#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>

using namespace std;
#define PI 3.1415927

extern Eigen::Matrix4f mModelView;
extern Eigen::Matrix4f mProjection;
extern Eigen::Matrix4f mViewPort;


void Test() {
    //矩阵用法
    Eigen::Matrix4f m;
    m.setZero();
    m(0, 0) = 1;
    m(3, 3) = 1;
    m(1, 1) = 2.3;
    m(1, 2) = 0.99;
    m(2, 1) = 1.1;
    m(2, 2) = 0;
    cout << "Matrix:" << endl;
    cout << m << endl;

    //向量用法
    Eigen::Vector4f v;
    v[0] = 1.0;
    v[1] = 2.0;
    v[2] = 3.0;
    v[3] = 1.0;
    //v[4] = 1.0;
    v.normalize();//归一化

    //输出矩阵与向量相乘的结果
    cout << "result:" << endl;
    cout << m * v << endl;
}

vector<Eigen::Vector3f> VertexTransform(vector<Eigen::Vector3f> vertices) {
    vector<Eigen::Vector3f> rlt(vertices.size());
    Eigen::Vector4f v;

    Eigen::Matrix4f M = mViewPort * mProjection * mModelView;

    for (int i = 0; i < vertices.size(); ++i) {
        v[3] = 1.0;
        v.head<3>() = vertices[i];

        v = M * v;
        rlt[i][0] = v[0] / v[3];
        rlt[i][1] = v[1] / v[3];
        rlt[i][2] = v[2] / v[3];
    }
    return rlt;
}

Eigen::Matrix4f
myLookAt(float ex, float ey, float ez, float atx, float aty, float atz, float upx, float upy, float upz) {
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面空白处完成此函数

    /*gluLookAt创建了一个视图变换矩阵，将经过模型变换（平移、旋转、缩放）的模型从世界坐标系下转换到相机坐标系下。
     * 然后经过投影矩阵（正交、透视）后映射到一个正方体中。
     * */

    /*视图变换矩阵的推导*/
    /*首先相机的坐标在相机坐标系中为（0,0,0），而相机的视线方向是z轴的负半轴，也就是相机看向的点为终点，以相机坐标为起点的向量的方向
     * 将此向量求出，然后归一化，然后取负，就是相机坐标系的z轴
     *
     * 相机坐标系也是右手坐标系，而且相机视线指向z轴负方向，那么相机的up方向相当于屏幕从底部到顶部的方向，即y方向。
     * 由于up只是处于yOz平面上，并不是与相机坐标系的y轴重合，所以可以先求出x轴，然后再叉积求出y轴。
     *
     * 三个轴向的求法确定之后，就可以处理相机了。
     * 相机的位置是相机坐标系的原点，一切线性变换都应该保持原点不变。
     * 而矩阵都是施加在世界坐标系上的，所以先用一个平移矩阵，将相机平移到世界坐标系原点，物体会同时平移，所以相机得到的画面不会变化。
     *
     * 接下来要将求得的相机坐标系的轴向旋转到与世界坐标系重合，这需要运用到旋转变换。
     * 相机坐标系到世界坐标系的旋转不是很好求，但让世界坐标系旋转到相机坐标系是很好求的，只需要将相机的x、y、z轴分别作为矩阵的第一、二、三列即可。
     * 然后求出这个矩阵的逆矩阵，就是将相机坐标系旋转到世界坐标系的矩阵了。
     * 并且，旋转变换矩阵是正交矩阵，所以它的逆矩阵就是它的转置。
     *
     * 所以视图变换矩阵的推到分为两步：
     * 1.平移变换矩阵：先左乘一个平移变换矩阵，平移的量为（-ex，-ey，-ez）
     * 2.旋转变换矩阵：先求出相机坐标系的x轴，y轴，z轴，然后将三根轴的向量作为矩阵的第一、二、三列，得到一个世界坐标系向相机坐标系旋转的矩阵，
     * 然后求出这个矩阵的转置（逆矩阵），就得到相机坐标系向世界坐标系的旋转变换矩阵。
     * */

    /*0.先将m置为单位阵*/
    m.setIdentity();

    // 临时矩阵temp，用于存储计算过程中产生的矩阵
    Eigen::Matrix4f temp;
    temp.setIdentity();

    // 将相机的位置、看向的点、顶部方向变成向量
    Eigen::Vector3f eye{ex, ey, ez};
    Eigen::Vector3f at{atx, aty, atz};
    Eigen::Vector3f up{upx, upy, upz};

    /*1.平移变换矩阵*/
    temp.col(3).head<3>() = -eye;

    // 施加平移变换
    m = temp * m;
    temp.setIdentity();

    /*2.求出相机坐标系的三根坐标轴*/
    // 2.1,先求出z轴负半轴，然后取负，得到z轴
//    Eigen::Vector3f zAxis = -(at - eye);
    Eigen::Vector3f zAxis = eye - at;// 也可以直接这样写,求出z轴正方向
    zAxis.normalize(); // 归一化

    // 2.2 求出x轴,用y轴叉乘z轴（如果上面求得的是z轴是负半轴，那么就应该用z叉乘y）
    Eigen::Vector3f xAxis = up.cross(zAxis);
    xAxis.normalize(); //  归一化

    // 2.3 求出y轴，用已经求出的z轴叉乘x轴即可（如果上面求的z轴是负半轴，那么就应该用x轴叉乘z轴）
    Eigen::Vector3f yAxis = zAxis.cross(xAxis);
    yAxis.normalize(); // 归一化

    /*3.求出世界坐标系向相机坐标系的旋转矩阵，然后转置（获得逆矩阵），得到相机向世界坐标系的旋转矩阵*/
    temp.col(0).head<3>() = xAxis; // 把x轴的三个分量赋值给矩阵第一列的前三个值
    temp.col(1).head<3>() = yAxis; // 把y轴的三个分量赋值给矩阵第二列的前三个值
    temp.col(2).head<3>() = zAxis; // 把z轴的三个分量赋值给矩阵第三列的前三个值

    // 现在的temp就是把世界坐标系旋转到相机坐标系的旋转矩阵，接下来求出它的逆矩阵
    temp.transposeInPlace(); // 自身转置
    // 现在的temp矩阵就是相机旋转到世界坐标系的旋转矩阵。

    // 应用旋转变换
    m = temp * m;
    temp.setIdentity();

    //请在上面空白处完成此函数

    mModelView *= m;
    return m;
}

Eigen::Matrix4f myRotateX(float angle) {
    float theta = angle / 180.0 * PI;
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面粘贴上次实验的代码

    m.setIdentity();

    /* 三角函数返回double类型，将其cast之后变为float类型（其实可以让编译器做隐式转换，但这样比较安全）*/
    auto cosa = static_cast<float>(cos(theta)), sina = static_cast<float>(sin(theta));
    /*在矩阵中设置为相应的值*/
    m(1, 1) = cosa, m(1, 2) = -sina;
    m(2, 1) = sina, m(2, 2) = cosa;


    //请在上面粘贴上次实验的代码

    mModelView *= m;
    return m;
}

Eigen::Matrix4f myRotateY(float angle) {
    float theta = angle / 180.0 * PI;
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面粘贴上次实验的代码

    m.setIdentity();//这句应该去掉

    /* 三角函数返回double类型，将其cast之后变为float类型（其实可以让编译器做隐式转换，但这样比较安全）*/
    auto cosa = static_cast<float>(cos(theta)), sina = static_cast<float>(sin(theta));
    /*在矩阵中设置为相应的值*/
    m(0, 0) = cosa, m(0, 2) = sina;
    m(2, 0) = -sina, m(2, 2) = cosa;


    //请在上面粘贴上次实验的代码

    mModelView *= m;
    return m;
}

Eigen::Matrix4f myRotateZ(float angle) {
    float theta = angle / 180.0 * PI;
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面粘贴上次实验的代码

    m.setIdentity();//这句应该去掉

    /* 三角函数返回double类型，将其cast之后变为float类型（其实可以让编译器做隐式转换，但这样比较安全）*/
    auto cosa = static_cast<float>(cos(theta)), sina = static_cast<float>(sin(theta));
    /*在矩阵中设置为相应的值*/
    m(0, 0) = cosa, m(0, 1) = -sina;
    m(1, 0) = sina, m(1, 1) = cosa;


    //请在上面粘贴上次实验的代码

    mModelView *= m;
    return m;
}


Eigen::Matrix4f myTranslate(float x, float y, float z) {
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面粘贴上次实验的代码

    m.setIdentity();//这句应该去掉

    /*平移矩阵比较简单，把第四列的一、二、三行分别设为x、y、z即可*/
    m(0, 3) = x;
    m(1, 3) = y;
    m(2, 3) = z;


    //请在上面粘贴上次实验的代码

    mModelView *= m;
    return m;
}

Eigen::Matrix4f myRotate(float angle, float x, float y, float z) {
    float theta = angle / 180.0 * PI;

    Eigen::Matrix4f m;

    //请在下面粘贴上次实验的代码

    m.setIdentity();//这句应该去掉


    /*绕任意轴旋转的思路是：先绕坐标轴x把旋转轴旋转到xOz平面，然后绕y轴把旋转轴旋转到与z轴重合，然后绕z轴旋转theta弧度，接着再把坐标轴绕着y、z轴旋转回原本的位置*/
    /*此过程中需要计算绕x、y轴旋转的角度rx与ry*/

    /*rx等于旋转轴在yOz平面的投影与z轴的夹角，求投影只需把旋转轴向量的x置为0*/
    /*ry等于旋转到yOz平面后的旋转轴与z轴的夹角，夹角求法同上*/

    /*对于夹角，可用点乘除以模长之积得到夹角余弦，再用公式计算正弦。*/
    /*不利用acos求出夹角，因为如果使用夹角的话，需要先调用acos再调用cos，等于没调用，而且可能造成精度损失和性能损失*/

    /* 先将x、y、z归一化，成为旋转轴*/
    Eigen::Vector3f rAxis(x, y, z);
    rAxis.normalized();

    float cosrx, cosry, cosrz;// 把旋转轴绕x、y、z轴旋转的角度的余弦值，下一行是正弦值
    float sinrx, sinry, sinrz;

    /*1.计算rx的正弦余弦，rx等于v在yOz平面上的投影与z轴的夹角的余弦值*/
    Eigen::Vector3f temp(0, rAxis[1], rAxis[2]);
//    cosrx=temp.dot(zAxis)/(temp.norm() * zAxis.norm()); // 由于z轴比较特殊，式子可以简化如下
    cosrx = temp[2] / temp.norm();// rx的余弦值
    sinrx = static_cast<float>(1.0 - pow(cosrx, 2));// rx的正弦值

    /*2.把旋转轴绕着x轴旋转到xOz平面,这里直接把公式套进代码，不另外使用矩阵乘法*/
//    Eigen::Vector3f temp(rAxis[0],rAxis[1]*cosrx-rAxis[1]*sinrx, rAxis[1]*sinrx+rAxis[2]*cosrx);// 由于向量处于xOz平面上，所以y值必为0，不需要进行计算，可简化如下
    rAxis = Eigen::Vector3f{rAxis[0], 0.0, rAxis[1] * sinrx + rAxis[2] * cosrx};// 旋转后的旋转轴

    /*3.计算ry的正弦余弦，ry等于旋转到xOz平面上的旋转轴与z轴的夹角*/
    cosry = rAxis[2] / rAxis.norm();
    sinry = static_cast<float>(1.0 - pow(cosrx, 2));

    /*临时矩阵，保存中间产生的旋转矩阵*/
    Eigen::Matrix4f t;
    t.setIdentity();

    /*4.把旋转轴绕着x轴旋转rx度，旋转到xOz平面*/
    t(1, 1) = cosrx, t(1, 2) = -sinrx;
    t(2, 1) = sinrx, t(2, 2) = cosrx;
    m = t * m;
    t.setIdentity();


    /*5.把旋转轴绕着y轴旋转ry弧度，与z轴重合*/
    t(0, 0) = cosry, t(0, 2) = sinry;
    t(2, 0) = -sinry, t(2, 2) = cosry;
    m = t * m;
    t.setIdentity();

    /*6.绕z轴旋转theta弧度*/
    cosrz = cos(theta), sinrz = sin(theta);
    t(0, 0) = cosrz, t(0, 1) = -sinrz;
    t(1, 0) = sinrz, t(1, 1) = cosrz;
    m = t * m;
    t.setIdentity();

    /*7.旋转轴绕着y轴旋转-ry弧度*/
    t(0, 0) = cosry, t(0, 2) = -sinry;
    t(2, 0) = sinry, t(2, 2) = cosry;
    m = t * m;
    t.setIdentity();

    /*8.旋转轴绕着x轴旋转-rx弧度*/
    t(1, 1) = cosrx, t(1, 2) = sinrx;
    t(2, 1) = -sinrx, t(2, 2) = cosrx;
    m = t * m;
    t.setIdentity();

    /*m即是绕任意轴旋转的旋转矩阵*/


    //请在上面粘贴上次实验的代码

    mModelView *= m;

    return m;
}

Eigen::Matrix4f myFrustum(float l, float r, float b, float t, float n, float f) {
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面空白处完成此函数

    /*透视投影矩阵推导过程*/
    /*
     * 透视投影矩阵的视椎体是一个四棱台体，台体的底面（较大的面）为远平面f，顶面（较小的面）为近平面n
     * 透视投影矩阵的本质：把视椎体转换为一个正方体[-1,1][-1,1][-1,1]
     * 透视矩阵的实现方式：先把四棱台体转化成长方体，然后用正交投影把长方体转化为正方体
     *
     * 透视投影的特性：物体在平面上的大小随z分量成反比。
     * 可以知道，离相机较远的物体会被压缩变小，越远压缩得越厉害，最终实现近大远小的效果。
     *
     * 在四棱台体变成长方体的过程中，近平面和远平面的位置应当是不变的，进平面大小是不变的，远平面会被压缩到和近平面一样大。
     * 可知，压缩之后的坐标的z`分量应当与压缩前坐标的z分量相同，即z=n时，z`也要等于n，z=f时，z`也要等于f。
     * 压缩前后，z分量是不变的，x、y应当会向z轴收缩，z越大，x、y收缩越厉害，所以x`,y`的变化与z成反比。
     *
     * x、y是如何变化的？
     * 假设在视椎体内有一点（x，y，z），那么它和它投影在观察平面上的点（x`,y`,z`)以及原点在一条直线上。
     * 让该点和它的投影点垂直z轴作线段，可以得到一组相似三角形。由相似关系可以得到：
     * n / z = x` / x = y` / y
     * 所以压缩之后，x、y、z的变化为：
     * x` = x * n / z
     * y` = y * n / z
     * z` = z
     * 这就是压缩变换的方程组。
     *
     *
     * ****************************第一次推导（这个推导似乎在齐次上有问题，总之失败了，请查看第二次推导）***************************
     * 假设有一个4*4矩阵可以实现压缩x、y，并且可以实现z`与z保持相同
     * 矩阵第一行，变换后只保留x，并且x变换为 n / z * x，于是第一行为[n / z, 0, 0, 0]
     * 矩阵第二行，变换后只保留y，并且y变换为 n / z * y，于是第二行为[0, n / z, 0, 0]
     *
     * 矩阵第三行，变换只保留z的话，也就是只有第三行有元素，无法实现z=n时z`=n，z=f时z`=f的效果，
     * 但第三行不会对x，y造成影响，所以第三行的第三、四列肯定都有元素，假设第三行为[0,0,a,b]
     * 此时涉及到齐次坐标的计算，假设原本坐标（x，y，z），设齐次坐标为（x，y，z，w）
     * 那么变换后的z` = a * z + b * w，由z=n时z`=n，z=f时z`=f这一条件可以得到方程组
     * n = a * n + b * w
     * f = a * f + b * w
     * 由于涉及到a、b、w三个未知数，却只有两个方程，需要结合第四行考虑。
     * 有第四行的参与，会对齐次坐标发生转化，w将会转化为1.0。
     * 假设第4行为[0,0,0,c]，那么有1.0 = c * w ，又多了一个未知数，但只有三个方程。放弃这种假设。
     * 再假设第4行为[0,0,c,d]，那么有1.0 = c * z + d * w，引入了更多未知数，方程仍然不够。。。
     * 继续假设第4行为[0,0,c,0]，那么1.0 = c * z,就可以得到c = 1 / z，但仍然无法解出a、b。
     *
     * **********************************************************************
     *
     *
     * ***********************第二次推导**********************************
     *
     * 第一次推导，打算自行完成齐次，所以矩阵中会出现z分量，但这是一个变量，我们所求的矩阵必须是常量构成的。
     * 所以，应当将z从变换矩阵中消去，且不能再自己的代码中消去，所以要用某种办法，让OpenGL自己完成这件事。
     *
     * 考虑到输入的坐标是三维笛卡尔坐标，而OpenGL进行变换是在四维齐次坐标下进行的。这一齐次过程是由OpenGL完成的。
     * OpenGL先将输入的（x，y，z）变为（x，y，z，1.0），然后经过一系列变换变成（x`，y`，z`，w）,所有变换完成之后，
     * 齐次坐标的每个元素除以w，变成（x` / w， y` / w， z` / w， 1.0），然后再变为笛卡尔坐标（x` / w， y` / w， z` / w）
     *
     * 齐次的工作由OpenGL自动完成，假如我们将变换所需的变量z在变换时转移到齐次分量的位置，
     * 那么在OpenGL做齐次的时候，就会将变量z的值作用到x、y上，完成我们所需的变换。
     *
     * 于是重新改变思路。我们需要先将透视的视椎体变成正交视椎体，也就是让（x，y，z）变为（x * n / z, y * n / z, z)
     * 如果考虑齐次，那就是让（x，y，z，1.0）变为（x * n / z, y * n / z, z， 1.0)
     * 由于齐次在OpenGL自动完成，所以我们只需要在代码中得到坐标（x * n / z, y * n / z, z， 1.0)在 齐次之前的坐标。
     *
     * （x，y，z，1.0）应当在变换之后变成（x`，y`，z`，w），齐次之后变成（x` / w， y` / w， z` / w， 1.0）。
     * 所以（x` / w， y` / w， z` / w， 1.0）应当与（x * n / z, y * n / z, z， 1.0)是相同的。
     * 如果让x`==x， y`==y，那么w = z / n，那么在压缩之后，被OpenGL齐次之前的坐标应该是（x, y, z * z / n, z / n）
     *
     * 需要实现的就是，将（x，y，z，1.0）变换成（x, y, z * z / n, z / n）
     * 假设有一个4*4的矩阵可以实现这个变换：
     * x、y在变换前后相同，所以第一行为[1,0,0,0],第二行为[0,1,0,0]
     *
     * 第四行决定了变换后的齐次位，先考虑第四行，在这一行让1.0变为 z / n.
     * 第四行不影响x，y，所以第一、二列为0。第四行必须能得到z的值，所以第三列绝对不能为0，假设为c。
     * 由于第四列只能得到一个方程，只能解出一个未知数，所以可以先假设第4列为0，如果无解再作打算。所以第四行就是[0,0,c,0]。
     * 可以知道 z / n = z * c，那么可以解得c = 1 / n。
     * 既然有解，那么假设正确。第四行就是[0,0,1/n,0]
     *
     * 第三行影响z的值，需要满足z=n时z`=n，z=f时z`=f的条件，两个条件可以构造两个方程，不妨假设第三行有两个未知数。
     * 第三行不影响x、y，所以第三行的第三、四列不为0，假设第3行为[0,0,a,b]
     *
     * 变换前坐标为(x,y,z,1.0),已知压缩变换后（x, y, z * z / n, z / n）
     * 矩阵第三行点乘坐标可以得到方程：z * z / n = a * z + b
     * 带入条件z=n时z`=n，z=f时z`=f，可得到方程组：
     * n * n / n = a * n + b
     * f * f / n = a * f + b
     * 两个未知数和两个方程，可解得
     * a = (f + n) / n
     * b = -f
     *
     * 注意从n到f是z轴正方向，而相机视线的方向是z轴负方向，所以要把n、f取负
     * 于是得到压缩矩阵为
     * [1   0   0       0]
     * [0   1   0       0]
     * [0   0   (n+f)/n f]
     * [0   0   -1/n     0]
     *
     * 可以发现，z这个变量从矩阵中消失了，我们的目的达成。
     *
     * 压缩变换理应修改x、y的值而不是z的值，但是这个矩阵不会修改x、y，对x、y的修改是在OpenGL做齐次除法时完成的。
     * 所以这次齐次除法的操作，不仅实现了齐次坐标向笛卡尔坐标的转变，而且同时实现了透视效果，就是所谓的透视除法。
     *
     * 这个变换将让视椎体变成一个长方体[l,r][b,t][n,f]
     *
     * *******************************************************************
     *
     * 压缩矩阵应用之后，再应用一次正交投影，就可以实现透视投影。
     * 正交投影矩阵的推导在myOrtho中。在这个函数里直接使用推导好的矩阵。
     * */

    /*0.先重置为单位阵*/
    m.setIdentity();

    // 临时矩阵,用于保存计算过程中间产生的变换矩阵
    Eigen::Matrix4f temp;
    temp.setIdentity();

    /******************首先得到压缩矩阵**********************/
    temp(2, 2) = (f + n) / n;
    temp(2, 3) = f; // 注意f要取负
    temp(3, 2) = -1.0f / n;//n要取负
    temp(3, 3) = 0.0f;

    // 施加压缩变换
    m = temp * m;
    temp.setIdentity();

    /******************应用正交投影矩阵**********************/
    /* 透视投影视椎体在变成正交投影视椎体之后
     * 左平面为l，右平面为r
     * 底部平面为b，顶部平面为t
     * 近平面为n，远平面为f
     *
     * 接下来施加正交投影变换，
     * 分为两步：
     * 1.平移
     * 2.缩放
     *
     * 需要注意近平面到远平面指向z轴正方向，在正交变换时需要乘以负一变成z轴负方向
     * */

    /*首先做平移变换，矩阵参数的计算详见myOrtho函数，此处直接使用计算好的值*/
    // temp变成平移矩阵
    temp(0, 3) = -(l + r) / 2.0f;
    temp(1, 3) = -(b + t) / 2.0f;
    temp(2, 3) = (n + f) / 2.0f; // 注意n和f要乘以负一，负号提出后与括号前的负号会抵消

    // 施加平移变换
    m = temp * m;
    temp.setIdentity();

    /*接下来做缩放变换，求缩放倍数的过程见myOrtho函数，此处直接使用推导好的值*/
    temp(0, 0) = 2.0f / (r - l);
    temp(1, 1) = 2.0f / (t - b);
    temp(2, 2) = 2.0f / (n - f); // 注意n和f要乘以负一，f-n变成-f+n，直接写成n-f

    // 施加变换
    m = temp * m;
    temp.setIdentity();

    /**
     * 以上是代码层面需要实现的所有变换，此时还没有完全实现透视。
     * 透视的完全实现还需要OpenGL进行齐次时进行透视除法，但这是OpenGL负责的。
     * **/

    //请在上面空白处完成此函数

    mProjection *= m;
    return m;
}

Eigen::Matrix4f myOrtho(float l, float r, float b, float t, float n, float f) {
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面空白处完成此函数

    /*正交投影矩阵推导过程*/
    /*
     * 正交投影矩阵的本质：把视椎体（一个长方体[l,r][b,t][n,f]）转换为一个正方体[-1,1][-1,1][-1,1]
     * 矩阵的实现方式： 先把长方体的几何中心平移到原点，然后再进行缩放变换变成正方体，因为缩放变换属于线性变换，（0,0,0）变换后仍是（0,0,0），
     * 为了让长方体缩放时保持几何中心位置不变，就将其先平移到原点，然后再缩放。
     *
     * 需要注意的是，相机视线的方向是面向z轴负方向的，而n到f是指向z轴正方向的，要让n到f映射到-1,1的区间，需要让n、f乘以负一
     * */

    /*0.先重置为单位阵*/
    m.setIdentity();

    // 临时矩阵,用于保存计算过程中间产生的变换矩阵
    Eigen::Matrix4f temp;
    temp.setIdentity();

    /*1.计算出长方体的几何中心，然后做平移变换*/
    float centerX = (l + r) / 2.0f;
    float centerY = (b + t) / 2.0f;
    float centerZ = -(n + f) / 2.0f;// 注意n、f要取负

    // temp变成平移矩阵
    temp(0, 3) = -centerX;
    temp(1, 3) = -centerY;
    temp(2, 3) = -centerZ;

    // 施加变换
    m = temp * m;
    // 重置临时矩阵
    temp.setIdentity();


    // 接下来计算三个方向的缩放倍数
    // 首先计算长方体在x、y、z三个轴向的边长
    float lengthX = r - l; // 右减去左，是x轴正方向
    float lengthY = t - b; // 上减去下，是y轴正方向
    float lengthZ = n - f; // 远减去近，是z轴正方向，这里n、f都取负了

    /*2.计算x轴（左、右）方向的缩放倍数*/
    /*正方体边长为2,长方体在此轴向上长度为lengthX,
     * 缩放倍数等于 1 / lengthX * 2，
     * 即2/lengthX*/
    float scaleX = 2.0f / lengthX;

    /*3.计算y轴（底，顶）方向的缩放倍数*/
    /*正方体边长为2,长方体在此轴向上长度为lengthY，
     * 缩放倍数等于 1 / lengthY * 2,
     * 即2/lengthY*/
    float scaleY = 2.0f / lengthY;

    /*4.计算z轴（近，远）方向的缩放倍数*/
    /*正方体边长为2,长方体在此轴向上长度为lengthZ，
     * 缩放倍数等于 1 / lengthZ * 2,
     * 即2/lengthZ*/
    float scaleZ = 2.0f / lengthZ;

    /*5.设置缩放矩阵，然后做缩放变换*/
    temp(0, 0) = scaleX;
    temp(1, 1) = scaleY;
    temp(2, 2) = scaleZ;

    // 施加变换
    m = temp * m;
    temp.setIdentity();

    //请在上面空白处完成此函数

    mProjection *= m;

    return m;
}

Eigen::Matrix4f myViewPort(float x, float y, float w, float h) {
    Eigen::Matrix4f m;
    m.setZero();

    //请在下面空白处完成此函数

    /*视口变换的作用
     * 顶点数据经过模型变换（平移、旋转、缩放）之后，经过视图变换，变为相机坐标系，进入相机的视椎体内部。
     * 然后经过投影变换，视椎体中的模型变成一个边长为2的立方体中的顶点。
     * 这个边长为2的立方体叫做NDC空间。
     *
     * 视口变换，就是将NDC空间中的顶点数据映射到一个二维的矩形区域内,也就是屏幕上。
     * */

    /*视口变换矩阵的推导
     *
     * 在立方体中，x、y、z三个坐标都从-1到1变化。
     * 视口左下角是（0,0），右上角是（w，h）
     * 视口在屏幕上的位置是由传入的参数x、y决定的。
     *
     * 先把x、y映射到屏幕上，变换是线性的，所以可以用 x_S = k * x_N + b + x 这样的线性方程求解（x_S是屏幕坐标，x_N是NDC空间内的坐标，x是参数中的x）。
     * 当x=-1，y=-1时，视口坐标x`=0，y`=0，当x=1，y=1是，视口坐标x`=w，y`=h，带入方程得到方程组，解得x、y的映射关系为
     * x_S = w * x_N / 2.0f + w / 2.0f + x
     * y_S = h * y_N / 2.0f + h / 2.0f + y
     *
     * z坐标不会再视口变换中处理，而是用于之后的深度测试，所以矩阵第三行
     * NDC坐标齐次为（x_N，y_N，z_N，1.0）的顶点，经过变换后变为
     * （w * x_N / 2.0f + w / 2.0f + x，h * y_N / 2.0f + h / 2.0f + y，z，1.0）
     * 可以推导出矩阵各行为：
     * [w / 2.0f, 0, 0, x + w / 2.0f]
     * [0, h / 2.0f, 0, y + h / 2.0f]
     * [0,0,1,0]
     * [0,0,0,1]
     * */

    /*0.先重置为单位阵*/
    m.setIdentity();

    // 临时矩阵,用于保存计算过程中间产生的变换矩阵
    Eigen::Matrix4f temp;
    temp.setIdentity();

    /*1.先计算出w/2，h/2*/
    float w2 = w / 2.0f, h2 = h / 2.0f;
    temp(0, 0) = w2;
    temp(0, 3) = x + w2;
    temp(1, 1) = h2;
    temp(1, 3) = y + h2;

    // 施加变换
    m = temp * m;
    temp.setIdentity();

    //请在上面空白处完成此函数

    mViewPort = m;

    return m;
}

Eigen::Matrix4f myPerspective(float fovy, float aspect, float zNear, float zFar) {
    Eigen::Matrix4f m;
    m.setZero();
    const float pi = 3.1415926535897932384626433832795;

    //请在下面空白处完成此函数

    // glPerspective的作用也是生成一个透视投影矩阵
    /*透视投影矩阵的推导在myFrustum函数中详细解释了，在此不作赘述。
     *
     * 在这个函数中所要做的，就是利用视角、宽高比例、近平面、远平面的位置求得l、r、b、t、n、f六个参数。从而得到视椎体。
     * 然后结合myFrustum函数中推导的结果，利用这六个参数来构造变换矩阵。
     * */

    /*透视投影视椎体参数的推导
     *
     * 首先zNear平面就是视椎体的近平面，所以n=zNear，zFar平面就是视椎体的远平面，所以f=zFar。
     * 首先视角的角平分线一定是z轴负半轴，视角的边界线、z轴和近平面构成一个直角三角形
     * 那么t和b就可以计算出来，t等于n * tan(fovy / 2.0),而b就等于-t。
     * 关于t，b的正负，因为从b到t的方向是y轴正方向，所以t一定为正数，b一定为负数。
     * 而输入的n是正的，fovy必定小于180度，所以三角函数计算出的值也是正的，所以可以利用上面的公式
     *
     * 然后利用宽高比就可以计算出l和r。
     * 高度可以有t-b得到，然后用高度乘以宽高比就可以得到宽度，和t、b类似，l和r也是对称的，r-l等于宽度，所以r等于宽度的一半，而l等于负的r
     * h = t - b;
     * w = aspect * h;
     * r = w / 2.0;
     * l = -r;
     * 于是得到了l和r。
     * 这样一来，构建透视投影视椎体的6个参数都得到了。
     *
     * 接下来，利用这六个参数计算透视投影矩阵
     * */


    /*0.m置为单位阵*/
    m.setIdentity();

    // 临时矩阵，存储计算过程中产生的矩阵。
    Eigen::Matrix4f temp;
    temp.setIdentity();

    /*********1.求得透视投影视椎体的所有参数***********/
    float l, r, b, t, n, f;

    // n和f
    n = zNear, f = zFar;

    // b和t，需要主义，fovy是角度制，而tan是弧度制，所以要先把角度转换成弧度
//    fovy = fovy * pi / 180.0f; // 先转换为弧度
//    t = n * tan(fovy / 2.0f);
    t = n * tan(fovy * pi / 360.0f); // 角度转弧度，同时平分角度
    b = -t;

    // l和r
    float w = aspect * (t - b);
    r = w / 2.0f;
    l = -r;

    /*到此其实可以直接调用myFrustum，然后传入那六个参数*/
//    myFrustum(l, r, b, t, n, f);
    /******2.求得透视投影视椎体变换为正交投影视椎体的矩阵******/
    temp(2, 2) = (f + n) / n;
    temp(2, 3) = f; // 注意f要取负
    temp(3, 2) = -1.0f / n; // 注意n要取负
    temp(3, 3) = 0.0f;

    // 施加压缩变换
    m = temp * m;
    temp.setIdentity();

    /***************3.求得正交投影的平移变换的矩阵**************/
    // temp变成平移矩阵
    temp(0, 3) = -(l + r) / 2.0f;
    temp(1, 3) = -(b + t) / 2.0f;
    temp(2, 3) = (n + f) / 2.0f; // 注意n和f要取负

    // 施加平移变换
    m = temp * m;
    temp.setIdentity();

    /***********4.求得正交投影的缩放变换的矩阵**************/
    /*接下来做缩放变换，求缩放倍数的过程见myOrtho函数，此处直接使用推导好的值*/
    temp(0, 0) = 2.0f / (r - l);
    temp(1, 1) = 2.0f / (t - b);
    temp(2, 2) = 2.0f / (n - f); // 注意n和f要取负

    // 施加变换
    m = temp * m;
    temp.setIdentity();


    //请在上面空白处完成此函数

    mProjection *= m;
    return m;
}