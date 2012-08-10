#include <tools4k/Math.h>
#include <kj/Common.h>
#include <kj/Client/OpenGL.h>
#include <kj/Client/Camera.h>

using tools4k::Pi;
using tools4k::Deg2Rad;


// --- Camera ---

Camera::Camera() :
	m_Screen(1,1),
	m_Near(0.1), m_Far(100),
	m_NeedsUpdate(true),
	m_Position(0, 0, -2),
	m_Rotation(0, 0, 0, 1)
{
}

void Camera::setScreen( vec2f screen )
{
	m_NeedsUpdate = true;
	m_Screen = screen;
}

vec2f Camera::getScreen() const
{
	return m_Screen;
}

void Camera::setNear( float near )
{
	m_NeedsUpdate = true;
	m_Near = near;
}

float Camera::getNear() const
{
	return m_Near;
}

void Camera::setFar( float far )
{
	m_NeedsUpdate = true;
	m_Far = far;
}

float Camera::getFar() const
{
	return m_Far;
}

void Camera::setPosition( vec3f position )
{
	m_NeedsUpdate = true;
	m_Position = position;
}

vec3f Camera::getPosition() const
{
	return m_Position;
}

void Camera::setRotation( Quaternion rotation )
{
	m_NeedsUpdate = true;
	m_Rotation = rotation;
}

Quaternion Camera::getRotation() const
{
	return m_Rotation;
}

void Camera::update()
{
	if(!m_NeedsUpdate)
		return;
	calcMatrix();
	m_NeedsUpdate = false;
}

void Camera::upload()
{
	glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(m_ProjectionMatrix);
	glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(m_ModelViewMatrix);
}


// --- PerspectivicCamera ---

PerspectivicCamera::PerspectivicCamera() : Camera(), m_Fov(75), m_AspectModifier(1)
{
}

void PerspectivicCamera::setFov( float fov )
{
	m_NeedsUpdate = true;
	m_Fov = fov;
}

float PerspectivicCamera::getFov() const
{
	return m_Fov;
}

void PerspectivicCamera::setAspectModifier( float aspect )
{
	m_NeedsUpdate = true;
	m_AspectModifier = aspect;
}

float PerspectivicCamera::getAspectModifier() const
{
	return m_AspectModifier;
}

float cotan( float v )
{
	return tan((Pi/2.0) - v);
}

void CreatePerspectivicMatrix( mat4f& out, float fov, float aspect, float near, float far )
{
	float f = cotan(Deg2Rad(fov)/2.0f); // cotangent(fovy/2)
	
	out.at(0,0) = f / aspect;
	out.at(1,0) = 0;
	out.at(2,0) = 0;
	out.at(3,0) = 0;
	
	out.at(0,1) = 0;
	out.at(1,1) = f;
	out.at(2,1) = 0;
	out.at(3,1) = 0;
	
	out.at(0,2) = 0;
	out.at(1,2) = 0;
	out.at(2,2) = (far+near) / (near-far);
	out.at(3,2) = (2.0f*far*near) / (near-far);
		
	out.at(0,3) = 0;
	out.at(1,3) = 0;
	out.at(2,3) = -1;
	out.at(3,3) = 0;
}

void PerspectivicCamera::calcMatrix()
{
	CreatePerspectivicMatrix(m_ProjectionMatrix,
		m_Fov,
		(m_Screen.x/m_Screen.y) * m_AspectModifier,
		m_Near, m_Far);
	
	m_ModelViewMatrix.setRotation(m_Rotation);
	m_ModelViewMatrix.translate( vec4f(m_Position.x, m_Position.y, m_Position.z, 1) );
}


// --- OrthogonalCamera ---

OrthogonalCamera::OrthogonalCamera()
{
}

void CreateOrthogonalMatrix( mat4f& out, float right, float left, float top, float bottom, float near, float far )
{
	out.at(0,0) = 2.0f/(right-left);
	out.at(1,0) = 0;
	out.at(2,0) = 0;
	out.at(3,0) = -(right+left)/(right-left);
	
	out.at(0,1) = 0;
	out.at(1,1) = 2.0f/(top-bottom);
	out.at(2,1) = 0;
	out.at(3,1) = -(top+bottom)/(top-bottom);
	
	out.at(0,2) = 0;
	out.at(1,2) = 0;
	out.at(2,2) = -2.0f/(far-near);
	out.at(3,2) = -(far+near)/(far-near);
	
	out.at(0,3) = 0;
	out.at(1,3) = 0;
	out.at(2,3) = 0;
	out.at(3,3) = 1;
}

void OrthogonalCamera::calcMatrix()
{
	CreateOrthogonalMatrix(m_ProjectionMatrix,
		-m_Screen.x/2., // right
		 m_Screen.x/2., // left
		-m_Screen.y/2., // top
		 m_Screen.y/2., // bottom
		 m_Near, m_Far);
	
	m_ModelViewMatrix.setRotation(m_Rotation);
	m_ModelViewMatrix.translate( vec4f(m_Position.x, m_Position.y, m_Position.z, 1) );
}
