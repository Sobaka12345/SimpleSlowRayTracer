std::vector<Figure*> Figure::Figures = {
	new Sphere(vec3(0, 0, 250.0f), 200 , vec3(0, 0, 255), 5),
	new Sphere(vec3(-400, 100, 250.0f), 200 , vec3(0, 255, 255), 5),
	new Sphere(vec3(400, 100, 250.0f), 200 , vec3(0, 255, 0), 5),
	new Plane(vec3(0,0, 250.0f), vec3(0,10,250.0f), vec3(1,10,250.0f), vec3(255,255,0), 100)
};


static const std::vector<Light*> Lights = {
	new Ambient(0.2f),
	new Directional(0.2f, vec3(0, 0, 1)),
	new Point(0.6f, vec3(200, 100, 40.0f)),
	//new Point(0.6f, vec3(-400, 300, 50)),
	//new Point(0.6f, vec3(0,-205.0f, 200.0f))
};
