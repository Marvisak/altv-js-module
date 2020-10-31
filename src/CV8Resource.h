#pragma once

#include "cpp-sdk/IResource.h"
#include "cpp-sdk/entities/IEntity.h"

#include "helpers/V8ResourceImpl.h"

#include <queue>

#include "helpers/Log.h"

class CV8ScriptRuntime;

class CV8ResourceImpl : public V8ResourceImpl
{
public:
	CV8ResourceImpl(alt::IResource *resource, v8::Isolate *isolate) : V8ResourceImpl(isolate, resource)
	{
	}

	~CV8ResourceImpl()
	{
		Log::Debug << __FUNCTION__;
	}

	bool Start() override;

	bool Stop() override;

	bool OnEvent(const alt::CEvent *ev) override;

	void OnTick() override;

	void OnPromiseRejectedWithNoHandler(v8::PromiseRejectMessage &data);
	void OnPromiseHandlerAdded(v8::PromiseRejectMessage &data);

	void SubscribeWebView(alt::Ref<alt::IWebView> view, const std::string &evName, v8::Local<v8::Function> cb, V8::SourceLocation &&location)
	{
		webViewHandlers[view].insert({evName, V8::EventCallback{isolate, cb, std::move(location)}});
	}

	void UnsubscribeWebView(alt::Ref<alt::IWebView> view, const std::string &evName, v8::Local<v8::Function> cb)
	{
		auto it = webViewHandlers.find(view);
		if (it != webViewHandlers.end())
		{
			auto &viewEvents = it->second;
			auto range = viewEvents.equal_range(evName);

			for (auto it = range.first; it != range.second; ++it)
			{
				if (it->second.fn.Get(isolate)->StrictEquals(cb))
					it->second.removed = true;
			}
		}
	}

	void AddGxtText(uint32_t hash, const std::string &text)
	{
		std::unique_lock<std::mutex> lock(gxtAccessMutex);
		if (gxtEntries.count(hash) > 0)
			gxtEntries[hash] = text;
		else
			gxtEntries.insert(std::pair<uint32_t, std::string>(hash, text));
	}

	void RemoveGxtText(uint32_t hash)
	{
		std::unique_lock<std::mutex> lock(gxtAccessMutex);
		if (gxtEntries.count(hash) > 0)
			gxtEntries.erase(hash);
	}

	const std::string &GetGxtText(uint32_t hash)
	{
		static std::string emptyString = "";
		std::unique_lock<std::mutex> lock(gxtAccessMutex);
		if (gxtEntries.count(hash) > 0)
			return gxtEntries[hash];
		return emptyString;
	}

	void AddOwned(alt::Ref<alt::IBaseObject> handle)
	{
		ownedObjects.insert(handle);
	}

	void OnRemoveBaseObject(alt::Ref<alt::IBaseObject> handle)
	{
		ownedObjects.erase(handle);

		if (handle->GetType() == alt::IBaseObject::Type::WEBVIEW)
			webViewHandlers.erase(handle.As<alt::IWebView>());

		V8ResourceImpl::OnRemoveBaseObject(handle);
	}

	bool ToggleCursor(bool state)
	{
		if (cursorsCount > 0 || state)
		{
			cursorsCount += state ? 1 : -1;
			return true;
		}

		return false;
	}

	void ToggleGameControls(bool state)
	{
		gameControlsEnabled = state;
	}

	bool CursorVisible()
	{
		return cursorsCount > 0;
	}

	bool GameControlsActive()
	{
		return gameControlsEnabled;
	}

	v8::Local<v8::Object> GetLocalStorage()
	{
		if (localStorage.IsEmpty())
		{
			std::vector<v8::Local<v8::Value>> args;
			localStorage.Reset(isolate, V8Class::Get("LocalStorage")->New(GetContext(), args).As<v8::Object>());
		}

		return localStorage.Get(isolate);
	}

	bool IsValidModule(const std::string &name);
	std::deque<std::string> GetModuleKeys(const std::string &name);
	std::string GetModulePath(v8::Local<v8::Module> moduleHandle);

	v8::MaybeLocal<v8::Value> Require(const std::string &name);
	v8::MaybeLocal<v8::Module> ResolveFile(const std::string &name, v8::Local<v8::Module> referrer);
	v8::MaybeLocal<v8::Module> ResolveModule(const std::string &name, v8::Local<v8::Module> referrer);

private:
	using WebViewEvents = std::unordered_multimap<std::string, V8::EventCallback>;

	CV8ScriptRuntime *runtime;

	std::unordered_map<std::string, v8::UniquePersistent<v8::Value>> requires;
	std::unordered_map<std::string, v8::UniquePersistent<v8::Module>> modules;

	std::unordered_map<alt::Ref<alt::IWebView>, WebViewEvents> webViewHandlers;
	std::unordered_map<uint32_t, std::string> gxtEntries;

	std::unordered_set<alt::Ref<alt::IBaseObject>> ownedObjects;

	v8::Persistent<v8::Object> localStorage;

	std::mutex gxtAccessMutex;

	uint32_t cursorsCount = 0;
	bool gameControlsEnabled = true;
};
