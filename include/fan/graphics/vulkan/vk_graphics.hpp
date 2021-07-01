#pragma once

#include <fan/graphics/camera.hpp>

#include <fan/graphics/vulkan/vk_core.hpp>

#include <mutex>

namespace fan_2d {

	namespace vk {

		namespace graphics {

			struct rectangle {

				rectangle(fan::camera* camera);
				~rectangle();

				void push_back(const fan::vec2& position, const fan::vec2& size, const fan::color& color, f32_t angle = 0);

				void draw();
				// begin must not be bigger than end, otherwise not drawn
				void draw(uint32_t begin, uint32_t end);

				void write_data();

				void edit_data();

				uint32_t size() const;

				f32_t get_angle(uint32_t i) const;
				void set_angle(uint32_t i, f32_t angle);

				std::mutex m;

			protected:

				struct instance_t {

					fan::vec2 position;
					fan::vec2 size;
					fan::color color;
					f32_t angle;

				};

				std::vector<instance_t> m_instance;

				using staging_buffer_t = fan::vk::gpu_memory::glsl_location_handler<fan::vk::gpu_memory::buffer_type::staging>;

				using instance_buffer_t = fan::vk::gpu_memory::glsl_location_handler<fan::vk::gpu_memory::buffer_type::buffer>;

				staging_buffer_t* staging_buffer;
				instance_buffer_t* instance_buffer;

				fan::camera* m_camera;

				uint32_t m_begin;
				uint32_t m_end;
			};

		}

	}

}