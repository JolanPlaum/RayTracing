#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		//Temporary value to pass to HitTest functions
		HitRecord hitRecord{};

		for (auto& sphere : m_SphereGeometries)
		{
			//Perform Sphere HitTest
			GeometryUtils::HitTest_Sphere(sphere, ray, hitRecord);

			//Update closest hit if new hit is closer
			if (hitRecord.t < closestHit.t)
			{
				closestHit.didHit = true;
				closestHit.materialIndex = hitRecord.materialIndex;
				closestHit.normal = hitRecord.normal;
				closestHit.origin = hitRecord.origin;
				closestHit.t = hitRecord.t;
			}
		}

		for (auto& plane : m_PlaneGeometries)
		{
			//Perform Plane HitTest
			GeometryUtils::HitTest_Plane(plane, ray, hitRecord);

			//Update closest hit if new hit is closer
			if (hitRecord.t < closestHit.t)
			{
				closestHit.didHit = true;
				closestHit.materialIndex = hitRecord.materialIndex;
				closestHit.normal = hitRecord.normal;
				closestHit.origin = hitRecord.origin;
				closestHit.t = hitRecord.t;
			}
		}

		for (auto& triangleMesh : m_TriangleMeshGeometries)
		{
			//Perform TriangleMesh HitTest
			GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray, hitRecord);

			//Update closest hit if new hit is closer
			if (hitRecord.t < closestHit.t)
			{
				closestHit.didHit = true;
				closestHit.materialIndex = hitRecord.materialIndex;
				closestHit.normal = hitRecord.normal;
				closestHit.origin = hitRecord.origin;
				closestHit.t = hitRecord.t;
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (auto& sphere : m_SphereGeometries)
		{
			//Perform Sphere HitTest
			if (GeometryUtils::HitTest_Sphere(sphere, ray)) return true;
		}

		for (auto& plane : m_PlaneGeometries)
		{
			//Perform Plane HitTest
			if (GeometryUtils::HitTest_Plane(plane, ray)) return true;
		}

		for (auto& triangleMesh : m_TriangleMeshGeometries)
		{
			//Perform TriangleMesh HitTest
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray)) return true;
		}

		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENES
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f,3.f,-9.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({-5.f, 0.f, 0.f }, { 1.f,0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f,	0.f, 0.f }, { -1.f,0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f,	0.f, 0.f }, { 0.f,1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f,10.f, 0.f }, { 0.f,-1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f,	0.f,10.f }, { 0.f,0.f,-1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({ -1.75f,	1.f,	0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f,	1.f,	0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f,	1.f,	0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f,	3.f,	0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f,	3.f,	0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f,	3.f,	0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f,5.f,-5.f }, 70.f, colors::White);
	}
	void Scene_W3_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//Materials
		const unsigned char matId_Lambert_Red = AddMaterial(new Material_Lambert(colors::Red, 1.f));
		const unsigned char matId_LambertPhong_Blue = AddMaterial(new Material_LambertPhong(colors::Blue, 1.f, 1.f, 60.f));
		const unsigned char matId_Lambert_Yellow = AddMaterial(new Material_Lambert(colors::Yellow, 1.f));

		//Spheres
		AddSphere({ -0.75f, 1.f, 0.f }, 1.f, matId_Lambert_Red);
		AddSphere({ 0.75f, 1.f, 0.f }, 1.f, matId_LambertPhong_Blue);

		//Plane
		AddPlane({ 0.f,0.f,0.f }, { 0.f,1.f,0.f }, matId_Lambert_Yellow);

		//Light
		AddPointLight({ 0.f,5.f,5.f }, 25.f, colors::White);
		AddPointLight({ 0.f,2.5f,-5.f }, 25.f, colors::White);
	}
	void Scene_W3_Scene::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//Materials
		const unsigned char matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 1.f));
		const unsigned char matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 0.6f));
		const unsigned char matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 0.1f));
		const unsigned char matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
		const unsigned char matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 0.6f));
		const unsigned char matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 0.1f));

		const unsigned char matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Spheres
		AddSphere({ -1.75f,	1.f,	0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f,	1.f,	0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f,	1.f,	0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f,	3.f,	0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f,	3.f,	0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f,	3.f,	0.f }, .75f, matCT_GraySmoothPlastic);

		//Lights
		AddPointLight({ 0.f,	5.f,	5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f }); //Backlight
		AddPointLight({ -2.5f,	5.f,	-5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f,	2.5f,	-5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}

	void Scene_W4_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//Materials
		const unsigned char matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const unsigned char matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Triangle Mesh
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		//Utils::ParseOBJ("Resources/simple_cube.obj",
		//Utils::ParseOBJ("Resources/simple_object.obj",
		Utils::ParseOBJ("Resources/simple_quad.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 0.7f, 0.7f, 0.7f });
		pMesh->Translate({ 0.f, 1.f, 0.f });

		pMesh->UpdateTransforms();


		//Lights
		AddPointLight({ 0.f,	5.f,	5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f }); //Backlight
		AddPointLight({ -2.5f,	5.f,	-5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f,	2.5f,	-5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
	void Scene_W4_TestScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}

	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//Materials
		const unsigned char matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 1.f));
		const unsigned char matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 0.6f));
		const unsigned char matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 0.1f));
		const unsigned char matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
		const unsigned char matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 0.6f));
		const unsigned char matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 0.1f));

		const unsigned char matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const unsigned char matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Spheres
		AddSphere({ -1.75f,	1.f,	0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f,	1.f,	0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f,	1.f,	0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f,	3.f,	0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f,	3.f,	0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f,	3.f,	0.f }, .75f, matCT_GraySmoothPlastic);

		//CW Winding Order!
		const Triangle baseTriangle = { Vector3(-0.75f, 1.5f, 0.f), Vector3(0.75f, 0.f, 0.f), Vector3(-0.75f,0.f,0.f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f,	5.f,	5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f }); //Backlight
		AddPointLight({ -2.5f,	5.f,	-5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f,	2.5f,	-5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}

	void Scene_W4_BunnyScene::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;
		m_Camera.fov = tanf(m_Camera.fovAngle / 2.f * TO_RADIANS);

		//Materials
		const unsigned char matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence(fresnel::Silver, 1.f, 0.6f));
		const unsigned char matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const unsigned char matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//Triangle Mesh
		const auto triangleMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matCT_GrayMediumMetal);
		Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
			triangleMesh->positions,
			triangleMesh->normals,
			triangleMesh->indices);

		triangleMesh->Scale({ 2.f, 2.f, 2.f });

		triangleMesh->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f,	5.f,	5.f }, 50.f, ColorRGB{ 1.f, 0.61f, 0.45f }); //Backlight
		AddPointLight({ -2.5f,	5.f,	-5.f }, 70.f, ColorRGB{ 1.f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f,	2.5f,	-5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
#pragma endregion
}
