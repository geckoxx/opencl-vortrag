__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void edge_detection(__read_only image2d_t imageIn,__write_only image2d_t imageOut)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int width = get_global_size(0);
    const int height = get_global_size(1);
    
    const int4 srcColor = read_imagei(imageIn, sampler, (int2)(x, y));
    
    uint4 destColor;
    uint4 tmpColor;
    
    int2 pixelCoords[8];
    int4 pixels[8];
    
    pixelCoords[0].xy = (int2){x - 1, y - 1};
    pixelCoords[1].xy = (int2){x , y - 1};
    pixelCoords[2].xy = (int2){x + 1, y - 1};
    pixelCoords[3].xy = (int2){x - 1, y};
    pixelCoords[4].xy = (int2){x + 1, y};
    pixelCoords[5].xy = (int2){x - 1, y + 1};
    pixelCoords[6].xy = (int2){x , y + 1};
    pixelCoords[7].xy = (int2){x + 1, y + 1};
    
    pixels[0] = read_imagei(imageIn, sampler, pixelCoords[0]);
    pixels[1] = read_imagei(imageIn, sampler, pixelCoords[1]);
    pixels[2] = read_imagei(imageIn, sampler, pixelCoords[2]);
    pixels[3] = read_imagei(imageIn, sampler, pixelCoords[3]);
    pixels[4] = read_imagei(imageIn, sampler, pixelCoords[4]);
    pixels[5] = read_imagei(imageIn, sampler, pixelCoords[5]);
    pixels[6] = read_imagei(imageIn, sampler, pixelCoords[6]);
    pixels[7] = read_imagei(imageIn, sampler, pixelCoords[7]);
    
    destColor = abs(pixels[2] - pixels[5]);
    
    tmpColor = abs(pixels[7] - pixels[0]);
    destColor = max(tmpColor, destColor);
    
    tmpColor = abs(pixels[1] - pixels[6]);
    destColor = max(tmpColor, destColor);
    
    tmpColor = abs(pixels[4] - pixels[3]);
    destColor = max(tmpColor, destColor);
    
    uint4 finalColor = (uint4)(destColor.x, destColor.y, destColor.z, srcColor.w);
    
    write_imageui(imageOut, (int2)(x, y), finalColor);
}