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
#include <future> //async
#include <ppl.h> //parallel_for

using namespace dae;

//#define ASYNC
#define PARALLEL_FOR

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);

	m_XAddition = (1.f - m_Width) / 2.f;
	m_YAddition = (m_Height - 1.f) / 2.f;
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t nrPixels = m_Width * m_Height;

#if defined(ASYNC)
	//Async Logic
	const uint32_t nrCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t nrPixelsPerTask = nrPixels / nrCores;
	uint32_t nrUnassignedPixels = nrPixels % nrCores;
	uint32_t currPixelIndex = 0;

	for (uint32_t coreId{ 0 }; coreId < nrCores; ++coreId)
	{
		uint32_t taskSize = nrPixelsPerTask;
		if (nrUnassignedPixels > 0)
		{
			++taskSize;
			--nrUnassignedPixels;
		}

		async_futures.push_back(std::async(std::launch::async, [=, this] {
			//Render all pizels for this task (currPixelIndex > currPixelIndex + taskSize)
			const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
			for (uint32_t pixelIndex{ currPixelIndex }; pixelIndex < pixelIndexEnd; ++pixelIndex)
			{
				RenderPixel(pScene, pixelIndex, camera, lights, materials);
			}
			}));

		currPixelIndex += taskSize;
	}

	//Wait for async completion for all tasks
	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}
#elif defined(PARALLEL_FOR)
	//Parellel-For Logic
	concurrency::parallel_for(0u, nrPixels, [=, this](int i) {
		RenderPixel(pScene, i, camera, lights, materials);
		});
#else
	//Synchronous Logic (no threading)
	for (uint32_t i{ 0 }; i < nrPixels; ++i)
	{
		RenderPixel(pScene, i, camera, lights, materials);
	}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float multiply{ 2.f * camera.fov / (float)m_Height };

	float cx{ multiply * (px + m_XAddition) };
	float cy{ multiply * (-py + m_YAddition) };

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

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
