#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

#define IGNOREHITRECORD_SEPERATE
//#define HITTEST_SPHERE_ANALYTIC

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
	#if defined(HITTEST_SPHERE_ANALYTIC)
			//Variables for quadratic equation
			float a{ ray.direction.SqrMagnitude() };
			float b{ Vector3::Dot(2.f * ray.direction, ray.origin - sphere.origin) };
			float c{ (ray.origin - sphere.origin).SqrMagnitude() - sphere.radius * sphere.radius};

			//Return false if ray does not intersect (discriminant needs to be larger than 0)
			float discriminant{ b * b - 4.f * a * c };
			if (discriminant <= 0.f) return false;

			//Set discrimant as the square root of itself (prevents having to calculate it multiple times)
			discriminant = sqrtf(discriminant);

			//Calculate smaller interval t at which the ray intersects
			float t = (-b - discriminant) / (2.f * a);
			if (t < ray.min || t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				t = (-b + discriminant) / (2.f * a);

				//Return false if t is still outside ray interval
				if (t < ray.min || t > ray.max)
					return false;
			}

		#if !defined(IGNOREHITRECORD_SEPERATE)
			//Return true if hitrecord can be ignored
			if (ignoreHitRecord)
				return true;
		#endif

			//Set hit values and return true
			hitRecord.t = t;
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
			hitRecord.materialIndex = sphere.materialIndex;
			return hitRecord.didHit = true;
	#else
			//Vector from ray origin to sphere origin
			Vector3 tc = sphere.origin - ray.origin;
			float dotProduct = tc * ray.direction;
			float oppositeSideSquared = tc.SqrMagnitude() - (dotProduct * dotProduct);

			//Return false if ray does not intersect (sphere radius needs to be larger than opposite side)
			float tcAdjacent = (sphere.radius * sphere.radius) - oppositeSideSquared;
			if (tcAdjacent < 0.f) return false;

			//Square root the ray-sphere adjacent side for the actual length (since we are now sure that it's above 0)
			tcAdjacent = sqrtf(tcAdjacent);

			//Calculate smaller interval t at which the ray intersects
			float t = dotProduct - tcAdjacent;
			if (t < ray.min || t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				t = dotProduct + tcAdjacent;

				//Return false if t is still outside ray interval
				if (t < ray.min || t > ray.max)
					return false;
			}

		#if !defined(IGNOREHITRECORD_SEPERATE)
			//Return true if hitrecord can be ignored
			if (ignoreHitRecord)
				return true;
		#endif

			//Set hit values and return true
			hitRecord.t = t;
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = hitRecord.origin - sphere.origin;
			hitRecord.materialIndex = sphere.materialIndex;
			return hitRecord.didHit = true;
	#endif
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
	#if defined(IGNOREHITRECORD_SEPERATE)
		#if defined(HITTEST_SPHERE_ANALYTIC)
			//Variables for quadratic equation
			float a{ ray.direction.SqrMagnitude() };
			float b{ Vector3::Dot(2.f * ray.direction, ray.origin - sphere.origin) };
			float c{ (ray.origin - sphere.origin).SqrMagnitude() - sphere.radius * sphere.radius };

			//Return false if ray does not intersect (discriminant needs to be larger than 0)
			float discriminant{ b * b - 4.f * a * c };
			if (discriminant <= 0.f) return false;

			//Set discrimant as the square root of itself (prevents having to calculate it multiple times)
			discriminant = sqrtf(discriminant);

			//Calculate smaller interval t at which the ray intersects
			float t = (-b - discriminant) / (2.f * a);
			if (t < ray.min || t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				t = (-b + discriminant) / (2.f * a);

				//Return false if t is still outside ray interval
				if (t < ray.min || t > ray.max)
					return false;
			}
			
			return true;
		#else
			//Vector from ray origin to sphere origin
			Vector3 tc = sphere.origin - ray.origin;
			float dotProduct = tc * ray.direction;
			float oppositeSideSquared = tc.SqrMagnitude() - (dotProduct * dotProduct);

			//Return false if ray does not intersect (sphere radius needs to be larger than opposite side)
			float tcAdjacent = (sphere.radius * sphere.radius) - oppositeSideSquared;
			if (tcAdjacent < 0.f) return false;

			//Square root the ray-sphere adjacent side for the actual length (since we are now sure that it's above 0)
			tcAdjacent = sqrtf(tcAdjacent);

			//Calculate smaller interval t at which the ray intersects
			float t = dotProduct - tcAdjacent;
			if (t < ray.min || t > ray.max)
			{
				//Calculate higher interval t if t is outside ray interval
				t = dotProduct + tcAdjacent;

				//Return false if t is still outside ray interval
				if (t < ray.min || t > ray.max)
					return false;
			}

			return true;
		#endif
	#else
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
	#endif
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//Return false if ray does not intersect (when ray direction and plane normal are perpendicular)
			float dotProduct{ ray.direction * plane.normal };
			if (dotProduct == 0.f) return false;

			//Calculate at which interval t the ray intersects
			float t = ((plane.origin - ray.origin) * plane.normal) / dotProduct;

			//Return false if t is outside ray interval
			if (t < ray.min || t > ray.max)
				return false;

#if !defined(IGNOREHITRECORD_SEPERATE)
			//Return true if hitrecord can be ignored
			if (ignoreHitRecord)
				return true;
#endif

			//Set hit values and return true
			hitRecord.t = t;
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = plane.normal;
			hitRecord.materialIndex = plane.materialIndex;
			return hitRecord.didHit = true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
#if defined(IGNOREHITRECORD_SEPERATE)
			//Return false if ray does not intersect (when ray direction and plane normal are perpendicular)
			float dotProduct{ ray.direction * plane.normal };
			if (dotProduct == 0.f) return false;

			//Calculate at which interval t the ray intersects
			float t = ((plane.origin - ray.origin) * plane.normal) / dotProduct;

			//Return false if t is outside ray interval
			if (t < ray.min || t > ray.max)
				return false;

			return true;
#else
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
#endif
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//Return false if triangle is not visible (cull mode check)
			float dotProduct{ ray.direction * triangle.normal };
#if defined(IGNOREHITRECORD_SEPERATE)
			TriangleCullMode culling{ triangle.cullMode };
#else
			TriangleCullMode culling{ (ignoreHitRecord) ? TriangleCullMode((int)triangle.cullMode * -1) : triangle.cullMode };
#endif
			switch (culling)
			{
			case TriangleCullMode::FrontFaceCulling:
				if (dotProduct <= 0.f) return false;
				break;

			case TriangleCullMode::BackFaceCulling:
				if (dotProduct >= 0.f) return false;
				break;

			case TriangleCullMode::NoCulling:
				if (dotProduct == 0.f) return false;
				break;
			}

			//Calculate center of triangle
			Vector3 center{ (triangle.v0 + triangle.v1 + triangle.v2) / 3.f };

			//Calculate at which interval t the ray intersects
			float t = ((center - ray.origin) * triangle.normal) / dotProduct;

			//Return false if t is outside ray interval
			if (t < ray.min || t > ray.max)
			{
				return false;
			}

			//Calculate point on triangle (plane)
			Vector3 point = ray.origin + t * ray.direction;

			//Check if point is inside triangle
			Vector3 edge = triangle.v1 - triangle.v0;
			Vector3 pointToSide = point - triangle.v0;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

			edge = triangle.v2 - triangle.v1;
			pointToSide = point - triangle.v1;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

			edge = triangle.v0 - triangle.v2;
			pointToSide = point - triangle.v2;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

#if !defined(IGNOREHITRECORD_SEPERATE)
			if (ignoreHitRecord)
				return true;
#endif

			//Set hit values and return true
			hitRecord.t = t;
			hitRecord.origin = point;
			hitRecord.normal = triangle.normal;
			hitRecord.materialIndex = triangle.materialIndex;
			return hitRecord.didHit = true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
#if defined(IGNOREHITRECORD_SEPERATE)
			//Return false if triangle is not visible (cull mode check)
			float dotProduct{ ray.direction * triangle.normal };
			switch (TriangleCullMode((int)triangle.cullMode * -1))
			{
			case TriangleCullMode::FrontFaceCulling:
				if (dotProduct <= 0.f) return false;
				break;

			case TriangleCullMode::BackFaceCulling:
				if (dotProduct >= 0.f) return false;
				break;

			case TriangleCullMode::NoCulling:
				if (dotProduct == 0.f) return false;
				break;
			}

			//Calculate center of triangle
			Vector3 center{ (triangle.v0 + triangle.v1 + triangle.v2) / 3.f };

			//Calculate at which interval t the ray intersects
			float t = ((center - ray.origin) * triangle.normal) / dotProduct;

			//Return false if t is outside ray interval
			if (t < ray.min || t > ray.max)
			{
				return false;
			}

			//Calculate point on triangle (plane)
			Vector3 point = ray.origin + t * ray.direction;

			//Check if point is inside triangle
			Vector3 edge = triangle.v1 - triangle.v0;
			Vector3 pointToSide = point - triangle.v0;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

			edge = triangle.v2 - triangle.v1;
			pointToSide = point - triangle.v1;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

			edge = triangle.v0 - triangle.v2;
			pointToSide = point - triangle.v2;
			if (triangle.normal * Vector3::Cross(edge, pointToSide) < 0.f) return false;

			return true;
#else
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
#endif
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			// slabtest
			if (!SlabTest_TriangleMesh(mesh, ray)) return false;

			//Temporary value to pass to HitTest function
			HitRecord record{};
			Triangle triangle{};
			triangle.cullMode = mesh.cullMode;

			//Loop over all indices in sets of 3 (each triangle has 3 points)
			for (size_t index{}; index < mesh.indices.size() - 2; index += 3)
			{
				triangle.v0 = mesh.transformedPositions[mesh.indices[index]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[index + 1]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[index + 2]];
				triangle.normal = mesh.transformedNormals[index / 3];

				// If the ray hits a triangle in the mesh, check if it is closer then the previous hit triangle
				if (HitTest_Triangle(triangle, ray, record, ignoreHitRecord))
				{
#if !defined(IGNOREHITRECORD_SEPERATE)
					// If the hit records needs to be ignored, it doesn't matter where the triangle is, so just return true
					if (ignoreHitRecord) return true;
#endif

					// Check if the current hit is closer then the previous hit
					if (hitRecord.t > record.t)
					{
						hitRecord.didHit = true;
						hitRecord.normal = record.normal;
						hitRecord.origin = record.origin;
						hitRecord.t = record.t;
					}
				}
			}

			hitRecord.materialIndex = mesh.materialIndex;
			return hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
#if defined(IGNOREHITRECORD_SEPERATE)
			// slabtest
			if (!SlabTest_TriangleMesh(mesh, ray)) return false;

			//Temporary value to pass to HitTest function
			HitRecord record{};
			Triangle triangle{};
			triangle.cullMode = mesh.cullMode;

			//Loop over all indices in sets of 3 (each triangle has 3 points)
			for (size_t index{}; index < mesh.indices.size() - 2; index += 3)
			{
				triangle.v0 = mesh.transformedPositions[mesh.indices[index]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[index + 1]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[index + 2]];
				triangle.normal = mesh.transformedNormals[index / 3];

				// If the ray hits a triangle in the mesh, check if it is closer then the previous hit triangle
				if (HitTest_Triangle(triangle, ray))
				{
					return true;
				}
			}

			return false;
#else
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
#endif
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
					std::string i0, i1, i2;
					file >> i0 >> i1 >> i2;

					if (i0 != "")
					{
						indices.push_back((int)stoi(i0) - 1);
						indices.push_back((int)stoi(i1) - 1);
						indices.push_back((int)stoi(i2) - 1);
					}
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

				if (isnan(normal.x))
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