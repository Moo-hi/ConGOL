#pragma once

#include <fan/graphics/camera.hpp>

#include <fan/graphics/vulkan/vk_core.hpp>

#include <mutex>

namespace fan_2d {

	namespace vk {

		namespace graphics {

			struct rectangle_corners_t {

				fan::vec2 top_left;
				fan::vec2 top_right;
				fan::vec2 bottom_left;
				fan::vec2 bottom_right;

				const fan::vec2 operator[](const uint_t i) const {
					return !i ? top_left : i == 1 ? top_right : i == 2 ? bottom_left : bottom_right;
				}

				fan::vec2& operator[](const uint_t i) {
					return !i ? top_left : i == 1 ? top_right : i == 2 ? bottom_left : bottom_right;
				}

			};

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

				void insert(uint32_t i, const fan::vec2& position, const fan::vec2& size, const fan::color& color, f32_t angle = 0);

				void reserve(uint32_t size);
				void resize(uint32_t size, const fan::color& color);

				void draw(uint32_t begin = fan::uninitialized, uint32_t end = fan::uninitialized) const;

				void erase(uint32_t i);
				void erase(uint32_t begin, uint32_t end);

				// erases everything
				void clear();

				rectangle_corners_t get_corners(uint32_t i = 0) const;

				f32_t get_angle(uint32_t i) const;
				void set_angle(uint32_t i, f32_t angle);

				const fan::color get_color(uint32_t i = 0) const;
				void set_color(uint32_t i, const fan::color& color);

				fan::vec2 get_position(uint32_t i = 0) const;
				void set_position(uint32_t i, const fan::vec2& position);

				fan::vec2 get_size(uint32_t i = 0) const;
				void set_size(uint32_t i, const fan::vec2& size);

			//	void release_queue(bool position, bool size, bool angle, bool color, bool indices);

				bool inside(uint_t i, const fan::vec2& position = fan::math::inf) const;

				fan::camera* m_camera = nullptr;

			protected:

				struct instance_t {

					fan::vec2 position;
					fan::vec2 size;
					fan::color color;
					f32_t angle;

				};

				using instance_buffer_t = fan::vk::gpu_memory::buffer_object<instance_t, fan::vk::gpu_memory::buffer_type::buffer>;

				instance_buffer_t* instance_buffer = nullptr;

				uint32_t m_begin;
				uint32_t m_end;
			};

		}

	}

}