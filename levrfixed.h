#include <stdlib.h>
#include <stdint.h>


/****************************************
LEVR_DECLARATION BLOCK
****************************************/
#define LEVR_WIDE 0

#define LEVR_CLAMP(X, lo, hi) (X<lo ? lo : X>hi ? hi : X)
#define LEVR_MIN(a, b) ((a) > (b) ? (b) : (a))
#define LEVR_MAX(a, b) ((a) < (b) ? (b) : (a))
#define LEVR_ABS(a) ((a) > 0 ? (a) : -(a))

typedef
#if LEVR_WIDE
  int64_t    
#else
  int32_t 
#endif
  LEVR_fixed;

#define LEVR_FP_SCALE 9
#define LEVR_FP_UNIT 512
#define LEVR_SHIFT_MASK ((1 << LEVR_FP_SCALE) - 1) 
#define LEVR_ONE (1 << LEVR_FP_SCALE)

#define LEVR_int2fix(a) ((a) << LEVR_FP_SCALE)
#define LEVR_fix2int(a) ((a) >> LEVR_FP_SCALE)

#define LEVR_float2fix(a) ((LEVR_fixed)(a * (float)LEVR_FP_UNIT))
#define LEVR_fix2float(a) ((float)(a) / (float)LEVR_FP_UNIT)

#define LEVR_sfpmul(a, b) (LEVR_fixed)(((int64_t)a * (int64_t)b) >> LEVR_FP_SCALE)
#define LEVR_fpmul(a, b) (LEVR_fixed)(((a) * (b)) / LEVR_ONE)
#define LEVR_fpdiv(a, b) (b == 0 ? 0 : (LEVR_fixed)(((a) * LEVR_ONE) / (b)))

#define LEVR_floor(a) LEVR_int2fix(LEVR_fix2int((a)))

#define LEVR_PI (LEVR_fpdiv(355 * LEVR_ONE, 113 * LEVR_ONE))
//define LEVR_fpdsin(a) LEVR_fpdiv((LEVR_fpmul(LEVR_fpmul(4, a), (LEVR_int2fix(180) - a))), (LEVR_int2fix(40500) - LEVR_fpmul(a, (LEVR_int2fix(180) - a))))
//define LEVR_fpsin(a) LEVR_fpdiv(LEVR_fpmul(LEVR_fpmul(16 * LEVR_ONE, (a)), LEVR_PI - (a)), LEVR_fpmul(5 * LEVR_ONE, LEVR_fpmul(LEVR_PI, LEVR_PI)) - LEVR_fpmul(LEVR_fpmul(4 * LEVR_ONE, (a)), LEVR_PI - (a)))
//define LEVR_fpcos(a) LEVR_fpsin(LEVR_fpdiv(LEVR_PI, 2 * LEVR_ONE) - (a))
#define LEVR_fpsin(a) LEVR_sin(a)
#define LEVR_fpcos(a) LEVR_cos(a)
typedef struct _LEVR_Screen      LEVR_Screen;
typedef struct _LEVR_vec3        LEVR_vec3;
typedef struct _LEVR_vec2        LEVR_vec2;
typedef struct _LEVR_vec3i       LEVR_vec3i;
typedef struct _LEVR_Pixel       LEVR_Pixel;
typedef struct _LEVR_Ray         LEVR_Ray;
typedef struct _LEVR_Camera      LEVR_Camera;
typedef struct _LEVR_Material    LEVR_Material;
typedef struct _LEVR_MaterialSet LEVR_MaterialSet;
typedef struct _LEVR_Grid        LEVR_Grid;
//typedef struct _LEVR_Grid16      LEVR_Grid16;
//typedef struct _LEVR_Grid32      LEVR_Grid32;
//typedef struct _LEVR_Grid64      LEVR_Grid64;
typedef struct _LEVR_State       LEVR_State;

void       LEVR_screen_flush(LEVR_Screen* screen, uint32_t color);
void       LEVR_screen_resize(LEVR_Screen* screen, uint32_t width, uint32_t height);
uint32_t   LEVR_rgb_to_u32(uint8_t r, uint8_t g, uint8_t b);
LEVR_vec3  LEVR_vec3_add(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_sub(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_scale(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_scalefp(LEVR_vec3 a, LEVR_fixed b);
LEVR_vec3  LEVR_vec3_divfp(LEVR_vec3 a, LEVR_fixed b);
LEVR_vec3  LEVR_vec3_normalize(LEVR_vec3 a);
LEVR_fixed LEVR_vec3_dot(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_cross(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_rotate(LEVR_vec3 a, LEVR_vec3 b);

LEVR_vec3 LEVR_intersect(LEVR_Ray ray, LEVR_Grid grid);
/****************************************
LEVR_DECLARATION BLOCK END
****************************************/


/****************************************
LEVR_SYS BLOCK
****************************************/

struct _LEVR_Screen{
	uint32_t* pixels;
	uint32_t width;
	uint32_t height;
};

void LEVR_screen_flush(LEVR_Screen* screen, uint32_t color){
	memset(screen->pixels, color, sizeof( screen->pixels )*screen->width*screen->height);
}

void LEVR_screen_resize(LEVR_Screen* screen, uint32_t width, uint32_t height){
	screen->width = width;
	screen->height = height;
	screen->pixels = (uint32_t*)malloc(sizeof(uint32_t*)*screen->width*screen->height);
}

/****************************************
LEVR_SYS BLOCK END
****************************************/

/****************************************
LEVR_DATA BLOCK
****************************************/

uint32_t LEVR_rgb_to_u32(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t alpha = 255;
  return (alpha << 24) + (b << 16) + (g << 8) + r;
}

/****************************************
LEVR_DATA BLOCK END
****************************************/

/****************************************
LEVR_MATH BLOCK
****************************************/

struct _LEVR_vec3 {
	LEVR_fixed x, y, z;
};

struct _LEVR_vec2 {
	LEVR_fixed x, y;
};

struct _LEVR_vec3i {
	int32_t x, y, z;
};

LEVR_fixed LEVR_sqrt(LEVR_fixed x)
{
  int32_t previousError = -1;
  
  for (int test = 0; test <= x; ++test)
  {
    int32_t error = x - (test * test) / LEVR_FP_UNIT;

    if (error == 0)
      return test;
    else if (error < 0)
      error *= -1;

    if (previousError > 0 && error > previousError)
      return test - 1;

    previousError = error;
  }
  return 0;
}

LEVR_fixed LEVR_wrap(LEVR_fixed a, LEVR_fixed b){
	return a >= 0 ? (a % b) : (b + (a % b) - 1);
}
#define LEVR_SIN_TABLE_LENGTH 128

#define LEVR_SIN_TABLE_UNIT_STEP\
  (LEVR_FP_UNIT / (LEVR_SIN_TABLE_LENGTH * 4))

static const LEVR_fixed LEVR_sinTable[LEVR_SIN_TABLE_LENGTH] =
{
  (0*LEVR_FP_UNIT)/511, (6*LEVR_FP_UNIT)/511, 
  (12*LEVR_FP_UNIT)/511, (18*LEVR_FP_UNIT)/511, 
  (25*LEVR_FP_UNIT)/511, (31*LEVR_FP_UNIT)/511, 
  (37*LEVR_FP_UNIT)/511, (43*LEVR_FP_UNIT)/511, 
  (50*LEVR_FP_UNIT)/511, (56*LEVR_FP_UNIT)/511, 
  (62*LEVR_FP_UNIT)/511, (68*LEVR_FP_UNIT)/511, 
  (74*LEVR_FP_UNIT)/511, (81*LEVR_FP_UNIT)/511, 
  (87*LEVR_FP_UNIT)/511, (93*LEVR_FP_UNIT)/511, 
  (99*LEVR_FP_UNIT)/511, (105*LEVR_FP_UNIT)/511, 
  (111*LEVR_FP_UNIT)/511, (118*LEVR_FP_UNIT)/511, 
  (124*LEVR_FP_UNIT)/511, (130*LEVR_FP_UNIT)/511, 
  (136*LEVR_FP_UNIT)/511, (142*LEVR_FP_UNIT)/511, 
  (148*LEVR_FP_UNIT)/511, (154*LEVR_FP_UNIT)/511, 
  (160*LEVR_FP_UNIT)/511, (166*LEVR_FP_UNIT)/511, 
  (172*LEVR_FP_UNIT)/511, (178*LEVR_FP_UNIT)/511, 
  (183*LEVR_FP_UNIT)/511, (189*LEVR_FP_UNIT)/511, 
  (195*LEVR_FP_UNIT)/511, (201*LEVR_FP_UNIT)/511, 
  (207*LEVR_FP_UNIT)/511, (212*LEVR_FP_UNIT)/511, 
  (218*LEVR_FP_UNIT)/511, (224*LEVR_FP_UNIT)/511, 
  (229*LEVR_FP_UNIT)/511, (235*LEVR_FP_UNIT)/511, 
  (240*LEVR_FP_UNIT)/511, (246*LEVR_FP_UNIT)/511, 
  (251*LEVR_FP_UNIT)/511, (257*LEVR_FP_UNIT)/511, 
  (262*LEVR_FP_UNIT)/511, (268*LEVR_FP_UNIT)/511, 
  (273*LEVR_FP_UNIT)/511, (278*LEVR_FP_UNIT)/511, 
  (283*LEVR_FP_UNIT)/511, (289*LEVR_FP_UNIT)/511, 
  (294*LEVR_FP_UNIT)/511, (299*LEVR_FP_UNIT)/511, 
  (304*LEVR_FP_UNIT)/511, (309*LEVR_FP_UNIT)/511, 
  (314*LEVR_FP_UNIT)/511, (319*LEVR_FP_UNIT)/511, 
  (324*LEVR_FP_UNIT)/511, (328*LEVR_FP_UNIT)/511, 
  (333*LEVR_FP_UNIT)/511, (338*LEVR_FP_UNIT)/511, 
  (343*LEVR_FP_UNIT)/511, (347*LEVR_FP_UNIT)/511, 
  (352*LEVR_FP_UNIT)/511, (356*LEVR_FP_UNIT)/511, 
  (361*LEVR_FP_UNIT)/511, (365*LEVR_FP_UNIT)/511, 
  (370*LEVR_FP_UNIT)/511, (374*LEVR_FP_UNIT)/511, 
  (378*LEVR_FP_UNIT)/511, (382*LEVR_FP_UNIT)/511, 
  (386*LEVR_FP_UNIT)/511, (391*LEVR_FP_UNIT)/511, 
  (395*LEVR_FP_UNIT)/511, (398*LEVR_FP_UNIT)/511, 
  (402*LEVR_FP_UNIT)/511, (406*LEVR_FP_UNIT)/511, 
  (410*LEVR_FP_UNIT)/511, (414*LEVR_FP_UNIT)/511, 
  (417*LEVR_FP_UNIT)/511, (421*LEVR_FP_UNIT)/511, 
  (424*LEVR_FP_UNIT)/511, (428*LEVR_FP_UNIT)/511, 
  (431*LEVR_FP_UNIT)/511, (435*LEVR_FP_UNIT)/511, 
  (438*LEVR_FP_UNIT)/511, (441*LEVR_FP_UNIT)/511, 
  (444*LEVR_FP_UNIT)/511, (447*LEVR_FP_UNIT)/511, 
  (450*LEVR_FP_UNIT)/511, (453*LEVR_FP_UNIT)/511, 
  (456*LEVR_FP_UNIT)/511, (459*LEVR_FP_UNIT)/511, 
  (461*LEVR_FP_UNIT)/511, (464*LEVR_FP_UNIT)/511, 
  (467*LEVR_FP_UNIT)/511, (469*LEVR_FP_UNIT)/511, 
  (472*LEVR_FP_UNIT)/511, (474*LEVR_FP_UNIT)/511, 
  (476*LEVR_FP_UNIT)/511, (478*LEVR_FP_UNIT)/511, 
  (481*LEVR_FP_UNIT)/511, (483*LEVR_FP_UNIT)/511, 
  (485*LEVR_FP_UNIT)/511, (487*LEVR_FP_UNIT)/511, 
  (488*LEVR_FP_UNIT)/511, (490*LEVR_FP_UNIT)/511, 
  (492*LEVR_FP_UNIT)/511, (494*LEVR_FP_UNIT)/511, 
  (495*LEVR_FP_UNIT)/511, (497*LEVR_FP_UNIT)/511, 
  (498*LEVR_FP_UNIT)/511, (499*LEVR_FP_UNIT)/511, 
  (501*LEVR_FP_UNIT)/511, (502*LEVR_FP_UNIT)/511, 
  (503*LEVR_FP_UNIT)/511, (504*LEVR_FP_UNIT)/511, 
  (505*LEVR_FP_UNIT)/511, (506*LEVR_FP_UNIT)/511, 
  (507*LEVR_FP_UNIT)/511, (507*LEVR_FP_UNIT)/511, 
  (508*LEVR_FP_UNIT)/511, (509*LEVR_FP_UNIT)/511, 
  (509*LEVR_FP_UNIT)/511, (510*LEVR_FP_UNIT)/511, 
  (510*LEVR_FP_UNIT)/511, (510*LEVR_FP_UNIT)/511, 
  (510*LEVR_FP_UNIT)/511, (510*LEVR_FP_UNIT)/511
};


LEVR_fixed LEVR_sin(LEVR_fixed a){
  a = LEVR_wrap(a / LEVR_SIN_TABLE_UNIT_STEP,LEVR_SIN_TABLE_LENGTH * 4);
  int8_t positive = 1;

  if (a < LEVR_SIN_TABLE_LENGTH)
  {
  }
  else if (a < LEVR_SIN_TABLE_LENGTH * 2)
  {
    a = LEVR_SIN_TABLE_LENGTH * 2 - a - 1;
  }
  else if (a < LEVR_SIN_TABLE_LENGTH * 3)
  {
    a = a - LEVR_SIN_TABLE_LENGTH * 2;
    positive = 0;
  }
  else
  {
    a = LEVR_SIN_TABLE_LENGTH - (a - LEVR_SIN_TABLE_LENGTH * 3) - 1;
    positive = 0;
  }

  return positive ? LEVR_sinTable[a] : -1 * LEVR_sinTable[a];

}

LEVR_fixed LEVR_cos(LEVR_fixed a)
{
  return LEVR_sin(a + LEVR_FP_UNIT / 4);
}

LEVR_vec3 LEVR_vec3_add(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_vec3 r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

LEVR_vec3 LEVR_vec3_sub(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_vec3 r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
}

LEVR_vec3 LEVR_vec3_scale(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_vec3 r;
	r.x = LEVR_fpmul(a.x, b.x);
	r.y = LEVR_fpmul(a.x, b.x);
	r.z = LEVR_fpmul(a.x, b.x);
	return r;
}

LEVR_vec3 LEVR_vec3_scalefp(LEVR_vec3 a, LEVR_fixed b){
	LEVR_vec3 r;
	r.x = LEVR_fpmul(a.x, b);
	r.y = LEVR_fpmul(a.y, b);
	r.z = LEVR_fpmul(a.z, b);
	return r;
}

LEVR_vec3  LEVR_vec3_divfp(LEVR_vec3 a, LEVR_fixed b){
	LEVR_vec3 r;
	r.x = LEVR_fpdiv(a.x, b);
	r.y = LEVR_fpdiv(a.y, b);
	r.z = LEVR_fpdiv(a.z, b);
	return r;
}


LEVR_vec3 LEVR_vec3_normalize(LEVR_vec3 a){
	LEVR_fixed norm = LEVR_sqrt(LEVR_fpmul(a.x, a.x) + LEVR_fpmul(a.y, a.y) + LEVR_fpmul(a.z, a.z));
	if(norm == 0) return a;
	return LEVR_vec3_divfp(a, norm);
}
LEVR_fixed LEVR_vec3_dot(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_fixed r = LEVR_fpmul(a.x, b.x) + LEVR_fpmul(a.y, b.y) + LEVR_fpmul(a.z, b.z);
	return r;
}

LEVR_vec3 LEVR_vec3_cross(LEVR_vec3 a, LEVR_vec3 b)
{
	LEVR_vec3 r;
	r.x = LEVR_fpmul(a.y, b.z) - LEVR_fpmul(a.z, b.y);
	r.y = LEVR_fpmul(a.z, b.x) - LEVR_fpmul(a.x, b.z);
	r.z = LEVR_fpmul(a.x, b.y) - LEVR_fpmul(a.y, b.x);
	return r;
}

LEVR_vec3 LEVR_vec3_rotate(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_vec3 r;
	LEVR_fixed pitch = LEVR_fpmul(b.x, LEVR_fpdiv(LEVR_PI, LEVR_int2fix(180)));
	LEVR_fixed yaw = LEVR_fpmul(b.y, LEVR_fpdiv(LEVR_PI, LEVR_int2fix(180)));
	LEVR_fixed roll = LEVR_fpmul(b.z, LEVR_fpdiv(LEVR_PI, LEVR_int2fix(180)));
	
	
	LEVR_fixed cosa = LEVR_fpcos(roll);
	LEVR_fixed sina = LEVR_fpsin(roll);

	LEVR_fixed cosb = LEVR_fpcos(yaw);
	LEVR_fixed sinb = LEVR_fpsin(yaw);

	LEVR_fixed cosc = LEVR_fpcos(pitch);
	LEVR_fixed sinc = LEVR_fpsin(pitch);

	LEVR_fixed Axx = LEVR_fpmul(cosa, cosb);
	LEVR_fixed Axy = LEVR_fpmul(LEVR_fpmul(cosa, sinb), sinc) - LEVR_fpmul(sina, cosc);
	LEVR_fixed Axz = LEVR_fpmul(LEVR_fpmul(cosa, sinb), cosc) + LEVR_fpmul(sina, sinc);

	LEVR_fixed Ayx = LEVR_fpmul(sina, cosb);
	LEVR_fixed Ayy = LEVR_fpmul(LEVR_fpmul(sina, sinb), sinc) + LEVR_fpmul(cosa, cosc);
	LEVR_fixed Ayz = LEVR_fpmul(LEVR_fpmul(sina, sinb), cosc) - LEVR_fpmul(cosa, sinc);

	LEVR_fixed Azx = -sinb;
	LEVR_fixed Azy = LEVR_fpmul(cosb, sinc);
	LEVR_fixed Azz = LEVR_fpmul(cosb, cosc);

	r.x = LEVR_fpmul(Axx, a.x) + LEVR_fpmul(Axy, a.y) + LEVR_fpmul(Axz, a.z);
	r.y = LEVR_fpmul(Ayx, a.x) + LEVR_fpmul(Ayy, a.y) + LEVR_fpmul(Ayz, a.z);
	r.z = LEVR_fpmul(Azx, a.x) + LEVR_fpmul(Azy, a.y) + LEVR_fpmul(Azz, a.z);

	return r;

}

/****************************************
LEVR_MATH BLOCK END
****************************************/

/****************************************
LEVR_RENDER BLOCK
****************************************/

struct _LEVR_Pixel{
	uint32_t x;
	uint32_t y;
	uint32_t color;
};

struct _LEVR_Ray{
	LEVR_vec3 pos;
	LEVR_vec3 dir;
};

struct _LEVR_Camera{
	uint32_t width;
	uint32_t height;
	LEVR_vec3 pos;
	LEVR_vec3 dir;
	LEVR_vec3 up;
	LEVR_fixed fov;
};

struct _LEVR_Material{
	uint32_t color;
	LEVR_fixed emittance;
	LEVR_fixed reflectance;
	LEVR_fixed diffuse;
	LEVR_fixed specular;
	LEVR_fixed shininess;
};

struct _LEVR_MaterialSet{
	LEVR_Material* materials;
	uint8_t max_materials;
};

struct _LEVR_Grid{
	uint8_t* voxels;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

struct _LEVR_State{
	uint32_t flags;
	void (*set_pixel)(LEVR_State*, LEVR_Screen*, LEVR_Pixel*);
};


uint8_t LEVR_cast(LEVR_Ray ray, LEVR_Grid grid){
	int i = 0;
	int index = 0;
	uint8_t voxel = 0;
	LEVR_vec3 coord = {LEVR_floor(ray.pos.x), LEVR_floor(ray.pos.y), LEVR_floor(ray.pos.z)};
	/*
	printf("COORD X %d.%04d\n",coord.x / LEVR_FP_UNIT,
    ((coord.x % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("COORD Y %d.%04d\n",coord.y / LEVR_FP_UNIT,
    ((coord.y % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("COORD Z %d.%04d\n",coord.z / LEVR_FP_UNIT,
    ((coord.z % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("DIR X %d.%04d\n",ray.dir.x / LEVR_FP_UNIT,
    ((ray.dir.x % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("DIR Y %d.%04d\n",ray.dir.y / LEVR_FP_UNIT,
    ((ray.dir.y % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("DIR Z %d.%04d\n",ray.dir.z / LEVR_FP_UNIT,
    ((ray.dir.z % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	*/

	LEVR_vec3 delta = {LEVR_fpdiv(LEVR_ONE, LEVR_ABS(ray.dir.x)), LEVR_fpdiv(LEVR_ONE, LEVR_ABS(ray.dir.y)), LEVR_fpdiv(LEVR_ONE, LEVR_ABS(ray.dir.z))};
	/*
	printf("DELTA X %d.%04d\n",delta.x / LEVR_FP_UNIT,
    ((delta.x % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("DELTA Y %d.%04d\n",delta.y / LEVR_FP_UNIT,
    ((delta.y % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("DELTA Z %d.%04d\n",delta.z / LEVR_FP_UNIT,
    ((delta.z % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	*/
	LEVR_vec3 step = {LEVR_fpmul(ray.dir.x, delta.x), LEVR_fpmul(ray.dir.y, delta.y), LEVR_fpmul(ray.dir.z, delta.z)};
	/*
	printf("STEP X %d.%04d\n",step.x / LEVR_FP_UNIT,
    ((step.x % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("STEP Y %d.%04d\n",step.y / LEVR_FP_UNIT,
    ((step.y % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	printf("STEP Z %d.%04d\n",step.z / LEVR_FP_UNIT,
    ((step.z % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	*/
	LEVR_vec3 tmax = {0, 0, 0};
	
	if(ray.dir.x < 0){
		tmax.x = LEVR_fpmul(ray.pos.x - (coord.x), delta.x); 
	} else {
		tmax.x = LEVR_fpmul(LEVR_ONE - (ray.pos.x - (coord.x)), delta.x); 
	}
	if(ray.dir.y < 0){
		tmax.y = LEVR_fpmul(ray.pos.y - (coord.y), delta.y); 
	} else {
		tmax.y = LEVR_fpmul(LEVR_ONE - (ray.pos.y - (coord.y)), delta.y); 
	}
	if(ray.dir.z < 0){
		tmax.z = LEVR_fpmul(ray.pos.z - (coord.z), delta.z); 
	} else {
		tmax.z = LEVR_fpmul(LEVR_ONE - (ray.pos.z - (coord.z)), delta.z);		
	}
	
	while ( i < 32 && voxel == 0 ){
		if(tmax.x < tmax.y){
			if(tmax.x < tmax.z){
				coord.x += step.x;
				tmax.x += delta.x;
			} else {
				coord.z += step.z;
				tmax.z += delta.z;
			}
		} else {
			if(tmax.y < tmax.z){
				coord.y += step.y;
				tmax.y += delta.y;
			} else {
				coord.z += step.z;
				tmax.z += delta.z;
			}
		}
		
		index = LEVR_fix2int(coord.x) + LEVR_fix2int(coord.y) * grid.width + LEVR_fix2int(coord.z) * grid.width * grid.height;
		//printf("POS %i, %i, %i\n", LEVR_fix2int(coord.x), LEVR_fix2int(coord.y), LEVR_fix2int(coord.z));
		//printf("I %i\n", i);
		if(index >= grid.width * grid.height * grid.depth || index <= 0){
			//printf("RETURNED\n");
			return 100;
		} else {
			voxel = grid.voxels[index];
			//printf("VOXEL %i\n", voxel);
		}
		i++;
	}
	if(i < 32 && voxel != 0){
		return voxel;
	} else {
		return 100;
	}
	
}

void LEVR_render(LEVR_State state, LEVR_Camera camera, LEVR_Screen* screen, LEVR_Grid grid, LEVR_MaterialSet ms){
	LEVR_fixed theta = LEVR_fpmul(LEVR_fpdiv((camera.fov * LEVR_ONE), 2 * LEVR_ONE), LEVR_fpdiv(LEVR_PI, 180 * LEVR_ONE));
	//printf("THETA %d.%04d\n",theta / LEVR_FP_UNIT,
    //((theta % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	//printf("THETA\n");
	LEVR_fixed tan = LEVR_fpdiv(LEVR_fpsin(theta), LEVR_fpcos(theta));
	//printf("sin %d.%04d\n",LEVR_fpsin(theta) / LEVR_FP_UNIT,
    //((LEVR_fpsin(theta) % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	//printf("cos %d.%04d\n",LEVR_fpcos(theta) / LEVR_FP_UNIT,
    //((LEVR_fpsin(theta) % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT); 
	LEVR_fixed fov = LEVR_fpdiv(screen->width * LEVR_ONE, LEVR_fpmul(tan, 2 * LEVR_ONE));
	//LEVR_fixed fov = LEVR_float2fix(68.55F);
	printf("FOV %d.%04d\n",fov / LEVR_FP_UNIT,
    ((fov % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
	LEVR_fixed aspectratio = LEVR_fpdiv(screen->width * LEVR_ONE, screen->height * LEVR_ONE);
	//printf("ASPECT\n");
	for(int i = 0; i < screen->width; i++){
		for(int j = 0; j < screen->height; j++){
				LEVR_Pixel pixel = {0};
				pixel.x = i;
				pixel.y = j;
				LEVR_vec3 proj = {0};				
				LEVR_fixed px = LEVR_fpmul(LEVR_fpmul((LEVR_fpmul(LEVR_fpdiv((i * LEVR_ONE) + LEVR_fpdiv(LEVR_ONE, 2 * LEVR_ONE), screen->width * LEVR_ONE), 2 * LEVR_ONE) - LEVR_ONE), fov), aspectratio);
				LEVR_fixed py = LEVR_fpmul((LEVR_ONE - LEVR_fpmul(LEVR_fpdiv((j * LEVR_ONE) + LEVR_fpdiv(LEVR_ONE, 2 * LEVR_ONE), screen->height * LEVR_ONE), 2 * LEVR_ONE)), fov);
				LEVR_vec3 dir = LEVR_vec3_rotate((LEVR_vec3){px, py, camera.fov * LEVR_ONE}, camera.dir);
				LEVR_Ray ray = {camera.pos, dir};
				//printf("PRECAST %i, %i\n", i, j);
				/*if(i == 40 && j == 20){
					printf("ASPECT %d.%04d\n",aspectratio / LEVR_FP_UNIT,
					((aspectratio % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
					printf("FOV %d.%04d\n",fov / LEVR_FP_UNIT,
					((fov % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
					
					printf("PX %d.%04d\n",px / LEVR_FP_UNIT,
					((px % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
					printf("PY %d.%04d\n",py / LEVR_FP_UNIT,
					((py % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
														
					
					printf("RAY X %d.%04d\n",dir.x / LEVR_FP_UNIT,
					((dir.x % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
					printf("RAY Y %d.%04d\n",dir.y / LEVR_FP_UNIT,
					((dir.y % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);
					printf("RAY Z %d.%04d\n",dir.z / LEVR_FP_UNIT,
					((dir.z % LEVR_FP_UNIT) * 10000) / LEVR_FP_UNIT);

				}
				*/
				uint8_t mat = LEVR_cast(ray, grid);
				if(mat == 100){
					pixel.color = 0xBBBBBB;
				} else {
					pixel.color = ms.materials[mat].color;
				}
				state.set_pixel(&state, screen, &pixel);
		}
	}
}

/****************************************
LEVR_RENDER BLOCK END
****************************************/

/****************************************
LEVR_LOGIC BLOCK
****************************************/



/*
typedef struct Grid{
	uint8_t materialSize;
	ivec3 size;
	uint8_t* voxels;
	Material* materials;
} Grid;

void create_grid(Grid* grid, ivec3 size){
	int i, j, k, totalSize;
	grid->size = size;
	totalSize = grid->size.x * grid->size.y * grid->size.z;
	grid->voxels = (uint8_t*)malloc(totalSize * sizeof(uint8_t));
	for(i = 0; i < grid->size.x; i++){
		for(j = 0; j < grid->size.y; j++){
			for(k = 0; k < grid->size.z; k++){
				//printf("%i, %i, %i\n", i, j, k);
				grid->voxels[i + j * grid->size.x + k*grid->size.x*grid->size.y] = 0;
			}
		}
	}
	grid->materialSize = 1;
	grid->materials = malloc(sizeof(Material));
	grid->materials[0].color = (bvec3){200, 0, 0}; 
	grid->materials[0].emittance = 0;
	grid->materials[0].reflectance = 0;
	grid->materials[0].kd = 0;
	grid->materials[0].ks = 0;
	grid->materials[0].shininess = 0;
	printf("\nmat %i\n", grid->materialSize);
	printf("Grid %i, %i, %i\n", grid->size.x, grid->size.y, grid->size.z);
}*/

/*
void add_material_for_grid(Grid* grid, Material material){
	grid->materialSize++;
	Material* tmp = realloc(grid->materials, grid->materialSize * sizeof(Material));
	if(tmp){
		grid->materials = tmp;
		grid->materials[grid->materialSize - 1] = material;
	}
}
*/

/****************************************
LEVR_LOGIC BLOCK END
****************************************/