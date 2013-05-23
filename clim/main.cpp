#include <fstream>
#include <iostream>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <CL/cl.hpp>
#include <Magick++.h>

using namespace cl;

char *getCLErrorString(cl_int err) {
    switch (err) {
        case CL_SUCCESS: return (char *) "Success!";
        case CL_DEVICE_NOT_FOUND: return (char *) "Device not found.";
        case CL_DEVICE_NOT_AVAILABLE: return (char *) "Device not available";
        case CL_COMPILER_NOT_AVAILABLE: return (char *) "Compiler not available";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return (char *) "Memory object allocation failure";
        case CL_OUT_OF_RESOURCES: return (char *) "Out of resources";
        case CL_OUT_OF_HOST_MEMORY: return (char *) "Out of host memory";
        case CL_PROFILING_INFO_NOT_AVAILABLE: return (char *) "Profiling information not available";
        case CL_MEM_COPY_OVERLAP: return (char *) "Memory copy overlap";
        case CL_IMAGE_FORMAT_MISMATCH: return (char *) "Image format mismatch";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED: return (char *) "Image format not supported";
        case CL_BUILD_PROGRAM_FAILURE: return (char *) "Program build failure";
        case CL_MAP_FAILURE: return (char *) "Map failure";
        case CL_INVALID_VALUE: return (char *) "Invalid value";
        case CL_INVALID_DEVICE_TYPE: return (char *) "Invalid device type";
        case CL_INVALID_PLATFORM: return (char *) "Invalid platform";
        case CL_INVALID_DEVICE: return (char *) "Invalid device";
        case CL_INVALID_CONTEXT: return (char *) "Invalid context";
        case CL_INVALID_QUEUE_PROPERTIES: return (char *) "Invalid queue properties";
        case CL_INVALID_COMMAND_QUEUE: return (char *) "Invalid command queue";
        case CL_INVALID_HOST_PTR: return (char *) "Invalid host pointer";
        case CL_INVALID_MEM_OBJECT: return (char *) "Invalid memory object";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return (char *) "Invalid image format descriptor";
        case CL_INVALID_IMAGE_SIZE: return (char *) "Invalid image size";
        case CL_INVALID_SAMPLER: return (char *) "Invalid sampler";
        case CL_INVALID_BINARY: return (char *) "Invalid binary";
        case CL_INVALID_BUILD_OPTIONS: return (char *) "Invalid build options";
        case CL_INVALID_PROGRAM: return (char *) "Invalid program";
        case CL_INVALID_PROGRAM_EXECUTABLE: return (char *) "Invalid program executable";
        case CL_INVALID_KERNEL_NAME: return (char *) "Invalid kernel name";
        case CL_INVALID_KERNEL_DEFINITION: return (char *) "Invalid kernel definition";
        case CL_INVALID_KERNEL: return (char *) "Invalid kernel";
        case CL_INVALID_ARG_INDEX: return (char *) "Invalid argument index";
        case CL_INVALID_ARG_VALUE: return (char *) "Invalid argument value";
        case CL_INVALID_ARG_SIZE: return (char *) "Invalid argument size";
        case CL_INVALID_KERNEL_ARGS: return (char *) "Invalid kernel arguments";
        case CL_INVALID_WORK_DIMENSION: return (char *) "Invalid work dimension";
        case CL_INVALID_WORK_GROUP_SIZE: return (char *) "Invalid work group size";
        case CL_INVALID_WORK_ITEM_SIZE: return (char *) "Invalid work item size";
        case CL_INVALID_GLOBAL_OFFSET: return (char *) "Invalid global offset";
        case CL_INVALID_EVENT_WAIT_LIST: return (char *) "Invalid event wait list";
        case CL_INVALID_EVENT: return (char *) "Invalid event";
        case CL_INVALID_OPERATION: return (char *) "Invalid operation";
        case CL_INVALID_GL_OBJECT: return (char *) "Invalid OpenGL object";
        case CL_INVALID_BUFFER_SIZE: return (char *) "Invalid buffer size";
        case CL_INVALID_MIP_LEVEL: return (char *) "Invalid mip-map level";
        default: return (char *) "Unknown";
    }
}

inline void checkErr(cl_int err, const char * name)
{
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << name
        << " (" << err << "): " << getCLErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    cl_int err;
    std::vector<Platform> platforms;
    Platform::get(&platforms);
    Context context;
    std::vector<Device> devices;
    Device device;

    //Nach GPU suchen
    for(int i = 0; i < platforms.size(); i++) {
        Platform pl = platforms.at(i);
        cl_context_properties cps[3] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(platforms[i])(),
            0
        };
        
        context = Context(CL_DEVICE_TYPE_GPU, cps);

        devices = context.getInfo<CL_CONTEXT_DEVICES>();
        if(devices.size() > 0) {
            device = devices[0];
			std::cout << "Max Image 2D Width: " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << std::endl;
			std::cout << "Max Image 2D Height: " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << std::endl;
			std::cout << "Profiling Timer Resolution in ns: " << device.getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION>() << std::endl;
			std::cout << "Vendor: " << pl.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
            break;
        }
    }

    
	CommandQueue queue = CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE);
    
    Magick::Image image1;
    image1.read("Ball.png");
    long image1_size = 4 * image1.rows() * image1.columns();
    int height = image1.rows();
    int width = image1.columns();
    int8_t *image1_pixels = new int8_t[image1_size];
    image1.write(0, 0, image1.columns(), image1.rows(), "RGBA", MagickCore::CharPixel, image1_pixels);
    
    cl::size_t<3> origin;
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);
    cl::size_t<3> region;
    region.push_back(width);
    region.push_back(height);
    region.push_back(1);
    
    cl::ImageFormat inFormat;
    inFormat.image_channel_data_type = CL_SIGNED_INT8;
    inFormat.image_channel_order = CL_RGBA;
    cl::Image2D imageIn(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inFormat, width, height, 0, image1_pixels, &err);
    checkErr(err, "Image2D()");
    
    cl::ImageFormat outFormat;
    outFormat.image_channel_data_type = CL_UNSIGNED_INT8;
    outFormat.image_channel_order = CL_RGBA;
    cl::Image2D imageOut(context, CL_MEM_WRITE_ONLY, outFormat, width, height, 0, NULL, &err);
    checkErr(err, "Image2D()");
    
    std::ifstream kernelFile("image_manipulation.cl");
    std::string imageProg(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
    cl::Program::Sources imageSource(1, std::make_pair(imageProg.c_str(), imageProg.length() + 1));
    cl::Program imageProgram(context, imageSource, &err);
    checkErr(err, "Program()");
    
    err = imageProgram.build(devices);
    checkErr(err, "Program::build()");
        
    cl::Kernel basic(imageProgram, "edge_detection", &err);
    checkErr(err, "Kernel()");
        
    basic.setArg(0, imageIn);
    basic.setArg(1, imageOut);
    
    Event kernel_event;

    queue.enqueueNDRangeKernel(basic, cl::NullRange, cl::NDRange(width, height), cl::NDRange(1, 1), NULL, &kernel_event);    
    kernel_event.wait();
	cl_ulong kernel_event_start, kernel_event_end;
	kernel_event_start = kernel_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	kernel_event_end = kernel_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
	std::cout << "Kernelcommand (Edge Detection) in ns: " << (kernel_event_end - kernel_event_start) << std::endl;

	Event read_event;
    uint8_t *image2_pixels = new uint8_t[image1_size];
    err = queue.enqueueReadImage(imageOut, CL_TRUE, origin, region, 0, 0, image2_pixels, NULL, &read_event);
    checkErr(err, "enqueueReadImage()");
    read_event.wait();
	cl_ulong read_event_start, read_event_end;
	read_event_start = read_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	read_event_end = read_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
	std::cout << "Image Write to new Buffer in ns" << (read_event_end - read_event_start) << std::endl;

    Magick::Image image2;
    image2.read(image1.columns(), image1.rows(), "RGBA", MagickCore::CharPixel, image2_pixels);
    image2.write("Ball_out.png");

    return 0;
}
