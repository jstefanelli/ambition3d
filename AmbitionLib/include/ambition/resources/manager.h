#pragma once
#include "type.h"
#include "resource.h"
#include "mesh.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace ambition::resources {
	template<Type t>
	class Address;

	class Manager {
	protected:
		static std::shared_ptr<Manager> _default;
		std::unordered_map<std::string, std::weak_ptr<Mesh>> meshes;
		std::unordered_map<std::string, std::weak_ptr<Texture>> textures;

	public:
		template<Type T>
		std::shared_ptr<Resource> Get(const Address<T>&);

		template<Type T>
		bool QueryLoaded(const Address<T>&) const;


		static std::shared_ptr<Manager> Default();
	};
}