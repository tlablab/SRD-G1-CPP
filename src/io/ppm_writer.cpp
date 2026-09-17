#include "io/ppm_writer.hpp"
#include <fstream>

bool writePpm(const Framebuffer& framebuffer, const std::string& filename){
    std::ofstream output(filename.c_str());

    if (!output) {
        return false;
    }

    output << "P3\n";
    output << Framebuffer::kWidth << ' ' << Framebuffer::kHeight << '\n';
    output << "255\n";

    for (int y = 0; y < static_cast<int>(Framebuffer::kHeight); ++y) {
        for (int x = 0; x < static_cast<int>(Framebuffer::kWidth); ++x) {
            Pixel pixel = framebuffer.read(x, y);
            int value = pixel.on ? 255 : 0;

            output << value << ' ' << value << ' ' << value << '\n';
        }
    }
    return true;
}
