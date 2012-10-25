#include <CL/cl.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
using namespace std;

const char* cl_source =
    "__kernel void MatMul(int n, int p, __global float* a, __global float* b, __global float* c)\n\
 {\n\
 float value = 0;\n\
 int i = get_global_id(0);\n\
 int j = get_global_id(1);\n\
 for(int k = 0; k < n; k++)\n\
 {\n\
 value += a[i * n + k] * b[k * p + j];\n\
 }\n\
 c[i * p + j] = value;\n\
 }";

int m, n, p;
float *a_host, *b_host, *c_host;

float my_rand() {
    return (float) (rand() % 10000) / 100.0 - 5.0;
}

void createMat() {
    srand(time(0));
    m = 5;
    n = 5;
    p = 5;
    a_host = (float*)malloc(sizeof(float) * m * n);
    b_host = (float*)malloc(sizeof(float) * n * p);
    c_host = (float*)malloc(sizeof(float) * m * p);

    cout << "A: " << endl;
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            float v = my_rand();
            cout << v << ", ";
            a_host[j * m + i] = v;
        }
        cout << endl;
    }
    cout << endl;

    cout << "B: " << endl;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < p; j++) {
            float v = my_rand();
            cout << v << ", ";
            b_host[j * n + i] = v;
        }
        cout << endl;
    }
    cout << endl;
}

int main(int argc, char **argv) {
    createMat();
    size_t length;
    char* info;
    cl_platform_id platform;
    cl_int error;
    cl_uint n_plat;
    cl_platform_id* platforms;

    error = clGetPlatformIDs(0, 0, &n_plat);
    cout << "Anzahl der Platformen: " << n_plat << endl;

    platforms = (cl_platform_id*) malloc(n_plat * sizeof(cl_platform_id));
    for (int i = 0; i < n_plat; i++) {
        cl_uint n_dev;
        cl_device_id* devices;
        cl_device_id device;
        char* name;

        error = clGetPlatformIDs(n_plat, platforms, 0);
        platform = *(platforms + i);
        cout << endl << "Platform " << i << ":" << endl;

        error = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, 0, &length);
        info = (char*) malloc(length);
        error = clGetPlatformInfo(platform, CL_PLATFORM_NAME, length, info, 0);
        cout << "  Name: " << info << endl;

        error = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, 0, &length);
        info = (char*) malloc(length);
        error = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, length, info, 0);
        cout << "  Hersteller: " << info << endl;

        error = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 0, 0, &length);
        info = (char*) malloc(length);
        error = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, length, info, 0);
        cout << "  Version: " << info << endl;

        error = clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, 0, 0, &length);
        info = (char*) malloc(length);
        error = clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, length, info, 0);
        cout << "  Profil: " << info << endl;

        error = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, 0, 0, &length);
        info = (char*) malloc(length);
        error = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, length, info, 0);
        cout << "  Erweiterungen: " << info << endl;

        error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, 0, &n_dev);
        devices = (cl_device_id*)malloc(n_dev * sizeof(cl_device_id));
        error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, n_dev, devices, 0);
        cout << "  Geräte-Anzahl: " << n_dev << endl;

        for (int j = 0; j < n_dev; j++) {
            size_t length2;
            cl_device_type type;
            cl_bool available;
            cl_ulong gl_memsize;
            cl_ulong l_memsize;
            cl_uint max_clock;
            cl_uint max_units;
            size_t group_size;
            cl_uint max_dim;
            device = *(devices + j);

            cout <<  endl << "  Gerät " << j << ":" << endl;

            error = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, 0, &length2);
            name = (char*)malloc(length2);
            error = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_NAME, length2, name, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_AVAILABLE, sizeof(cl_bool), &available, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gl_memsize, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &l_memsize, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &max_clock, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &max_units, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &group_size, 0);
            error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &max_dim, 0);

            cout << "    Typ: ";
            switch(type) {
            case CL_DEVICE_TYPE_DEFAULT:
                cout << "Default";
                break;
            case CL_DEVICE_TYPE_CPU:
                cout << "CPU";
                break;
            case CL_DEVICE_TYPE_GPU:
                cout << "GPU";
                break;
            case CL_DEVICE_TYPE_ACCELERATOR:
                cout << "Accelerator";
            }
            cout << endl;

            cout << "    Name: " << name << endl;
            cout << "    Verfügbar: " << available << endl;
            cout << "    Globale Speichergröße: " << gl_memsize << " Byte" << endl;
            cout << "    Lokale Speichergröße: " << l_memsize << " Byte" << endl;
            cout << "    max. Taktrate: " << max_clock << " Herz" << endl;
            cout << "    Max. Gruppen-Größe: " << group_size << endl;
            cout << "    Max. Work-Item Dimension: " << max_dim << endl;
            cout << "    Max. Recheneinheiten: " << max_units << endl;


            cl_context context;
            cl_command_queue queue;
            cl_kernel matmul;
            cl_mem a_dev, b_dev, c_dev;

            cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };
            size_t program_size = sizeof(char) * 265;
            context = clCreateContext(cprops, j + 1, devices, 0, 0, &error);

            cl_program program = clCreateProgramWithSource(context, 1, &cl_source, &program_size, &error);
            error = clBuildProgram(program, j + 1, &device, 0, 0, 0);
            // Lese die Länge des Compiler-Logs ein.
            error = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,0, 0, &length);
            // Reserviere Speicher.
            char* log = (char*)malloc(length);
            // Lese das Compiler-Log ein.
            error = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, length, log, 0);
            cout << log << endl;

            a_dev = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * m * n, a_host, 0);
            b_dev = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n * p, b_host, 0);
            c_dev = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * m * p, 0, 0);

            queue = clCreateCommandQueue(context, device, 0, &error);
            matmul = clCreateKernel(program, "MatMul", &error);

            // Setze die Argumente des Kernels.
            error = clSetKernelArg(matmul, 0, sizeof(int), &n);
            error = clSetKernelArg(matmul, 1, sizeof(int), &p);
            error = clSetKernelArg(matmul, 2, sizeof(cl_mem), &a_dev);
            error = clSetKernelArg(matmul, 3, sizeof(cl_mem), &b_dev);
            error = clSetKernelArg(matmul, 4, sizeof(cl_mem), &c_dev);
            size_t gws[] = {m, p};
            size_t lws[] = {1, 1};
            error = clEnqueueNDRangeKernel(queue, matmul, 2, 0, gws, lws, 0, 0, 0);
            error = clEnqueueReadBuffer(queue, c_dev, CL_TRUE, 0, sizeof(float) * m * p, c_host, 0, NULL, NULL);

            cout << endl << "C: " << endl;
            for(int i = 0; i < m; i++) {
                for(int j = 0; j < p; j++) {
                    cout << c_host[i * m + j] << ", ";
                }
                cout << endl;
            }
            cout << endl;


            // Gebe Ressourcen wieder frei.
            error = clReleaseKernel(matmul);
            error = clReleaseCommandQueue(queue);
            error = clReleaseMemObject(a_dev);
            error = clReleaseMemObject(b_dev);
            error = clReleaseMemObject(c_dev);
            error = clReleaseProgram(program);
            error = clReleaseContext(context);

        }

        delete devices;
        delete name;
    }

    delete info;
    delete platforms;

    return 0;
}
