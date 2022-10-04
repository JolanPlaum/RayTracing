#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle},
			fov{tanf(_fovAngle / 2.f * TO_RADIANS)}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{1.f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		const float movementSpeed{ 10.f };
		const float rotationSpeed{ .5f };


		Matrix CalculateCameraToWorld()
		{
			forward.Normalize();
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			return cameraToWorld = Matrix{
				right,
				up,
				forward,
				origin
			};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);


			//Camera Movement/Rotation
			float moveSpeed{ movementSpeed * deltaTime * (pKeyboardState[SDL_SCANCODE_LSHIFT] * 3 + 1) };
			float rotSpeed{ rotationSpeed * deltaTime };

			origin += pKeyboardState[SDL_SCANCODE_W] * forward * moveSpeed;
			origin -= pKeyboardState[SDL_SCANCODE_S] * forward * moveSpeed;
			origin += pKeyboardState[SDL_SCANCODE_D] * right * moveSpeed;
			origin -= pKeyboardState[SDL_SCANCODE_A] * right * moveSpeed;

			bool lmb = mouseState == SDL_BUTTON_LMASK;
			bool rmb = mouseState == SDL_BUTTON_RMASK;
			bool lrmb = mouseState == (SDL_BUTTON_LMASK ^ SDL_BUTTON_RMASK);
			origin -= lmb * forward * moveSpeed * (float)mouseY;
			origin -= lrmb * up * moveSpeed * (float)mouseY;

			totalPitch -= rmb * rotSpeed * (float)mouseY;
			totalYaw += lmb * rotSpeed * (float)mouseX;
			totalYaw += rmb * rotSpeed * (float)mouseX;

			Matrix finalRotation{ Matrix::CreateRotationX(totalPitch) * Matrix::CreateRotationY(totalYaw) };
			forward = finalRotation.TransformVector(Vector3::UnitZ);

			CalculateCameraToWorld();
		}
	};
}
