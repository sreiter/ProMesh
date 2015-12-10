/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ ÷lbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
 * Authors: Rossitza Piperkova, Martin Rupp
 *
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 ß7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#ifndef __H__UG__interpolated_heightfield__
#define __H__UG__interpolated_heightfield__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "heightfield_interface.h"

namespace ug
{

///	Loads a pointset from a file and performs linear interpolation between them.
/**	This class was implemented by Rossitza Piperkova during her work at the
 * GCSC-Frankfurt.
 */
class InterpolatedHeightfield : public IHeightfield
{
public:
	// "public" sind funktionen, die von "außen" sichtbar sind

	InterpolatedHeightfield()
	{
		// der Konstruktor der Klasse setzt alle Variablen auf ihre Anfangswerte
		X = Y = Z = 0;
		rows = columns = 0;
	}

	~InterpolatedHeightfield()
	{
		// das hier ist der Dekonstruktor, er sorgt dafür dass alle Speicherbereiche freigegeben werden usw.
		// entspricht deinem "finish"
		cleanupDataMemory();
	}

	bool loads_from_file()
	{
		// wir geben true zurück, da wir eine Datei laden möchten
		return true;
	}

	const char* file_name_extensions()
	{
		// dateiendungen, damit nur diese im dateidialog angezeigt werden
		return "*.mesh";
	}

	bool initialize(const char* filename, number xMin, number yMin, number xMax, number yMax)
	{
		return readmesh( filename ) == 0;
	}

	number height(number x, number y)
	{
		int			idx_a_i=0, idx_a_j=0,
		idx_b_i=0, idx_b_j=0,
		idx_c_i=0, idx_c_j=0,
		idx_d_i=0, idx_d_j=0;
		double		alpha;
		double		z, z_ab, z_cd;

		if(X==0)	// Keine Daten bisher gelesen?
			return 0;

		// in welchem gitterquadrat sich (x,y) befindet
		for(int i=0; i<rows; i++) {
			for(int j=0; j<columns; j++) {
				//printf("%f %f\n", X[i], Y[j]);
				if(X[i][j] >= x && Y[i][j]>= y) {
					idx_d_i = i;
					idx_d_j = j;
					idx_c_i = i;
					idx_c_j = j-1;
					idx_b_i = i-1;
					idx_b_j = j;
					idx_a_i = i-1;
					idx_a_j = j-1;
					goto stop;
				}
			}
		}
	stop:

		//printf("%d %d\n", idx_d_i, idx_d_j);

		if(idx_d_i==0 && idx_d_j==0) {
			z= Z[idx_d_i][idx_d_j];	// kein a, b, c
		}else if(idx_d_i==0) {
			alpha = (x - X[idx_c_i][idx_c_j]) / (X[idx_d_i][idx_d_j] - X[idx_c_i][idx_c_j]);
			z = interpoliere(Z[idx_c_i][idx_c_j], Z[idx_d_i][idx_d_j], alpha);	//interpoliere c und d (über x-koordinate)
		}else if(idx_d_j==0) {
			alpha = (y - Y[idx_b_i][idx_b_j]) / (Y[idx_d_i][idx_d_j] - Y[idx_b_i][idx_b_j]);
			z = interpoliere(Z[idx_b_i][idx_b_j], Z[idx_d_i][idx_d_j], alpha);	//interpoliere b und d (über y-koordinate)
		}else {
			alpha = (x - X[idx_c_i][idx_c_j]) / (X[idx_d_i][idx_d_j] - X[idx_c_i][idx_c_j]);	// Allgemeinfall
			z_ab = interpoliere(Z[idx_a_i][idx_a_j], Z[idx_b_i][idx_b_j], alpha);
			z_cd = interpoliere(Z[idx_c_i][idx_c_j], Z[idx_d_i][idx_d_j], alpha);
			alpha = (y - Y[idx_a_i][idx_a_j]) / (Y[idx_c_i][idx_c_j] - Y[idx_a_i][idx_a_j]);
			z = interpoliere(z_ab, z_cd, alpha);
		}

		return z;
	}


private:
	// hier kommen Sachen hin, die nur diese Klasse interessieren, also deine Funktionen readline oder interpoliere
	int readmesh( const char* filename ) {
		using namespace std;
		ifstream	f;
		string		line;
		//char		*token;
		//size_t  	size = 1024;
		int		err=0;
		vector<std::string>	tokens;

		cleanupDataMemory();	// Lösche Daten, falls schon vorher mal verwendet wurde

		//		line = (char*)malloc(size);
		f.open(filename);

		if(!f.good()) {
			err=-1;
			goto ende2;
		}

		getline( f, line );
		rows = atoi(line.c_str());

		if(!f.good()) {
			err=-2;
			goto ende2;
		}
		getline( f, line );
		columns = atoi (line.c_str());

		if(columns<=0 || rows<=0) {
			err=-3;
			goto ende;
		}

		if(!f.good()) {
			err=-2;
			goto ende2;
		}

		//		printf("%d %d\n",rows,columns);

		if(!allocateDataMemory())
			goto ende;


		for(int i=0; i<rows; i++) {
			if(f.good()) {
				getline( f, line );
			}else {
				err=-2;
				cleanupDataMemory();
				goto ende;
			}

			if(TokenizeString(line, ";", true, tokens)<(size_t)columns) {
				err=-4;
				cleanupDataMemory();
				goto ende;
			}
			for(int j=0; j<columns; j++) {
				X[i][j] = atof( tokens[j].c_str() );
			}
		}


		for(int i=0; i<rows; i++) {
			if(f.good()) {
				getline( f, line );
			}else {
				err=-2;
				cleanupDataMemory();
				goto ende;
			}

			if(TokenizeString(line, ";", true, tokens)<(size_t)columns) {
				err=-4;
				cleanupDataMemory();
				goto ende;
			}
			for(int j=0; j<columns; j++) {
				Y[i][j] = atof( tokens[j].c_str() );
			}
		}


		for(int i=0; i<rows; i++) {
			if(f.good()) {
				getline( f, line );
			}else {
				err=-2;
				cleanupDataMemory();
				goto ende;
			}

			if(TokenizeString(line, ";", true, tokens)<(size_t)columns) {
				err=-4;
				cleanupDataMemory();
				goto ende;
			}
			for(int j=0; j<columns; j++) {
				Z[i][j] = atof( tokens[j].c_str() );
			}
		}
	ende:
		f.close();
	ende2:

		/*	for(int k=0; k<rows*columns; k++)
		 *		printf("%f\n",X[k]);
		 */
		return err;
	}

	double interpoliere(double a, double b, double alpha) {
		return (a*(1-alpha) + (b*alpha));
	}

	/*	char *readline( FILE *f, char **buffer, size_t *size ) {
	 int     read;

	 read = getline(buffer, size, f);

	 if(read<0)
	 return 0;

	 return *buffer;
	 }
	 */
	bool allocateDataMemory() {
		//X = (double**)malloc(rows*sizeof(double*));
		X = new double*[rows];
		if(X)
			memset(X, 0, rows*sizeof(double*));
		//Y = (double**)malloc(rows*sizeof(double*));
		Y = new double*[rows];
		if(Y)
			memset(Y, 0, rows*sizeof(double*));
		//Z = (double**)malloc(rows*sizeof(double*));
		Z = new double*[rows];
		if(Z)
			memset(Z, 0, rows*sizeof(double*));

		if(X==0 || Y==0 || Z==0) {
			cleanupDataMemory();
			return false;
		}
		for(int k=0; k<rows; k++) {
			//X[k] = (double*)malloc(columns*sizeof(double));
			X[k] = new double[columns];
			if(!X[k]) {
				cleanupDataMemory();
				return false;
			}
		}
		for(int k=0; k<rows; k++) {
			//Y[k] = (double*)malloc(columns*sizeof(double));
			Y[k] = new double[columns];
			if(!Y[k]) {
				cleanupDataMemory();
				return false;
			}
		}
		for(int k=0; k<rows; k++) {
			//Z[k] = (double*)malloc(columns*sizeof(double));
			Z[k] = new double[columns];
			if(!Z[k]) {
				cleanupDataMemory();
				return false;
			}
		}
		return true;
	}

	void cleanupDataMemory() {
		if(X) {
			for(int i=0; i<rows; i++)
				if(X[i])
					delete[](X[i]);
				else
					break;
			delete[] X;
			X=0;
		}
		if(Y) {
			for(int i=0; i<rows; i++)
				if(Y[i])
					delete[](Y[i]);
				else
					break;
			delete[] Y;
			Y=0;
		}
		if(Z) {
			for(int i=0; i<rows; i++)
				if(Z[i])
					delete[](Z[i]);
				else
					break;
			delete[] Z;
			Z=0;
		}
		rows = columns = 0;
	}



	/* Hilfsfunktion um einen String zu zerlegen */
	unsigned TokenizeString(const std::string& i_source,
							const std::string& i_seperators,
							bool i_discard_empty_tokens,
							std::vector<std::string>& o_tokens)
	{
		size_t prev_pos = 0;
		size_t pos = 0;
		unsigned number_of_tokens = 0;

		o_tokens.clear();
		pos = i_source.find_first_of(i_seperators, pos);
		while (pos != std::string::npos)
		{
			std::string token = i_source.substr(prev_pos, pos - prev_pos);
			if (!i_discard_empty_tokens || token != "")
			{
				o_tokens.push_back(token);
				number_of_tokens++;
			}

			pos++;
			prev_pos = pos;
			pos = i_source.find_first_of(i_seperators, pos);
		}

		if (prev_pos < i_source.length())
		{
			o_tokens.push_back(i_source.substr(prev_pos));
			number_of_tokens++;
		}

		return number_of_tokens;
	}

	// hier kommen die variablen hin, die bei dir vorher global waren, also
	double **X;
	double **Y;
	double **Z;
	int rows;
	int columns;
};

// Das ist ein total einfaches heightfield, zum testen
class SquareHeightfield : public IHeightfield
{
public:

	SquareHeightfield()
	{
	}

	~SquareHeightfield()
	{
	}

	bool loads_from_file()
	{
		return false;
	}

	const char* file_name_extensions()
	{
		return "";
	}

	bool initialize(const char* filename, number xMin, number yMin, number xMax, number yMax)
	{
		return true;
	}

	number height(number x, number y)
	{
		return x*x + y*y;
	}
};


}//	end of namespace

#endif
