#pragma once
#include "stdafx.h"

#ifdef _WINDOWS
#else
#define _USE_MATH_DEFINES // M_PI etc.
#include <iostream>
#include <memory>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>
#include <math.h>
#include <map>
#endif

using namespace std;

namespace OpenGLUtils {

    // Forward declarations
    template <class NumType, int m, int n> class mat;
    template <class NumType, int n> class sqmat;
    template <class NumType> class Triple;
    template <class NumType> class Vector;
    template <class NumType> class Quaternion;
    template <class NumType> class AngleAndAxis;
    template <class NumType, int m, int n> ostream& operator<< (ostream&, mat<NumType,m,n>&);
    template <class NumType> sqmat<NumType,4> ModelMatrix(Triple<NumType>&, Quaternion<NumType>&);
    template <class NumType> Vector<NumType> operator* (sqmat<NumType,4>&, Vector<NumType>&);

    template <class NumType, int m, int n> class mat {
        protected:
            NumType     m_data[m][n];
        public:
            mat() {};
            mat(NumType[][n]);
            void operator= (NumType[][n]);
            NumType* data() { return &m_data[0][0]; };
            NumType* operator[] (int row) { return m_data[row]; };
            mat<NumType,n,m> T();
            friend ostream& operator<< <> (ostream&, mat<NumType,m,n>&);
    };

    template <class NumType, int n> class sqmat : public mat<NumType,n,n> {
        public:
            sqmat() {};
            sqmat(NumType l_data[][n]) : mat(l_data) {};
            sqmat(mat& l_base) : mat(l_base) {};
            sqmat<NumType,n> operator* (sqmat<NumType,n>);
            sqmat<NumType,n> Inverse();
            void LoadIdentity();
            //void LoadZero();
            //friend Vector<NumType> operator* <> (sqmat<NumType,4>&, Vector<NumType>&);
    };

    void GenericErrorDumper(const char* text);
    void GenericErrorDumper(string& text);
    void DumpOpenGLError(GLenum status);
    GLboolean CheckForOpenGLErrors();

    class MouseInfo {
        private:
            //static const float MouseSpeed;
            GLboolean   m_leftdown;
            GLboolean   m_rightdown;
            GLint       m_xdelta;
            GLint       m_ydelta;
        public:
            MouseInfo();
            const float GetMouseSpeed() { return 0.5f; }; //return MouseSpeed; };
            GLboolean GetLeftDown() { return m_leftdown; };
            void SetLeftDown(GLboolean l_leftdown) { m_leftdown = l_leftdown; };
            GLboolean GetRightDown() { return m_rightdown; };
            void SetRightDown(GLboolean l_rightdown) { m_rightdown = l_rightdown; };
            GLint GetXDelta() { return m_xdelta; };
            GLint GetYDelta() { return m_ydelta; };
            void AddMouseMove(GLint x, GLint y) { m_xdelta += x; m_ydelta+= y; };
            void ResetMouseMove() { m_xdelta = 0; m_ydelta = 0; };
            void ResetAll();
    };

    //const float MouseInfo::MouseSpeed = 0.5f;

    class DisplayContext {
        private:
            uint16_t              m_width, m_height;
            uint16_t              m_offset_x, m_offset_y;
            sqmat<float,4>        m_Perspective;
            MouseInfo             m_MouseInfo;
        protected:
            float                 m_Near, m_Far;
            void SetSize(uint16_t l_width, uint16_t l_height) { m_width = l_width; m_height = l_height; };
            void SetOffset(uint16_t x, uint16_t y) { m_offset_x = x; m_offset_y = y; };
        public:
            DisplayContext() { m_Near = 1.0f; m_Far = 10000.0f; };
            uint16_t GetWidth() { return m_width; };
            uint16_t GetHeight() { return m_height; };
            uint16_t GetOffsetX() { return m_offset_x; };
            uint16_t GetOffsetY() { return m_offset_y; };
            sqmat<float,4> GetPerspective() { return m_Perspective; };
            void CalcPerspective();
            MouseInfo* MouseInfo() { return &m_MouseInfo; };
            virtual void SwapBuff() =0;
            void ReSizeGLScene();
            void ReSizeGLScene(uint32_t,uint32_t);
#ifndef _WINDOWS
            int TakeScreenShot(const char *);
#endif
    };

#ifdef _WINDOWS
    class WindowsDisplayContext : public DisplayContext {
        private:
            HDC          m_hDC;            // Private GDI Device Context
            HGLRC        m_hRC;            // Permanent Rendering Context
            HWND         m_hWnd;           // Holds Our Window Handle
            //HINSTANCE    m_hInstance;      // Holds The Instance Of The Application
            bool         m_fullscreen;
            bool         m_active;
        public:
            WindowsDisplayContext();
            void SwapBuff();
            bool Init(const wchar_t * title, int bits);
            void KillGLWindow();
            HDC hDC() { return m_hDC; };
            HGLRC hRC() { return m_hRC; };
            HWND hWnd() { return m_hWnd; };
            bool fullscreen() { return m_fullscreen; };
            void ToggleFullScreen() { m_fullscreen = !m_fullscreen; };
            virtual LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);
            bool RegisterInputs();
    };
#endif

#ifdef _EGL
    class EGLDisplayContext : public DisplayContext {
        private:
            EGLNativeWindowType   m_hWnd;
            auto_ptr<EGLDisplay>  m_eglDisplay;
            auto_ptr<EGLContext>  m_eglContext;
            auto_ptr<EGLSurface>  m_eglSurface;
            Display*              m_Xdisplay;
            bool                  m_XWindows;
        public:
            EGLDisplayContext() {};
            EGLBoolean Init(const char *, const EGLint *, const bool);
            EGLNativeWindowType* hWnd() { return &m_hWnd; };
            EGLDisplay* eglDisplay() { return m_eglDisplay.get(); };
            EGLSurface* eglSurface() { return m_eglSurface.get(); };
            Display* x_display() { return m_Xdisplay; };
            bool XWindows() { return m_XWindows; }
            void SetXWindows(bool l_XWindows) { m_XWindows = l_XWindows; };
            void SwapBuff();
    };

    void DumpError(EGLint);

#endif

    typedef map<int, string> ParamBindings;

    GLuint CreateTextureFromFile(const char *);
    GLuint LoadShader(GLenum, const char *);
    GLuint LoadProgram(const string&, const string&, const ParamBindings& AttrBindings);

    template <class NumType, int m, int n> mat<NumType,m,n>::mat(NumType l_data[][n]) {
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) {
                m_data[i][j] = l_data[i][j];
            }
        }
    }

    template <class NumType, int m, int n> void mat<NumType,m,n>::operator=(NumType l_data[][n]) {
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) {
                m_data[i][j] = l_data[i][j];
            }
        }
    }

    template <class NumType, int n> sqmat<NumType,n> sqmat<NumType,n>::operator* (sqmat<NumType,n> rhs) {
        int i,j,k;
        sqmat<NumType,n> result;

        for (i=0; i<n; i++) {
            for (j=0; j<n; j++) {
                NumType sum = 0;
                for (k=0; k<n; k++) {
                    sum += m_data[i][k] * rhs[k][j];
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    template <class NumType, int m, int n> mat<NumType,n,m> mat<NumType,m,n>::T() {
        mat<NumType,n,m> result;
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) {
                result[j][i] = m_data[i][j];
            }
        }
        return result;
    }

    // Gird yourself for some hot Gauss-Jordan action
    // Note that cells which must be zero (by construction) are not explicitly calculated
    template <class NumType, int n> sqmat<NumType,n> sqmat<NumType,n>::Inverse() {
        int row, rank, col, maxlead;
        NumType tmp[n][n*2];
        //NumType tmprow[n*2];
        NumType tmp2;

        // Create "augmented" matrix from original matrix and identity
        for (row=0; row<n; row++) {
            for (col=0; col<n; col++) {
                tmp[row][col] = m_data[row][col];
                if (row == col) {
                    tmp[row][col+n] = 1;
                } else {
                    tmp[row][col+n] = 0;
                }
            }
        }

        // Subtract each row from the rows below it
        // Now with added pivoting
        for (row=0; row<n; row++) {
            // Find the row with the biggest (absolute) lead value
            maxlead = row;
            for (rank=(row+1); rank<n; rank++) {
                if (abs(tmp[rank][row]) > abs(tmp[maxlead][row])) {
                    maxlead = rank;
                }
            }

            for (col=row; col<(n*2); col++) {
                tmp2 = tmp[row][col];
                tmp[row][col] = tmp[maxlead][col];
                tmp[maxlead][col] = tmp2;
            }

            // Divide chosen pivot row by the leading element
            // While we're at it, swap it with the first row
            //for (col=row; col<(n*2); col++) {
            //    tmprow[col] = tmp[row][col];
            //}
            for (col=row+1; col<(n*2); col++) {
                tmp[row][col] = tmp[row][col] / tmp[row][row];
                //tmp[row][col] = tmp[maxlead][col] / tmp[maxlead][row];
            }
            tmp[row][row] = 1;
            //for (col=row; col<(n*2); col++) {
            //    tmp[maxlead][col] = tmprow[col];
            //}

            // Subtract this row times leading element of each row
            for (rank=(row+1); rank<n; rank++) {
                for (col=row+1; col<(n*2); col++) {
                    tmp[rank][col] = tmp[rank][col] - (tmp[row][col] * tmp[rank][row]);
                }
                tmp[rank][row] = 0;
            }
        }

        // Now go back up the rows from the bottom, eliminating each cell above the diagonal
        for (row=(n-1); row>0; row--) {
            for (rank=0; rank<row; rank++) {
                for (col=row+1; col<(n*2); col++) {
                    tmp[rank][col] = tmp[rank][col] - (tmp[row][col] * tmp[rank][row]);
                }
                tmp[rank][row] = 0;
            }
        }

        // All done so copy over the result
        sqmat<NumType,n> result;
        for (row=0; row<n; row++) {
            for (col=0; col<n; col++) {
                result[row][col] = tmp[row][col+n];
            }
        }

        return result;
    }

    template <class NumType, int n> void sqmat<NumType,n>::LoadIdentity() {
        for (int i=0; i<n; i++) {
            for (int j=0; j<n; j++) {
                if (i=j) {
                    m_data[i][j] = static_cast<NumType>(1);
                } else {
                    m_data[i][j] = static_cast<NumType>(0);
                }
            }
        }
    }

    template <class NumType, int m, int n> ostream& operator<< (ostream& out, mat<NumType,m,n>& Matrix) {
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) {
                out << Matrix[i][j];
                if (j==(n-1)) {
                    out << endl;
                } else {
                    out << "\t";
                }
            }
        }
        return out;
    }

    template <class NumType> sqmat<NumType,4> PerspectiveMatrix(NumType fovy, NumType aspect, NumType zNear, NumType zFar) {
        NumType f = 1/tan(fovy*static_cast<NumType>(M_PI)/360);
    
        NumType data[4][4] = {f/aspect, 0,                             0,                                 0,
                                     0, f,                             0,                                 0,
                                     0, 0, (zFar + zNear)/(zNear - zFar), 2 * zFar * zNear / (zNear - zFar),
                                     0, 0,                            -1,                                 0};

        sqmat<NumType,4> result(data);

        return result;
    }

template <class NumType> class Triple {
    protected:
        NumType   m_data[3];
    public:
        Triple() {};
        Triple(NumType l_x, NumType l_y, NumType l_z);
        Triple(NumType[3]);
        NumType x() const { return m_data[0]; };
        NumType y() const { return m_data[1]; };
        NumType z() const { return m_data[2]; };
        NumType* data() { return m_data; };
        NumType operator [] (int n) const { return m_data[n]; };
        Triple operator + (const Triple&) const;
        Triple operator - (const Triple&) const;
        Triple operator * (const NumType&) const;
        sqmat<NumType,4> ToTranslation();
        friend sqmat<NumType,4> ModelMatrix <> (Triple<NumType>&, Quaternion<NumType>&);
};

template <class NumType> Triple<NumType>::Triple(NumType l_x, NumType l_y, NumType l_z) {
    m_data[0] = l_x;
    m_data[1] = l_y;
    m_data[2] = l_z;
}

template <class NumType> Triple<NumType>::Triple(NumType l_coords[3]) {
    m_data[0] = l_coords[0];
    m_data[1] = l_coords[1];
    m_data[2] = l_coords[2];
}

template <class NumType> Triple<NumType> Triple<NumType>::operator + (const Triple& b) const {
    return Triple(x()+b.x(),y()+b.y(),z()+b.z());
}

template <class NumType> Triple<NumType> Triple<NumType>::operator - (const Triple& b) const {
    return Triple(x()-b.x(),y()-b.y(),z()-b.z());
}

template <class NumType> Triple<NumType> Triple<NumType>::operator * (const NumType& k) const {
    return Triple(k*x(),k*y(),k*z());
}

template <class NumType> sqmat<NumType,4> Triple<NumType>::ToTranslation() {
    sqmat<NumType,4> result;
    result[0][0] = 1.0f,
    result[0][1] = 0.0f;
    result[0][2] = 0.0f;
    result[0][3] = m_x;
    result[1][0] = 0.0f,
    result[1][1] = 1.0f;
    result[1][2] = 0.0f;
    result[1][3] = m_y;
    result[2][0] = 0.0f,
    result[2][1] = 0.0f;
    result[2][2] = 1.0f;
    result[2][3] = m_z;
    result[3][0] = 0.0f,
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
    return result;
}

template <class NumType> Triple<NumType> operator * (const NumType& k, const Triple<NumType>& t) {
    return t * k;
}

template <class NumType> Triple<NumType> operator - (const Triple<NumType>& b) {
    return Triple(-b.x(),-b.y(),-b.z());
}

template <class NumType> class Vector : public Triple<NumType> {
    public:
        Vector() : Triple() {};
        Vector(Triple t) : Triple(t) {};
        Vector(NumType l_x, NumType l_y, NumType l_z) : Triple<NumType>(l_x, l_y, l_z) {};
        NumType Magnitude();
        Vector<NumType> Normalise();
        Vector<NumType> operator * (const NumType& k) const { return Vector(k*x(),k*y(),k*z()); };
        Vector<NumType> operator * (Quaternion<NumType>&);
        //friend Vector<NumType> operator* (sqmat<NumType,4>&, Vector<NumType>&);
};

template <class NumType> NumType Vector<NumType>::Magnitude() {
    return sqrt(pow(x(),2) + pow(y(),2) + pow(z(),2));
}

template <class NumType> Vector<NumType> Vector<NumType>::Normalise() {
    float magnitude = this->Magnitude();
    if (magnitude == 0.0f)
        return Vector(0.0f,1.0f,0.0f);
    else
        return Vector(x()/magnitude, y()/magnitude, z()/magnitude);
}

template <class NumType> Vector<NumType> Vector<NumType>::operator * (Quaternion<NumType>& rhs) {
    float w = rhs.w();
    float x = rhs.x();
    float y = rhs.y();
    float z = rhs.z();
    Vector retval(m_data[0] * (1 - 2*pow(y,2) - 2*pow(z,2)) + m_data[1] * (2*x*y - 2*w*z) + m_data[2] * (2*x*z + 2*w*y),
                  m_data[0] * (2*x*y + 2*w*z) + m_data[1] * (1 - 2*pow(x,2) - 2*pow(z,2)) + m_data[2] * (2*y*z - 2*w*x),
                  m_data[0] * (2*x*z - 2*w*y) + m_data[1] * (2*y*z + 2*w*x) + m_data[2] * (1 - 2*pow(x,2) - 2*pow(y,2)));
    //Vector retval(m_x * (1 - 2*pow(y,2) - 2*pow(z,2)) + m_y * (2*x*y + 2*w*z) + m_z * (2*x*z - 2*w*y),
    //              m_x * (2*x*y - 2*w*z) + m_y * (1 - 2*pow(x,2) - 2*pow(z,2)) + m_z * (2*y*z + 2*w*x),
    //              m_x * (2*x*z + 2*w*y) + m_y * (2*y*z - 2*w*x) + m_z * (1 - 2*pow(x,2) - 2*pow(y,2)));
    return retval;
}

template <class NumType> Vector<NumType> operator - (const Vector<NumType>& b) {
    return Vector<NumType>(-b.x(),-b.y(),-b.z());
}

template <class NumType> Vector<NumType> cross_product(Vector<NumType> a, Vector<NumType> b) {
    return Vector<NumType>(a.y() * b.z() - a.z() * b.y(),
                           a.z() * b.x() - a.x() * b.z(),
                           a.x() * b.y() - a.y() * b.x());
}

template <class NumType> NumType dot_product(Vector<NumType> a, Vector<NumType> b) {
    return NumType(a.x() * b.x() + a.y() * b.y() + a.z() * b.z());
}

template <class NumType> class Quaternion {
    private:
        NumType       m_w,m_x,m_y,m_z;
        template <int n> void RotationMatrix(sqmat<NumType,n>&);
    public:
        Quaternion (NumType a, NumType b, NumType c, NumType d) { m_w=a; m_x=b; m_y=c; m_z=d; };
        NumType w() const { return m_w; };
        NumType x() const { return m_x; };
        NumType y() const { return m_y; };
        NumType z() const { return m_z; };
        NumType angle() const { return 360 * acos(m_w) / static_cast<NumType>(M_PI); };
        Quaternion<NumType> operator * (const Quaternion<NumType>&) const;
        AngleAndAxis<NumType> ToAngleAndAxis();
        sqmat<NumType,3> ToMatrix3();
        sqmat<NumType,4> ToMatrix4();
        friend sqmat<NumType,4> ModelMatrix <> (Triple<NumType>&, Quaternion<NumType>&);
};

template <class NumType> Quaternion<NumType> Quaternion<NumType>::operator * (const Quaternion<NumType>& rhs) const {
    NumType a = m_w;
    NumType b = m_x;
    NumType c = m_y;
    NumType d = m_z;
    NumType e = rhs.w();
    NumType f = rhs.x();
    NumType g = rhs.y();
    NumType h = rhs.z();

    Quaternion<NumType> retval(a*e - b*f - c*g - d*h,
                               a*f + b*e + c*h - d*g,
                               a*g - b*h + c*e + d*f,
                               a*h + b*g - c*f + d*e);

    return retval;
}

template <class NumType> template <int n> void Quaternion<NumType>::RotationMatrix(sqmat<NumType,n>& result) {
    result[0][0] = 1 - 2*pow(m_y,2) - 2*pow(m_z,2);
    result[0][1] = 2*m_x*m_y - 2*m_w*m_z;
    result[0][2] = 2*m_x*m_z + 2*m_w*m_y;
    result[1][0] = 2*m_x*m_y + 2*m_w*m_z;
    result[1][1] = 1 - 2*pow(m_x,2) - 2*pow(m_z,2);
    result[1][2] = 2*m_y*m_z - 2*m_w*m_x;
    result[2][0] = 2*m_x*m_z - 2*m_w*m_y;
    result[2][1] = 2*m_y*m_z + 2*m_w*m_x;
    result[2][2] = 1 - 2*pow(m_x,2) - 2*pow(m_y,2);
}

template <class NumType> sqmat<NumType,3> Quaternion<NumType>::ToMatrix3() {
    sqmat<NumType,3> result;
    RotationMatrix<3>(result);
    return result;
}

template <class NumType> sqmat<NumType,4> Quaternion<NumType>::ToMatrix4() {
    sqmat<NumType,4> result;
    RotationMatrix<4>(result);
    result[0][3] = 0;
    result[1][3] = 0;
    result[2][3] = 0;
    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;
    return result;
}

//template <class NumType> sqmat<NumType,4> Quaternion<NumType>::ToMatrix4() {
//    sqmat<NumType,4> result;
//    result[0][0] = static_cast<NumType>(1) - 2*pow(m_y,2) - 2*pow(m_z,2);
//    result[0][1] = 2*m_x*m_y + 2*m_w*m_z;
//    result[0][2] = 2*m_x*m_z - 2*m_w*m_y;
//    result[0][3] = 0;
//    result[1][0] = 2*m_x*m_y - 2*m_w*m_z;
//    result[1][1] = static_cast<NumType>(1) - 2*pow(m_x,2) - 2*pow(m_z,2);
//    result[1][2] = 2*m_y*m_z + 2*m_w*m_x;
//    result[1][3] = 0;
//    result[2][0] = 2*m_x*m_z + 2*m_w*m_y;
//    result[2][1] = 2*m_y*m_z - 2*m_w*m_x;
//    result[2][2] = static_cast<NumType>(1) - 2*pow(m_x,2) - 2*pow(m_y,2);
//    result[2][3] = 0;
//    result[3][0] = 0;
//    result[3][1] = 0;
//    result[3][2] = 0;
//    result[3][3] = static_cast<NumType>(1);
//    return result;
//}

template <class NumType> sqmat<NumType,4> ModelMatrix(Triple<NumType>& l_position, Quaternion<NumType>& l_orientation) {
    sqmat<NumType,4> result;
    l_orientation.RotationMatrix<4>(result);
    result[0][3] = l_position.m_data[0];
    result[1][3] = l_position.m_data[1];
    result[2][3] = l_position.m_data[2];
    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;
    return result;
}

template <class NumType> class AngleAndAxis {
    private:
        NumType             m_angle;
        Vector<NumType>     m_vector;
    public:
        AngleAndAxis(NumType angle, Vector<NumType> vec) { m_angle = angle; m_vector = vec; };
        AngleAndAxis(NumType angle, NumType a, NumType b, NumType c) { m_angle = angle; m_vector = Vector(a,b,c); };
        NumType angle () { return m_angle; };
        NumType x() { return m_vector.x(); };
        NumType y() { return m_vector.y(); };
        NumType z() { return m_vector.z(); };
        void operator = (const NumType angle) { m_angle = angle; };
        void operator += (const NumType delta) { m_angle += delta; };
        Vector<NumType> Normalise() { return m_vector.Normalise(); };
        Quaternion<NumType> ToQuaternion();
};

template <class NumType> AngleAndAxis<NumType> Quaternion<NumType>::ToAngleAndAxis() {
    NumType phi = acos(m_w) * 360.0f / static_cast<NumType>(M_PI);
    AngleAndAxis<NumType> retval(phi,Vector<NumType>(m_x,m_y,m_z));
    return retval;
}


template <class NumType> Quaternion<NumType> AngleAndAxis<NumType>::ToQuaternion() {
    Vector<NumType> unit_vec = this->Normalise();
    NumType phi_2 = m_angle * static_cast<NumType>(M_PI) / 360.0f;
    NumType w = cos(phi_2);
    Vector<NumType> xyz = unit_vec * sin(phi_2);
    Quaternion<NumType> retval(w,xyz.x(),xyz.y(),xyz.z());
    return retval;
}

template <class NumType> Vector<NumType> operator* (sqmat<NumType,4>& l_mat, Vector<NumType>& l_vec) {
    NumType result[3];
    for (int j=0; j<3; j++) {
        result[j] = l_mat[j][0]*l_vec[0] + l_mat[j][1]*l_vec[1] + l_mat[j][2]*l_vec[2];
    }
    return Vector<NumType>(result);
}

} //namespace OpenGLUtils
