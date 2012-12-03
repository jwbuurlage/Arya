#include "Camera.h"
#include "common/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

const float PI = 3.14159265358979323846264338327950288f;

namespace Arya
{
    Camera::Camera(){
        updateMatrix = true;
        camZoomSpeed = 0.0f;
        camYawSpeed = 0.0f;
        camPitchSpeed = 0.0f;
        camDist = 5.0f;
        minCamDist = 3.0f;
        maxCamDist = 100.0f;
        freeYaw = false;
        yaw = 0.0f;
        pitch = 0.0f;
        projectionMatrix = glm::perspective(45.0f, 1.6f, 0.1f, 50.0f);

        position = vec3(0.0f);
        targetPosition = vec3(0.0f);
        LOG_INFO(targetPosition.x << " " << targetPosition.y << " " <<  targetPosition .z << " " << position.x << " " << position.y << " " << position.z);
    }

    Camera::~Camera(){
    }

    void Camera::setProjectionMatrix(float fov, float aspect, float near, float far)
    {
        projectionMatrix = glm::perspective(fov, aspect, near, far);
    }

    void Camera::update(float elapsedTime){
        //---------------------------------------------
        //TODO: Camera shouldn't move beneath the floor
        //		so we need to check with a heightmap or
        //		for now probably just the y = -20 plane.
        //---------------------------------------------

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
                //When the object is moving in a single direction then the camera will be (PlayerSpeed/2.0f) units behind the object
                position += ToMove * 2.0f * elapsedTime; //speed is proportional to distance
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
        }

        //2. Camera rotation
        //For yaw rotation we have a free rotation bool. Pitch rotation is always free
        if( freeYaw ){
            if( camYawSpeed ){
                if( camYawSpeed > 1.5f*PI ) camYawSpeed = 1.5f*PI;
                if( camYawSpeed < -1.5f*PI ) camYawSpeed = -1.5f*PI;
                yaw += camYawSpeed * elapsedTime;
                updateMatrix = true;
                deAccelerate(camYawSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
            }
        }
        //Free pitch rotation
        if( camPitchSpeed ){
            if( camPitchSpeed > PI ) camPitchSpeed = PI;
            else if( camPitchSpeed < -PI ) camPitchSpeed = -PI;
            pitch += camPitchSpeed * elapsedTime;
            updateMatrix = true;
            //Decrease the speed (which causes a natural 'slow down')
            deAccelerate(camPitchSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
        }

        //3. Camera zoom
        if( camZoomSpeed ){
            if( camZoomSpeed > 40 ) camZoomSpeed = 40;
            else if( camZoomSpeed < -40 ) camZoomSpeed = -40;
            //Adjust the zoom with the speed
            camDist += camZoomSpeed * elapsedTime;
            updateMatrix = true;
            //Make sure it does not zoom in or out too far
            if( camDist < minCamDist ){ camDist = minCamDist; camZoomSpeed = 0; }
            else if( camDist > maxCamDist ){ camDist = maxCamDist; camZoomSpeed = 0; }
            //Decrease the speed (which causes a natural 'slow down')
            deAccelerate(camZoomSpeed, 40 * elapsedTime); //Deacceleration of 40 units per second per second
        }
        return;
    }

    void Camera::deAccelerate(float& Speed, float Acceleration){
        if( Speed > 0 ){
            if( Speed < Acceleration ) Speed = 0;
            else Speed -= Acceleration;
        }else{
            if( Speed > -Acceleration ) Speed = 0;
            else Speed += Acceleration;
        }
    }

    void Camera::setTargetLocation(const vec3 &location, bool FreeYawRotation) {
        targetPosition = location;
        freeYaw = FreeYawRotation;
    }

    void Camera::rotateCamera(float Yaw, float Pitch){
        yaw += Yaw, 
            pitch += Pitch;
        updateMatrix = true;
    }

    vec3 Camera::getRealCameraPosition()
    {
        updateViewProjectionMatrix(0);
        vec3 relative(0, 0, camDist);
        relative = glm::rotateX(relative, pitch);
        relative = glm::rotateY(relative, yaw);
        return getPosition() + relative;
    }

    bool Camera::updateViewProjectionMatrix(mat4* outMatrix){
        if( updateMatrix ){
            viewMatrix = glm::translate( mat4(1.0f), -position );
            viewMatrix = glm::rotate( viewMatrix, -yaw, vec3(0.0, 1.0, 0.0) );
            viewMatrix = glm::rotate( viewMatrix, -pitch, vec3(1.0, 0.0, 0.0) );
            viewMatrix = glm::translate( viewMatrix, vec3(0, 0, -camDist) );
            vpMatrix = projectionMatrix * viewMatrix;

            //if( inverseViewMatrix ){
            //	mat zoomMat, rotateMat, translateMat;
            //	zoomMat.setTranslation(0, 0, mCamDist);
            //	rotateMat.setRotationX(pitch);
            //	rotateMat.rotateY(yaw);
            //	translateMat.setTranslation(position);
            //	*inverseViewMatrix = translateMat * rotateMat * zoomMat;
            //}
        }
        if( outMatrix ) *outMatrix = projectionMatrix * viewMatrix;
        if( updateMatrix )
        {
            updateMatrix = false;
            return true;
        }
        return false;
    }

    mat4 Camera::getVPMatrix()
    {
        if( updateMatrix ) updateViewProjectionMatrix(0);
        return vpMatrix;
    }

}
