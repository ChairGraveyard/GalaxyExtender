#pragma once

#include "soewrappers.h"
#include "Object.h"

#include "UIPage.h"
#include "UiManager.h"
#include "CuiMediator.h"

#define CUIFACTORY_ACTIVATE_ADDRESS 0x008840D0
#define CUIFACTORY_GET_ADDRESS 0x00883FB0

class CuiMediator;

class CuiMediatorFactory {
public:
	class ConstructorBase {
	protected:
		bool duplicateOnly;
	public:
		ConstructorBase(bool duplicateOnlyA) : duplicateOnly(duplicateOnlyA) {}

		virtual CuiMediator* get(bool create = true) = 0;
		virtual CuiMediator* create(UIPage& page) const = 0;
		virtual CuiMediator* createInto(UIPage& page) const = 0;
		virtual const type_info& getTypeId() const = 0;

		virtual ~ConstructorBase() = 0 {};
		virtual void detachMediator() = 0;
	};

	template<typename T>
	class Constructor : public ConstructorBase {
	public:
		explicit Constructor(const soe::string& path, bool duplicate = false) :
			ConstructorBase(duplicate),
			path(path),
			mediator(0) {
		}

		CuiMediator* get(bool create = true) {
			if (!mediator && create) {
				UIPage* const root = UIManager::gUIManager()->GetRootPage();
				auto obj = root->GetObjectFromPath(path.c_str());

				UIPage* const page = (UIPage*) BaseHookedObject::dynamicCast(obj, (PVOID)UIBaseObject::RTTI, (PVOID)UIPage::RTTI);

				mediator = this->create(*page);
				mediator->fetch();
			}

			return mediator;
		}

		CuiMediator* create(UIPage& page) const {
			return BaseHookedObject::create_soe_object<T, UIPage&>(page);
		}

		CuiMediator* createInto(UIPage& page) const;

		~Constructor() {
			detachMediator();
		}

		void detachMediator() {
			if (mediator) 			{
				mediator->release();
			}
			
			mediator = 0;
		}

		const type_info& getTypeId() const {
			return typeid (T);
		}

		Constructor & operator= (const Constructor & rhs) = delete;
		Constructor(const Constructor & rhs) = delete;
		Constructor() = delete;

		soe::string path;
		CuiMediator* mediator;
	};

	static CuiMediator* activate(const char* name, const char* unk = nullptr, bool create = true) {
		return Call<CUIFACTORY_ACTIVATE_ADDRESS, CuiMediator*, const char*, const char*, bool>::run(name, unk, create);
	}

	static CuiMediator* get(const char* name, bool create = true) {
		return Call<CUIFACTORY_GET_ADDRESS, CuiMediator*, const char*, bool>::run(name, create);
	}

	static void toggle(const char* name) {
		Call<0x008841B0, void, const char*>::run(name);
	}

	static void addConstructor(const char* name, ConstructorBase* ctor) {
		Call<0x00884340, void, const char*, ConstructorBase*>::run(name, ctor);
	}

};

template<typename T>
CuiMediator* CuiMediatorFactory::Constructor<T>::createInto(UIPage& page) const {
	UIPage* const dupe = UIPage::DuplicateInto(page, path.c_str());
	return BaseHookedObject::create_soe_object<T, UIPage&>(*dupe);
}
