#pragma once
#include <memory>
#include <crlib/cc_task.h>
#include "../renderTask.h"

namespace ambition {
	namespace logic {
		class Element;

		class Component {
		protected:
			std::weak_ptr<Element> element;

            explicit Component(const std::shared_ptr<Element>& element);
            Component(const Component& other) = default;
		public:
			std::weak_ptr<Element> WeakElement() const;
			std::shared_ptr<Element> RealElement() const;
			virtual crlib::Task Update(float delta) = 0;
			virtual crlib::Task LateUpdate(float delta) = 0;
			virtual RenderTask_t<bool> Draw() = 0;
		};
	}
}