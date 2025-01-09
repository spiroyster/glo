#ifndef GLOC_HPP
#define GLOC_HPP

//#include <math.h>
#include <cmath>

# define GLOC_PI           3.14159265358979323846

namespace glo
{
	/// <summary>
	/// Position is on viewport plane (centred), prp is behind...
	/// fov = 0 denotes orthographic, fov > 0 is perspective
	/// prp = position for ortho
	/// out_ is out towards scene...
	/// </summary>
	class camera
	{
	public:

		struct vec3 { float x_, y_, z_; };
		struct vec2 { float x_, y_; };
		struct axis { vec3 along_, up_, out_; };
		struct matrix44 { float m_[16]; };				// row major

	protected:
		static float safe_acos(float v)
		{
			return v <= -1.0f ? GLOC_PI : (v >= 1.0f ? 0 : acos(v));
		}
		static float length(const vec3& v) { return sqrt((v.x_ * v.x_) + (v.y_ * v.y_) + (v.z_ * v.z_)); }
		static vec3 unitise(const vec3& v) { float m = 1.0f / length(v); return { v.x_ * m, v.y_ * m, v.z_ * m }; }
		static vec3 scale(const vec3& v, float s) { return { v.x_ * s, v.y_ * s, v.z_ * s }; }
		static vec3 add(const vec3& a, const vec3& b) { return { a.x_ + b.x_, a.y_ + b.y_ , a.z_ + b.z_ }; }
		static vec3 subtract(const vec3& a, const vec3& b) { return { a.x_ - b.x_, a.y_ - b.y_ , a.z_ - b.z_ }; }
		static vec3 cross(const vec3& a, const vec3& b) { return { (a.y_ * b.z_) - (a.z_ * b.y_), (a.z_ * b.x_) - (a.x_ * b.z_), (a.x_ * b.y_) - (a.y_ * b.x_) }; }
		static float dot(const vec3& a, const vec3& b) { return (a.x_ * b.x_) + (a.y_ * b.y_) + (a.z_ * b.z_); }
		static float point_plane_distance(const vec3& v, const vec3& p, const vec3& n) { return dot(subtract(v, p), n); }
		static vec3 ray_plane_intersect(const vec3& r, const vec3& d, const vec3& p, const vec3& n) { return add(r, scale(d, dot(subtract(p, r), n) / dot(d, n))); }
		static vec3 rotate_vec3(const vec3& v, const vec3& axis, float angle)
		{
			float c = static_cast<float>(cos(angle));
			vec3 vRot = scale(v, c);
			vRot = add(scale(cross(axis, v), static_cast<float>(sin(angle))), vRot);
			return add(scale(axis, dot(axis, v) * (1.0f - c)), vRot);
		}

		static bool checkNaN(const vec3& v)
		{
			if (std::isnan(v.x_))
			{
				int y = 0;
				++y;
				return true;
			}
			return false;
		}
		static bool checkNaN(float v)
		{
			if (std::isnan(v))
			{
				int y = 0;
				++y;
				return true;
			}
			return false;
		}

		vec3 position_;
		vec3 out_;
		vec3 along_;
		vec3 up_;
		float fov_ = 0;
		float width_ = 1.0f;
		float height_ = 1.0f;
		float near_ = 0.001f;
		float far_ = 100.0f;
		float focal_distance_ = 0.0f;
		
	public:

		camera(const vec3& position, const vec3& along, const vec3& up)
			: position_(position), along_(unitise(along)), up_(unitise(up)), out_(unitise(scale(cross(along, up), -1.0f))), focal_distance_(1.0f)
		{
		}

		vec3 position() const { return position_; }
		vec3 out() const { return out_; }
		vec3 along() const { return along_; }
		vec3 up() const { return up_; }
		vec3 focal() const { return add(position_, scale(out_, focal_distance_)); }
		float width() const { return width_; }
		float height() const { return width_; }
		float fov() const { return fov_; }
		float focal_distance() const { return focal_distance_; }

		void width(float width) { width_ = width; }
		void height(float height) { height_ = height; }
		void fov(float fov) { fov_ = fov; }
		void nr(float n) { near_ = n; }
		void fr(float f) { far_ = f; }

		
		// Projection matrix...
		matrix44 P() const
		{
			matrix44 result = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			if (fov_)
			{
				result.m_[5] = cos(0.5 * fov_) / sin(0.5 * fov_);
				result.m_[0] = result.m_[5] * height_ / width_;
				result.m_[10] = -(far_ + near_) / (far_ - near_);
				result.m_[11] = -1.0f;
				result.m_[14] = -(2.0f * far_ * near_) / (far_ - near_);
			}
			else
			{
				result.m_[0] = 2.0f / ((width_ * 0.5f) - (width_ * -0.5f));
				result.m_[5] = 2.0f / ((height_ * 0.5f) - (height_ * -0.5f));
				result.m_[10] = -2.0f / (far_ - near_);
				result.m_[12] = -((width_ * 0.5f) + (width_ * -0.5f)) / ((width_ * 0.5f) - (width_ * -0.5f));
				result.m_[13] = -((height_ * 0.5f) + (height_ * -0.5f)) / ((height_ * 0.5f) - (height_ * -0.5f));
				result.m_[14] = -(far_ + near_) / (far_ - near_);
				result.m_[15] = 1.0f;
			}
			return result;
		}

		// View matrix...
		matrix44 V() const
		{
			vec3 f = unitise(subtract(focal(), position_));
			vec3 s = unitise(cross(f, up_));
			vec3 u = cross(s, f);

			return 
			{
				s.x_,	s.y_,	s.z_,	-dot(s, position_),
				u.x_,	u.y_,	u.z_,	-dot(u, position_),
				-f.x_,	-f.y_,	-f.z_,	dot(f, position_),
				0.0f,	0.0f,	0.0f,	1.0f
			};
		}
		
		void position(float x, float y, float z) { position_ = { x, y, z }; }
		
		void focus(const vec3& v, const vec3& up)
		{ 
			// N.B If focus is position!!!!
			// calculate new axis....
			vec3 dir = subtract(v, position_);
			focal_distance_ = length(dir);
			checkNaN(focal_distance_);
			out_ = unitise(dir);
			checkNaN(out_);
			along_ = cross(out_, unitise(up));
			checkNaN(along_);
			up_ = cross(along_, out_);
			checkNaN(up_);
		}

		// orbit a focal point...
		void orbit(const vec3& up, const vec3& along, float bearing, float elevation, float distance)
		{
			auto focus = focal();
			along_ = rotate_vec3(along, up, bearing - (GLOC_PI * 0.5f));
			checkNaN(along_);
			up_ = rotate_vec3(up, along_, elevation);
			checkNaN(up_);
			out_ = unitise(scale(cross(along_, up_), -1.0f));
			checkNaN(out_);
			position_ = add(focus, scale(out_, -distance));
			checkNaN(position_);
			focal_distance_ = distance;
		}

		// rotate about an arbitary point...
		void rotate(const vec3& origin, const vec3& up, const vec3& along, float theta, float phi)
		{
			//auto arm = rotate_vec3(subtract(position_, origin), up, theta);
			



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

		float heading(const vec3& up, const vec3& north)
		{
			// project onto horizontal plane...
			auto v = unitise(add(out_, scale(up, -point_plane_distance(out_, { 0, 0, 0 }, up))));
			checkNaN(v);
			float d = dot(north, v);
			checkNaN(d);
			float h = safe_acos(d);
			checkNaN(h);
			return h < 0 ? (GLOC_PI * 2.0f) - h : h;
		}

		float elevation(const vec3& up)
		{
			// calculate angle from north...
			auto v = unitise(add(out_, scale(up, -point_plane_distance(out_, { 0, 0, 0 }, up))));
			checkNaN(v);
			float d = dot(v, out_);
			checkNaN(d);
			float e = safe_acos(d);
			checkNaN(e);
			return e;
		}

		std::pair<vec3, vec3> ray(const vec2& view_coordinate)
		{
			auto viewport_point = world_coordinate(view_coordinate, 0.0f);
			return std::make_pair(viewport_point, fov_ ? unitise(subtract(viewport_point, projection_reference_point())) : out_);
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

		vec3 projection_reference_point()
		{
			float projection_reference_distance = (width_ * 0.5f) / (tan((fov_ * (GLOC_PI / 180.0f)) * 0.5f));
			return add(position_, scale(out_, -projection_reference_distance));
		}

		// world 
		vec3 world_coordinate(const vec2& view_cooridnate, float depth)
		{
			float u = view_cooridnate.x_ - 0.5f;
			float v = view_cooridnate.y_ - 0.5f;

			vec3 viewport_point =
			{
				position_.x_ + (along_.x_ * (u * width_)) + (up_.x_ * (v * height_)),
				position_.y_ + (along_.y_ * (u * width_)) + (up_.y_ * (v * height_)),
				position_.z_ + (along_.z_ * (u * width_)) + (up_.z_ * (v * height_))
			};

			return fov_ ? 
				add(viewport_point, scale(unitise(subtract(viewport_point, projection_reference_point())), depth)) :		// perspective
				add(viewport_point, scale(out_, depth));																	// ortho
		}

		vec2 view_coordinate(const vec3& world)
		{
			// Calculate the viewport intersection point...
			if (point_plane_distance(world, position_, out_) < 0)
				return { NAN, NAN };

			// Calculate the intersection...
			vec3 viewport_intersection = ray_plane_intersect(world, fov_ ? unitise(subtract(projection_reference_point(), world)) : scale(out_, -1.0f), position_, out_);

			// calculate the viewport coord...
			vec3 blc = world_coordinate({ 0.0f, 0.0f }, 0.0f);
			
			return { point_plane_distance(viewport_intersection, blc, along_), point_plane_distance(viewport_intersection, blc, up_) };
		}

		vec2 view_coordinate_normalised(const vec2& vc)
		{
			return { vc.x_ / width_, vc.x_ / height_ };
		}

		void zoom(const std::vector<vec3>& vertices)
		{
			// Calculate bounding sphere...
			vec3 mn = vertices[0];
			vec3 mx = vertices[0];
			for (unsigned int v = 0; v < vertices.size(); v += 3)
			{
				mn.x_ = (std::min)(mn.x_, vertices[v].x_);
				mn.y_ = (std::min)(mn.y_, vertices[v].y_);
				mn.z_ = (std::min)(mn.z_, vertices[v].z_);
				mx.x_ = (std::max)(mx.x_, vertices[v].x_);
				mx.y_ = (std::max)(mx.y_, vertices[v].y_);
				mx.z_ = (std::max)(mx.z_, vertices[v].z_);
			}

			// Position camera at edge of sphere (wrt to out direction)...
			vec3 mid = scale(add(mn, mx), 0.5f);
			float radius = length(mid) * 1.1f;
			position_ = add(mid, scale(out_, -radius));
			focal_distance_ = radius;
			far_ = 2 * radius;

			// Project all the points to view coordinates deduce the view region...
			vec2 vpmn = view_coordinate(vertices.front());
			vec2 vpmx = vpmn;
			for (unsigned int v = 0; v < vertices.size(); ++v)
			{
				auto viewport_point = view_coordinate(vertices[v]);
				vpmn.x_ = (std::min)(vpmn.x_, viewport_point.x_);
				vpmn.y_ = (std::min)(vpmn.y_, viewport_point.y_);
				vpmx.x_ = (std::max)(vpmx.x_, viewport_point.x_);
				vpmx.y_ = (std::max)(vpmx.y_, viewport_point.y_);
			}
				
			// Calculate the current aspect ratio(s)...
			float vp_aspect = (vpmx.x_ - vpmn.x_) / (vpmx.y_ - vpmn.y_);
			float aspect = width_ / height_;

			if (aspect >= vp_aspect)
			{
				width_ = (vpmx.y_ - vpmn.y_) * aspect;
				height_ = vpmx.y_ - vpmn.y_;
			}
			else
			{
				width_ = vpmx.x_ - vpmn.x_;
				height_ = (vpmx.x_ - vpmn.x_) / aspect;
			}
		}

		void zoom(const std::vector<float>& vertices)
		{
			// Calculate bounding sphere...
			std::vector<vec3> verts(vertices.size() / 3);
			for (unsigned int v = 0; v < verts.size(); ++v)
				verts[v] = { vertices[(v * 3) + 0], vertices[(v * 3) + 1], vertices[(v * 3) + 2] };

			return zoom(verts);
		}

	};

}

#endif // GLOC_HPP