#include <efi.h>
#include <efilib.h>

// Donut properties
#define R_INNER 1.0f
#define R_OUTER 2.0f
#define X_ROTATION_SPEED 0.02f
#define Z_ROTATION_SPEED 0.007f
#define DISTANCE 5.0f

// Donut sampling density
#define DTHETA 0.07f;
#define DPHI 0.02f;

// Image dimensions
#define WIDTH 80
#define HEIGHT 30

// Auto camera scaling
#define CAMERA_DEPTH ((float)HEIGHT * DISTANCE * 3.0f / (8.0f * (R_INNER + R_OUTER)))

// Rendering symbols
#define INTENSITY u".,-~:;=!*#$@"
#define INTENSITY_RANGE 11

// Maths constants
#define ROOT_TWO 1.41421356f
#define PI 3.141592654f
#define HALF_PI PI / 2.0f
#define TWO_PI PI * 2.0f

float mod(float x, float y) {
  return (x) - (int)((x) / (y)) * (y);
}

float cos(float x) {
  x = mod(x, TWO_PI);
  int sign = 1;
  if (x > PI)
  {
    x -= PI;
    sign = -1;
  }
  float xx = x * x;

  return sign * (
    1 - ((xx) / (2))
    + ((xx * xx) / (24))
    - ((xx * xx * xx) / (720))
    + ((xx * xx * xx * xx) / (40320))
    - ((xx * xx * xx * xx * xx) / (3628800))
    + ((xx * xx * xx * xx * xx * xx) / (479001600))
  );
}

float sin(float x) {
  return cos(x-HALF_PI);
}

typedef struct {
  float x;
  float y;
  float z;
} point;

void project(point *point) {
  float z_inverse = 1.0 / point->z;
  point->x = point->x * CAMERA_DEPTH * z_inverse;
  point->y = point->y * CAMERA_DEPTH * z_inverse;
  point->z = z_inverse;
}

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

  float x_rotation = 0.0f;
  float z_rotation = 0.0f;

  int running = 1;
  while (running) {
    int IMAGE[HEIGHT*WIDTH];
    float ZBUFFER[HEIGHT*WIDTH];
    for (int i=0; i<WIDTH*HEIGHT; i++) {
        IMAGE[i] = 0;
        ZBUFFER[i] = 0.0f;
    }

    x_rotation += X_ROTATION_SPEED;
    z_rotation += Z_ROTATION_SPEED;
    float cosx = cos(x_rotation);
    float sinx = sin(x_rotation);
    float cosz = cos(z_rotation);
    float sinz = sin(z_rotation);

    float theta = 0.0f;
    while (theta < 2.0f*PI) {
      float costheta = cos(theta);
      float sintheta = sin(theta);

      float phi = 0.0f;
      while (phi < 2.0*PI) {
        float cosphi = cos(phi);
        float sinphi = sin(phi);

        // Pre-rotation circle coordinates
        float circlex = R_OUTER + R_INNER * costheta;
        float circley = R_INNER * sintheta;

        // Position of the donut point
        point point = {
          circlex * (cosz * cosphi + sinx * sinz * sinphi) - circley * cosx * sinz,
          circlex * (cosphi * sinz - cosz * sinx * sinphi) + circley * cosx * cosz,
          DISTANCE + cosx * circlex * sinphi + circley * sinx,
        };

        // Apply projection to the point
        project(&point);

        // Luminance given a light from (0, 1, -1).
        float luminance = (
          cosphi * costheta * sinz
          - cosx * costheta * sinphi
          - sinx * sintheta
          + cosz * (cosx * sintheta - costheta * sinx * sinphi)
        ) / ROOT_TWO;

        // Map the point to the screen and record the luminance as an intensity index
        int image_index_x = (int)(WIDTH / 2.0f + point.x);
        int image_index_y = (int)(HEIGHT / 2.0f - point.y);
        int index = image_index_x + WIDTH * image_index_y;
        if (image_index_x < WIDTH && image_index_x > 0 && image_index_y < HEIGHT && image_index_y > 0) {
          if (point.z > ZBUFFER[index]) {
            ZBUFFER[index] = point.z;
            luminance = luminance > 0 ? luminance : -1.0f * luminance;
            IMAGE[index] = (int)(luminance * (float)INTENSITY_RANGE);
          }
        }

        phi += DPHI;
      }
      theta += DTHETA;
    }

    // Render the image
    uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 3, SystemTable->ConOut, 0, 0);
    for (int i=0; i<HEIGHT; i++) {
      for (int j=0; j<WIDTH; j++) {
        if (ZBUFFER[j + WIDTH * i] > 0) {
          Print(u"%c", INTENSITY[IMAGE[j + WIDTH * i]]);
        }
        else {
          Print(u" ");
        }
      }
      Print(u"\n\r");
    }
  
    Print(u"Exit: q");
    EFI_INPUT_KEY efi_input_key;
    uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &efi_input_key);
    if (efi_input_key.UnicodeChar == 113){
      Exit(EFI_SUCCESS, 0, NULL);
    }
  }


  return EFI_SUCCESS;
}
