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

	static Figure * getIntersection(vec3 FOV, vec3 view, float t_min, float t_max, float& t_out)
	{
		float closest_t = INFINITY;
		Figure * closest_figure = nullptr;

		for (auto x : Figures)
		{
			std::vector<float> result = x->rayIntersect(FOV, view);
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

	virtual std::vector<float> rayIntersect(vec3 FOV, vec3 view) = 0;
	virtual double calcLight(vec3 view, vec3 FOV, float t, std::vector<Light*> lights) = 0;

protected:
	virtual double calcAmbient(Ambient * light) { return light->intensity; };
	virtual double calcPoint(Point * light, vec3 view, vec3 FOV, float t) = 0;
	virtual double calcDirectional(Directional * light, vec3 view, vec3 FOV, float t) = 0;
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

	virtual std::vector<float> rayIntersect(vec3 FOV, vec3 view) override
	{
		vec3  ViewToCenter = view - center;
		vec3  ViewToFOV = FOV - view;

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

	virtual double calcLight(vec3 view, vec3 FOV, float t, std::vector<Light*> lights) override
	{
		double i = 0;
		for (auto x : lights)
		{
			if (x->getName() == "Point")
				i += calcPoint(dynamic_cast<Point*>(x), view, FOV, t);
			else if (x->getName() == "Ambient")
				i += calcAmbient(dynamic_cast<Ambient*>(x));
			else if (x->getName() == "Directional")
				i += calcDirectional(dynamic_cast<Directional*>(x), view, FOV, t);
		}

		return i > 1 ? 1 : i;
	}

private:
	double calcPoint(Point * light, vec3 view, vec3 FOV, float t) override
	{
		double i = 0;

		vec3 P = (FOV - view) * t + view;
		vec3 L = light->position - P;
		vec3 N = (P - center);
		N /= length(N);

		float kek;
		if (getIntersection(L, P, 0.001f, INFINITY, kek) != nullptr)
			return i;

		if (dot(N, L) > 0)
			i += (dot(N, L) / (length(N) * length(L))) * light->intensity;

		if (spec != -1)
		{
			vec3 V = view - FOV;
			vec3 R = 2.0f * N * dot(N, L) - L;
			if (dot(R, V) > 0)
				i += light->intensity * pow(dot(R, V) / (length(R)*length(V)), spec);
		}

		return i;
	}

	double calcDirectional(Directional * light, vec3 view, vec3 FOV, float t) override
	{
		double i = 0;

		vec3 P = (FOV - view) * t + view;
		vec3 N = (P - center);
		vec3 L = light->direction;
		N /= length(N);

		float kek;
		if (getIntersection(L, P, 0.001f, INFINITY, kek) != nullptr)
			return i;

		if (dot(N, L) > 0)
			i += (dot(N, L) / (length(N) * length(L))) * light->intensity;


		if (spec != -1)
		{
			vec3 V = view - FOV;
			vec3 R = 2.0f * N * dot(N, L) - L;
			if (dot(R, V) > 0)
				i += light->intensity * pow(dot(R, V) / (length(R)*length(V)), spec);
		}

		return i;
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

	virtual std::vector<float> rayIntersect(vec3 FOV, vec3 view) override
	{
		double val = dot(ABC, (FOV - view));
		if (val == 0)
			return std::vector<float>();
		double x = ((-1) * (dot(ABC, view) + D)) / val;
		

		return std::vector<float>{ (float)x };
	}

	virtual double calcLight(vec3 view, vec3 FOV, float t, std::vector<Light*> lights) override
	{
		double i = 0;
		for (auto x : lights)
		{
			if (x->getName() == "Point")
				i += calcPoint(dynamic_cast<Point*>(x), view, FOV, t);
			else if (x->getName() == "Ambient")
				i += calcAmbient(dynamic_cast<Ambient*>(x));
			else if (x->getName() == "Directional")
				i += calcDirectional(dynamic_cast<Directional*>(x), view, FOV, t);
		}

		return i > 1 ? 1 : i;
	}

private:
	double calcPoint(Point * light, vec3 view, vec3 FOV, float t) override
	{
		double i = 0;

		vec3 P = (FOV - view) * t + view;
		vec3 L = light->position - P;

		float kek;
		if (getIntersection(L, P, 0.001f, INFINITY, kek) != nullptr)
			return i;

		vec3 N(ABC.x / glm::abs(ABC.x), ABC.y / glm::abs(ABC.y), ABC.z / glm::abs(ABC.z));
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

	double calcDirectional(Directional * light, vec3 view, vec3 FOV, float t) override
	{
		double i = 0;

		vec3 P = (FOV - view) * t + view;
		vec3 N(ABC.x / glm::abs(ABC.x), ABC.y / glm::abs(ABC.y), ABC.z / glm::abs(ABC.z));
		vec3 L = light->direction;
		float N_dot_L = dot(N, L);

		float kek;
		if (getIntersection(L, P, 0.001f, INFINITY, kek) != nullptr)
			return i;

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
};