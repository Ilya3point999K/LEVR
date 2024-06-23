/****************************************
LEVR_CONFIGURATION BLOCK
****************************************/
#include <stdlib.h>
#include <stdint.h>

#define LEVR_USE_OMP 0
#define LEVR_CHECKERBOARD_RENDERING 0

#if LEVR_USE_OMP
#include <omp.h>
#endif

#define LEVR_REAL double

#define LEVR_CLAMP(X, lo, hi) (X<lo ? lo : X>hi ? hi : X)
#define LEVR_MIN(a, b)        ((a) > (b) ? (b) : (a))
#define LEVR_MAX(a, b)        ((a) < (b) ? (b) : (a))
#define LEVR_ABS(a)           ((a) > 0 ? (a) : -(a))

#define LEVR_mul(a, b)        (a * b)
#define LEVR_div(a, b)        (a / b)

#define LEVR_floor(a)         floor(a)
#define LEVR_sqrt(a)          sqrt(a)
#define LEVR_pow(a, b)        pow(a, b)

#define LEVR_PI               3.1415926
#define LEVR_sin(a)           sin(a)
#define LEVR_cos(a)           cos(a)
#define LEVR_tan(a)           tan(a)

typedef struct _LEVR_Screen      LEVR_Screen;
typedef struct _LEVR_vec3        LEVR_vec3;
typedef struct _LEVR_vec3i       LEVR_vec3i;
typedef struct _LEVR_Pixel       LEVR_Pixel;
typedef struct _LEVR_Ray         LEVR_Ray;
typedef struct _LEVR_Camera      LEVR_Camera;
typedef struct _LEVR_Material    LEVR_Material;
typedef struct _LEVR_MaterialSet LEVR_MaterialSet;
typedef struct _LEVR_Grid        LEVR_Grid;
typedef struct _LEVR_Scene		 LEVR_Scene;
typedef struct _LEVR_Hit		 LEVR_Hit;
typedef struct _LEVR_State       LEVR_State;

void       LEVR_screen_flush(LEVR_Screen* screen, uint32_t color);
void       LEVR_screen_resize(LEVR_Screen* screen, uint32_t width, uint32_t height);
uint32_t   LEVR_rgb_to_u32(uint8_t r, uint8_t g, uint8_t b);
LEVR_vec3  LEVR_vec3_add(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_sub(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_scale(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_scalef(LEVR_vec3 a, LEVR_REAL b);
LEVR_vec3  LEVR_vec3_divf(LEVR_vec3 a, LEVR_REAL b);
LEVR_vec3  LEVR_vec3_normalize(LEVR_vec3 a);
LEVR_REAL  LEVR_vec3_dot(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_cross(LEVR_vec3 a, LEVR_vec3 b);
LEVR_vec3  LEVR_vec3_rotate(LEVR_vec3 a, LEVR_vec3 b);

LEVR_Hit LEVR_intersect(LEVR_Ray ray, LEVR_Grid grid, uint32_t renderdist);
/****************************************
LEVR_CONFIGURATION BLOCK END
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
	LEVR_REAL x, y, z;
};

struct _LEVR_vec3i {
	int32_t x, y, z;
};

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
	r.x = LEVR_mul(a.x, b.x);
	r.y = LEVR_mul(a.y, b.y);
	r.z = LEVR_mul(a.z, b.z);
	return r;
}

LEVR_vec3 LEVR_vec3_scalef(LEVR_vec3 a, LEVR_REAL b){
	LEVR_vec3 r;
	r.x = LEVR_mul(a.x, b);
	r.y = LEVR_mul(a.y, b);
	r.z = LEVR_mul(a.z, b);
	return r;
}

LEVR_vec3 LEVR_vec3_divf(LEVR_vec3 a, LEVR_REAL b){
	LEVR_vec3 r;
	if(b == 0) return a;
	r.x = LEVR_div(a.x, b);
	r.y = LEVR_div(a.y, b);
	r.z = LEVR_div(a.z, b);
	return r;
}

LEVR_REAL LEVR_vec3_length(LEVR_vec3 a)
{
	LEVR_REAL l = LEVR_sqrt(LEVR_pow(a.x, 2) + LEVR_pow(a.y, 2) + LEVR_pow(a.z, 2));
	return l;
}

LEVR_vec3 LEVR_vec3_normalize(LEVR_vec3 a)
{
	return LEVR_vec3_divf(a, LEVR_vec3_length(a));
}

LEVR_REAL LEVR_vec3_dot(LEVR_vec3 a, LEVR_vec3 b)
{
	LEVR_REAL r = LEVR_mul(a.x, b.x) + LEVR_mul(a.y, b.y) + LEVR_mul(a.z, b.z);
	return r;
}

LEVR_vec3 LEVR_vec3_reflect(LEVR_vec3 a, LEVR_vec3 b)
{
	LEVR_vec3 r = LEVR_vec3_sub(a, LEVR_vec3_scalef(b, LEVR_mul(2, LEVR_vec3_dot(a, b))));
	return r;
}


LEVR_vec3 LEVR_vec3_rotate(LEVR_vec3 a, LEVR_vec3 b){
	LEVR_vec3 r;
    LEVR_REAL pitch = LEVR_div(LEVR_mul(b.x, LEVR_PI), 180);
    LEVR_REAL yaw   = LEVR_div(LEVR_mul(b.y, LEVR_PI), 180);
    LEVR_REAL roll  = LEVR_div(LEVR_mul(b.z, LEVR_PI), 180);
    
    LEVR_REAL cosa = LEVR_cos(roll);
    LEVR_REAL sina = LEVR_sin(roll);

    LEVR_REAL cosb = LEVR_cos(yaw);
    LEVR_REAL sinb = LEVR_sin(yaw);

    LEVR_REAL cosc = LEVR_cos(pitch);
    LEVR_REAL sinc = LEVR_sin(pitch);

    LEVR_REAL Axx = LEVR_mul(cosa, cosb);
    LEVR_REAL Axy = LEVR_mul(LEVR_mul(cosa, sinb), sinc) - LEVR_mul(sina, cosc);
    LEVR_REAL Axz = LEVR_mul(LEVR_mul(cosa, sinb), cosc) + LEVR_mul(sina, sinc);

    LEVR_REAL Ayx = LEVR_mul(sina, cosb);
    LEVR_REAL Ayy = LEVR_mul(LEVR_mul(sina, sinb), sinc) + LEVR_mul(cosa, cosc);
    LEVR_REAL Ayz = LEVR_mul(LEVR_mul(sina, sinb), cosc) - LEVR_mul(cosa, sinc);

    LEVR_REAL Azx = -sinb;
    LEVR_REAL Azy = LEVR_mul(cosb, sinc);
    LEVR_REAL Azz = LEVR_mul(cosb, cosc);

    r.x = LEVR_mul(Axx, a.x) + LEVR_mul(Axy, a.y) + LEVR_mul(Axz, a.z);
    r.y = LEVR_mul(Ayx, a.x) + LEVR_mul(Ayy, a.y) + LEVR_mul(Ayz, a.z);
    r.z = LEVR_mul(Azx, a.x) + LEVR_mul(Azy, a.y) + LEVR_mul(Azz, a.z);

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
	uint32_t fov;
};

struct _LEVR_Material{
	uint32_t color;
	LEVR_REAL ambient;
	LEVR_REAL emittance;
	LEVR_REAL diffuse;
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

struct _LEVR_Scene{
	LEVR_Grid grid;
	LEVR_vec3i* lights;
	uint32_t max_lights;
};

struct _LEVR_Hit{
	uint8_t material;
	uint32_t dist;
	LEVR_vec3i pos;
	LEVR_vec3 normal;
};

struct _LEVR_State{
	uint32_t flags;
	void (*set_pixel)(LEVR_State*, LEVR_Pixel*);
};

LEVR_Hit LEVR_intersect(LEVR_Ray ray, LEVR_Grid grid, uint32_t renderdist){
	int i = 0;
	int index = 0;
	uint8_t voxel = 0;
	int side = 0;
	LEVR_vec3 coord = {LEVR_floor(ray.pos.x), LEVR_floor(ray.pos.y), LEVR_floor(ray.pos.z)};
	int x, y, z;
	LEVR_vec3 delta = {1 / LEVR_ABS(ray.dir.x), 1 / LEVR_ABS(ray.dir.y), 1 / LEVR_ABS(ray.dir.z)};
	LEVR_vec3 step = {LEVR_mul(ray.dir.x, delta.x), LEVR_mul(ray.dir.y, delta.y), LEVR_mul(ray.dir.z, delta.z)};
	LEVR_vec3 tmax = {0, 0, 0};
	LEVR_vec3 normal = {0, 0, 0};
	LEVR_Hit hit;
	hit.pos = (LEVR_vec3i){0, 0, 0};
	hit.normal = normal;
	hit.material = 0;
	hit.dist = 0;
	
	if(ray.dir.x < 0){
		tmax.x = LEVR_mul((ray.pos.x - (coord.x)), delta.x); 
	} else {
		tmax.x = LEVR_mul((1.0F - (ray.pos.x - (coord.x))), delta.x); 
	}
	if(ray.dir.y < 0){
		tmax.y = LEVR_mul((ray.pos.y - (coord.y)), delta.y); 
	} else {
		tmax.y = LEVR_mul((1.0F - (ray.pos.y - (coord.y))), delta.y); 
	}
	if(ray.dir.z < 0){
		tmax.z = LEVR_mul((ray.pos.z - (coord.z)), delta.z); 
	} else {
		tmax.z = LEVR_mul((1.0F - (ray.pos.z - (coord.z))), delta.z);		
	}
	
	
	while ( i < renderdist && voxel == 0 ){
		if (tmax.x < tmax.y && tmax.x < tmax.z) {
			coord.x += step.x;
			tmax.x += delta.x;
			side = 0;
		} else if (tmax.y < tmax.z) {
			coord.y += step.y;
			tmax.y += delta.y;
			side = 1;
		} else {
			coord.z += step.z;
			tmax.z += delta.z;
			side = 2;
		}
		x = ceil(coord.x);
		y = ceil(coord.y);
		z = ceil(coord.z);

		if(x >= grid.width || x < 0){
			return hit;
		}
		if(y >= grid.height || y < 0){
			return hit;
		}
		if(z >= grid.depth || z < 0){
			return hit;
		}

		index = x + y * grid.height + z * grid.width * grid.height;
		if(index >= grid.width * grid.height * grid.depth || index < 0){
			return hit;
		} else {
			voxel = grid.voxels[index];
		}
		i++;
	}
	if(i < renderdist && voxel != 0){
		if(side == 0){
			if(ray.dir.x < 0){
				normal.x = 1;
			} else {
				normal.x = -1;
			}
			normal.y = 0;
			normal.z = 0;
		}
		else if (side == 1){
			if(ray.dir.y < 0){
				normal.y = 1;
			} else {
				normal.y = -1;
			}
			normal.x = 0;
			normal.z = 0;
		}
		else{
			if(ray.dir.z < 0){
				normal.z = 1;
			} else {
				normal.z = -1;
			}
			normal.x = 0;
			normal.y = 0;
		}
		hit.pos.x = x;
		hit.pos.y = y;
		hit.pos.z = z;
		hit.material = voxel;
		hit.normal = normal;
		hit.dist = i;
		return hit;
	} else {
		return hit;
	}
}

uint32_t LEVR_cast(LEVR_Ray ray, LEVR_Scene scene, LEVR_MaterialSet ms){
	LEVR_Hit hit = LEVR_intersect(ray, scene.grid, 512);
	LEVR_vec3 light_pos = {0};
	LEVR_vec3 hit_pos = {0};
	LEVR_vec3 res = {0};
	int i;
	if (hit.material == 0) return 0xBBBF00;
	if (ms.materials[hit.material].emittance > 0) return ms.materials[hit.material].color;
	LEVR_REAL ambient = 0.5;
	uint8_t red = (ms.materials[hit.material].color >> 16) & 0xff;
	uint8_t green = (ms.materials[hit.material].color >> 8) & 0xff;
	uint8_t blue = ms.materials[hit.material].color & 0xff;
	LEVR_vec3 color = {0};
	color.x = blue;
	color.y = green;
	color.z = red;

	res = LEVR_vec3_add(res, LEVR_vec3_scalef(color, ambient));
	for (i = 0; i < scene.max_lights; i++){

	light_pos.x = scene.lights[i].x + 0.5;
	light_pos.y = scene.lights[i].y + 0.5;
	light_pos.z = scene.lights[i].z + 0.5;
	hit_pos.x = hit.pos.x + hit.normal.x;
	hit_pos.y = hit.pos.y + hit.normal.y;
	hit_pos.z = hit.pos.z + hit.normal.z;

	LEVR_vec3 light_dir = LEVR_vec3_normalize(LEVR_vec3_sub(light_pos, hit_pos));
	LEVR_Ray light_ray = {hit_pos, light_dir};
	LEVR_Hit light_hit = LEVR_intersect(light_ray, scene.grid, 512);
	LEVR_REAL emit = ms.materials[light_hit.material].emittance;
	LEVR_vec3 light_color;
	if(emit > 0){
		uint8_t lred = (ms.materials[light_hit.material].color >> 16) & 0xff; // red
		uint8_t lgreen = (ms.materials[light_hit.material].color >> 8) & 0xff; // green
		uint8_t lblue = ms.materials[light_hit.material].color & 0xff; // blue
		light_color = (LEVR_vec3){lred / 255, lgreen / 255, lblue / 255};
	} else {
		light_color	= (LEVR_vec3){emit, emit, emit};
	}
	LEVR_REAL diffuse = LEVR_MAX(LEVR_vec3_dot(LEVR_vec3_normalize(hit.normal), light_dir), 0.0);
	res = LEVR_vec3_add(res, LEVR_vec3_scalef(LEVR_vec3_scale(color, light_color), diffuse));
	//res = LEVR_vec3_scalefp(res, 1.5);
	}
	return LEVR_rgb_to_u32(LEVR_CLAMP(res.x, 0, 0xFFFFFF), LEVR_CLAMP(res.y, 0, 0xFFFFFF), LEVR_CLAMP(res.z, 0, 0xFFFFFF));
}

void LEVR_render(LEVR_State* state, LEVR_Camera camera, LEVR_Scene scene, LEVR_MaterialSet ms){
	#if LEVR_CHECKERBOARD_RENDERING
	char flag = (state->flags & (1 << 0));
	#endif
	LEVR_REAL arg = LEVR_mul(LEVR_div(camera.fov, 2),  LEVR_div(LEVR_PI, 180));
	LEVR_REAL tang = LEVR_tan(arg);
    LEVR_REAL FOV = LEVR_div(camera.width, LEVR_mul(tang, 2));
	LEVR_REAL aspectratio = LEVR_div(camera.width, camera.height);
	LEVR_REAL px, py;
	LEVR_Pixel pixel = {0};
	LEVR_vec3 dir = {0};
	LEVR_Ray ray = {0};

#ifdef LEVR_USE_OMP
	#pragma omp parallel for private(px, py, pixel, ray, dir)
#endif
	for(int i = 0; i < camera.width; i++){
		#if LEVR_CHECKERBOARD_RENDERING
			flag = !flag;
		#endif
		for(int j = 0; j < camera.height; j++){
			
				#if LEVR_CHECKERBOARD_RENDERING
					flag = !flag;
					if (flag) continue;
				#endif
				
				pixel.x = i;
				pixel.y = j;
				px = LEVR_mul(LEVR_mul((LEVR_mul(2, (LEVR_div((i + 0.5), camera.width))) - 1), FOV), aspectratio);
				py = LEVR_mul((1 - LEVR_mul(2, (LEVR_div((j + 0.5), camera.height)))), FOV);
				dir = LEVR_vec3_rotate((LEVR_vec3){px, py, camera.fov}, camera.dir);
				ray = (LEVR_Ray){camera.pos, dir};
				pixel.color = LEVR_cast(ray, scene, ms);
				state->set_pixel(&state, &pixel);
		}
	}
	
	#if LEVR_CHECKERBOARD_RENDERING
		state->flags ^= (1 << 0);
	#endif
}

/****************************************
LEVR_RENDER BLOCK END
****************************************/