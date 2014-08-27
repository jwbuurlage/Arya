#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace Arya
{
    class Camera
    {
        public:
            Camera();
            ~Camera();

            //! Instantaneously move the look-at position of the camera
            //! Note that the look-at position will be near the ground for RTS games
            void setPosition(const vec3& pos) { position = pos; updateMatrix = updateInverse = true; }

            //! Smooth movement
            //! Call this function with a location to make the camera smoothly glide to this location
            void setPositionSmooth(const vec3& pos) { targetPosition = pos; }

            //! Get the look-at position
            const vec3& getPosition() const { return position; }

            //! Computes the position of the camera eye point
            //! Needs matrix multiplication since this position is not stored or needed elsewhere
            vec3 getEyePosition();

            //! Instantaneously set the camera yaw
            //! If one wants the camera to stay behind the object (like when following a car)
            //! then call setYaw every frame with the objects yaw
            void setYaw(float _yaw) { yaw = _yaw; updateMatrix = updateInverse = true; }
            
            //! Instantaneously set the camera pitch
            void setPitch(float _pitch) { pitch = _pitch; freeYaw = false; updateMatrix = updateInverse = true; }

            //! Instantaneously rotate the camera
            void rotate(float deltaYaw, float deltaPitch);

            //! Smooth movement
            //! Sets the camera rotation speed in radians per second
            void rotateSmooth(float yawspeed, float pitchspeed);

            //! Get camera yaw
            float getYaw() const { return yaw; }
            //! Get camera pitch
            float getPitch() const { return pitch; }

            //! Set the distance from the camera eye to the look-at position
            //! A higher (positive) value means the camera is further away
            void setZoom(float zoom) { camDist = zoom; updateMatrix = updateInverse = true; }
            float getZoom() const { return camDist; }

            //! Set the projection matrix
            //! Call this on startup and when the window has resized
            void setProjectionMatrix(float fov, float aspect, float near, float far);

            //
            // Methods called by Graphics
            //

            //! This method should be called every frame by Graphics to ensure smooth movement
            void update(float elapsedTime);

            void updateViewProjectionMatrix();
            bool updateMatrix;

            void updateInverseMatrix();
            bool updateInverse;

            float camZoomSpeed; //positive is zooming out
            float camYawSpeed;
            float camPitchSpeed;

            mat4 getVMatrix();
            mat4 getVPMatrix();
            mat4 getInverseVPMatrix();

            //! Checks if a box is visible, used for culling
            bool isBoxVisible(int minX, int maxX, int minY, int maxY, int minZ, int maxZ);

            //Get the intersections of the y=0 surface (ground plane) with the 'screen corners'
            //Returns the 4 (x,0,z) locations. Order: bottom-left, top-left, top-right, bottom-right
            void getCornerGroundLocations(vec3 out[4]);

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
