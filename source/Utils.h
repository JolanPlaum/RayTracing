#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			/*
			*/
			Vector3 tc = sphere.origin - ray.origin;
			float dp = Vector3::Dot(tc, ray.direction);
			float odSquare = tc.SqrMagnitude() - (dp * dp);
			float tca = (sphere.radius * sphere.radius) - odSquare;
			if (tca < 0.f) return false;
			tca = sqrtf(tca);
			hitRecord.t = dp - tca;

			if (hitRecord.t < ray.min || hitRecord.t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				hitRecord.t = dp + tca;

				//Return false if t is still outside ray interval
				if (hitRecord.t < ray.min || hitRecord.t > ray.max)
				{
					hitRecord.t = FLT_MAX;
					return false;
				}
			}

			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = hitRecord.origin - sphere.origin;
			hitRecord.materialIndex = sphere.materialIndex;
			return hitRecord.didHit = true;

			/*
			//Variables for quadratic equation
			float a{ ray.direction.SqrMagnitude() };
			float b{ Vector3::Dot(2.f * ray.direction, ray.origin - sphere.origin) };
			float c{ (ray.origin - sphere.origin).SqrMagnitude() - Square(sphere.radius)};

			//Return false if ray does not intersect (discriminant needs to be larger than 0)
			float discriminant{ Square(b) - 4.f * a * c };
			if (discriminant <= 0.f) return false;

			//Set discrimant as the square root of itself (prevents having to calculate it multiple times)
			discriminant = sqrtf(discriminant);

			//Calculate smaller interval t at which the ray intersects
			hitRecord.t = (-b - discriminant) / (2.f * a);
			if (hitRecord.t < ray.min || hitRecord.t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				hitRecord.t = (-b + discriminant) / (2.f * a);

				//Return false if t is still outside ray interval
				if (hitRecord.t < ray.min || hitRecord.t > ray.max)
				{
					hitRecord.t = FLT_MAX;
					return false;
				}
			}

			//Set hit values and return true
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
			hitRecord.materialIndex = sphere.materialIndex;
			return hitRecord.didHit = true;
			*/
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//Return false if ray does not intersect (when ray direction and plane normal are perpendicular)
			float dotProduct{ Vector3::Dot(ray.direction, plane.normal) };
			if (dotProduct == 0.f) return false;

			//Calculate at which interval t the ray intersects
			hitRecord.t = Vector3::Dot(plane.origin - ray.origin, plane.normal) / dotProduct;

			//Return false if t is outside ray interval
			if (hitRecord.t < ray.min || hitRecord.t > ray.max)
			{
				hitRecord.t = FLT_MAX;
				return false;
			}

			//Set hit values and return true
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = plane.normal;
			hitRecord.materialIndex = plane.materialIndex;
			return hitRecord.didHit = true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			switch (light.type)
			{
			case LightType::Point:
				return light.origin - origin;

			case LightType::Directional:
				return -light.direction;

			default:
				return {};
			}
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			switch (light.type)
			{
			case LightType::Point:
				return light.color * (light.intensity / (light.origin - target).SqrMagnitude());

			case LightType::Directional:
				return light.color * light.intensity;

			default:
				return {};
			}
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}