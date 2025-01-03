#ifndef GLOC_HPP
#define GLOC_HPP

namespace glo
{
	namespace vector
	{
		struct vec3 { float x_, y_, z_; };
		struct vec2 { float x_, y_; };

		float magnitude(const vec3& v) { return sqrt((v.x_ * v.x_) + (v.y_ * v.y_) + (v.z_ * v.z_)); }
		vec3 unitise(const vec3& v) { float m = 1.0f / magnitude(v); return { v.x_ * m, v.y_ * m, v.z_ * m }; }

	}

	class camera
	{
	public:

		vector::vec3 position() const { return position_; }
		
		void position(float x, float y, float z) 
		{ 
			position_.x_ = x; 
			position_.y_ = y; 
			position_.z_ = z; 
		}
		
		vector::vec3 focal() const 
		{ 
			return 
			{ 
				position_.x_ + ( out_.x_ * focal_distance_), 
				position_.y_ + (out_.x_ * focal_distance_) , 
				position_.x_ + (out_.x_ * focal_distance_)  
			}; 
		}
		
		void focal(float x, float y, float z) 
		{ 
			// Set the focal point and calculate the axis...

			//focal_.x_ = x; focal_.y_ = y; focal_.z_ = z; 
		}

		float camera_width() const { return width_; }
		void camera_width(float width) { width_ = width; }
		float camera_height() const { return width_; }
		void camera_height(float height) { height_ = height; }
		
		// 0 for orthographic, > 0 for perspective....
		float fov() const { return fov_; }
		void fov(float fov) { fov_ = fov; }

		

		void orbit(const vector::vec3& focal_point, float heading, float elevation, float distance)
		{
			//float polarCap = Geometry::Trigonometry::Pi::HalfPi - 0.001f;
			//elevation = elevation < polarCap ? elevation : polarCap;
			//elevation = elevation > -polarCap ? elevation : -polarCap;
			//axis_ = CameraAxis();

			//// Since default camera axis is x (along), and -y (camera in), headingh result is wrt to -y (along) and x (camera in)... negate pi/2
			//axis_.Rotate(heading + Geometry::Trigonometry::Pi::HalfPi, axis_.CameraUp());

			//axis_.Rotate(-elevation, axis_.CameraAlong());
			//focalPoint_ = focus;
			//position_ = focalPoint_ + (axis_.CameraIn() * distance);
		}

		void rotate(const vector::vec3& origin, float heading, float elevation)
		{
			//float focalDistance = FocalDistance();

			//// first calculate the heading change...
			//Geometry::Vector3 rotationArm = position_ - rotationOrigin;
			//CameraAxis basis;

			//Geometry::Vector3 v = position_ - focalPoint_;
			//float xyProjection = sqrt((v.x_ * v.x_) + (v.y_ * v.y_));
			//float elevationPos = -atan2(v.z_, xyProjection);
			//float polarCap = AxW::Geometry::Trigonometry::Pi::HalfPi - 0.001f;
			//float elevationAngleE = elevationPos + elevationAngle;

			//if (headingAngle)
			//{
			//	// rotate the arm by the heading...
			//	rotationArm = Geometry::Vector::Rotate(rotationArm, basis.CameraUp(), headingAngle);

			//	// rotate the camera aixs, but the heading angle...
			//	//CameraAxis cameraAxis = axis_;
			//	axis_.Rotate(headingAngle, basis.CameraUp());

			//	// calculate the new position...
			//	position_ = rotationOrigin + rotationArm;

			//	// Set the focal position...
			//	focalPoint_ = position_ + (axis_.CameraIn() * -focalDistance);
			//}

			//if (elevationAngle)
			//{
			//	if ((elevationAngleE < polarCap) && (elevationAngleE > -polarCap))
			//	{
			//		// rotate the arm by the elevation...
			//		rotationArm = Geometry::Vector::Rotate(rotationArm, axis_.CameraAlong(), elevationAngle);

			//		// rotate the camera aixs, but the elevation angle...
			//		axis_.Rotate(elevationAngle, axis_.CameraAlong());

			//		// calculate the new position...
			//		position_ = rotationOrigin + rotationArm;

			//		// Set the focal position...
			//		focalPoint_ = position_ + (axis_.CameraIn() * -focalDistance);
			//	}
			//}
		}

		void look_at(const vector::vec3& position, const vector::vec3& focal, const vector::vec3& up)
		{

		}

		float heading()
		{
			/*Geometry::Vector3 v = position_ - focalPoint_;
			return Geometry::Trigonometry::WrapZero2Pi(atan2(v.y_, v.x_));*/
		}

		float elevation()
		{
			/*Geometry::Vector3 v = position_ - focalPoint_;
			float xyProjection = sqrt((v.x_ * v.x_) + (v.y_ * v.y_));
			return atan2(v.z_, xyProjection);*/
		}

		std::pair<vector::vec3, vector::vec3> ray(const vector::vec2& view_coordinate)
		{
			// Bug that initialises world coordinate at incorrect position fixed by offsetting by large amount. This method should only be used to get a camera ray with 
			// Orthographic projection, since it is assumed ray is from view to interrogate sceneObject we offset by larger amount to ensure ray source is outside model scene...
			/*float CameraInDepthOffset = -1000.0f;
			return std::make_pair(CalculateWorldCoordinate(viewCoordinateNormalised, CameraInDepthOffset), AxW::Geometry::Vector::Reverse(GetAxis().CameraIn()));*/
		}

		void dolly(float distance)
		{
			position_.x_ += (out_.x_ * distance);
			position_.x_ += (out_.x_ * distance);
			position_.x_ += (out_.x_ * distance);
		}

		void truck(float distance)
		{
			position_.x_ += (along_.x_ * distance);
			position_.x_ += (along_.x_ * distance);
			position_.x_ += (along_.x_ * distance);
		}

		void pedestal(float distance)
		{
			position_.x_ += (up_.x_ * distance);
			position_.x_ += (up_.x_ * distance);
			position_.x_ += (up_.x_ * distance);
		}

		// world 
		vector::vec3 world(const vector::vec2& view_cooridnate, float depth)
		{
			//Geometry::Vector3 bottomLeftCorner = position_ + (axis_.CameraAlong() * (-0.5f * width_)) + (axis_.CameraUp() * (-0.5f * height_));
			//return bottomLeftCorner + (axis_.CameraAlong() * (width_ * coordinateNormalised.x_)) + (axis_.CameraUp() * (height_ * coordinateNormalised.y_)) + (axis_.CameraIn() * -depth);
		}

		// normalised view coordinate
		vector::vec2 view(const vector::vec3& world_coordinate)
		{
			// Project the point onto the plane...
			//Geometry::Vector3 cameraOut = axis_.K() * -1.0f;
			//Geometry::Vector3 cameraIn = axis_.K() * 1.0f;
			//Geometry::Vector3 blc = position_ + (axis_.I() * (width_ * -0.5f)) + (axis_.J() * (height_ * -0.5f));

			//if (!fov_)
			//{
			//	Geometry::Vector3 viewportPlanePoint = *Geometry::Intersect::LinePlane<Geometry::Vector3>(world, world + cameraIn, position_, cameraOut);
			//	Geometry::Vector3 blcOffset = viewportPlanePoint - blc;
			//	float hyp = Geometry::Vector::Magnitude(blcOffset);

			//	// Calculate the normalised coordinate...
			//	float thita = Geometry::Vector::Angle(axis_.I(), Geometry::Vector::Unitise(blcOffset), cameraIn);

			//	Geometry::Vector2 viewportPlanePoint2D = { cos(thita) * hyp, sin(thita) * hyp };
			//	return { viewportPlanePoint2D.x_ / width_, viewportPlanePoint2D.y_ / height_ };
			//}

			//return ViewCoordinateNormalised(0.0f, 0.0f);
		}

		
		vector::vec3 projection_reference_point()
		{
			/*float dist = (width_ * 0.5f) / (tan(Geometry::Trigonometry::DegreesToRadians(fov_) * 0.5f));
			return position_ + (axis_.CameraIn() * dist);*/
		}

		void zoom(const vector::vec2& view_cooridnate_min, const vector::vec2& view_cooridnate_max)
		{

		}

		std::vector<float> matrix() const
		{

		}

	private:
		vector::vec3 position_;
		vector::vec3 out_;
		vector::vec3 along_;
		vector::vec3 up_;
		float focal_distance_;
		float fov_;
		float width_;
		float height_;
		float near_;
		float far_;
	};

}

#endif // GLOC_HPP