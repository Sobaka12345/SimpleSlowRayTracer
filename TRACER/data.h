std::vector<Figure*> Figure::Figures;

std::vector<Figure*> Figures = {
	new Sphere(vec3(0, 0, 250.0f), 200 , vec3(0, 0, 200), 5),
	new Sphere(vec3(-400, 100, 250.0f), 200 , vec3(0, 200, 200), 5),
	new Sphere(vec3(400, 100, 250.0f), 200 , vec3(0, 200, 0), 5),
	new Plane(vec3(0,0, 250.0f), vec3(0,10,250.0f), vec3(1,10,250.0f), vec3(255,255,0), 100)
};


static const std::vector<Light*> Lights = {
	new Ambient(0.4f),
	new Directional(0.6f, vec3(0.5, -0.5, 1)),
	new Point(0.6f, vec3(200, 100, -100.0f)),
	//new Point(0.6f, vec3(-400, 300, 50)),
	//new Point(0.6f, vec3(0,-205.0f, 200.0f))
};
