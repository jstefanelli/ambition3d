#include "ambition/logic/element.h"

namespace ambition::logic {
	crlib::Task Element::Update(float delta) {
		std::vector<crlib::Task> tasks;
		for (auto& t : components) {
			tasks.push_back(t->Update(delta));
		}

		for (auto& c : children) {
			tasks.push_back(c->Update(delta));
		}

		co_await crlib::WhenAll(tasks);
	}

	crlib::Task Element::LateUpdate(float delta) {
		std::vector<crlib::Task> tasks;
		for (auto& t : components) {
			tasks.push_back(t->LateUpdate(delta));
		}

		for (auto& c : children) {
			tasks.push_back(c->LateUpdate(delta));
		}

		co_await crlib::WhenAll(tasks);

		update_transform.FullModel();
	}

	RenderTask_t<bool> Element::Draw() {
		std::vector<RenderTask_t<bool>> tasks;

		for (auto& comp : components) {
			tasks.push_back(comp->Draw());
		}

		for (auto& child : children) {
			tasks.push_back(child->Draw());
		}

		co_await crlib::WhenAll(tasks);

		co_return true;
	}

	crlib::Task Element::CommitFrame() {
		render_transform = update_transform;

		std::vector<crlib::Task> tasks;
		for (auto& c : children) {
			tasks.push_back(c->CommitFrame());
		}

		co_await crlib::WhenAll(tasks);
	}
}