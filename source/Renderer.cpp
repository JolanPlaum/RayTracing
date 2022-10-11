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

	float multiply{ 2.f * camera.fov / (float)m_Height };
	float xAddition{ (1.f - m_Width) / 2.f };
	float yAddition{ (m_Height - 1.f) / 2.f };

	//For each pixel
	for (int px{}; px < m_Width; ++px)
	{
		//Convert x coordinate to camera space
		float cx{ multiply * (px + xAddition) };

		for (int py{}; py < m_Height; ++py)
		{
			//Convert y coordinate to camera space
			float cy{ multiply * (-py + yAddition) };

			//Convert camera space to world space
			Vector3 rayDirection = (cx * camera.right + cy * camera.up + camera.forward).Normalized();

			//Ray we are casting from the camera towards each pixel
			Ray viewRay{ camera.origin, rayDirection };

			//Color to write to the color buffer
			ColorRGB finalColor{};

			//HitRecord containing more information about a potential hit
			HitRecord closestHit{};
			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				//If we hit something, keep track of the material color
				Material* const mat = materials[closestHit.materialIndex];

				//Normalize closest hit normal
				closestHit.normal.Normalize();

				for (const Light& light : lights)
				{
					//Light direction
					Vector3 invLightDirection = LightUtils::GetDirectionToLight(light, closestHit.origin);
					float distance = invLightDirection.Normalize();

					//Shawdow
					Ray invLightRay{ closestHit.origin, invLightDirection, 0.001f, distance };
					if (m_ShadowsEnabled && pScene->DoesHit(invLightRay)) continue;

					//Observed area (lambert cosine law)
					float dotProduct = closestHit.normal * invLightDirection;

					//Lighting equation
					switch (m_CurrentLightingMode)
					{
					case Renderer::LightingMode::ObservedArea:
						if (dotProduct < 0.f) continue;
						finalColor += {dotProduct, dotProduct, dotProduct};
						break;

					case Renderer::LightingMode::Radiance:
						finalColor += LightUtils::GetRadiance(light, closestHit.origin);
						break;

					case Renderer::LightingMode::BRDF:
						finalColor += mat->Shade(closestHit, invLightDirection, -rayDirection);
						break;

					case Renderer::LightingMode::Combined:
						if (dotProduct < 0.f) continue;
						finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dotProduct
							* mat->Shade(closestHit, invLightDirection, -rayDirection);
						break;
					}
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
