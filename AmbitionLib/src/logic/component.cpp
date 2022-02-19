#include "ambition/logic/component.h"
#include "ambition/logic/element.h"

namespace ambition::logic {
	std::weak_ptr<Element> Component::WeakElement() const {
		return element;
	}

	std::shared_ptr<Element> Component::RealElement() const {
		auto e = element.lock();
		if (e == nullptr) {
			throw std::runtime_error("Component could not lock onto owner element");
		}

		return e;
	}

	Component::Component(const std::shared_ptr<Element>& element) : element(element) {

	}
}