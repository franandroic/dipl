#include "BufferObject.hpp"

BufferObject::BufferObject(Device *inDevice, VkDeviceSize inSize, VkBufferUsageFlags inUsage, VkMemoryPropertyFlags inProperties) {

	device = inDevice;
	size = inSize;
	usage = inUsage;
	properties = inProperties;
}

void BufferObject::createBuffer(VkPhysicalDevice physicalDevice) {

	//Generic function used to create a buffer.
	//Includes allocating and binding memory to it and storing the handles.

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	if (vkCreateBuffer(device->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = DeviceUtils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	//TODO: Right now we're calling vkAllocateMemory for every buffer creation, but look into
	//creating a custom allocator that calls it once for multiple buffers
	if (vkAllocateMemory(device->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device->device, buffer, bufferMemory, 0);
}

void BufferObject::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool) {

	//To copy one buffer to another we need to record a single-use command buffer
	//that calls the copy command.

	VkCommandBuffer commandBuffer = CommandUtils::beginSingleTimeCommands(device->device, commandPool);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	CommandUtils::endSingleTimeCommands(commandBuffer, device->device, commandPool, device->graphicsQueue);
}