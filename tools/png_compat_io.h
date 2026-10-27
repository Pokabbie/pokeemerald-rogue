/* Keep FILE operations in the executable's CRT when libpng uses another CRT. */
static void PngReadData(png_structp png, png_bytep data, png_size_t length)
{
    FILE *stream = (FILE *)png_get_io_ptr(png);
    if (fread(data, 1, length, stream) != length)
        png_error(png, "Read error");
}

static void PngWriteData(png_structp png, png_bytep data, png_size_t length)
{
    FILE *stream = (FILE *)png_get_io_ptr(png);
    if (fwrite(data, 1, length, stream) != length)
        png_error(png, "Write error");
}

static void PngFlushData(png_structp png)
{
    FILE *stream = (FILE *)png_get_io_ptr(png);
    if (fflush(stream) != 0)
        png_error(png, "Flush error");
}
