#pragma once

namespace V8::Vehicle
{
	void ModKitGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE();
		V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
		V8_RETURN_INTEGER(_this->GetModKit());
	}

	void ModKitSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(_this, IVehicle);
		V8_TO_INTEGER(value, _val);
		V8_CHECK(_this->SetModKit(_val), "wrong modkit");
	}

	void NeonActiveGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(_this, IVehicle);

		bool left, right, front, back;
		_this->GetNeonActive(&left, &right, &front, &back);

		v8::Local<v8::Object> neonActive = v8::Object::New(isolate);
		V8::DefineOwnProperty(isolate, ctx, neonActive, "left", v8::Boolean::New(isolate, left));
		V8::DefineOwnProperty(isolate, ctx, neonActive, "right", v8::Boolean::New(isolate, right));
		V8::DefineOwnProperty(isolate, ctx, neonActive, "front", v8::Boolean::New(isolate, front));
		V8::DefineOwnProperty(isolate, ctx, neonActive, "back", v8::Boolean::New(isolate, back));

		V8_RETURN(neonActive);
	}

	void NeonActiveSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(_this, IVehicle);

		V8_TO_OBJECT(value, neonActive);
		V8_OBJECT_GET_BOOLEAN(neonActive, "left", left);
		V8_OBJECT_GET_BOOLEAN(neonActive, "right", right);
		V8_OBJECT_GET_BOOLEAN(neonActive, "front", front);
		V8_OBJECT_GET_BOOLEAN(neonActive, "back", back);

		_this->SetNeonActive(left, right, front, back);
	}

	void SetWheels(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(2);

		V8_ARG_TO_INTEGER(1, type);
		V8_ARG_TO_INTEGER(2, variation);

		vehicle->SetWheels(type, variation);
	}

	void SetRearWheels(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(1);

		V8_ARG_TO_INTEGER(1, variation);

		vehicle->SetRearWheels(variation);
	}

	void GetModsCount(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(1);

		V8_ARG_TO_INTEGER(1, category);

		V8_RETURN_INTEGER(vehicle->GetModsCount(category));
	}

	void SetMod(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(2);

		V8_ARG_TO_INTEGER(1, category);
		V8_ARG_TO_INTEGER(2, id);

		if (!vehicle->SetMod(category, id))
		{
			if (vehicle->GetModKit() == 0)
				V8Helpers::Throw(isolate, "modkit is not set");
			else
				V8Helpers::Throw(isolate, "mod doesn't exist");
		}
	}

	void GetMod(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(1);

		V8_ARG_TO_INTEGER(1, category);

		V8_RETURN_INTEGER(vehicle->GetMod(category));
	}

	void SetExtra(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(2);

		V8_ARG_TO_INTEGER(1, category);
		V8_ARG_TO_BOOLEAN(2, state);

		vehicle->ToggleExtra(category, state);
	}

	void GetExtra(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(1);

		V8_ARG_TO_INTEGER(1, category);

		V8_RETURN_BOOLEAN(vehicle->IsExtraOn(category));
	}

	void SetAppearanceData(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE_CONTEXT();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);
		V8_CHECK_ARGS_LEN(1);

		V8_ARG_TO_STRING(1, data);

		vehicle->LoadAppearanceDataFromBase64(data);
	}

	void GetAppearanceData(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		V8_GET_ISOLATE();
		V8_GET_THIS_BASE_OBJECT(vehicle, IVehicle);

		V8_RETURN_STRING(vehicle->GetAppearanceDataBase64().CStr());
	}
}
