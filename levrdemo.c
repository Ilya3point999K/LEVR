#define  APP_IMPLEMENTATION
#define  APP_WINDOWS
#include "app.h"
#include <stdlib.h>
#include <stdio.h>
#include "levr.h"
LEVR_Screen screen = {0};
LEVR_State state = {0};
LEVR_Grid grid = {0};
LEVR_MaterialSet ms = {0};
LEVR_Camera cam = {0};
LEVR_Scene scene = {0};
#define SCWIDTH 256
#define SCHEIGHT 144

void draw_pixel(LEVR_State* state, LEVR_Pixel* pixel){
	screen.pixels[pixel->x + pixel->y * screen.width] = pixel->color;
}

void CreateWalls()
{
	int i, j, k;
	for (i = 0; i < grid.width; i++)
	{
		grid.voxels[i*grid.width*grid.height + 1*grid.width + 1] = 3;
		grid.voxels[1*grid.width*grid.height + i*grid.width + 1] = 2;
		grid.voxels[1*grid.width*grid.height + 1*grid.width + i] = 1;

	}
}

void CreateGrid()
{
	grid.width = 512;
	grid.height = 512;
	grid.depth = 512;
	grid.voxels = malloc(grid.width * grid.height * grid.depth * sizeof(uint8_t));
	uint8_t *buf = malloc(256 * 256 * 256 * sizeof(uint8_t));

	FILE *file = fopen("sponza.raw", "r");
	int c;
	int i = 0;
	int j = 0; 
	int k = 0;
	while ((c = fgetc(file)) != EOF)
	{
		buf[i] = c - '0';
		i++;
	}
	fclose(file);
	for(i = 0; i < 256; i++){
		for(j = 0; j < 256; j++){
			for(k = 0; k < 256; k++){
				grid.voxels[(k)*grid.width*grid.height + (j)*grid.height + (i)] = buf[k*255*255 + j*255+i];
			}
		}
	}
	free(buf);

	grid.voxels[(50)*grid.width*grid.height + (20)*grid.height + (50)] = 4;
}

void InitMaterials(){
	ms.max_materials = 6;
	
	LEVR_Material air = {0};
	air.color = LEVR_rgb_to_u32(0, 0, 0);
	
	LEVR_Material red = {0};
	red.color = LEVR_rgb_to_u32(100, 0, 0);
	red.emittance = 0;
	red.ambient = 0.5;
	red.diffuse = 0;


	LEVR_Material green = {0};
	green.color = LEVR_rgb_to_u32(0, 100, 0);
	green.emittance = 0;
	green.ambient = 0.5;
	green.diffuse = 0;

	LEVR_Material blue = {0};
	blue.color = LEVR_rgb_to_u32(0, 0, 100);
	blue.emittance = 0;
	blue.ambient = 0.5;
	blue.diffuse = 0;

	LEVR_Material light = {0};
	light.color = 0xFFFFFF;
	light.emittance = 1.0;
	
	LEVR_Material yellow = {0};
	yellow.color = LEVR_rgb_to_u32(0, 100, 100);
	yellow.emittance = 0;
	yellow.ambient = 0.5;
	yellow.diffuse = 0;

	ms.materials = malloc(sizeof(LEVR_Material) * ms.max_materials);
	ms.materials[0] = air;
	ms.materials[1] = red;
	ms.materials[2] = green;
	ms.materials[3] = blue;	
	ms.materials[4] = light;
	ms.materials[5] = yellow;
	
}

void CreateCamera(){
	cam.width = SCWIDTH;
	cam.height = SCHEIGHT;
	
	LEVR_vec3 pos = {9, 9, 9};
	LEVR_vec3 dir = {0.1, 0.1, 0};
	LEVR_vec3 up = {0, 1, 0};
	cam.pos = pos;
	cam.dir = dir;
	cam.up = up;
	cam.fov = 70;
}

int app_proc( app_t* app ){
	LEVR_screen_resize(&screen, SCWIDTH, SCHEIGHT);
	LEVR_screen_flush(&screen, 0xffffff);
	CreateCamera();
	CreateGrid();
	//CreateWalls();
	InitMaterials();
	int cell = 0;
	scene.grid = grid;
	scene.max_lights = 2;
	scene.lights = malloc(scene.max_lights * sizeof(LEVR_vec3i));
	scene.lights[0] = (LEVR_vec3i){50, 20, 50};
	scene.lights[1] = (LEVR_vec3i){30, 10, 60};
	state.set_pixel = &draw_pixel;
	state.flags = 0;
	app_screenmode(app, APP_SCREENMODE_WINDOW );
	app_window_size(app, 1024, 576);
	app_interpolation(app, APP_INTERPOLATION_NONE);
	LEVR_vec3 forward = {0, 0, 1};
	LEVR_vec3 right = {1, 0, 0};
	LEVR_vec3 up = {0, 1, 0};
	LEVR_REAL speed = 2;
	while( app_yield( app ) != APP_STATE_EXIT_REQUESTED ){
		LEVR_vec3 cameraForward = LEVR_vec3_rotate(forward, cam.dir);
		LEVR_vec3 cameraRight = LEVR_vec3_rotate(right, cam.dir);
		LEVR_vec3 cameraUp = LEVR_vec3_rotate(up, cam.dir);
	
		app_input_t input = app_input(app);
		for (int i = 0; i < input.count; i++) {
			if (input.events[i].type == APP_INPUT_SCROLL_WHEEL) {
				if (input.events[i].data.wheel_delta > 0)
				{;					
				} else if(input.events[i].data.wheel_delta < 0){
					;
				}
			}
			if (input.events[i].type == APP_INPUT_MOUSE_DELTA && app->has_focus){
				cam.dir.y += input.events[i].data.mouse_delta.x;
				cam.dir.x += input.events[i].data.mouse_delta.y;
				app_pointer_pos(app, 512, 576 / 2);
			}
			if(input.events[i].type == APP_INPUT_KEY_DOWN){

					if(input.events[i].data.key == APP_KEY_LEFT){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].x += 1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 
					if(input.events[i].data.key == APP_KEY_RIGHT){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].x += -1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 
					if(input.events[i].data.key == APP_KEY_UP){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].z += 1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 
					if(input.events[i].data.key == APP_KEY_DOWN){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].z += -1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 
					if(input.events[i].data.key == APP_KEY_W){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraForward, speed));
					} 
					if(input.events[i].data.key == APP_KEY_S){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraForward, -speed));
					} 
					if(input.events[i].data.key == APP_KEY_A){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraRight, -speed));
					} 
					if(input.events[i].data.key == APP_KEY_D){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraRight, speed));
					} 
					if(input.events[i].data.key == APP_KEY_Q){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraUp, -speed));
					} 
					if(input.events[i].data.key == APP_KEY_E){
						cam.pos = LEVR_vec3_add(cam.pos, LEVR_vec3_scalef(cameraUp, speed));
					} 
					if(input.events[i].data.key == APP_KEY_F){
						cam.fov += 5;
					} 
					if(input.events[i].data.key == APP_KEY_G){
						cam.fov -= 5;
					}
					if(input.events[i].data.key == APP_KEY_R){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].y += -1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 
					if(input.events[i].data.key == APP_KEY_T){
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 0;
						scene.lights[0].y += 1;
						scene.grid.voxels[scene.lights[0].z*grid.width*grid.height + scene.lights[0].y*grid.height + scene.lights[0].x] = 4;
					} 		
					if(input.events[i].data.key == APP_KEY_H){
						scene.grid.voxels[cell] = 3;
						cell++;
					}	
			}
		}
		LEVR_render(&state, cam, scene, ms);
		app_present( app, screen.pixels, screen.width, screen.height, 0xffffff, 0x000000 );
	}
	return 0;
}

int main( int argc, char** argv ){
	(void) argc, argv;
	return app_run( app_proc, NULL, NULL, NULL, NULL );
}