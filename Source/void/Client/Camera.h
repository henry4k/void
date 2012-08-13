#ifndef __CLIENT_CAMERA_H__
#define __CLIENT_CAMERA_H__

#include <tools4k/Vector.h>
#include <tools4k/Matrix.h>
#include <tools4k/Quaternion.h>

using tools4k::vec2f;
using tools4k::vec3f;
using tools4k::vec4f;
using tools4k::Quaternion;
using tools4k::mat4f;

class Camera
{
	public:
		/// Access the virtual screen size
		void setScreen( vec2f screen );
		vec2f getScreen() const;
		
		/// Access the near clip plane distance
		void setNear( float near );
		float getNear() const;
		
		/// Access the far clip plane distance
		void setFar( float far );
		float getFar() const;
		
		/// Read the matrix
		const mat4f& getProjectionMatrix() const { return m_ProjectionMatrix; }
		const mat4f& getModelViewMatrix() const  { return m_ModelViewMatrix; }
		
		/// Access the position
		void setPosition( vec3f position );
		vec3f getPosition() const;
		
		/// Access the rotation
		void setRotation( Quaternion rotation );
		Quaternion getRotation() const;
		
		/// Recalculate the matrix if something has changed
		virtual void update();
		
		/// Upload matrix
		void upload();

	protected:
		Camera();
		
		vec2f m_Screen;
		float m_Near, m_Far;

		bool m_NeedsUpdate;
		mat4f m_ProjectionMatrix;
		mat4f m_ModelViewMatrix;
		
		vec3f m_Position;
		Quaternion m_Rotation;
		
		virtual void calcMatrix() = 0;
};

class PerspectivicCamera : public Camera
{
	public:
		PerspectivicCamera();

		/// Access the field of view
		void setFov( float fov );
		float getFov() const;
		
		/// Access the view aspect (width / height)
		void setAspectModifier( float aspect );
		float getAspectModifier() const;
		
	protected:
		float m_Fov;
		float m_AspectModifier;
		
		void calcMatrix();
};

class OrthogonalCamera : public Camera
{
	public:
		OrthogonalCamera();
		
	protected:
		void calcMatrix();
};



#endif
