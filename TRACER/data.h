std::vector<Figure*> Figure::Figures;

std::vector<Figure*> Figures = {
	new Sphere(vec3(0, 0, 250.0f), 200 , vec3(0, 0, 170), 50, 0.6f),
	new Sphere(vec3(-500, 300, 600.0f), 200 , vec3(255, 0, 134), 50, 0.8f),
	new Sphere(vec3(400, 100, 250.0f), 200 , vec3(0, 255, 0), 50, 0.7f),
	new Plane(vec3(0,-240.0f, 0), vec3(0,0.0f,600.0f), vec3(1,0.0f,600.0f), vec3(255,0,0), 100, 1.0f)
};


static const std::vector<Light*> Lights = {
	new Ambient(0.2f),
	new Directional(0.2f, vec3(0.5, -0.5, 1)),
	new Point(0.6f, vec3(5, 200, -100.0f)),
	//new Point(0.6f, vec3(-400, 300, 50)),
	//new Point(0.6f, vec3(0,-205.0f, 200.0f))
};
