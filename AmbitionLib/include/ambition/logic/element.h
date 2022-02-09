#pragma once
#include "./component.h"
#include "../transform.h"
#include <crlib/cc_task.h>
#include <vector>

namespace ambition {
	namespace logic {
		class Element {
		protected:
			std::vector<std::shared_ptr<Component>> components;
			std::vector<std::shared_ptr<Element>> children;
			Transform update_transform;
			Transform render_transform;
		public:
			crlib::Task Update(float delta);
			crlib::Task LateUpdate(float delta);
			ambition::RenderTask_t<bool> Draw();
			crlib::Task CommitFrame();

			Transform& UpdateTransform();
			Transform& RenderTransform();
		};
	}
}