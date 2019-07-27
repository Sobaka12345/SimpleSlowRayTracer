#pragma once

struct Figure
{
	vec3 color;
	int32 spec;
	static std::vector<Figure*> Figures;

	Figure(vec3 _color, int32 _spec) : 
		color(_color),
		spec(_spec)
	{
		Figure::Figures.push_back(this);
	}

	static Figure * getIntersection(vec3 Dir, vec3 view, float t_min, float t_max, float& t_out)
	{
		float closest_t = INFINITY;
		Figure * closest_figure = nullptr;

		for (auto x : Figures)
		{
			std::vector<float> result = x->rayIntersect(Dir, view);
			if (result.empty()) continue;
			for (auto y : result)
				if (y > t_min && y < t_max && y < closest_t)
				{
					closest_t = y;
					closest_figure = x;
				}
		}
		t_out = closest_t;

		return closest_figure;
	}

	static vec3 getL(Light * light, vec3 P)
	{
		std::string name = light->getName();
		if (name == "Ambient")
			return vec3();
		else if (name == "Directional")
			return -light->getVec();
		else if (name == "Point")
			return light->getVec() - P;

		return vec3();
	}

	double calcLight(vec3 view, vec3 FOV, float t, std::vector<Light*> lights)
	{
		double i = 0;
		for (auto x : lights)
		{
				i += calcLightPart(x, view, FOV, t);
		}

		return i > 1 ? 1 : i;
	}

	virtual std::vector<float> rayIntersect(vec3 Dir, vec3 view) = 0;

protected:
	double calcLightPart(Light * light, vec3 view, vec3 FOV, float t)
	{
		if (light->getName() == "Ambient") return light->intensity;

		double i = 0;

		vec3 P = (FOV - view) * t + view;
		vec3 L = getL(light, P);
		vec3 N = getN(P);

		float kek;
		//if (light->getName() == "Point")

		if (getIntersection(L, P, 0.001f, INFINITY, kek) != nullptr)
			return i;

		float N_dot_L = dot(N, L);
		if (N_dot_L > 0)
			i += (N_dot_L / (length(N) * length(L))) * light->intensity;

		if (spec != -1)
		{
			vec3 V = view - FOV;
			vec3 R = 2.0f * N * N_dot_L - L;
			float R_dot_V = dot(R, V);
			if (R_dot_V > 0)
				i += light->intensity * pow(R_dot_V / (length(R)*length(V)), spec);
		}

		return i;
	}

	virtual vec3 getN(vec3 P) = 0;
};

struct Sphere : public Figure
{
	vec3 center;
	float32 radius;

	Sphere(vec3 _center, float32 _radius, vec3 _color, int32 _spec):
		Figure(_color, _spec),
		center(_center),
		radius(_radius)
	{}

	std::vector<float> rayIntersect(vec3 Dir, vec3 view) override
	{
		vec3  ViewToCenter = view - center;
		vec3  ViewToFOV = Dir;

		double A = dot(ViewToFOV, ViewToFOV);
		double B = dot(ViewToCenter, ViewToFOV) * 2.0;
		double C = dot(ViewToCenter, ViewToCenter) - radius * radius;


		double D = B * B - 4 * A * C;


		if (D < 0)
			return std::vector<float>();


		double x1 = (-B - sqrt(D)) / (2.0 * A);
		double x2 = (-B + sqrt(D)) / (2.0 * A);


		return std::vector<float>{(float)x1, (float)x2};
	}

private:
	vec3 getN(vec3 P) override
	{
		return P - center;
	}
};


struct Plane : public Figure
{
	vec3 ABC;
	double D;

	Plane(vec3 point_1, vec3 point_2, vec3 point_3, vec3 _color, int32 _spec) : Figure(_color, _spec)
	{
		glm::mat2x2 M1(
			vec2(point_2.y - point_1.y, point_2.z - point_1.z),
			vec2(point_3.y - point_1.y, point_3.z - point_1.z)
		);
		glm::mat2x2 M2(
			vec2(point_2.x - point_1.x, point_2.z - point_1.z),
			vec2(point_3.x - point_1.x, point_3.z - point_1.z)
		);
		glm::mat2x2 M3(
			vec2(point_2.x - point_1.x, point_2.y - point_1.y),
			vec2(point_3.x - point_1.x, point_3.y - point_1.y)
		);

		double A = glm::determinant(M1);
		double B = -glm::determinant(M2);
		double C = glm::determinant(M3);
		double d = point_1.y * (-B) - point_1.x * A - point_1.z * C;
		ABC = vec3(A, B, C);
		D = d;
	}

	std::vector<float> rayIntersect(vec3 Dir, vec3 view) override
	{
		double val = dot(ABC, Dir);
		if (val == 0)
			return std::vector<float>();
		double x = ((-1) * (dot(ABC, view) + D)) / val;
		

		return std::vector<float>{ (float)x };
	}


private:
	vec3 getN(vec3 P) override
	{
		float mul = 1.0f / (ABC.x + ABC.y + ABC.z);
		return -vec3(ABC.x, ABC.y, ABC.z) * mul;
	}
	
};