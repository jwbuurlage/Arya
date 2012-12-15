#pragma once

#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;

namespace Arya
{
    class Camera
    {
        public:
            Camera();
            ~Camera();

            //Sets the projection matrix
            void setProjectionMatrix(float fov, float aspect, float near, float far);

            //Use setPostion to instantly move the look-at position of the camera
            //Use setTargetLocation to make the camera smoothly move to that look-at position

            //This is the look-at position of the camera.
            void setPosition( const vec3& CamPos ){ position = CamPos; updateMatrix = updateInverse = true; }
            const vec3& getPosition() const { return position; }
            float getYaw() const { return yaw; }
            float getPitch() const { return pitch; }

            //To make the camera stay looking at an object smoothly,
            //call setTargetLocation every frame with the object as location
            void setTargetLocation( const vec3& location, bool FreeYawRotation = false);
            //if you want the camera to stay BEHIND the object, call setCameraYaw every frame
            //with the yaw of the object
            void setCameraYaw(float yaw);

            //For smooth zoom you should just set the camZoomSpeed variable
            //and the zoom will be set accordingly in the update function
            void setZoom(float zoom){ camDist = zoom; updateMatrix = updateInverse = true; }

            //Smooth camera movement, should be called by Scene every frame
            //(not by the application)
            void update(float elapsedTime);


            //
            //These should be called by the mouse handler
            //

            //This will give the camera a swing
            void rotateCameraSwing(float yawspeed, float pitchspeed);
            //This will also rotate the camera but will instantly apply the angle and not swing
            void rotateCamera(float deltaYaw, float deltaPitch); //Add yaw/pitch to current angle
            void setCameraAngle(float Yaw, float Pitch){ yaw = Yaw; pitch = Pitch; updateMatrix = updateInverse = true; }

            vec3 getRealCameraPosition();

            void updateViewProjectionMatrix();
            bool updateMatrix;

            void updateInverseMatrix();
            bool updateInverse;

            float camZoomSpeed; //Zoomspeed. positive means zooming out
            float camYawSpeed; //Yaw speed in radians per second
            float camPitchSpeed; //Pitch speed in radians per second

            mat4 getVMatrix();
            mat4 getVPMatrix();
            mat4 getInverseVPMatrix();

        private:
            vec3 position; //look-at-position. Actual camera is 'zoomed out' from here
            mat4 viewMatrix;
            mat4 projectionMatrix;
            mat4 vpMatrix;
            mat4 inverseViewMatrix;
            mat4 inverseProjectionMatrix;
            mat4 inverseVPMatrix;
            float yaw, pitch;
            float camDist; //Zoom. Higher means further away
            float minCamDist;
            float maxCamDist;

            bool freeYaw; //Free yaw rotation
            vec3 targetPosition;

            void deAccelerate(float& Speed, float Acceleration);
    };

}
