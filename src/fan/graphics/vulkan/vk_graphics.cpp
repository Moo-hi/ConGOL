#include <fan/graphics/vulkan/vk_graphics.hpp>

#if fan_renderer == fan_renderer_vulkan

fan_2d::vk::graphics::rectangle::rectangle(fan::camera* camera)
	: m_camera(camera), m_begin(0), m_end(0)
{

	VkVertexInputBindingDescription binding_description;

	binding_description.binding = 0;
	binding_description.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
	binding_description.stride = sizeof(instance_t);

	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;

	attribute_descriptions.resize(4);

	attribute_descriptions[0].binding = 0;
	attribute_descriptions[0].location = 0;
	attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[0].offset = offsetof(instance_t, position);

	attribute_descriptions[1].binding = 0;
	attribute_descriptions[1].location = 1;
	attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[1].offset = offsetof(instance_t, size);

	attribute_descriptions[2].binding = 0;
	attribute_descriptions[2].location = 2;
	attribute_descriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attribute_descriptions[2].offset = offsetof(instance_t, color);

	attribute_descriptions[3].binding = 0;
	attribute_descriptions[3].location = 3;
	attribute_descriptions[3].format = VK_FORMAT_R32_SFLOAT;
	attribute_descriptions[3].offset = offsetof(instance_t, angle);

	fan::vulkan* vk_instance = camera->m_window->m_vulkan;

	vk_instance->pipelines.push_back(
		binding_description,
		attribute_descriptions,
		camera->m_window->get_size(),
		"glsl/2D/test_vertex.vert",
		"glsl/2D/test_fragment.frag"
	);

	instance_buffer = new instance_buffer_t(vk_instance->device, vk_instance->physicalDevice, 1);

	vk_instance->push_back_draw_call([&](uint32_t i, uint32_t j) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_camera->m_window->m_vulkan->commandBuffers[0][i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_camera->m_window->m_vulkan->renderPass;
		renderPassInfo.framebuffer = m_camera->m_window->m_vulkan->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_camera->m_window->m_vulkan->swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_camera->m_window->m_vulkan->commandBuffers[0][i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(m_camera->m_window->m_vulkan->commandBuffers[0][i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_camera->m_window->m_vulkan->pipelines.pipeline_info[j].pipeline);

		VkBuffer vertexBuffers[] = { instance_buffer->m_buffer_object };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_camera->m_window->m_vulkan->commandBuffers[0][i], 0, 1, vertexBuffers, offsets);

		//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		//vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(m_camera->m_window->m_vulkan->commandBuffers[0][i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_camera->m_window->m_vulkan->pipelines.pipeline_layout, 0, 1, &m_camera->m_window->m_vulkan->descriptorSets[i], 0, nullptr);

		vkCmdDraw(m_camera->m_window->m_vulkan->commandBuffers[0][i], 6, m_end, 0, m_begin);

		vkCmdEndRenderPass(m_camera->m_window->m_vulkan->commandBuffers[0][i]);

		if (vkEndCommandBuffer(m_camera->m_window->m_vulkan->commandBuffers[0][i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	});

}

fan_2d::vk::graphics::rectangle::~rectangle()
{
	delete instance_buffer;
}

void fan_2d::vk::graphics::rectangle::push_back(const fan::vec2& position, const fan::vec2& size, const fan::color& color, f32_t angle) {

	m_instance.emplace_back(instance_t{ position, size, color, angle });

	if (!fan::vk::gpu_memory::gpu_queue) {
		this->write_data();
	}

}

void fan_2d::vk::graphics::rectangle::draw()
{

	uint32_t begin = 0;
	uint32_t end = this->size();

	bool reload = false;

	if (begin != m_begin) {
		reload = true;
		m_begin = begin;
	}

	if (end != m_end) {
		reload = true;
		m_end = end;
	}

	if (reload) {

		vkDeviceWaitIdle(m_camera->m_window->m_vulkan->device);

		m_camera->m_window->m_vulkan->erase_command_buffers();
		m_camera->m_window->m_vulkan->createCommandBuffers();

	}

}

void fan_2d::vk::graphics::rectangle::draw(uint32_t begin, uint32_t end)
{
	if (begin >= end) {
		return;
	}

	bool reload = false;

	if (begin != m_begin) {
		reload = true;
		m_begin = begin;
	}

	if (end != m_end) {
		reload = true;
		m_end = end;
	}

	if (reload) {

		vkDeviceWaitIdle(m_camera->m_window->m_vulkan->device);

		m_camera->m_window->m_vulkan->erase_command_buffers();
		m_camera->m_window->m_vulkan->createCommandBuffers();

	}

}

void fan_2d::vk::graphics::rectangle::write_data()
{

	vkDeviceWaitIdle(m_camera->m_window->m_vulkan->device);

	fan::vulkan* vk_instance = m_camera->m_window->m_vulkan;

	VkDeviceSize bufferSize = sizeof(instance_t) * m_instance.size();

	staging_buffer = new staging_buffer_t(vk_instance->device, vk_instance->physicalDevice, bufferSize);

	instance_buffer->free_buffer();
	instance_buffer->allocate_buffer(bufferSize);

	void* data;
	vkMapMemory(
		vk_instance->device,
		staging_buffer->m_device_memory,
		0,
		bufferSize,
		0,
		&data
	);

	memcpy(data, m_instance.data(), (size_t)bufferSize);
	vkUnmapMemory(vk_instance->device, staging_buffer->m_device_memory);

	fan::vk::gpu_memory::copy_buffer(vk_instance->device, vk_instance->commandPool, vk_instance->graphicsQueue, staging_buffer->m_buffer_object, instance_buffer->m_buffer_object, bufferSize);


	m_camera->m_window->m_vulkan->erase_command_buffers();

	m_camera->m_window->m_vulkan->createCommandBuffers();

	delete staging_buffer;
}

void fan_2d::vk::graphics::rectangle::edit_data()
{

	static VkSubmitInfo submit_info{};

	static VkCommandBufferBeginInfo begin_info{};

	if (!m_camera->m_window->m_vulkan->commandBuffers[1][0]) {
		

		static VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = m_camera->m_window->m_vulkan->commandPool;
		alloc_info.commandBufferCount = 1;

		vkAllocateCommandBuffers(m_camera->m_window->m_vulkan->device, &alloc_info, &m_camera->m_window->m_vulkan->commandBuffers[1][0]);

		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

		

	}

	vkBeginCommandBuffer(m_camera->m_window->m_vulkan->commandBuffers[1][0], &begin_info);

	vkCmdUpdateBuffer(m_camera->m_window->m_vulkan->commandBuffers[1][0], instance_buffer->m_buffer_object, 0, sizeof(instance_t) * this->size(), m_instance.data());

	vkEndCommandBuffer(m_camera->m_window->m_vulkan->commandBuffers[1][0]);

	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_camera->m_window->m_vulkan->commandBuffers[1][0];

	vkQueueSubmit(m_camera->m_window->m_vulkan->graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);

//	fan::print(command_buffer);

	vkQueueWaitIdle(m_camera->m_window->m_vulkan->graphicsQueue);

	//delete staging_buffer;
}

uint32_t fan_2d::vk::graphics::rectangle::size() const {

	return m_instance.size();
}

f32_t fan_2d::vk::graphics::rectangle::get_angle(uint32_t i) const
{
	return m_instance[i].angle;
}

void fan_2d::vk::graphics::rectangle::set_angle(uint32_t i, f32_t angle)
{
	m_instance[i].angle = angle;

	if (!fan::vk::gpu_memory::gpu_queue) {

		fan::vulkan* vk_instance = m_camera->m_window->m_vulkan;

		VkDeviceSize bufferSize = sizeof(m_instance[0]);

		staging_buffer = new staging_buffer_t(vk_instance->device, vk_instance->physicalDevice, bufferSize);

		void* data;
		vkMapMemory(
			vk_instance->device,
			staging_buffer->m_device_memory,
			0,
			bufferSize,
			0,
			&data
		);
		
		memcpy(data, &m_instance[i], (std::size_t)bufferSize);
		vkUnmapMemory(vk_instance->device, staging_buffer->m_device_memory);

		fan::vk::gpu_memory::copy_buffer(vk_instance->device, vk_instance->commandPool, vk_instance->graphicsQueue, staging_buffer->m_buffer_object, instance_buffer->m_buffer_object, bufferSize, 0, sizeof(instance_t) * i);

		m_camera->m_window->m_vulkan->erase_command_buffers();

		m_camera->m_window->m_vulkan->createCommandBuffers();

		delete staging_buffer;
	}

}

#endif