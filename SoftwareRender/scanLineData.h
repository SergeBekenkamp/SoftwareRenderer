#pragma once
struct scanLineData
{
	int currentY;
	float ndotla;
	float ndotlb;
	float ndotlc;
	float ndotld;
	scanLineData(float ndotlA) {
		ndotla = ndotlA;
	}
};