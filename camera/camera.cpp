#include <memory>
#include <assert.h>

#include "math_utils/matrix4.h"
#include "math_utils/vector3.h"
#include "camera.h"

using namespace MathUtils;

namespace Core {
  Camera::Camera(): 
    fieldOfView(0.0f), near(0.1f), far(100.0f),
    aspectRatio(1.0f), zoomFactor(1.0f), cameraMode(Camera::Mode::ORTHOGRAPHIC),
    left(-1.0f), right(1.0f), bottom(-1.0f), top(1.0f),
    viewMatrix(std::make_unique<Matrix4>(MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, -2.0f)))), 
    projectionMatrix(std::make_unique<Matrix4>(MathUtils::createOrthographicMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f))) {}

  Camera::Camera(float w, float h, float fov, float n, float f):
    aspectRatio(w / h), zoomFactor(1.0f), fieldOfView(fov), near(n), far(f),
    cameraMode{Camera::Mode::PERSPECTIVE},
    viewMatrix(std::make_unique<Matrix4>(MathUtils::makeTranslationMatrix(Vector3(0.0f, 0.0f, -2.0f)))), 
    projectionMatrix(std::make_unique<Matrix4>(MathUtils::createPerspectiveMatrix(w / h, fov, n, f))) {

      std::cout << "camera projection matrix: " << std::endl;
      for (int i = 0;i < MAT4_SIZE; ++i) {
        std::cout << (*projectionMatrix)[i] << " ";
        if (i % MAT4_DIM == 3) {
          std::cout << "\n";
        }
      }
      std::cout << "\n\n";

    }

  Camera::~Camera() = default;

  // Need to invert the translation because camera view space moves in the opposite direction as intended
  void Camera::translate(float xOffset, float yOffset, float zOffset) {
    *viewMatrix *= MathUtils::makeTranslationMatrix(Vector3(xOffset, yOffset, zOffset));;
  }

  void Camera::translate(const Vector3& offset) {
    *viewMatrix *= MathUtils::makeTranslationMatrix(offset);
  }

  void Camera::pan(float xOffset, float yOffset) {
    translate(xOffset, yOffset, 0.0f);
  }

  void Camera::dolly(float zOffset) {
    translate(0.0f, 0.0f, zOffset);
  }

  void Camera::rotate(const Vector3& rotationalAxis, float angleDegrees) {
    float angleRadians = (MathUtils::PI / 180.0f) * angleDegrees;
    viewMatrix = std::make_unique<Matrix4>(MathUtils::rotate(*viewMatrix, angleRadians, rotationalAxis.normalized()));
  }

  void Camera::rotateX(float angleDegrees) {
    Camera::rotate(Vector3(1.0f, 0.0f, 0.0f), angleDegrees);
  }

  void Camera::rotateY(float angleDegrees) {
    Camera::rotate(Vector3(0.0f, 1.0f, 0.0f), angleDegrees);
  }

  void Camera::rotateZ(float angleDegrees) {
    Camera::rotate(Vector3(0.0f, 0.0f, 1.0f), angleDegrees);
  }

  void Camera::setProjectionMode(Camera::Mode newMode) {
    cameraMode = newMode;
    if (cameraMode == Camera::Mode::ORTHOGRAPHIC) {
      projectionMatrix = std::make_unique<Matrix4>(MathUtils::createOrthographicMatrix(left, right, bottom, top, near, far));
    } else if (cameraMode == Camera::Mode::PERSPECTIVE) {
      projectionMatrix = std::make_unique<Matrix4>(MathUtils::createPerspectiveMatrix(aspectRatio, zoomFactor * fieldOfView, near, far));
    }
  }

  void Camera::zoom(float newZoomFactor) {
    assert(newZoomFactor > 0.0f);
    // new field of view cannot >= 180.0f degrees 
    assert(newZoomFactor * fieldOfView < 180.0f);

    zoomFactor = newZoomFactor;
    projectionMatrix = std::make_unique<Matrix4>(MathUtils::createPerspectiveMatrix(aspectRatio, zoomFactor * fieldOfView, near, far));
  }

  MathUtils::Matrix4& Camera::getViewMatrix() const {
    return *viewMatrix;
  }

  MathUtils::Matrix4& Camera::getProjectionMatrix() const {
    return *projectionMatrix;
  }

  void Camera::setViewMatrix(const Matrix4& newViewMatrix) {
    viewMatrix = std::make_unique<Matrix4>(newViewMatrix);
  }

  void Camera::setOrthoExtents(float left, float right, float bottom, float top, float near, float far) {
    assert(left != right);
    assert(bottom != top);
    assert(near != far);
    
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->near = near;
    this->far = far;

    projectionMatrix = std::make_unique<Matrix4>(MathUtils::createOrthographicMatrix(left, right, bottom, top, near, far));
  }

}