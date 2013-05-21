__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void copy(__read_only image2d_t imageIn,__write_only image2d_t imageOut)
{
    const int2 pos = (int2)(get_global_id(0), get_global_id(1) );
    uint4 image1_pixel = read_imageui(imageIn, sampler, pos);
    uint4 pixel = (uint4)(image1_pixel.x,
                          image1_pixel.y,
                          image1_pixel.z,
                          image1_pixel.w);
    write_imageui(imageOut, pos, pixel);
}