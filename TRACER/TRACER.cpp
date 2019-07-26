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


std::vector<vec3> raytrace(vec3 view, int width, int height);
vec3 setView(double angle);


const int Width = 640;
const int Height = 480;
const double ViewAngle = glm::pi<double>() / 3;


int main()
{
	vec3 view = setView(ViewAngle);

	std::vector<vec3> points(raytrace(view, Width, Height));

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

std::vector<vec3> raytrace(vec3 view, int width, int height)
{
	std::vector<vec3> points;
	for (int i = height / 2; i >= -height / 2; i--)
	{
		for (int j = -width / 2; j <= width / 2; j++)
		{
			if (i == 0 || j == 0) continue;
			vec3 FOV(j, i, 0);
			vec3 outColor(0, 0, 0);
			float closest_t;

			Figure * closest_figure = Figure::getIntersection(FOV, view, 1, INFINITY, closest_t);
			if (closest_figure == nullptr) {
				points.push_back(vec3(0, 0, 0));
				continue;
			}
			outColor = closest_figure->color * float(closest_figure->calcLight(view, FOV, closest_t, Lights));
			points.push_back(outColor);
		}
	}

	return points;
}


vec3 setView(double angle)
{
	double PI = glm::pi<double>();

	angle /= 2;

	double dz = glm::abs((Width / 2) / glm::tan(angle));

	return vec3(0, 0, -dz);
}