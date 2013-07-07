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
        //The fov is in the y direction!!
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
                if(elapsedTime < 1.0f/12.0f)
                    position += ToMove * 12.0f * elapsedTime; //speed is proportional to distance
                else
                    position = targetPosition;
            }else{ //Really close
                //Now it is not possible to have a speed proportional to the distance, because
                //this would result in asymptotic behaviour, where the camera never reaches
                //the targetposition. Therefore, in this regime, the camera moves at constant speed
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


    void Camera::getCornerGroundLocations(vec3 out[4])
    {
        //This function calculates the intersection of the y=0 surface (ground plane) with the 'screen corners'
        //Steps:
        // - transform y=0 plane to eye-space (in this space, camera is at origin, looking in z- direction)
        //   write it as a point and a normal vector (the point is on the negative z axis)
        // - intersect lines from origin through (left/right,top/bottom,nearplane) with that plane

        //updateViewProjectionMatrix();
        updateInverseMatrix();
        
        vec4 eyeSpaceSurfaceLocation = vec4(0.0, 0.0, -camDist, 0.0); //TODO: this assume position.y=0
        //vec4 eyeSpaceSurfaceNormal = viewMatrix * vec4(0.0, 1.0, 0.0, 0.0);
        vec4 eyeSpaceSurfaceNormal = glm::rotateX(vec4(0.0, 1.0, 0.0, 0.0), -pitch);

		//valType range = tan(radians(fovy/2.0)) * zNear;	
		//valType left = -range * aspect;
		//valType right = range * aspect;
		//valType bottom = -range;
		//valType top = range;
        float right = 1.0f/projectionMatrix[0][0];
        float left = -right;
        float top = 1.0f/projectionMatrix[1][1];
        float bottom = -top;

        vec4 corner[4];
        corner[0] = vec4(left,  bottom, -1.0, 0.0);
        corner[1] = vec4(left,  top,    -1.0, 0.0);
        corner[2] = vec4(right, top,    -1.0, 0.0);
        corner[3] = vec4(right, bottom, -1.0, 0.0);

        float value1 = glm::dot(eyeSpaceSurfaceNormal, eyeSpaceSurfaceLocation);

        for(int i = 0; i < 4; ++i)
        {
            float value2 = glm::dot(eyeSpaceSurfaceNormal, corner[i]);
            if( glm::abs(value2) < 0.0001 )
            {
                LOG_WARNING("Camera corner-line may be parallel to ground plane. No intersection");
                value2 = 0.0001;
            }
            vec4 eyeSpaceIntersection = (value1/value2)*corner[i];
            eyeSpaceIntersection.w = 1.0;

            vec4 worldIntersection = inverseViewMatrix * eyeSpaceIntersection;
            worldIntersection /= worldIntersection.w;

            out[i] = vec3(worldIntersection.x, worldIntersection.y, worldIntersection.z);
        }
    }

}
