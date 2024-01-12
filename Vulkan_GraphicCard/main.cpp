#include <vulkan.h>
#include <fstream>

int main()
{

	VkResult resultat = VK_SUCCESS;
	VkInstanceCreateInfo* insinfo = new VkInstanceCreateInfo{};
	VkApplicationInfo* appinfo = new VkApplicationInfo{};
	VkInstance* instance = new VkInstance{};

	appinfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo->pApplicationName = "Micro vulkan app";
	appinfo->applicationVersion = 1;
	appinfo->apiVersion = VK_MAKE_VERSION(1, 0, 0);

	insinfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	insinfo->pApplicationInfo = appinfo;

	resultat = vkCreateInstance
	(
		insinfo,
		nullptr,
		instance
	);


	uint32_t physicalDeviceCount;
	VkPhysicalDevice physicalDevice;

	resultat = vkEnumeratePhysicalDevices
	(
		*instance,
		&physicalDeviceCount,
		&physicalDevice
	);



	VkPhysicalDeviceProperties pDeviceProperties;
	if (resultat == VK_SUCCESS) {
		vkGetPhysicalDeviceProperties(
			physicalDevice,
			&pDeviceProperties
		);
	}

	VkPhysicalDeviceFeatures pDeviceFeatures;
	if (resultat == VK_SUCCESS) {
		vkGetPhysicalDeviceFeatures(
			physicalDevice,
			&pDeviceFeatures
		);
	}

	std::fstream fstr;
	fstr.open("Active physical devices.txt", std::ios::out);
	if (resultat == VK_SUCCESS) {
		fstr << "Device count: " << physicalDeviceCount << std::endl;
		fstr << "Device name: \"" << pDeviceProperties.deviceName << "\"" << std::endl;
	}
	else fstr << "Vulkan devices initialization failed or incomplete." << std::endl;
	fstr.close();





	return 0;
}