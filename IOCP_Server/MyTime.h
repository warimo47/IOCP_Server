#pragma once

class MyTime
{
public:
	long long t;
	int id;
	unsigned char eventType;

	bool operator()(MyTime a, MyTime b)
	{
		return a.t > b.t;
	}

	MyTime();
	~MyTime();
};

