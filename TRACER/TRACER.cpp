#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>



#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


#include "lights.h"
#include "figures.h"
#include "data.h"


void raytrace(vec3 view, int width, int height);
vec3 setView(double angle);


const int Width = 640;
const int Height = 480;
const double ViewAngle = glm::pi<double>() / 3;


int main()
{
	vec3 view = setView(ViewAngle);

	std::vector<vec3> points;

	raytrace(view, Width, Height);

	for (auto x : Figures)
		delete x;
	for (auto x : Lights)
		delete x;
	return 0;
}

void raytrace(vec3 view, int width, int height)
{
	FILE * file = fopen("keak.ppm", "w+");
	fprintf(file, "P3\n%d %d\n255\n", width + 1, height + 1);
	for (int i = height / 2; i >= -height / 2; i--)
	{
		for (int j = -width / 2; j <= width / 2; j++)
		{
			float closest_t = INFINITY;
			Figure * closest_figure = nullptr;
			vec3 FOV(j, i, 0);
			vec3 outColor(0, 0, 0);

			for (auto x : Figures)
			{
				std::vector<float> result = x->rayIntersect(FOV, view);
				if (result.empty()) continue;
				for(auto y : result)
					if (y > 1 && y < UINT_MAX && y < closest_t)
					{
						closest_t = y;
						closest_figure = x;
					}
			}
			if (closest_figure == nullptr) {
				fprintf(file, "%d %d %d\n", 0, 0, 0);
				continue;
			}
			outColor = closest_figure->color * float(closest_figure->calcLight(view, FOV, closest_t, Lights));
			fprintf(file, "%d %d %d\n", (int)outColor.x, (int)outColor.y, (int)outColor.z);
			//points.push_back(vec3(j / (Width * 0.5f), i / (Height * 0.5f), 0));
		}
	}

	fclose(file);
}


vec3 setView(double angle)
{
	double PI = glm::pi<double>();

	angle /= 2;

	double dz = glm::abs((Width / 2) / glm::tan(angle));

	return vec3(0, 0, -dz);
}