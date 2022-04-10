#pragma once
#include "type.h"
#include "resource.h"
#include "manager.h"
#include <string>
#include <memory>

#define AMBITION_RES_ORIGIN_UNKNOWN 0
#define AMBITION_RES_ORIGIN_FILE 1
#define AMBITION_RES_BUILTIN 2

namespace ambition::resources {
	template<Type t>
	class Address {
		std::string path;
		uint32_t origin;
	public:
		Address(std::string path, uint32_t origin) : path(std::move(path)), origin(origin) {

		}

		bool operator==(const Address<t>& other) const {
			return path == other.path && origin == other.origin;
		}

		std::shared_ptr<Resource> Get(std::shared_ptr<Manager> manager = nullptr) const;
		virtual bool QueryLoaded(std::shared_ptr<Manager> manager = nullptr) const {
			if (manager == nullptr)
				manager = Manager::Default();

			return manager->QueryLoaded<t>(*this);
		}
	};
}