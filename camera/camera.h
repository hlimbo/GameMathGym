#ifndef CAMERA_H
#define CAMERA_H
#include <memory>

/* Forward Declarations */
namespace MathUtils {
  struct Matrix4;
  struct Vector3;
}

namespace Core {
  class Camera {
    public:
      enum class Mode {
        ORTHOGRAPHIC = 0,
        PERSPECTIVE = 1,
        LENGTH,
      };
    public:
      Camera();
      Camera(float width, float height, float fov, float near, float far);
      ~Camera();

      void translate(float xOffset, float yOffset, float zOffset);
      void translate(const MathUtils::Vector3& offset);
      void pan(float xOffset, float yOffset);
      void dolly(float zOffset);
      void rotate(const MathUtils::Vector3& rotationalAxis, float angleDegrees);
      void rotateX(float angleDegrees);
      void rotateY(float angleDegrees);
      void rotateZ(float angleDegrees);
      void setProjectionMode(Mode newMode);
      void zoom(float newZoomFactor);

      MathUtils::Matrix4& getViewMatrix() const;
      MathUtils::Matrix4& getProjectionMatrix() const;
      void setViewMatrix(const MathUtils::Matrix4& newViewMatrix);

      void setOrthoExtents(float left, float right, float bottom, float top, float near, float far);
    
      Mode cameraMode;
    private:
      float fieldOfView; // measured in degrees
      float near, far;
      float aspectRatio;
      float zoomFactor;
      std::unique_ptr<MathUtils::Matrix4> viewMatrix;
      std::unique_ptr<MathUtils::Matrix4> projectionMatrix;

      // for orthographic camera
      float left, right, top, bottom;
  };
}

#endif // CAMERA_H