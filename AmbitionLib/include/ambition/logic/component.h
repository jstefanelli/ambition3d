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
		public:
			virtual crlib::Task Update(float delta);
			virtual crlib::Task LateUpdate(float delta);
			virtual RenderTask_t<bool> Draw();
		};
	}
}