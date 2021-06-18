#include "stdafx.h"

#include "helpers/V8Helpers.h"
#include "helpers/V8ResourceImpl.h"
#include "helpers/V8BindHelpers.h"

using namespace alt;

#include "vehicle/Appearance.h"
#include "vehicle/GameState.h"
#include "vehicle/Health.h"
#include "vehicle/Damage.h"
#include "vehicle/ScriptGameState.h"

using namespace V8::Vehicle;

static void DriverGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT_RESOURCE();
	V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
	V8_RETURN_BASE_OBJECT(_this->GetDriver());
}

static void GetAttached(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT_RESOURCE();
	V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
	V8_RETURN_BASE_OBJECT(_this->GetAttached());
}

static void GetAttachedTo(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT_RESOURCE();
	V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
	V8_RETURN_BASE_OBJECT(_this->GetAttachedTo());
}

static void DestroyedGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT();
	V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
	V8_RETURN_BOOLEAN(_this->IsDestroyed());
}

static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT_RESOURCE();
	V8_CHECK_CONSTRUCTOR();
	V8_CHECK_ARGS_LEN(7);

	V8_CHECK(info[0]->IsString() || info[0]->IsNumber(), "string or number expected");

	uint32_t modelHash;

	if (info[0]->IsNumber())
	{
		modelHash = info[0]->ToUint32(isolate->GetEnteredContext()).ToLocalChecked()->Value();
	}
	else
	{
		V8_ARG_TO_STRING(1, model);
		modelHash = alt::ICore::Instance().Hash(model);
	}

	V8_ARG_TO_NUMBER(2, x);
	V8_ARG_TO_NUMBER(3, y);
	V8_ARG_TO_NUMBER(4, z);
	V8_ARG_TO_NUMBER(5, rx);
	V8_ARG_TO_NUMBER(6, ry);
	V8_ARG_TO_NUMBER(7, rz);

	alt::Position pos(x, y, z);
	alt::Rotation rot(rx, ry, rz);

	Ref<IVehicle> veh = alt::ICore::Instance().CreateVehicle(modelHash, pos, rot);

	if (veh)
		resource->BindEntity(info.This(), veh.Get());
	else
		V8Helpers::Throw(isolate, "Failed to create Vehicle");
}

static void AllGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Isolate* isolate = info.GetIsolate();

	V8ResourceImpl* resource = V8ResourceImpl::Get(isolate->GetEnteredContext());
	V8_CHECK(resource, "invalid resource");

	info.GetReturnValue().Set(resource->GetAllVehicles());
}

static void StaticGetByID(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	V8_GET_ISOLATE_CONTEXT_RESOURCE();
	V8_CHECK_ARGS_LEN(1);

	V8_ARG_TO_INTEGER(1, id);

	alt::Ref<alt::IEntity> entity = alt::ICore::Instance().GetEntityByID(id);

	if (entity && entity->GetType() == alt::IEntity::Type::VEHICLE)
	{
		V8_RETURN_BASE_OBJECT(entity);
	}
	else
	{
		V8_RETURN_NULL();
	}
}

extern V8Class v8Entity;
extern V8Class v8Vehicle("Vehicle", v8Entity, Constructor, [](v8::Local<v8::FunctionTemplate> tpl) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	V8::SetStaticMethod(isolate, tpl, "getByID", StaticGetByID);
	V8::SetStaticAccessor(isolate, tpl, "all", AllGetter);

	// Common getter/setters
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsDestroyed>(isolate, tpl, "destroyed");
	V8::SetAccessor<IVehicle, Ref<IPlayer>, &IVehicle::GetDriver>(isolate, tpl, "driver");

	// Appearance getters/setters
	V8::SetAccessor(isolate, tpl, "modKit", &ModKitGetter, &ModKitSetter);
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetModKitsCount>(isolate, tpl, "modKitsCount");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetPrimaryColor, &IVehicle::SetPrimaryColor>(isolate, tpl, "primaryColor");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetSecondaryColor, &IVehicle::SetSecondaryColor>(isolate, tpl, "secondaryColor");
	V8::SetAccessor<IVehicle, RGBA, &IVehicle::GetPrimaryColorRGB, &IVehicle::SetPrimaryColorRGB>(isolate, tpl, "customPrimaryColor");
	V8::SetAccessor<IVehicle, RGBA, &IVehicle::GetSecondaryColorRGB, &IVehicle::SetSecondaryColorRGB>(isolate, tpl, "customSecondaryColor");
	V8::SetAccessor<IVehicle, RGBA, &IVehicle::GetTireSmokeColor, &IVehicle::SetTireSmokeColor>(isolate, tpl, "tireSmokeColor");
	V8::SetAccessor<IVehicle, RGBA, &IVehicle::GetNeonColor, &IVehicle::SetNeonColor>(isolate, tpl, "neonColor");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetPearlColor, &IVehicle::SetPearlColor>(isolate, tpl, "pearlColor");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetWheelColor, &IVehicle::SetWheelColor>(isolate, tpl, "wheelColor");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetInteriorColor, &IVehicle::SetInteriorColor>(isolate, tpl, "interiorColor");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetDashboardColor, &IVehicle::SetDashboardColor>(isolate, tpl, "dashboardColor");
	V8::SetAccessor<IVehicle, bool, &IVehicle::GetCustomTires, &IVehicle::SetCustomTires>(isolate, tpl, "customTires");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetSpecialDarkness, &IVehicle::SetSpecialDarkness>(isolate, tpl, "darkness");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetWindowTint, &IVehicle::SetWindowTint>(isolate, tpl, "windowTint");
	V8::SetAccessor(isolate, tpl, "neon", &NeonActiveGetter, &NeonActiveSetter);
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetDirtLevel, &IVehicle::SetDirtLevel>(isolate, tpl, "dirtLevel");
	V8::SetAccessor<IVehicle, uint32_t, &IVehicle::GetNumberplateIndex, &IVehicle::SetNumberplateIndex>(isolate, tpl, "numberPlateIndex");
	V8::SetAccessor<IVehicle, StringView, &IVehicle::GetNumberplateText, &IVehicle::SetNumberplateText>(isolate, tpl, "numberPlateText");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetLivery, &IVehicle::SetLivery>(isolate, tpl, "livery");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetRoofLivery, &IVehicle::SetRoofLivery>(isolate, tpl, "roofLivery");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetWheelType>(isolate, tpl, "wheelType");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetWheelVariation>(isolate, tpl, "frontWheels");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetRearWheelVariation>(isolate, tpl, "rearWheels");

	// Appearance methods
	V8::SetMethod(isolate, tpl, "getModsCount", &GetModsCount);
	V8::SetMethod(isolate, tpl, "setMod", &SetMod);
	V8::SetMethod(isolate, tpl, "getMod", &GetMod);
	V8::SetMethod(isolate, tpl, "setExtra", &SetExtra);
	V8::SetMethod(isolate, tpl, "getExtra", &GetExtra);
	V8::SetMethod(isolate, tpl, "setWheels", &SetWheels);
	V8::SetMethod(isolate, tpl, "setRearWheels", &SetRearWheels);
	V8::SetMethod(isolate, tpl, "getAppearanceDataBase64", &GetAppearanceData);
	V8::SetMethod(isolate, tpl, "setAppearanceDataBase64", &SetAppearanceData);

	// Gamestate getters/setters
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsEngineOn, &IVehicle::SetEngineOn>(isolate, tpl, "engineOn");
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsHandbrakeActive>(isolate, tpl, "handbrakeActive");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetHeadlightColor, &IVehicle::SetHeadlightColor>(isolate, tpl, "headlightColor");
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsSirenActive, &IVehicle::SetSirenActive>(isolate, tpl, "sirenActive");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetLockState, &IVehicle::SetLockState>(isolate, tpl, "lockState");
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsDaylightOn>(isolate, tpl, "daylightOn");
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsNightlightOn>(isolate, tpl, "nightlightOn");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetRoofState, &IVehicle::SetRoofState>(isolate, tpl, "roofState");
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsFlamethrowerActive>(isolate, tpl, "flamethrowerActive");
	V8::SetAccessor<IVehicle, uint32_t, &IVehicle::GetRadioStationIndex, &IVehicle::SetRadioStationIndex>(isolate, tpl, "activeRadioStation");
	V8::SetAccessor<IVehicle, float, &IVehicle::GetLightsMultiplier, &IVehicle::SetLightsMultiplier>(isolate, tpl, "lightsMultiplier");

	// Gamestate methods
	V8::SetMethod(isolate, tpl, "getDoorState", &GetDoorState);
	V8::SetMethod(isolate, tpl, "setDoorState", &SetDoorState);
	V8::SetMethod(isolate, tpl, "isWindowOpened", &GetWindowOpened);
	V8::SetMethod(isolate, tpl, "setWindowOpened", &SetWindowOpened);
	V8::SetMethod(isolate, tpl, "getGamestateDataBase64", &GetGamestateData);
	V8::SetMethod(isolate, tpl, "setGamestateDataBase64", &SetGamestateData);

	// Health getters/setters
	V8::SetAccessor<IVehicle, int32_t, &IVehicle::GetEngineHealth, &IVehicle::SetEngineHealth>(isolate, tpl, "engineHealth");
	V8::SetAccessor<IVehicle, int32_t, &IVehicle::GetPetrolTankHealth, &IVehicle::SetPetrolTankHealth>(isolate, tpl, "petrolTankHealth");
	V8::SetAccessor<IVehicle, uint32_t, &IVehicle::GetBodyHealth, &IVehicle::SetBodyHealth>(isolate, tpl, "bodyHealth");
	V8::SetAccessor<IVehicle, uint32_t, &IVehicle::GetBodyAdditionalHealth, &IVehicle::SetBodyAdditionalHealth>(isolate, tpl, "bodyAdditionalHealth");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetWheelsCount>(isolate, tpl, "wheelsCount");
	V8::SetAccessor<IVehicle, uint8_t, &IVehicle::GetRepairsCount>(isolate, tpl, "repairsCount");
	/*proto->SetAccessor(v8::String::NewFromUtf8(isolate, "lastAttacker"), &LastAttackerGetter);
	proto->SetAccessor(v8::String::NewFromUtf8(isolate, "lastAttackedWith"), &LastDamagedWithGetter);*/

	// Health methods
	V8::SetMethod(isolate, tpl, "setWheelBurst", &SetWheelBurst);
	V8::SetMethod(isolate, tpl, "isWheelBurst", &GetWheelBurst);
	V8::SetMethod(isolate, tpl, "setWheelHasTire", &SetWheelHasTire);
	V8::SetMethod(isolate, tpl, "doesWheelHasTire", &GetWheelHasTire);
	V8::SetMethod(isolate, tpl, "setWheelDetached", &SetWheelDetached);
	V8::SetMethod(isolate, tpl, "isWheelDetached", &GetIsWheelDetached);
	V8::SetMethod(isolate, tpl, "setWheelOnFire", &SetWheelOnFire);
	V8::SetMethod(isolate, tpl, "isWheelOnFire", &GetIsWheelOnFire);
	V8::SetMethod(isolate, tpl, "setWheelHealth", &SetWheelHealth);
	V8::SetMethod(isolate, tpl, "getWheelHealth", &GetWheelHealth);
	V8::SetMethod(isolate, tpl, "getHealthDataBase64", &GetHealthData);
	V8::SetMethod(isolate, tpl, "setHealthDataBase64", &SetHealthData);

	// Damage getters/setters
	V8::SetAccessor<IVehicle, bool, &IVehicle::HasArmoredWindows>(isolate, tpl, "hasArmoredWindows");

	// Damage methods
	V8::SetMethod(isolate, tpl, "getPartDamageLevel", &GetPartDamageLevel);
	V8::SetMethod(isolate, tpl, "setPartDamageLevel", &SetPartDamageLevel);
	V8::SetMethod(isolate, tpl, "getPartBulletHoles", &GetPartBulletHoles);
	V8::SetMethod(isolate, tpl, "setPartBulletHoles", &SetPartBulletHoles);
	V8::SetMethod(isolate, tpl, "isLightDamaged", &IsLightDamaged);
	V8::SetMethod(isolate, tpl, "setLightDamaged", &SetLightDamaged);
	V8::SetMethod(isolate, tpl, "isWindowDamaged", &IsWindowDamaged);
	V8::SetMethod(isolate, tpl, "setWindowDamaged", &SetWindowDamaged);
	V8::SetMethod(isolate, tpl, "isSpecialLightDamaged", &IsSpecialLightDamaged);
	V8::SetMethod(isolate, tpl, "setSpecialLightDamaged", &SetSpecialLightDamaged);
	V8::SetMethod(isolate, tpl, "getBumperDamageLevel", &GetBumperDamageLevel);
	V8::SetMethod(isolate, tpl, "setBumperDamageLevel", &SetBumperDamageLevel);
	V8::SetMethod(isolate, tpl, "getArmoredWindowHealth", &GetArmoredWindowHealth);
	V8::SetMethod(isolate, tpl, "setArmoredWindowHealth", &SetArmoredWindowHealth);
	V8::SetMethod(isolate, tpl, "getArmoredWindowShootCount", &GetArmoredWindowShootCount);
	V8::SetMethod(isolate, tpl, "setArmoredWindowShootCount", &SetArmoredWindowShootCount);
	V8::SetMethod(isolate, tpl, "getDamageStatusBase64", &GetDamageStatus);
	V8::SetMethod(isolate, tpl, "setDamageStatusBase64", &SetDamageStatus);
	V8::SetMethod(isolate, tpl, "repair", &SetFixed);
	V8::SetMethod(isolate, tpl, "setWheelFixed", &SetWheelFixed);

	// Script getters/setters
	V8::SetAccessor<IVehicle, bool, &IVehicle::IsManualEngineControl, &IVehicle::SetManualEngineControl>(isolate, tpl, "manualEngineControl");

	// Script methods
	V8::SetMethod(isolate, tpl, "getScriptDataBase64", &GetScriptData);
	V8::SetMethod(isolate, tpl, "setScriptDataBase64", &SetScriptData);


	V8::SetMethod(isolate, tpl, "getAttached", &GetAttached);
	V8::SetMethod(isolate, tpl, "getAttachedTo", &GetAttachedTo);
});
