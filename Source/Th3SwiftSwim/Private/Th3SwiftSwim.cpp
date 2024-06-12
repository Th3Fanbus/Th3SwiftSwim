/* SPDX-License-Identifier: MPL-2.0 */

#include "Th3SwiftSwim.h"
#include "GameFramework/PhysicsVolume.h"
#include "FGCharacterMovementComponent.h"
#include "FGCharacterPlayer.h"
#include "FGJumpingStilts.h"
#include "Patching/NativeHookManager.h"

class HFGCharacterMovementComponent
{
public:
	static void Hook_GetMaxSpeed(TCallScope<float(__cdecl*)(const UFGCharacterMovementComponent*)>& scope, const UFGCharacterMovementComponent* self)
	{
		if (self->IsSwimming()) {
			float Result = FMath::Max(self->MaxWalkSpeed, scope(self));
			if (self->GetIsSprinting()) {
				Result = FMath::Max(Result, self->mMaxSprintSpeed);
				AFGJumpingStilts* Stilts = self->mCachedJumpingStilts;
				if (IsValid(Stilts) and IsValid(Stilts->GetInstigator())) {
					Result = Stilts->GetAdjustedMaxSpeed(self->mMaxSprintSpeed);
				}
			}
			scope.Override(Result);
		}
	}

	static void Hook_CanSprint(TCallScope<bool(__cdecl*)(const UFGCharacterMovementComponent*)>& scope, const UFGCharacterMovementComponent* self)
	{
		if (self->IsSwimming()) {
			scope.Override(true);
		}
	}

	static void RegisterHooks()
	{
		SUBSCRIBE_UOBJECT_METHOD(UFGCharacterMovementComponent, GetMaxSpeed, &HFGCharacterMovementComponent::Hook_GetMaxSpeed);
		SUBSCRIBE_UOBJECT_METHOD(UFGCharacterMovementComponent, CanSprint, &HFGCharacterMovementComponent::Hook_CanSprint);
	}
};

void FTh3SwiftSwimModule::StartupModule()
{
	if (not WITH_EDITOR) {
		HFGCharacterMovementComponent::RegisterHooks();
	}
}

void FTh3SwiftSwimModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FTh3SwiftSwimModule, Th3SwiftSwim)