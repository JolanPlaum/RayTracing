//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	//const Matrix cameraToWorld = camera.CalculateCameraToWorld();

	//For each pixel
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			//Calculate center of pixel in raster space
			float pxc{ px + 0.5f };
			float pyc{ py + 0.5f };

			//Convert coordinate from raster space to camera space
			float cx{ (((2 * pxc) / m_Width) - 1.f) * ((float)m_Width / (float)m_Height) * camera.fov };
			float cy{ (1.f - ((2.f * pyc) / float(m_Height))) * camera.fov };

			//Convert camera space to world space
			//auto rayDirection = Vector3::Lico(cx, camera.right, cy, camera.up, 1.f, camera.forward).Normalized(); //average of 33.0 fps
			//auto rayDirection = cameraToWorld.TransformVector(cx, cy, 1.f).Normalized(); //average of 32.9 fps
			auto rayDirection = (cx * camera.right + cy * camera.up + camera.forward).Normalized(); //average of 36.2 fps

			//Ray we are casting from the camera towards each pixel
			Ray viewRay{ camera.origin, rayDirection };

			//Color to write to the color buffer
			ColorRGB finalColor{};

			//HitRecord containing more information about a potential hit
			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);
			closestHit.normal.Normalize();

			if (closestHit.didHit)
			{
				//If we hit something, keep track of the material color
				Material* const mat = materials[closestHit.materialIndex];

				for (const Light& light : lights)
				{
					//Light direction
					Vector3 invLightDirection = LightUtils::GetDirectionToLight(light, closestHit.origin);
					float distance = invLightDirection.Normalize();

					//Observed area (lambert cosine law)
					float dotProduct = Vector3::Dot(closestHit.normal, invLightDirection);
					if (dotProduct < 0.f) continue;

					//Shawdow
					Ray invLightRay{ closestHit.origin, invLightDirection, 0.001f, distance };
					if (pScene->DoesHit(invLightRay)) continue;

					//Lighting equation
					finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dotProduct 
						* mat->Shade(closestHit, -invLightDirection, rayDirection);
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
