#include <ruby.h>
#include <jpeglib.h>

struct steg_image {
  size_t width;
  size_t height;
  size_t buffer_size;
  unsigned char* buffer;
};

static void
steg_image_mark(void* ptr)
{
}

static void
steg_image_free(void* ptr)
{
  struct steg_image* image = ptr;

  if (image != NULL) {
  }
}

static size_t
steg_image_memsize(void const* ptr)
{
  struct steg_image* image = ptr;

  return sizeof(struct steg_image) + image->buffer_size;
}

static const rb_data_type_t steg_image_data_type = {
    "Steganography::Image",
    {
	steg_image_mark,
	steg_image_free,
	steg_image_memsize,
    },
    NULL, NULL, RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE
steg_image_s_alloc(VALUE klass)
{
  struct steg_image* image;
  VALUE obj = TypedData_Make_Struct(klass, struct steg_image, &steg_image_data_type, image);
  MEMZERO(image, 1, sizeof(struct steg_image));
  return obj;
}

static VALUE
steg_image_read_jpeg(VALUE self, VALUE filename)
{
  FILE* file;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  assert(TYPE(filename) == T_STRING);

  file = fopen(RSTRING_PTR(filename), "rb");
  if (file == NULL) {
    rb_sys_fail_path(filename);
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  /* ... */

  jpeg_start_decompress(&cinfo);

  /* ... */

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&cinfo);
}

static VALUE
steg_image_read_file(VALUE self, VALUE filename)
{
  SafeStringValueCStr(filename);

  if (strnstr(".jpg", RSTRING_PTR(filename) + RSTRING_LEN(filename) - 4) == 0) {
    return steg_image_read_jpeg(self, RSTRING_PTR(filename);
  }

  rb_raise(rb_eArgError, "unsupported image format: %s", filename);
  UNREACHABLE;
}

static VALUE
steg_image_initialize(int argc, VALUE** argv, VALUE self)
{
  switch (argc) {
    case 1:
      return steg_image_read_file(self, argv[0]);

    case 2:
      return steg_image_init_empty(self, argv[0], argv[1]);
  }

  rb_raise(rb_eArgError, "wrong number of argument");
  UNREACHABLE;
}

static VALUE
steg_image_s_read(VALUE klass, VALUE filename)
{
  VALUE obj = steg_image_s_alloc(klass);
  rb_obj_call_init(obj, 1, &filename);
  return obj;
}

void
Init_steganography(void)
{
  VALUE mSteganography;

  mSteganography = rb_define_module("Steganography");

  cImage = rb_define_class_under(mSteganography, "Image", rb_cObject);
  rb_define_singleton_method(cImage, "read", steg_image_s_read, 1);
  rb_define_method(cImage, "initialize", steg_image_initialize, -1);
}
