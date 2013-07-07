#include "Camera.h"
#include "common/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

const float PI = 3.14159265358979323846264338327950288f;

namespace Arya
{
    Camera::Camera()
    {
        updateMatrix = true;
        updateInverse = true;
        camZoomSpeed = 0.0f;
        camYawSpeed = 0.0f;
        camPitchSpeed = 0.0f;
        camDist = 50.0f;
        minCamDist = 10.0f;
        maxCamDist = 1000.0f;
        freeYaw = false;
        yaw = 0.0f;
        pitch = 0.0f;
        projectionMatrix = mat4(1.0f);
        inverseProjectionMatrix = mat4(1.0f);

        position = vec3(0.0f);
        targetPosition = vec3(0.0f);
    }

    Camera::~Camera()
    {

    }

    bool Camera::isBoxVisible(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
    {
        return false;
    }

    void Camera::setProjectionMatrix(float fov, float aspect, float near, float far)
    {
        projectionMatrix = glm::perspective(fov, aspect, near, far);
        inverseProjectionMatrix = glm::inverse(projectionMatrix);
    }

    void Camera::update(float elapsedTime)
    {
        //---------------------------------------------
        //TODO: Camera shouldn't move beneath the floor
        //		so we need to check with a heightmap or
        //		for now probably just the y = -20 plane.
        //---------------------------------------------

        // prevent camera from bugging. TODO: proper fix
        if(elapsedTime > 1.0)
            elapsedTime = 0.0;

        vec3 ToMove = targetPosition - position;
        float dist = ToMove.length();

        // LOG_INFO(targetPosition.x << " " << targetPosition.y << " " <<  targetPosition .z << " " << position.x << " " << position.y << " " << position.z << "to move: " << dist);

        //1. Camera position
        //We only need to move if the camera is not at the player
        if( dist > 0 ){
            //If the camera is really far away OR really close to the object and the object is not moving
            //then just teleport the camera to the exact object position
            if( dist > 100.0f ){
                position = targetPosition;
            }else if( dist > 0.1f ){ //We are at a normal distance from the object. Let camera fly with normal speed
                //ToMove already has a length, so if the camera is further away from the player,
                //then the camera will go faster.
                //The speed (in this frame) is equal to 12.0 * distance
                //When 'targetPosition' is moving at a constant speed v in a single direction then,
                //once stable, the camera will be at v/12 units behind the targetPosition
                position += ToMove * 12.0f * elapsedTime; //speed is proportional to distance
            }else{ //Really close
                vec3 newMove = glm::normalize(ToMove);
                newMove *= 3.0f * elapsedTime; //constant speed of 3.0f units per second
                if( newMove.length() >= dist ){
                    position = targetPosition;
                }else{
                    position += newMove;
                }
            }
            updateMatrix = true;
            updateInverse = true;
        }

        //2. Camera rotation
        //For yaw rotation we have a free rotation bool. Pitch rotation is always free
        if( freeYaw ){
            if( camYawSpeed ){
                if( camYawSpeed > 1.5f*PI ) camYawSpeed = 1.5f*PI;
                if( camYawSpeed < -1.5f*PI ) camYawSpeed = -1.5f*PI;
                yaw += camYawSpeed * elapsedTime;
                updateMatrix = true;
                updateInverse = true;
                deAccelerate(camYawSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
            }
        }
        //Free pitch rotation
        if( camPitchSpeed ){
            if( camPitchSpeed > PI ) camPitchSpeed = PI;
            else if( camPitchSpeed < -PI ) camPitchSpeed = -PI;
            pitch += camPitchSpeed * elapsedTime;
            updateMatrix = true;
            updateInverse = true;
            //Decrease the speed (which causes a natural 'slow down')
            deAccelerate(camPitchSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
        }

        //3. Camera zoom
        if( camZoomSpeed ){
            if( camZoomSpeed > 400 ) camZoomSpeed = 400;
            else if( camZoomSpeed < -400 ) camZoomSpeed = -400;
            //Adjust the zoom with the speed
            camDist += camZoomSpeed * elapsedTime;
            updateMatrix = true;
            updateInverse = true;
            //Make sure it does not zoom in or out too far
            if( camDist < minCamDist ){ camDist = minCamDist; camZoomSpeed = 0; }
            else if( camDist > maxCamDist ){ camDist = maxCamDist; camZoomSpeed = 0; }
            else deAccelerate(camZoomSpeed, 800 * elapsedTime); //Deacceleration of 40 units per second per second
        }
        return;
    }

    void Camera::deAccelerate(float& Speed, float Acceleration)
    {
        if( Speed > 0 ){
            if( Speed < Acceleration ) Speed = 0;
            else Speed -= Acceleration;
        }else{
            if( Speed > -Acceleration ) Speed = 0;
            else Speed += Acceleration;
        }
    }

    void Camera::setTargetLocation(const vec3 &location, bool FreeYawRotation) 
    {
        targetPosition = location;
        freeYaw = FreeYawRotation;
    }

    void Camera::rotateCamera(float deltaYaw, float deltaPitch)
    {
        yaw += deltaYaw;
        pitch += deltaPitch;
        updateMatrix = true;
        updateInverse = true;
    }

    vec3 Camera::getRealCameraPosition()
    {
        updateViewProjectionMatrix();
        vec3 relative(0, 0, camDist);
        relative = glm::rotateX(relative, pitch);
        relative = glm::rotateY(relative, yaw);
        return getPosition() + relative;
    }

    void Camera::updateViewProjectionMatrix()
    {
        if(!updateMatrix) return;
        updateMatrix = false;
        viewMatrix = mat4(1.0f);
        viewMatrix = glm::translate( viewMatrix, vec3(0, 0, -camDist) );
        viewMatrix = glm::rotate( viewMatrix, -pitch, vec3(1.0, 0.0, 0.0) );
        viewMatrix = glm::rotate( viewMatrix, -yaw, vec3(0.0, 1.0, 0.0) );
        viewMatrix = glm::translate( viewMatrix, -position );
        vpMatrix = projectionMatrix * viewMatrix;
        return;
    }

    void Camera::updateInverseMatrix()
    {
        if(!updateInverse) return;
        updateInverse = false;

        inverseViewMatrix = mat4(1.0f);
        inverseViewMatrix = glm::translate( inverseViewMatrix, position );
        inverseViewMatrix = glm::rotate( inverseViewMatrix, yaw, vec3(0.0, 1.0, 0.0) );
        inverseViewMatrix = glm::rotate( inverseViewMatrix, pitch, vec3(1.0, 0.0, 0.0) );
        inverseViewMatrix = glm::translate( inverseViewMatrix, vec3(0, 0, camDist) );
        inverseVPMatrix = inverseViewMatrix * inverseProjectionMatrix;
        return;
    }

    mat4 Camera::getVMatrix()
    {
        updateViewProjectionMatrix();
        return viewMatrix;
    }

    mat4 Camera::getVPMatrix()
    {
        updateViewProjectionMatrix();
        return vpMatrix;
    }

    mat4 Camera::getInverseVPMatrix()
    {
        updateInverseMatrix();
        return inverseVPMatrix;
    }

}
