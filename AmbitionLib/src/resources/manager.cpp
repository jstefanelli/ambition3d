#include <ambition/resources/manager.h>

namespace ambition::resources {
	std::shared_ptr<Manager> Manager::_default;

	std::shared_ptr<Manager> Manager::Default() {
		if (_default == nullptr)
			_default = std::make_shared<Manager>();

		return _default;
	}
}