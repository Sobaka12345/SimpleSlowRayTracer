#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <thread>

//SFML
#include <SFML/Graphics.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


#include "lights.h"
#include "figures.h"
#include "data.h"


std::vector<vec3> render(vec3 view, int width, int height);
vec3 raytrace(vec3 Dir, vec3 Origin, float min_t = 1.0f, float max_t = INFINITY, int depth = 3);
vec3 setView(double angle);


const int Width = 640;
const int Height = 480;
const double ViewAngle = glm::pi<double>() / 3;


int main()
{
	vec3 view = setView(ViewAngle);

	std::vector<vec3> points(render(view, Width, Height));

	sf::RenderWindow window(sf::VideoMode(Width, Height), "TRACER");

	sf::Uint8 * pixels = new sf::Uint8[Width*Height * 4];

	sf::Texture texture;
	texture.create(Width, Height);

	sf::Sprite sprite(texture);

	int counter = 0;
	for (auto x : points)
	{
		pixels[counter++] = (int)x.x;
		pixels[counter++] = (int)x.y;
		pixels[counter++] = (int)x.z;
		pixels[counter++] = 255;
	}

	texture.update(pixels);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(sprite);
		window.display();
	}


	delete pixels;
	for (auto x : Figure::Figures)
		delete x;
	for (auto x : Lights)
		delete x;
		
	return 0;
}

std::vector<vec3> render(vec3 view, int width, int height)
{
	std::vector<vec3> points;
	for (int i = height / 2; i >= -height / 2; i--)
	{
		for (int j = -width / 2; j <= width / 2; j++)
		{
			if (i == 0 || j == 0) continue;
			points.push_back(raytrace(vec3(j, i, 0) - view, view));
		}
	}

	return points;
}

vec3 raytrace(vec3 Dir, vec3 Origin, float min_t, float max_t, int depth)
{
	vec3 outColor(0, 0, 0);

	float closest_t;
	Figure * closest_figure = Figure::getIntersection(Dir, Origin, min_t, max_t, closest_t);
	if (closest_figure == nullptr) 
		return outColor;


	outColor = closest_figure->color * float(closest_figure->calcLight(Origin, Dir, closest_t, Lights));
	
	float r = closest_figure->reflective;
	if (depth <= 0 || r <= 0)
		return outColor;

	vec3 P = Dir * closest_t + Origin;
	vec3 R = closest_figure->reflectedRay(P, Dir);
	//if(depth != 3)
	//std::cout << R.x << " " << R.y << " " << R.z << "         " << Dir.x << " " << Dir.y << " " << Dir.z <<  "     " << depth << std::endl;

	vec3 reflectedColor = raytrace(R, P, 0.001f, INFINITY, depth - 1);
	//std::cout << reflectedColor.x << " " << reflectedColor.y << " " << reflectedColor.z << std::endl;

	return outColor * (1.0f - r) + reflectedColor * r;
}


vec3 setView(double angle)
{
	double PI = glm::pi<double>();

	angle /= 2;

	double dz = glm::abs((Width / 2) / glm::tan(angle));

	return vec3(0, 0, -dz);
}