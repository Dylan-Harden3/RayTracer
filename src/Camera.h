#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera {
    public:
        Camera(int x, int y, int z, float fov);
        void getRays(int w, int h, int x, int y, int z);
    private:
        int x_;
        int y_;
        int z_;
        float fov_;
        float bounds_;
        const float PI = 3.14159265358979323846; 
};

#endif